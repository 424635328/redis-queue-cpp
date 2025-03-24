#ifndef REDISQUEUECLIENT_H
#define REDISQUEUECLIENT_H

#include <string>

class RedisQueueClient {
public:
    RedisQueueClient(const std::string& host = "127.0.0.1", int port = 6379,
                     const std::string& queue_name = "default_queue");
    ~RedisQueueClient();

    void sendMessage(const std::string& message);
    std::string receiveMessage(int timeout = 0);
    long long getQueueLength();

private:
    std::string host_;
    int port_;
    std::string queue_name_;
    redisContext* context_;

    void connect();
    void disconnect();
};

#endif