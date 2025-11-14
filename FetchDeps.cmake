# FetchDeps.cmake，应该被include进入主CmakeLists.txt,
# 所有通过FetchContent模块产生的依赖，都要写到这个文件里面
# 这个文件会通过设置FETCHCONTENT_BASE_DIR，达到复用效果，即一个项目不用重复从其他git仓库反复拉依赖，拉一次存储在.cache/fetch目录下
# 这个文件会产生单个依赖列表
# FETCH_DEPS 这个是可执行文件的主依赖，主要是编译主文件的时候使用，任何编译主产物需要使用的依赖都需要加入
# RUNTIME_FETCH_DEPS 这是需要打包so包的通过fetch得到的依赖, 最终生产上线打包产物中需要加入的so包的依赖，需要加入
# TEST_FETCH_DEPS 这里主要是一些通过FETCH操作得到的测试过程中中的依赖，测试过程中需要的依赖

# 需要注意到，capnproto，gtest，poco，Fetch脚本并不是通用看到的脚本，主要原因是，使用
# 通用的Fetch指令，FetchContent_MakeAvailable(googletest), 就是你能在各个仓库里面看到他们给出的指令，
# 会在install的时候把这些依赖编译产生的so包都依赖进去, 所以需要关闭所有FetchContent自己的install部分
# 使用FetchContent_MakeAvailable，会触发自动安装，有两种方案关闭，如下链接解释了，如何关闭自动安装
# https://stackoverflow.com/questions/65527126/disable-install-for-fetchcontent


# 3.11之后可以用，直接用Linux控制台可以，直接在windows用clion兼容有问题, vscode 因为是直接在linux服务器操作所以也没问题
#set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/.cache/fetch")

# 包含 FetchContent 模块
include(FetchContent)

macro(fetch_dep repo tag)
    get_filename_component(name "${repo}" NAME_WE)
    string(TOLOWER "${name}" name)
    FetchContent_Declare(
            ${name}
            GIT_REPOSITORY ${repo}
            GIT_TAG        ${tag}
    )
    FetchContent_GetProperties(${name})
    if(NOT ${name}_POPULATED)
        FetchContent_Populate(${name})
        add_subdirectory(${${name}_SOURCE_DIR} ${${name}_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
endmacro()

#fetch_dep(poco https://gitee.com/mirrors/POCO.git poco-1.14.2-release)
# list(APPEND FETCH_DEPS Poco::Foundation)
# list(APPEND RUNTIME_FETCH_DEPS Poco::Foundation)


