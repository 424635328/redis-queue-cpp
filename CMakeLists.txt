cmake_minimum_required(VERSION 3.10)
project(redis-queue-cpp)

set(CMAKE_CXX_STANDARD 11)

# 查找 hiredis
find_package(Hiredis REQUIRED)
if (NOT Hiredis_FOUND)
    message(FATAL_ERROR "hiredis library not found. Please install it.")
endif()

# 头文件目录
include_directories(include)
include_directories(src) # 如果没有单独的 include 目录

# 添加可执行文件
add_executable(redis_queue_client src/main.cpp src/RedisQueueClient.cpp)

# 链接 hiredis
target_link_libraries(redis_queue_client Hiredis::hiredis)

# 添加测试 (可选)
add_subdirectory(test)