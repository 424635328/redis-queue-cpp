#include "RedisQueueClient.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    try {
        RedisQueueClient client("localhost", 6379, "my_cpp_queue");

        // Producer
        client.sendMessage("Message from C++ Producer 1");
        client.sendMessage("Message from C++ Producer 2");
        std::cout << "Messages sent." << std::endl;

        // Consumer
        std::cout << "Waiting for messages..." << std::endl;
        std::string message1 = client.receiveMessage(5); // 5秒超时
        if (!message1.empty()) {
            std::cout << "Received message: " << message1 << std::endl;
        } else {
            std::cout << "No message received within timeout." << std::endl;
        }

        std::string message2 = client.receiveMessage(5); // 5秒超时
        if (!message2.empty()) {
            std::cout << "Received message: " << message2 << std::endl;
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