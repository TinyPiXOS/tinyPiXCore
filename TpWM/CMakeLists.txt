cmake_minimum_required(VERSION 3.20.0)
project(TpWM VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(../cmake/TargetArchConfig.cmake)
include(../cmake/GlobalConfig.cmake)
include(../cmake/CreateSymlink.cmake)

# 添加源码目录
set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/src/TpWM/main.c
)

include_directories(/usr/include/SDL2)
include_directories(${SOURCE_PATH}/include/TpWM)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/TpWMEngine)
include_directories(${SOURCE_PATH}/include_p/TpWMEngine/engine)
include_directories(${SOURCE_PATH}/include_p/TpWMEngine/base)

add_executable(TpWM ${SOURCE_FILES})

# 添加宏定义
target_compile_definitions(TpWM PRIVATE TP_USE_SDL2 ENABLE_LOGO)

# 设置动态库版本 VERSION 是库的当前版本号，而 SOVERSION 是API的兼容性版本号。
# set_target_properties(TpWM PROPERTIES VERSION 1.0 SOVERSION 1)

# 指定依赖动态库目录
link_directories("/usr/lib")

# 库链接到可执行目标
target_link_libraries(TpWM
    SDL2
    pthread
    ${DEPEND_DYNAMIC_LIB_PATH}/libTpWMEngine.so
)

# 如果你需要将动态库安装到特定的目录，可以修改DESTINATION路径
install(TARGETS TpWM RUNTIME DESTINATION ${INSTALL_BIN_DIR})

# 创建软链接
CmakeCreateSymlink(
    TARGET TpWM
    INSTALL_DIR ${INSTALL_BIN_DIR}
    LINK_DIR ${LINK_BIN_DIR}             
)