cmake_minimum_required(VERSION 3.10)
project(redis-queue-cpp)

set(CMAKE_CXX_STANDARD 11)

#  手动设置 hiredis 的 include 目录和库文件
set(HIREDIS_INCLUDE_DIR /usr/include/hiredis)
set(HIREDIS_LIBRARY /usr/lib/x86_64-linux-gnu/libhiredis.so)

# 包含目录
include_directories(include)
include_directories(src)
include_directories(${HIREDIS_INCLUDE_DIR})

# 注释掉 find_package
# set(Hiredis_FIND_VERSION_EXACT TRUE)
# find_package(Hiredis REQUIRED VERSION 0.14.1)

# 添加可执行文件
add_executable(redis_queue_client src/main.cpp src/RedisQueueClient.cpp)

#  手动链接 hiredis 库
target_link_libraries(redis_queue_client ${HIREDIS_LIBRARY})

# 移除测试目录
add_subdirectory(test)