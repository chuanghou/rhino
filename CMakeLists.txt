cmake_minimum_required(VERSION 3.28.3)
project(rhino)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -mtune=native -flto -fno-rtti")

# 设定全局依赖目录，主要是第三方的手动复制的头文件，和自己的头文件，所以依赖目录设置为dep和src
list(APPEND GLOB_INCLUDE_DIRECTORY ${PROJECT_SOURCE_DIR}/dep ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/test)
file(GLOB_RECURSE COPY_DEP_H_FILES "dep/*.h")

list(APPEND COPY_SO_FILE_DIRS 
    dep/copy-dep-a-with-so/lib
    dep/copy-dep-b-with-so/lib
)

foreach (SO_FILE_DIR ${COPY_SO_FILE_DIRS})
    file(GLOB SO_FILES "${SO_FILE_DIR}/*.so*")
    foreach (SO_FILE ${SO_FILES})
        get_filename_component(SO_NAME ${SO_FILE} NAME)
        add_library(${SO_NAME} SHARED IMPORTED)
        set_target_properties(${SO_NAME} PROPERTIES IMPORTED_LOCATION ${SO_FILE})
        list(APPEND DEPS ${SO_NAME})
        list(APPEND RUNTIME_DEPS ${SO_NAME})
    endforeach ()
endforeach ()

# 多线程包，这个是操作系统本身提供的，只是为了编译通过，会在最后的编译指令里面增加 -lpthread
find_package(Threads REQUIRED)
set(DEPS Threads::Threads)


# Boost依赖
list(APPEND BOOST_RUNTIME_MODULES program_options filesystem system)
list(APPEND BOOST_MODULES headers ${BOOST_RUNTIME_MODULES})
find_package(Boost REQUIRED COMPONENTS ${BOOST_MODULES})
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
endif()
foreach(BOOST_MODULE ${BOOST_MODULES})
    list(APPEND DEPS Boost::${BOOST_MODULE})
endforeach()
foreach(BOOST_MODULE ${BOOST_RUNTIME_MODULES})
    list(APPEND RUNTIME_DEPS Boost::${BOOST_MODULE})
endforeach()


# FetchContent依赖
include(FetchDeps.cmake)
list(APPEND DEPS ${FETCH_DEPS})
list(APPEND TEST_DEPS ${TEST_FETCH_DEPS})
list(APPEND RUNTIME_DEPS ${RUNTIME_FETCH_DEPS})


# 项目主编译产物
file(GLOB_RECURSE SRC_FILES "src/*.cpp" "src/*.c++" "src/*.h")
add_executable(${PROJECT_NAME} ${SRC_FILES} ${COPY_DEP_H_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${GLOB_INCLUDE_DIRECTORY})
target_link_libraries(${PROJECT_NAME} ${DEPS})

# 测试配置及依赖
enable_testing()
file(GLOB_RECURSE TEST_FILES "src/*.cpp" "src/*.c++" "src/*.h")
set(TEST_TARGET ${PROJECT_NAME}_tests)
add_executable(${TEST_TARGET} ${SRC_FILES} ${TEST_FILES} ${COPY_DEP_H_FILES})
target_compile_definitions(${TEST_TARGET} PRIVATE COMPILE_WHEN_TEST)
target_link_libraries(${TEST_TARGET} PRIVATE ${DEPS} ${TEST_DEPS})
target_include_directories(${TEST_TARGET} PRIVATE ${GLOB_INCLUDE_DIRECTORY})
include(GoogleTest)
gtest_discover_tests(${TEST_TARGET})


# 安装部分
set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/target)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(DIRECTORY config DESTINATION ${CMAKE_INSTALL_PREFIX})
foreach (DEP ${RUNTIME_DEPS})
    install(CODE "
        file(INSTALL \"$<TARGET_FILE:${DEP}>\"
            DESTINATION \"\${CMAKE_INSTALL_PREFIX}/lib\"
            TYPE FILE
            FOLLOW_SYMLINK_CHAIN)
    ")
endforeach ()
