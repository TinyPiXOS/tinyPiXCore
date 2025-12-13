cmake_minimum_required(VERSION 3.20.0)
project(TpServiceLib VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../../../cmake/TargetArchConfig.cmake)

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/src/TpService/lib/TpAppManager.cpp
    ${SOURCE_PATH}/src/TpService/lib/TpSplashScreen.cpp
    ${SOURCE_PATH}/src/TpService/lib/TpWidgetsGateway.cpp

    ${SOURCE_PATH}/src_p/TpService/c_TpAppManager_client.cpp
    ${SOURCE_PATH}/src_p/TpService/TpAppManager_client.cpp
    ${SOURCE_PATH}/src_p/TpService/TpAppManager_interface.cpp
)

# 添加包含目录
include_directories(${SYSTEM_INCLUDE_PATH})

include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/TpCore)
include_directories(${SOURCE_PATH}/include/TpWM)
include_directories(${SOURCE_PATH}/include/TpService)
include_directories(${SOURCE_PATH}/include/TpUtils)
include_directories(${SOURCE_PATH}/include/TpGUI)
include_directories(${SOURCE_PATH}/include/TpGUI/GUI)
include_directories(${SOURCE_PATH}/include/TpGUI/Widgets)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/TpService)
include_directories(${SOURCE_PATH}/include_p/TpUtils)
include_directories(${SOURCE_PATH}/include_p/TpUtils/erpc)
include_directories(${SOURCE_PATH}/include_p/TpCore)
include_directories(${SOURCE_PATH}/include_p/TpGUI)
include_directories(${SOURCE_PATH}/include_p/TpGUI/GUI)
include_directories(${SOURCE_PATH}/src_p/TpService)

# 添加动态库目标
add_library(TpServiceLib SHARED ${SOURCE_FILES})

# 将动态库的输出名称设置为 "TpService"
set_target_properties(TpServiceLib PROPERTIES OUTPUT_NAME "TpService")

# 设置动态库版本 VERSION 是库的当前版本号，而 SOVERSION 是API的兼容性版本号。
# set_target_properties(TpService PROPERTIES VERSION 1.0 SOVERSION 1)

# 指定依赖动态库目录
if(CMAKE_CROSSCOMPILING)
    link_directories("${LOAD_SDK_LIB_PATH}")
else()
    link_directories("/usr/lib")
endif()

# 依赖的静态库
set(ERPC_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/liberpc.a)
set(THORVG_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libthorvg.a)

target_link_libraries(TpServiceLib PUBLIC
    dl pthread 
    ${ERPC_STATIC_LIB}
    ${DEPEND_DYNAMIC_LIB_PATH}/libTpWMEngine.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpCore.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpGUI.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpUtils.so
)
