cmake_minimum_required(VERSION 3.28.3)
project(rhino)

# 3.24之后可以用，直接用Linux控制台可以，直接在windows用clion兼容有问题
#set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/.cache/fetch")
#set(FETCHCONTENT_BINARY_DIR "${CMAKE_SOURCE_DIR}/.cache/fetch/build")

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


FetchContent_Declare(
        nlohmann-json
        GIT_REPOSITORY https://gitee.com/mirrors/nlohmann-json.git
        GIT_TAG        v3.12.0
)
FetchContent_MakeAvailable(nlohmann-json)

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
        CapnProto::capnp
        nlohmann_json::nlohmann_json)

# 添加测试
include(GoogleTest)

# 发现测试
gtest_discover_tests(testDemo)

# TODO 集成 https://github.com/yhirose/cpp-httplib

