#include "gtest/gtest.h"
#include "RedisQueueClient.h"
#include <thread>
#include <chrono>

TEST(RedisQueueClientTest, SendAndReceive) {
    RedisQueueClient client("localhost", 6379, "test_queue");

    // Clean the queue first.
    while (!client.receiveMessage(1).empty()) {} // 1 second timeout

    client.sendMessage("Test Message");
    std::string received = client.receiveMessage(1);
    ASSERT_EQ(received, "Test Message");
}

TEST(RedisQueueClientTest, QueueLength) {
    RedisQueueClient client("localhost", 6379, "test_queue_len");

     // Clean the queue first.
    while (!client.receiveMessage(1).empty()) {} // 1 second timeout

    client.sendMessage("Message 1");
    client.sendMessage("Message 2");
    ASSERT_EQ(client.getQueueLength(), 2);

    client.receiveMessage(1);
    ASSERT_EQ(client.getQueueLength(), 1);
}