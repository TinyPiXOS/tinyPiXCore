cmake_minimum_required(VERSION 3.20.0)
project(TpWM VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(../../cmake/TargetArchConfig.cmake)
include(../../cmake/GlobalConfig.cmake)
include(../../cmake/LoadCrossSDK.cmake)

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/src/TpWM/main.c
)

include_directories(${SYSTEM_INCLUDE_PATH})
include_directories(${SYSTEM_INCLUDE_PATH}/SDL2)
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
if(CMAKE_CROSSCOMPILING)
    link_directories("${LOAD_SDK_LIB_PATH}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath-link,${LOAD_SDK_LIB_PATH}")
else()
    link_directories("/usr/lib")
endif()

LoadSDKLibrary(SDL2_LIBRARY SDL2)

# 库链接到可执行目标
target_link_libraries(TpWM
	m
    pthread
    ${SDL2_LIBRARY}
    ${DEPEND_DYNAMIC_LIB_PATH}/libTpWMEngine.so
)
