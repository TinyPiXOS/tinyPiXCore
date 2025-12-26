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
add_subdirectory(src)
add_subdirectory(desktopApp)

add_dependencies(TpDesktop TpDesktopLib)
add_dependencies(TpDesktopLib copyDeskInstallFiles)
add_dependencies(TpDesktop copyDeskInstallFiles)

# 调安装所有动态库至安装目录，并在/usr/lib下创建软链接
# 构建拷贝命令
set(COPY_DESKTOP_DEPEND_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_DESKTOP_LIB_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_DESKTOP_APP_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_DESKTOP_INCLUDE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_DESKTOP_LIB_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_DESKTOP_APP_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_DESKTOP_INCLUDE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${INSTALL_DESKTOP_LIB_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DESKTOP_INSTALL_PATH}/include ${INSTALL_DESKTOP_INCLUDE_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DESKTOP_INSTALL_PATH}/bin ${INSTALL_DESKTOP_APP_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DESKTOP_INSTALL_PATH}/conf ${INSTALL_DESKTOP_APP_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DESKTOP_INSTALL_PATH}/data ${INSTALL_DESKTOP_APP_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DESKTOP_INSTALL_PATH}/res ${INSTALL_DESKTOP_APP_DIR}
)

# 创建自定义目标，执行拷贝和创建符号链接的命令
add_custom_target(create_desktop_symlinks
    COMMAND ${CMAKE_COMMAND} -E echo "开始处理依赖库与可执行程序..."
    ${COPY_DESKTOP_DEPEND_COMMANDS}
    COMMAND ${CMAKE_COMMAND} -E echo "开始创建符号链接..."
    COMMAND ${CMAKE_COMMAND} 
        -DINSTALL_DIR=${INSTALL_DESKTOP_LIB_DIR}
        -DLINK_DIR=${INSTALL_DESKTOP_LIB_DIR}
        -P "${CMAKE_SOURCE_DIR}/cmake/CreateSymlink.cmake"
    COMMENT "正在拷贝桌面生成库到 ${INSTALL_DESKTOP_LIB_DIR} 并创建符号链接到 ${INSTALL_DESKTOP_LIB_DIR}"
    VERBATIM
)

# 添加install后执行的脚本
install(CODE "
    message(STATUS \"安装完成，开始设置依赖库...\")
    execute_process(
        COMMAND \${CMAKE_COMMAND} --build \${CMAKE_BINARY_DIR} --target create_desktop_symlinks
        WORKING_DIRECTORY \${CMAKE_BINARY_DIR}
    )
")