cmake_minimum_required(VERSION 3.10)

find_package(Hiredis REQUIRED) # 确保找到 hiredis
find_package(GTest REQUIRED)   # 查找 Google Test (你需要先安装 Google Test)

include_directories(${GTEST_INCLUDE_DIRS})
include_directories(../include) # 添加 include 目录
include_directories(../src) # 如果没有单独的 include 目录

enable_testing()

add_executable(RedisQueueClient_test RedisQueueClient_test.cpp ../src/RedisQueueClient.cpp)
target_link_libraries(RedisQueueClient_test GTest::gtest_main Hiredis::hiredis)

include(GoogleTest)
gtest_discover_tests(RedisQueueClient_test) #自动发现 test cases