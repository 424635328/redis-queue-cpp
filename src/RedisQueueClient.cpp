#include "RedisQueueClient.h"
#include <iostream>
#include <hiredis/hiredis.h>
#include <stdexcept>

RedisQueueClient::RedisQueueClient(const std::string& host, int port, const std::string& queue_name)
    : host_(host), port_(port), queue_name_(queue_name), context_(nullptr) {
    connect();
}

RedisQueueClient::~RedisQueueClient() {
    disconnect();
}

void RedisQueueClient::connect() {
    context_ = redisConnect(host_.c_str(), port_);
    if (context_ == nullptr || context_->err) {
        if (context_) {
            std::cerr << "Connection error: " << context_->errstr << std::endl;
            redisFree(context_);
            context_ = nullptr;
        } else {
            std::cerr << "Connection error: can't allocate redis context" << std::endl;
        }
        throw std::runtime_error("Failed to connect to Redis.");
    }
}

void RedisQueueClient::disconnect() {
    if (context_) {
        redisFree(context_);
        context_ = nullptr;
    }
}

void RedisQueueClient::sendMessage(const std::string& message) {
    if (!context_) {
        throw std::runtime_error("Not connected to Redis.");
    }

    redisReply* reply = (redisReply*)redisCommand(context_, "LPUSH %s %s", queue_name_.c_str(), message.c_str());
    if (reply == nullptr) {
        throw std::runtime_error("Failed to execute LPUSH command.");
    }
    freeReplyObject(reply);
}

std::string RedisQueueClient::receiveMessage(int timeout) {
    if (!context_) {
        throw std::runtime_error("Not connected to Redis.");
    }

    redisReply* reply = (redisReply*)redisCommand(context_, "BRPOP %s %d", queue_name_.c_str(), timeout);
    if (reply == nullptr) {
        throw std::runtime_error("Failed to execute BRPOP command.");
    }

    std::string message;
    if (reply->type == REDIS_REPLY_ARRAY && reply->elements == 2) {
        message = reply->element[1]->str;
    } else if (reply->type == REDIS_REPLY_NIL) {
        message = "";
    } else {
        std::cerr << "Unexpected reply type: " << reply->type << std::endl;
        freeReplyObject(reply);
        throw std::runtime_error("Unexpected reply type from BRPOP.");
    }
    freeReplyObject(reply);
    return message;
}

long long RedisQueueClient::getQueueLength() {
    if (!context_) {
        throw std::runtime_error("Not connected to Redis.");
    }

    redisReply* reply = (redisReply*)redisCommand(context_, "LLEN %s", queue_name_.c_str());
    if (reply == nullptr) {
        throw std::runtime_error("Failed to execute LLEN command.");
    }

    long long length = -1;
    if (reply->type == REDIS_REPLY_INTEGER) {
        length = reply->integer;
    } else {
        std::cerr << "Unexpected reply type: " << reply->type << std::endl;
        freeReplyObject(reply);
        throw std::runtime_error("Unexpected reply type from LLEN.");
    }
    freeReplyObject(reply);
    return length;
}