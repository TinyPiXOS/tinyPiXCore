cmake_minimum_required(VERSION 3.20.0)
project(TinyPiXDesktop VERSION 1.0.0 LANGUAGES C CXX)

# 引入cmake模块
include(../cmake/TargetArchConfig.cmake)

# 设置构建生成的根目录
set(CMAKE_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")

# 设置输出目录（根据架构和构建类型）
set(DESKTOP_INSTALL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/install/${PLATFORM_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DESKTOP_INSTALL_PATH}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DESKTOP_INSTALL_PATH}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DESKTOP_INSTALL_PATH}/lib)

# 清理并拷贝数据目录；头文件、数据文件等目录
add_custom_target(copyDeskInstallFiles ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${DESKTOP_INSTALL_PATH}/res
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${DESKTOP_INSTALL_PATH}/conf
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${DESKTOP_INSTALL_PATH}/data
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${DESKTOP_INSTALL_PATH}/include

    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res/ ${DESKTOP_INSTALL_PATH}/res
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/conf/ ${DESKTOP_INSTALL_PATH}/conf
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data/ ${DESKTOP_INSTALL_PATH}/data
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/include/ ${DESKTOP_INSTALL_PATH}/include
)

# 添加子目录
add_subdirectory(src/app)
add_subdirectory(src/lib)

add_dependencies(TpDesktop TpDesktopLib)
add_dependencies(TpDesktopLib copyDeskInstallFiles)
add_dependencies(TpDesktop copyDeskInstallFiles)
