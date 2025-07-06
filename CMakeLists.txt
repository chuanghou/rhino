cmake_minimum_required(VERSION 3.24.0)
project(rhino)

# 3.24之后机制
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/cache/fetch" CACHE PATH "下载缓存目录")
set(FETCHCONTENT_BINARY_DIR "${FETCHCONTENT_BASE_DIR}/build" CACHE PATH "依赖项构建目录")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# inc 为项目对外提供的的include文件
include_directories(inc src)

find_package(Boost REQUIRED filesystem system)
find_package(Threads REQUIRED)

# 包含 FetchContent 模块
include(FetchContent)

# 拉取测试
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://gitee.com/mirrors/googletest.git
        GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(googletest)


# 拉取测试
FetchContent_Declare(
        capnproto
        GIT_REPOSITORY https://gitee.com/mirrors/capnproto.git
        GIT_TAG        v1.2.0
)
FetchContent_MakeAvailable(capnproto)

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

