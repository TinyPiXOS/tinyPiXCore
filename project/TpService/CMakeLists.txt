cmake_minimum_required(VERSION 3.20.0)
project(TpService VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../../cmake/TargetArchConfig.cmake)

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/src/TpService/main.cpp
    ${SOURCE_PATH}/src/TpService/TpGatewayServerPUBSUB.cpp
    ${SOURCE_PATH}/src_p/TpUtils/Service/c_TpSystemApi_server.cpp
    ${SOURCE_PATH}/src_p/TpUtils/Service/TpSystemApi_server.cpp
    ${SOURCE_PATH}/src_p/TpUtils/Service/TpSystemApi_interface.cpp
)

include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/TpCore)
include_directories(${SOURCE_PATH}/include/TpWM)
include_directories(${SOURCE_PATH}/include/TpUtils)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/TpUtils)
include_directories(${SOURCE_PATH}/include_p/TpUtils/erpc)
include_directories(${SOURCE_PATH}/src_p/TpUtils/Service)

# 指定依赖动态库目录
if(CMAKE_CROSSCOMPILING)
    link_directories("${LOAD_SDK_LIB_PATH}")
else()
    link_directories("/usr/lib")
endif()

# 添加动态库目标
add_executable(TpService ${SOURCE_FILES})

# 指定静态库的路径
set(NANOMSG_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libnanomsg.a)
set(ERPC_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/liberpc.a)

find_library(GLOG_LIBRARY glog PATHS ${ARM_SDK_PATH}/lib)
find_library(YAML_CPP_LIBRARY yaml-cpp PATHS ${ARM_SDK_PATH}/lib)
find_library(LEVELDB_LIBRARY leveldb PATHS ${ARM_SDK_PATH}/lib)
find_library(MARISA_LIBRARY marisa PATHS ${ARM_SDK_PATH}/lib)
find_library(OPENCC_LIBRARY opencc PATHS ${ARM_SDK_PATH}/lib)
# find_library(CRYPTO_LIBRARY
#     NAMES crypto3
#     HINTS ${ARM_SDK_PATH}/lib  # 指定基础路径
#     PATH_SUFFIXES openssl3     # 自动添加子目录后缀
#     NO_DEFAULT_PATH           # 可选：避免搜索系统路径
# )
# find_library(OPENSSL_LIBRARY
#     NAMES ssl3
#     HINTS ${ARM_SDK_PATH}/lib
#     PATH_SUFFIXES openssl3
#     NO_DEFAULT_PATH
# )
# 库链接到可执行目标
target_link_libraries(TpService
    dl
    pthread
    ${NANOMSG_STATIC_LIB} ${ERPC_STATIC_LIB}
    ${DEPEND_DYNAMIC_LIB_PATH}/librime.so.1.5.3
    ${GLOG_LIBRARY}
    ${YAML_CPP_LIBRARY}
    ${LEVELDB_LIBRARY}
    ${MARISA_LIBRARY}
    ${OPENCC_LIBRARY}
    ${DEPEND_DYNAMIC_LIB_PATH}/libTpWMEngine.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpCore.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpUtils.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpGUI.so
)
