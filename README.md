# Redis 消息队列客户端 (C++)

一个轻量级的，基于 Redis 的消息队列客户端，使用 C++ 和 hiredis 实现。

## 目录

1.  [特性](#特性)
2.  [依赖](#依赖)
3.  [构建和安装](#构建和安装)
    *   [Ubuntu](#ubuntu)
    *   [Windows (MinGW)](#windows-mingw)
4.  [使用方法](#使用方法)
    *   [基本示例](#基本示例)
5.  [功能扩展](#功能扩展)
    *   [消息确认 (Acknowledgement)](#消息确认-acknowledgement)
    *   [消息持久化 (Persistence)](#消息持久化-persistence)
    *   [消息优先级 (Prioritization)](#消息优先级-prioritization)
    *   [发布/订阅 (Publish/Subscribe)](#发布订阅-publishsubscribe)
6.  [发展方向](#发展方向)
    *   [连接池 (Connection Pooling)](#连接池-connection-pooling)
    *   [异步操作 (Asynchronous Operations)](#异步操作-asynchronous-operations)
    *   [单元测试 (Unit Testing)](#单元测试-unit-testing)
    *   [错误处理和重试 (Error Handling and Retries)](#错误处理和重试-error-handling-and-retries)
7.  [贡献](#贡献)
8.  [许可证](#许可证)

## 特性

*   **简单易用的 API:** 方便地发送和接收消息的函数。
*   **基于 Redis:** 利用 Redis 存储和检索消息。
*   **阻塞接收:** 支持设置超时时间的阻塞接收消息。
*   **队列长度:** 获取当前消息队列的长度。
*   **轻量级:** 最小化的依赖和占用空间。

## 依赖

*   [hiredis](https://github.com/redis/hiredis): 一个极简的 Redis C 客户端库。
*   [CMake](https://cmake.org/):  跨平台的构建系统生成器。
*   (可选) [Google Test](https://github.com/google/googletest): 用于编写和运行单元测试 (当前由于构建问题被禁用，参见 [发展方向](#发展方向))。

## 构建和安装

按照以下说明在你的系统上构建和安装该库。

### Ubuntu

1.  **安装依赖:**

    ```bash
    sudo apt update
    sudo apt install libhiredis-dev cmake
    ```

2.  **克隆仓库:**

    ```bash
    git clone <repository_url>
    cd redis-queue-cpp
    ```

3.  **创建构建目录:**

    ```bash
    mkdir build
    cd build
    ```

4.  **使用 CMake 配置项目:**

    ```bash
    cmake ..
    ```

    如果 CMake 无法找到 `hiredis`，请确保 `libhiredis-dev` 已正确安装。如果问题仍然存在，你可能需要在 CMake 命令中手动指定 `hiredis` 的 include 和库文件目录。当前的 `CMakeLists.txt` 文件手动指定了这些路径，以避免配置问题。

5.  **构建项目:**

    ```bash
    make
    ```

6.  **运行可执行文件:**

    ```bash
    ./redis_queue_client
    ```

### Windows (MinGW)

1.  **安装 MSYS2:**

    *   从 [https://www.msys2.org/](https://www.msys2.org/) 下载并安装 MSYS2。
    *   打开 MSYS2 MinGW 64-bit 终端。

2.  **安装 MinGW-w64 工具链:**

    ```bash
    pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-gdb
    ```

3.  **安装 CMake:**

    *   从 [https://cmake.org/download/](https://cmake.org/download/) 下载并安装 CMake。
    *   确保在安装过程中将 CMake 添加到系统 PATH。

4.  **克隆仓库:**

    ```bash
    git clone <repository_url>
    cd redis-queue-cpp
    ```

5.  **创建构建目录:**

    ```bash
    mkdir build
    cd build
    ```

6.  **使用 CMake 配置项目:**

    ```bash
    cmake -G "MinGW Makefiles" ..
    ```

7.  **构建项目:**

    ```bash
    mingw32-make
    ```

8.  **运行可执行文件:**

    ```bash
    .\redis_queue_client.exe
    ```

## 使用方法

### 基本示例

以下代码演示了如何使用 `RedisQueueClient` 类发送和接收消息：

```c++
#include "RedisQueueClient.h"
#include <iostream>

int main() {
    try {
        RedisQueueClient client("localhost", 6379, "my_queue");

        // 发送消息
        client.sendMessage("消息 1");
        client.sendMessage("消息 2");

        // 接收消息
        std::string message1 = client.receiveMessage(5); // 5 秒超时
        if (!message1.empty()) {
            std::cout << "接收到的消息: " << message1 << std::endl;
        } else {
            std::cout << "在超时时间内没有收到消息。" << std::endl;
        }

        std::string message2 = client.receiveMessage(5); // 5 秒超时
        if (!message2.empty()) {
            std::cout << "接收到的消息: " << message2 << std::endl;
        } else {
            std::cout << "在超时时间内没有收到消息。" << std::endl;
        }

        // 获取队列长度
        long long queueLength = client.getQueueLength();
        std::cout << "队列长度: " << queueLength << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "异常: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
```

## 功能扩展

以下是一些扩展 Redis 消息队列客户端功能的想法：

### 消息确认 (Acknowledgement)

实现消息确认机制，以确保消息被成功处理。 这可能涉及到：

*   一个单独的 "确认" 队列，消费者在成功处理消息后将消息 ID 放入该队列。
*   生产者定期检查 "确认" 队列，并将已确认的消息从主队列中删除。
*   使用 Redis 事务实现原子性的消息处理和确认。

### 消息持久化 (Persistence)

使用 Redis 的持久化功能 (RDB 或 AOF) 来确保消息在服务器重启后仍然可用。 这可以通过配置 Redis 的持久化选项来实现。

### 消息优先级 (Prioritization)

允许生产者为消息分配优先级，并确保消费者按照优先级顺序接收消息。 这可以通过使用 Redis 的 Sorted Set 数据结构来实现。

### 发布/订阅 (Publish/Subscribe)

支持发布/订阅模式，允许生产者将消息发布到多个订阅者。 这可以通过使用 Redis 的 Pub/Sub 功能来实现。

## 发展方向

该项目的未来发展方向包括：

### 连接池 (Connection Pooling)

实现连接池以提高性能，避免频繁创建和销毁 Redis 连接。

### 异步操作 (Asynchronous Operations)

使用 `hiredis` 的异步 API 来实现非阻塞的消息发送和接收，提高程序的并发性能。

### 单元测试 (Unit Testing)

添加单元测试以验证代码的正确性。  由于目前构建系统中存在一些问题，单元测试功能被暂时禁用。  未来的工作将集中在解决这些构建问题，并启用单元测试。

### 错误处理和重试 (Error Handling and Retries)

改进错误处理机制，例如在连接断开后自动重试连接，以及在消息发送失败或消费失败时进行重试。

## 贡献

欢迎贡献！ 请提交包含清晰描述的 pull request。

## 许可证

该项目基于 MIT 许可证 - 请参阅 [LICENSE](LICENSE) 文件了解详情。