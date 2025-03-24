cmake_minimum_required(VERSION 3.22.1)

find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})
include_directories(../include)
include_directories(../src)

# 手动添加 hiredis 的 include 目录
include_directories(/usr/include/hiredis) # 确保这个路径正确

enable_testing()

add_executable(RedisQueueClient_test RedisQueueClient_test.cpp ../src/RedisQueueClient.cpp)

# 手动链接 hiredis 库
target_link_libraries(RedisQueueClient_test GTest::gtest_main /usr/lib/x86_64-linux-gnu/libhiredis.so) # 确保这个路径正确

include(GoogleTest)
gtest_discover_tests(RedisQueueClient_test)