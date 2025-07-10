cmake_minimum_required(VERSION 3.28.3)
project(rhino)

# 3.24之后可以用，直接用Linux控制台可以，直接在windows用clion兼容有问题
# 这两个变量主要是构建目录如果删除之后，可以保留之前下载的记录，其实这个类似maven的本地仓库能力
# 这两个变量直接在linux上运行没问题，不过在clion内部添加，就不能用了，clion自己抛出了异常
# 其实不加这两个也问题大，从git上拉代码一般也很快，每次重新拉也ok一般也不会删除构建目录
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


# 这个不知道怎么回事，并不会把_deps里面的代码从linux服务器全部拉到windows本地
# 导致本地有些代码变红，不影响编译
# 可以手动复制进去本地的目录，应该也是clion的bug之类的，或者capnproto cmake 搞得不太好
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


FetchContent_Declare(
        POCO
        GIT_REPOSITORY https://gitee.com/mirrors/POCO.git
        GIT_TAG        poco-1.14.2-release
)
FetchContent_MakeAvailable(POCO)

# 启用测试
enable_testing()

set(MIDDLE_DEPS Threads::Threads
        Boost::boost
        Boost::filesystem
        Boost::system
        CapnProto::kj
        CapnProto::capnp
        nlohmann_json::nlohmann_json
        Poco::Foundation)

set(GTEST_DEPS GTest::gtest GTest::gmock GTest::gtest_main)

add_executable(TestDemo test/TestDemo.cpp
        src/util/json.h
        src/util/time.h
        src/util/string.h)
target_link_libraries(TestDemo PRIVATE ${MIDDLE_DEPS} ${GTEST_DEPS})

add_executable(WorkStation test/WorkStation.cpp)
target_link_libraries(WorkStation PRIVATE ${GTEST_DEPS})


add_executable(NlohmannTest test/NlohmannTest.cpp)
target_link_libraries(NlohmannTest PRIVATE ${GTEST_DEPS} nlohmann_json::nlohmann_json)

# 添加测试
include(GoogleTest)

gtest_discover_tests(TestDemo NlohmannTest WorkStation)

