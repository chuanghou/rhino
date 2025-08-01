cmake_minimum_required(VERSION 3.28.3)
project(rhino)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -mtune=native -flto -fno-rtti")

# 3.11之后可以用，直接用Linux控制台可以，直接在windows用clion兼容有问题
# 这变量主要是构建目录如果删除之后，可以保留之前下载的记录，其实这个类似maven的本地仓库能力
# 这变量直接在linux上运行没问题，不过在clion内部添加，就不能用了，clion自己抛出了异常
# 其实不加这两个也问题大，从git上拉代码一般也很快，每次重新拉也ok一般也不会删除构建目录

#set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/.cache/fetch")

# 设定全局依赖目录，主要是第三方的手动复制的头文件，和自己的头文件，所以依赖目录设置为dep/inc和src
list(APPEND GLOB_INCLUDE_DIRECTORY ${PROJECT_SOURCE_DIR}/dep/inc ${PROJECT_SOURCE_DIR}/src)

# 复制copy进来的so包，使用imported方式进来，添加进入DEPS变量（全局依赖列表），RUNTIMES_DEPS(运行时依赖，需要打包进入产物的）
#add_library(abc SHARED IMPORTED)
#set_target_properties(abc PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/dep/lib/libabc.so)
#list(APPEND DEPS abc)
#list(APPEND RUNTIME_DEPS abc)

# 多线程包，这个是操作系统本身提供的，只是为了编译通过，会在最后的编译指令里面增加 -lpthread
find_package(Threads REQUIRED)
set(DEPS Threads::Threads)

# 这个查找过程会产生4个目标 Boost::boost Boost::headers
# 其中Boost::boost 和 Boost::headers 都是纯头文件依赖，其中Boost::boost是为了兼容过去的代码，内部其实link到Boost::headers里面
# 可以看FindBoost.cmake源码验证
# 同时还提供了Boost_INCLUDE_DIRS和Boost_LIBRARIES两个依赖对过去兼容

list(APPEND BOOST_MODULES program_options filesystem system)
message("BOOST_MODULES IS ${BOOST_MODULES}")
find_package(Boost REQUIRED COMPONENTS ${BOOST_MODULES})
if(Boost_FOUND)
    # 可以看出来Boost::boost 依赖了 Boost::headers
    get_target_property(Boost_boost_INTERFACE_LINK_LIBRARIES Boost::boost INTERFACE_LINK_LIBRARIES)
    message(STATUS "Boost_boost_INTERFACE_LINK_LIBRARIES is ${Boost_boost_INTERFACE_LINK_LIBRARIES}")
    # 可以看出来Boost::headers 是真正纯头文件依赖目标
    get_target_property(Boost_headers_INTERFACE_INCLUDE_DIRECTORIES Boost::headers INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "Boost_headers_INTERFACE_INCLUDE_DIRECTORIES is ${Boost_headers_INTERFACE_INCLUDE_DIRECTORIES}")
else()
    message(FATAL_ERROR "Boost not found!")
endif()
foreach (MODULE ${BOOST_MODULES})
    list(APPEND DEPS Boost::${MODULE})
    list(APPEND RUNTIME_DEPS Boost::${MODULE})
endforeach ()

# 包含 FetchContent 模块
include(FetchContent)

FetchContent_Declare(
        capnproto
        GIT_REPOSITORY https://gitee.com/mirrors/capnproto.git
        GIT_TAG        v1.2.0
)
FetchContent_GetProperties(capnproto)
if (NOT capnproto_POPULATED)
    FetchContent_Populate(capnproto)
    add_subdirectory(${capnproto_SOURCE_DIR} ${capnproto_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()
list(APPEND DEPS CapnProto::kj CapnProto::capnp)

FetchContent_Declare(
        nlohmann-json
        GIT_REPOSITORY https://gitee.com/mirrors/nlohmann-json.git
        GIT_TAG        v3.12.0
)
FetchContent_MakeAvailable(nlohmann-json)
list(APPEND DEPS nlohmann_json::nlohmann_json)

FetchContent_Declare(
        poco
        GIT_REPOSITORY https://gitee.com/mirrors/POCO.git
        GIT_TAG        poco-1.14.2-release
)
FetchContent_GetProperties(poco)
if (NOT poco_POPULATED)
    FetchContent_Populate(poco)
    add_subdirectory(${poco_SOURCE_DIR} ${poco_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()
list(APPEND DEPS Poco::Foundation)
list(APPEND RUNTIME_DEPS Poco::Foundation)

file(GLOB_RECURSE cpp_files "src/*.cpp")
# for capnproto
file(GLOB_RECURSE c++_files "src/*.c++")
file(GLOB_RECURSE h_files "src/.h")

add_executable(${PROJECT_NAME} ${cpp_files} ${c++_files} ${h_files})
target_include_directories(${PROJECT_NAME} PUBLIC ${GLOB_INCLUDE_DIRECTORY})
target_link_libraries(${PROJECT_NAME} ${DEPS})

# GTEST
enable_testing()
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://gitee.com/mirrors/googletest.git
        GIT_TAG        v1.17.0
)
# 使用FetchContent_MakeAvailable，会触发自动安装，有两种方案关闭，如下链接解释了，如何关闭自动安装
# https://stackoverflow.com/questions/65527126/disable-install-for-fetchcontent
#FetchContent_MakeAvailable(googletest)
FetchContent_GetProperties(googletest)
if (NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()
list(APPEND GTEST_DEPS GTest::gtest GTest::gmock GTest::gmock_main)

file(GLOB_RECURSE test_cpp_files "test/*.cpp")
# for capnproto
file(GLOB_RECURSE test_c++_files "test/*.c++")
file(GLOB_RECURSE test_h_files "test/.h")

add_executable(GTESTS ${cpp_files} ${c++_files} ${h_files} ${test_cpp_files} ${test_c++_files} ${test_h_files})
target_compile_definitions(GTESTS PRIVATE GTESTS)
target_link_libraries(GTESTS PRIVATE ${DEPS} ${GTEST_DEPS})
target_include_directories(GTESTS PRIVATE ${GLOB_INCLUDE_DIRECTORY})
include(GoogleTest)
gtest_discover_tests(GTESTS)

#INSTALL
set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/target)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(DIRECTORY config DESTINATION ${CMAKE_INSTALL_PREFIX})

foreach (DEP IN LISTS RUNTIME_DEPS)
    install(CODE "
        file(INSTALL \"$<TARGET_FILE:${DEP}>\"
            DESTINATION \"\${CMAKE_INSTALL_PREFIX}/lib\"
            TYPE FILE
            FOLLOW_SYMLINK_CHAIN)
    ")
endforeach ()

