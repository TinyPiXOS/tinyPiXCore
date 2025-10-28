cmake_minimum_required(VERSION 3.20.0)
project(TpService VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../cmake/CreateSymlink.cmake)
include(../cmake/TargetArchConfig.cmake)

# 添加源码目录
set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/src/TpService/main.cpp
    ${SOURCE_PATH}/src/TpService/TpGatewayServer.cpp
)

include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/TpWM)
include_directories(${SOURCE_PATH}/include/TpUtils)
include_directories(${SOURCE_PATH}/include/TpExtUtils)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/TpUtils)

# 添加动态库目标
add_executable(TpService ${SOURCE_FILES})

# 指定依赖动态库目录
link_directories("/usr/lib")

# 指定静态库的路径
set(NANOMSG_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libnanomsg.a)

# 库链接到可执行目标
target_link_libraries(TpService
    ${NANOMSG_STATIC_LIB}
    dl
    pthread
    ${DEPEND_DYNAMIC_LIB_PATH}/libTpWMEngine.so
    TpGUI
    TpUtils
    TpExtUtils
)

# 如果你需要将动态库安装到特定的目录，可以修改DESTINATION路径
install(TARGETS TpService RUNTIME DESTINATION ${INSTALL_BIN_DIR})

# 创建软链接
CmakeCreateSymlink(
    TARGET TpService
    INSTALL_DIR ${INSTALL_BIN_DIR}
    LINK_DIR ${LINK_BIN_DIR}             
)
