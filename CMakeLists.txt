cmake_minimum_required(VERSION 3.17.5)
project(rhino)

# 启用 C++11 或更高标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含 FetchContent 模块
include(FetchContent)

# 拉取测试
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://gitee.com/mirrors/googletest.git
        GIT_TAG        release-1.10.0
)

# 使gtest可用
FetchContent_MakeAvailable(googletest)

# 启用测试
enable_testing()

# 添加你的可执行文件
add_executable(testDemo test/TestDemo.cpp)

# 正确链接 GTest 的目标
target_link_libraries(testDemo PRIVATE gtest_main gmock_main)

# 添加测试
include(GoogleTest)

# 发现测试
gtest_discover_tests(testDemo)

