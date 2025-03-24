#include "RedisQueueClient.h"
#include <iostream>
#include <hiredis/hiredis.h>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <chrono>
#include <thread>

std::string generateMessageId() {
    static long long counter = 0;
    std::stringstream ss;
    ss << time(0) << "-" << ++counter;
    return ss.str();
}

RedisQueueClient::RedisQueueClient(const std::string& host, int port, const std::string& queue_name, const std::string& ack_queue_name)
    : host_(host), port_(port), queue_name_(queue_name), ack_queue_name_(ack_queue_name), context_(nullptr) {
    connect();
}


RedisQueueClient::~RedisQueueClient() {
    disconnect();
}

void RedisQueueClient::connect() {
    context_ = redisConnect(host_.c_str(), port_);
    if (context_ == nullptr || context_->err) {
        std::string error_message;
        if (context_) {
            error_message = "Connection error: " + std::string(context_->errstr);
            redisFree(context_);
            context_ = nullptr;
        } else {
            error_message = "Connection error: can't allocate redis context";
        }
        throw std::runtime_error("Failed to connect to Redis: " + error_message);
    }
}

void RedisQueueClient::disconnect() {
    if (context_) {
        redisFree(context_);
        context_ = nullptr;
    }
}

// 发送带有优先级的消息，优先级高的先被消费
std::string RedisQueueClient::sendMessage(const std::string& message, int priority) {
    if (!context_) {
        throw std::runtime_error("Not connected to Redis.");
    }

    std::string messageId = generateMessageId();
    std::string messageWithId = messageId + ":" + message;

    // 使用 ZADD 将消息添加到 Sorted Set 中，优先级作为 score
    redisReply* reply = (redisReply*)redisCommand(context_, "ZADD %s %d %s", queue_name_.c_str(), priority, messageWithId.c_str());

    if (reply == nullptr) {
        throw std::runtime_error("Failed to execute ZADD command.");
    }
    freeReplyObject(reply);
    return messageId;
}

// 延迟发送消息
std::string RedisQueueClient::sendDelayedMessage(const std::string& message, int delaySeconds) {
    if (!context_) {
        throw std::runtime_error("Not connected to Redis.");
    }

    std::string messageId = generateMessageId();
    std::string messageWithId = messageId + ":" + message;

    // 计算消息可以被消费的时间戳
    auto triggerTime = std::chrono::system_clock::now() + std::chrono::seconds(delaySeconds);
    long long timestamp = std::chrono::system_clock::to_time_t(triggerTime);

    // 使用 ZADD 将消息添加到 Sorted Set 中，时间戳作为 score
    redisReply* reply = (redisReply*)redisCommand(context_, "ZADD %s %lld %s", queue_name_.c_str(), timestamp, messageWithId.c_str());

    if (reply == nullptr) {
        throw std::runtime_error("Failed to execute ZADD command for delayed message.");
    }
    freeReplyObject(reply);
    return messageId;
}

// 批量发送消息
std::vector<std::string> RedisQueueClient::sendMessages(const std::vector<std::string>& messages) {
    if (!context_) {
        throw std::runtime_error("Not connected to Redis.");
    }

    std::vector<std::string> messageIds;
    for (const auto& message : messages) {
        std::string messageId = generateMessageId();
        std::string messageWithId = messageId + ":" + message;
        redisReply* reply = (redisReply*)redisCommand(context_, "ZADD %s %d %s", queue_name_.c_str(), 0, messageWithId.c_str()); // 默认优先级
        if (reply == nullptr) {
            throw std::runtime_error("Failed to execute ZADD command.");
        }
        freeReplyObject(reply);
        messageIds.push_back(messageId);
    }
    return messageIds;
}

// 接收消息，返回消息ID和消息内容
std::pair<std::string, std::string> RedisQueueClient::receiveMessage(int timeout) {
    if (!context_) {
        throw std::runtime_error("Not connected to Redis.");
    }

    // 使用 ZREVRANGEBYSCORE 获取优先级最高的未过期消息
    redisReply* reply = (redisReply*)redisCommand(context_, "ZREVRANGEBYSCORE %s %lld 0 LIMIT 0 1", queue_name_.c_str(), (long long)time(0));

    if (reply == nullptr) {
        throw std::runtime_error("Failed to execute ZREVRANGEBYSCORE command.");
    }

    std::string messageId;
    std::string message;

    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) {
        // 有可以消费的消息
        if (reply->element[0]->type == REDIS_REPLY_STRING) {
            std::string fullMessage = reply->element[0]->str;

            // 分割消息ID和消息内容
            size_t delimiterPos = fullMessage.find(":");
            if (delimiterPos != std::string::npos) {
                messageId = fullMessage.substr(0, delimiterPos);
                message = fullMessage.substr(delimiterPos + 1);
            } else {
                std::cerr << "Invalid message format: " << fullMessage << std::endl;
                freeReplyObject(reply);
                throw std::runtime_error("Invalid message format received.");
            }
            // 从 Sorted Set 中删除已消费的消息
            redisReply* delReply = (redisReply*)redisCommand(context_, "ZREM %s %s", queue_name_.c_str(), fullMessage.c_str());
            if (delReply == nullptr) {
                throw std::runtime_error("Failed to execute ZREM command.");
            }
            freeReplyObject(delReply);
        } else {
            std::cerr << "Unexpected element type in reply array." << std::endl;
            freeReplyObject(reply);
            throw std::runtime_error("Unexpected element type in reply array.");
        }

    } else if (reply->type == REDIS_REPLY_ARRAY && reply->elements == 0) {
        // 没有可以立即消费的消息，等待 timeout 时间
        freeReplyObject(reply);
        std::this_thread::sleep_for(std::chrono::seconds(timeout));
        return std::make_pair("", ""); // 返回空消息，表示超时
    }
    else {
        std::cerr << "Unexpected reply type: " << reply->type << std::endl;
        freeReplyObject(reply);
        throw std::runtime_error("Unexpected reply type from ZRANGEBYSCORE.");
    }
    freeReplyObject(reply);
    return std::make_pair(messageId, message);
}


void RedisQueueClient::acknowledgeMessage(const std::string& messageId) {
    if (!context_) {
        throw std::runtime_error("Not connected to Redis.");
    }

    redisReply* reply = (redisReply*)redisCommand(context_, "LPUSH %s %s", ack_queue_name_.c_str(), messageId.c_str());
    if (reply == nullptr) {
        throw std::runtime_error("Failed to execute LPUSH command for acknowledgement.");
    }
    freeReplyObject(reply);
}

long long RedisQueueClient::getQueueLength() {
    if (!context_) {
        throw std::runtime_error("Not connected to Redis.");
    }

    redisReply* reply = (redisReply*)redisCommand(context_, "ZCARD %s", queue_name_.c_str()); // 使用 ZCARD 获取 Sorted Set 的大小
    if (reply == nullptr) {
        throw std::runtime_error("Failed to execute ZCARD command.");
    }

    long long length = -1;
    if (reply->type == REDIS_REPLY_INTEGER) {
        length = reply->integer;
    } else {
        std::cerr << "Unexpected reply type: " << reply->type << std::endl;
        freeReplyObject(reply);
        throw std::runtime_error("Unexpected reply type from ZCARD.");
    }
    freeReplyObject(reply);
    return length;
}