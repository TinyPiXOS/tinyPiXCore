cmake_minimum_required(VERSION 3.20.0)
project(TpLaunched VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../../../cmake/TargetArchConfig.cmake)

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/src/TpLaunched/main.cpp
)

include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/TpWM)
include_directories(${SOURCE_PATH}/include/TpCore)
include_directories(${SOURCE_PATH}/include/TpUtils)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/TpService)
include_directories(${SOURCE_PATH}/include_p/TpUtils)

# 指定依赖动态库目录
if(CMAKE_CROSSCOMPILING)
    link_directories("${LOAD_SDK_LIB_PATH}")
else()
    link_directories("/usr/lib")
endif()

# 添加动态库目标
add_executable(TpLaunched ${SOURCE_FILES})

# 指定静态库的路径
# 库链接到可执行目标
target_link_libraries(TpLaunched
    dl
    pthread
    # ${DEPEND_DYNAMIC_LIB_PATH}/libTpWMEngine.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpCore.so
    # ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpUtils.so
)
