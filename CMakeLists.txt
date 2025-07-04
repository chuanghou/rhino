cmake_minimum_required(VERSION 3.28.3)
project(rhino)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# inc 为项目对外提供的的include文件
include_directories(inc src)

find_package(Boost REQUIRED filesystem system)
find_package(Threads REQUIRED)
find_package(CapnProto REQUIRED)


# 包含 FetchContent 模块
include(FetchContent)

# 拉取测试
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://gitee.com/mirrors/googletest.git
        GIT_TAG        v1.17.0
)

# 使gtest可用
FetchContent_MakeAvailable(googletest)

# 启用测试
enable_testing()

# 添加你的可执行文件
add_executable(testDemo test/TestDemo.cpp)

# 正确链接 GTest 的目标
target_link_libraries(testDemo PRIVATE
        GTest::gtest
        GTest::gmock
        GTest::gtest_main
        Threads::Threads
        Boost::boost
        Boost::filesystem
        Boost::system
        CapnProto::kj
        CapnProto::capnp)

# 添加测试
include(GoogleTest)

# 发现测试
gtest_discover_tests(testDemo)

# TODO 集成 https://github.com/yhirose/cpp-httplib

