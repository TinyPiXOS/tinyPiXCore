cmake_minimum_required(VERSION 3.20.0)
project(TpDesktopLib VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加源码目录
set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/TpAppManager.cpp
    ${SOURCE_PATH}/TpSplashScreen.cpp
    ${SOURCE_PATH}/TpWidgetsGateway.cpp

    ${SOURCE_PATH}/ServiceRPC/c_TpAppManager_client.cpp
    ${SOURCE_PATH}/ServiceRPC/TpAppManager_client.cpp
    ${SOURCE_PATH}/ServiceRPC/TpAppManager_interface.cpp
)

# 添加包含目录
include_directories(${SYSTEM_INCLUDE_PATH})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include/Service)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include/InteractDataDef)

include_directories(${CMAKE_SOURCE_DIR}/src/include)
include_directories(${CMAKE_SOURCE_DIR}/src/include/TpCore)
include_directories(${CMAKE_SOURCE_DIR}/src/include/TpWM)
include_directories(${CMAKE_SOURCE_DIR}/src/include/TpService)
include_directories(${CMAKE_SOURCE_DIR}/src/include/TpUtils)
include_directories(${CMAKE_SOURCE_DIR}/src/include/TpGUI)
include_directories(${CMAKE_SOURCE_DIR}/src/include/TpGUI/GUI)
include_directories(${CMAKE_SOURCE_DIR}/src/include/TpGUI/Widgets)
include_directories(${CMAKE_SOURCE_DIR}/src/include_p)
include_directories(${CMAKE_SOURCE_DIR}/src/include_p/TpService)
include_directories(${CMAKE_SOURCE_DIR}/src/include_p/TpUtils)
include_directories(${CMAKE_SOURCE_DIR}/src/include_p/TpUtils/erpc)
include_directories(${CMAKE_SOURCE_DIR}/src/include_p/TpCore)
include_directories(${CMAKE_SOURCE_DIR}/src/include_p/TpGUI)
include_directories(${CMAKE_SOURCE_DIR}/src/include_p/TpGUI/GUI)
include_directories(${CMAKE_SOURCE_DIR}/src/src_p/TpService/ServiceRPC)

# 添加动态库目标
add_library(TpDesktopLib SHARED ${SOURCE_FILES})

# 将动态库的输出名称设置为 "TpService"
set_target_properties(TpDesktopLib PROPERTIES OUTPUT_NAME "TpDesktop")

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

target_link_libraries(TpDesktopLib PUBLIC
    dl pthread 
    ${ERPC_STATIC_LIB}
    ${DEPEND_DYNAMIC_LIB_PATH}/libTpWMEngine.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpCore.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpGUI.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpUtils.so
)
