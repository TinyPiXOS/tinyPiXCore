cmake_minimum_required(VERSION 3.20.0)
project(PiXService VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../cmake/CreateTinyCoreSymlink.cmake)

# 添加源码目录
set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/src/PiXService/main.cpp
    ${SOURCE_PATH}/src/PiXService/gatewayServer.cpp
)

include_directories(/usr/include/PiXWM)
include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/Utils)
include_directories(${SOURCE_PATH}/include/ExternUtils)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/Api)
include_directories(${SOURCE_PATH}/include_p/Utils)

# 添加动态库目标
add_executable(PiXService ${SOURCE_FILES})

# 指定依赖动态库目录
link_directories("/usr/lib")

# 直接指定静态库的路径
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86|i686|i386|AMD64")
    set(NANOMSG_STATIC_LIB ${SOURCE_PATH}/depend_lib/static/x86_x64/libnanomsg.a)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
    set(NANOMSG_STATIC_LIB ${SOURCE_PATH}/depend_lib/static/arm_64/libnanomsg.a)
endif()

# 库链接到可执行目标
target_link_libraries(PiXService
    ${NANOMSG_STATIC_LIB}
    dl
    pthread
    PiXApi
    PiXUtils
    PiXExternUtils
)

# 如果你需要将动态库安装到特定的目录，可以修改DESTINATION路径
install(TARGETS PiXService RUNTIME DESTINATION ${INSTALL_BIN_DIR})

# 创建软链接
cmake_create_core_symlink(
    TARGET PiXService
    INSTALL_DIR ${INSTALL_BIN_DIR}
    LINK_DIR ${LINK_BIN_DIR}             
)
