cmake_minimum_required(VERSION 3.20.0)
project(TinyPiXDesktop VERSION 1.0.0 LANGUAGES C CXX)

# 引入cmake模块
include(../cmake/TargetArchConfig.cmake)

# 设置构建生成的根目录
set(CMAKE_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")

# 设置输出目录（根据架构和构建类型）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/install/${PLATFORM_NAME}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/install/${PLATFORM_NAME}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/install/${PLATFORM_NAME}/lib)

# 添加子目录
add_subdirectory(src/app)
add_subdirectory(src/lib)

add_dependencies(TpDesktop TpDesktopLib)
