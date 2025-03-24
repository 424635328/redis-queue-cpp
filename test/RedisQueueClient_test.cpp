// test/RedisQueueClient_test.cpp
#include "gtest/gtest.h"
#include "RedisQueueClient.h"
#include <thread>
#include <chrono>

TEST(RedisQueueClientTest, SendAndReceiveMessage) {
    RedisQueueClient client("localhost", 6379, "test_queue");
    std::string testMessage = "Test Message Content";

    // Clean the queue first
    while (!client.receiveMessage(1).first.empty()) {} // 1 second timeout

    std::string messageId = client.sendMessage(testMessage);
    ASSERT_FALSE(messageId.empty());

    std::pair<std::string, std::string> receivedPair = client.receiveMessage(1);
    ASSERT_EQ(receivedPair.first, messageId);
    ASSERT_EQ(receivedPair.second, testMessage);
}

TEST(RedisQueueClientTest, PriorityMessages) {
    RedisQueueClient client("localhost", 6379, "priority_queue");

    // Clean the queue first
    while (!client.receiveMessage(1).first.empty()) {} // 1 second timeout

    //使用更大的数值表示更高的优先级
    client.sendMessage("Low Priority", 1);      //最低优先级
    client.sendMessage("High Priority", 100);    //最高优先级
    client.sendMessage("Medium Priority", 50);   //中等优先级

    std::pair<std::string, std::string> received1 = client.receiveMessage(1);
    ASSERT_EQ(received1.second, "High Priority");

    std::pair<std::string, std::string> received2 = client.receiveMessage(1);
    ASSERT_EQ(received2.second, "Medium Priority");

    std::pair<std::string, std::string> received3 = client.receiveMessage(1);
    ASSERT_EQ(received3.second, "Low Priority");
}

TEST(RedisQueueClientTest, DelayedMessages) {
    RedisQueueClient client("localhost", 6379, "delayed_queue");

    // Clean the queue first
    while (!client.receiveMessage(1).first.empty()) {} // 1 second timeout

    std::string delayedMessage = "Delayed Message Content";
    client.sendDelayedMessage(delayedMessage, 3); // Delay for 3 seconds

    // Try to receive immediately - should be empty
    std::pair<std::string, std::string> received1 = client.receiveMessage(1);
    ASSERT_TRUE(received1.first.empty());

    // Wait for delay
    std::this_thread::sleep_for(std::chrono::seconds(4));

    // Now receive - should have message
    std::pair<std::string, std::string> received2 = client.receiveMessage(1);
    ASSERT_EQ(received2.second, delayedMessage);
}

TEST(RedisQueueClientTest, QueueLength) {
    RedisQueueClient client("localhost", 6379, "test_queue_len");

    // 确保context不为空
    if (!client.getContext()) {
        FAIL() << "Not connected to Redis.";
        return;
    }

    // 使用 DEL 命令安全地清空队列
    redisReply* delReply = (redisReply*)redisCommand(client.getContext(), "DEL %s", client.getQueueName().c_str());
    if (delReply == nullptr) {
        FAIL() << "Failed to execute DEL command.";
        return;
    }
    freeReplyObject(delReply);

    client.sendMessage("Message 1");
    client.sendMessage("Message 2");
    ASSERT_EQ(client.getQueueLength(), 2);
    client.receiveMessage(1);
    client.receiveMessage(1);
    ASSERT_EQ(client.getQueueLength(), 0);
}