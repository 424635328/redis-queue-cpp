// RedisQueueClient.h
#ifndef REDISQUEUECLIENT_H
#define REDISQUEUECLIENT_H

#include <string>
#include <vector>
#include <hiredis/hiredis.h>

class RedisQueueClient {
public:
    RedisQueueClient(const std::string& host, int port, const std::string& queue_name, const std::string& ack_queue_name = "");
    ~RedisQueueClient();

    std::string sendMessage(const std::string& message, int priority = 0);
    std::string sendDelayedMessage(const std::string& message, int delaySeconds = 0);
    std::vector<std::string> sendMessages(const std::vector<std::string>& messages);
    std::pair<std::string, std::string> receiveMessage(int timeout = 5);
    void acknowledgeMessage(const std::string& messageId);
    long long getQueueLength();

    // Getter methods for accessing private members
    redisContext* getContext() const { return context_; }
    const std::string& getQueueName() const { return queue_name_; }

private:
    std::string host_;
    int port_;
    std::string queue_name_;
    std::string ack_queue_name_;
    redisContext* context_;

    void connect();
    void disconnect();
};

std::string generateMessageId(); //声明
#endif