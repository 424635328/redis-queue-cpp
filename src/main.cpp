#include "RedisQueueClient.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    try {
        RedisQueueClient client("localhost", 6379, "my_cpp_queue");

        // Producer
        client.sendMessage("High priority message", 1); // 高优先级
        client.sendMessage("Normal message"); // 默认优先级
        client.sendDelayedMessage("Delayed message", 20);
        std::cout << "Messages sent." << std::endl;

        // Consumer
        std::cout << "Waiting for messages..." << std::endl;

        // 先接收高优先级消息
        std::pair<std::string, std::string> received1 = client.receiveMessage(5);
        if (!received1.second.empty()) {
            std::cout << "Received message: " << received1.second << ", ID: " << received1.first << std::endl;
        } else {
            std::cout << "No message received within timeout." << std::endl;
        }

        // 接收普通优先级消息
        std::pair<std::string, std::string> received2 = client.receiveMessage(5);
        if (!received2.second.empty()) {
            std::cout << "Received message: " << received2.second << ", ID: " << received2.first << std::endl;
        } else {
            std::cout << "No message received within timeout." << std::endl;
        }

        // 等待延迟消息可以被消费
        std::this_thread::sleep_for(std::chrono::seconds(10));

        // 接收延迟消息
        std::pair<std::string, std::string> received3 = client.receiveMessage(5);
        if (!received3.second.empty()) {
            std::cout << "Received message: " << received3.second << ", ID: " << received3.first << std::endl;
        } else {
            std::cout << "No message received within timeout." << std::endl;
        }

        long long queueLength = client.getQueueLength();
        std::cout << "Queue length: " << queueLength << std::endl;


    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}