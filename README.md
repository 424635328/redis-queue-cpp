# Redis 消息队列客户端 (C++)

一个轻量级的、基于 Redis 的消息队列客户端，使用 C++ 和 hiredis 实现。

## 目录

1.  [特性](#特性)
2.  [依赖](#依赖)
3.  [构建和安装](#构建和安装)
    *   [通用构建步骤](#通用构建步骤)
    *   [Ubuntu](#ubuntu)
    *   [Windows (MinGW)](#windows-mingw)
    *   [macOS](#macos)
    *   [常见构建问题及解决方案](#常见构建问题及解决方案)
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
*   **支持优先级队列:** 可以发送带有优先级的消息，优先级高的消息会先被消费。
*   **支持延迟队列:** 可以发送延迟消息，指定消息在未来某个时间点才能被消费。

## 依赖

*   [hiredis](https://github.com/redis/hiredis): 一个极简的 Redis C 客户端库。
*   [CMake](https://cmake.org/):  跨平台的构建系统生成器。
*   (可选) [Google Test](https://github.com/google/googletest): 用于编写和运行单元测试。

## 构建和安装

以下说明指导你在不同系统上构建和安装该库。

### 通用构建步骤

1.  **安装依赖:** 确保安装了所有必要的依赖项（hiredis、CMake）。
2.  **克隆仓库:**
    ```bash
    git clone https://github.com/424635328/redis-queue-cpp
    cd redis-queue-cpp
    ```
3.  **创建构建目录:**
    ```bash
    mkdir build
    cd build
    ```
4.  **生成构建文件:** 使用 CMake 生成对应平台的构建文件（例如，Makefile、Visual Studio 项目等）。
5.  **构建项目:** 使用平台对应的构建工具（例如，make、msbuild 等）构建项目。
6.  **运行可执行文件:** 构建成功后，运行生成的可执行文件。

### Ubuntu

1.  **安装依赖:**
    ```bash
    sudo apt update
    sudo apt install libhiredis-dev cmake build-essential
    ```
2.  **配置项目:**
    ```bash
    cmake ..
    ```
3.  **构建项目:**
    ```bash
    make
    ```
4.  **运行示例程序 (可选):**
    ```bash
    ./redis_queue_client
    ```
5. **运行测试 (可选):**
   ```bash
   ./test/RedisQueueClient_test
   ```

### Windows (MinGW)

1.  **安装 MSYS2:**
    *   从 [https://www.msys2.org/](https://www.msys2.org/) 下载并安装 MSYS2。
    *   打开 MSYS2 MinGW 64-bit 终端。
2.  **安装 MinGW-w64 工具链:**
    ```bash
    pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-gdb mingw-w64-x86_64-cmake
    ```
3.  **安装 hiredis:**
        ```bash
        pacman -S mingw-w64-x86_64-hiredis
        ```
4.  **克隆仓库 (如果还没有克隆):**
    ```bash
    git clone <repository_url>
    cd redis-queue-cpp
    ```
5.  **创建构建目录:**
    ```bash
    mkdir build
    cd build
    ```
6.  **配置项目:**
    ```bash
    cmake -G "MinGW Makefiles" ..
    ```

7.  **构建项目:**
    ```bash
    mingw32-make
    ```
8.  **运行示例程序 (可选):**
    ```bash
    ./redis_queue_client.exe
    ```

9. **运行测试 (可选):**
   ```bash
   ./test/RedisQueueClient_test.exe
   ```

### macOS

1.  **安装 Homebrew (如果尚未安装):**
    ```bash
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    ```
2.  **安装依赖:**
    ```bash
    brew update
    brew install hiredis cmake
    ```
3.  **克隆仓库 (如果还没有克隆):**
    ```bash
    git clone <repository_url>
    cd redis-queue-cpp
    ```
4.  **创建构建目录:**
    ```bash
    mkdir build
    cd build
    ```
5.  **配置项目:**
    ```bash
    cmake ..
    ```
6.  **构建项目:**
    ```bash
    make
    ```
7. **运行示例程序 (可选):**
    ```bash
    ./redis_queue_client
    ```
8. **运行测试 (可选):**
   ```bash
   ./test/RedisQueueClient_test
   ```

### 常见构建问题及解决方案

*   **找不到 `hiredis`:**
    *   **问题:** CMake 无法找到 `hiredis` 库。
    *   **解决方案:**
        *   确保 `hiredis` 已正确安装（使用包管理器或手动构建）。
        *   在 CMake 命令中手动指定 `hiredis` 的 include 和库文件目录:
          ```cmake
          cmake -DHIREDIS_INCLUDE_DIR=/path/to/hiredis/include -DHIREDIS_LIBRARY=/path/to/hiredis/lib/libhiredis.a ..
          ```
        * 对于MSYS2 MinGW，确保hiredis是通过pacman安装的，并且使用MinGW 64-bit 终端。

*   **链接错误 (undefined reference):**
    *   **问题:** 链接器无法找到 `hiredis` 函数的定义。
    *   **解决方案:**
        *   确认 CMake 正确地链接了 `hiredis` 库。  检查 `target_link_libraries` 命令是否包含 `hiredis`。
        *  确保你的 CMakeLists.txt 中 `-DHiredis_FOUND` 为 TRUE

*   **编译错误 (C++ 标准):**
    *   **问题:**  编译过程中出现与 C++ 标准相关的错误。
    *   **解决方案:**
        *   在 `CMakeLists.txt` 中设置 C++ 标准：
          ```cmake
          set(CMAKE_CXX_STANDARD 11) # 或者更高版本，例如 14, 17, 20
          set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
          ```
*   **Windows 下编译问题:**
        * 确保使用 MinGW 64-bit 终端进行编译。
        * 检查 MinGW 的 bin 目录是否已添加到系统 PATH 环境变量中。
*   **测试用例运行失败:**
       * 确保 Redis 服务器正在运行，并且监听在 localhost:6379。
       * 检查测试用例使用的队列名称是否与你的配置匹配。

## 使用方法

### 基本示例

以下代码演示了如何使用 `RedisQueueClient` 类发送和接收消息：

```c++
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
```

## 功能扩展

以下是一些扩展 Redis 消息队列客户端功能的想法：

### 消息确认 (Acknowledgement)

实现消息确认机制，以确保消息被成功处理。 可能涉及到：

*   一个单独的 "确认" 队列，消费者在成功处理消息后将消息 ID 放入该队列。
*   生产者定期检查 "确认" 队列，并将已确认的消息从主队列中删除。
*   使用 Redis 事务实现原子性的消息处理和确认。

### 消息持久化 (Persistence)

使用 Redis 的持久化功能 (RDB 或 AOF) 来确保消息在服务器重启后仍然可用。 可以通过配置 Redis 的持久化选项来实现。

### 消息优先级 (Prioritization)

允许生产者为消息分配优先级，并确保消费者按照优先级顺序接收消息。 可以通过使用 Redis 的 Sorted Set 数据结构来实现。

### 发布/订阅 (Publish/Subscribe)

支持发布/订阅模式，允许生产者将消息发布到多个订阅者。 可以通过使用 Redis 的 Pub/Sub 功能来实现。

## 发展方向

该项目的未来发展方向包括：

### 连接池 (Connection Pooling)

实现连接池以提高性能，避免频繁创建和销毁 Redis 连接。

### 异步操作 (Asynchronous Operations)

使用 `hiredis` 的异步 API 来实现非阻塞的消息发送和接收，提高程序的并发性能。

### 单元测试 (Unit Testing)

添加单元测试以验证代码的正确性。  建议使用 Google Test 框架。

### 错误处理和重试 (Error Handling and Retries)

改进错误处理机制，例如在连接断开后自动重试连接，以及在消息发送失败或消费失败时进行重试。

## 贡献

欢迎贡献！ 请提交包含清晰描述的 pull request。

## 许可证

该项目基于 MIT 许可证 - 请参阅 [LICENSE](LICENSE) 文件了解详情。
