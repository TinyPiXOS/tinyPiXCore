cmake_minimum_required(VERSION 3.20.0)
project(TpCore VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../../cmake/TargetArchConfig.cmake)

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/src/TpCore/TpCoreApp.cpp
    ${SOURCE_PATH}/src/TpCore/TpObject.cpp
    ${SOURCE_PATH}/src/TpCore/TpRect.cpp
    ${SOURCE_PATH}/src/TpCore/TpRectF.cpp
    ${SOURCE_PATH}/src/TpCore/TpPoint.cpp
    ${SOURCE_PATH}/src/TpCore/TpPointF.cpp
    ${SOURCE_PATH}/src/TpCore/TpSize.cpp
    ${SOURCE_PATH}/src/TpCore/TpSizeF.cpp
    ${SOURCE_PATH}/src/TpCore/TpRange.cpp
    ${SOURCE_PATH}/src/TpCore/TpVariant.cpp
    ${SOURCE_PATH}/src/TpCore/TpMap.cpp
    ${SOURCE_PATH}/src/TpCore/TpString.cpp
    ${SOURCE_PATH}/src/TpCore/TpVector.cpp
    ${SOURCE_PATH}/src/TpCore/TpStack.cpp
    ${SOURCE_PATH}/src/TpCore/TpRegex.cpp
    ${SOURCE_PATH}/src/TpCore/TpTimer.cpp
    ${SOURCE_PATH}/src_p/TpCore/TpAutoObject.cpp
    ${SOURCE_PATH}/src_p/TpCore/TpMessage.cpp
)

include_directories(${SYSTEM_INCLUDE_PATH})

include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/TpCore)
include_directories(${SOURCE_PATH}/include/TpWM)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/TpCore)

# 添加动态库目标
add_library(TpCore SHARED ${SOURCE_FILES})

# 设置动态库版本 VERSION 是库的当前版本号，而 SOVERSION 是API的兼容性版本号。
# set_target_properties(TpGUI PROPERTIES VERSION 1.0 SOVERSION 1)

# 指定依赖动态库目录
if(CMAKE_CROSSCOMPILING)
    link_directories("${LOAD_SDK_LIB_PATH}")
else()
    link_directories("/usr/lib")
endif()

LoadSDKLibrary(SSL_LIBRARY ssl)

# 库链接到可执行目标
target_link_libraries(TpCore PUBLIC
    dl pthread
    ${SSL_LIBRARY}
)
