cmake_minimum_required(VERSION 3.20.0)
project(TinyPiXOS VERSION 1.0.0 LANGUAGES C CXX)

# 不再提示未使用变量警告
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

# 添加SDK路径选项，支持命令行传入
set(DEFAULT_SDK_PATH "/home/hawk/Public/arm_64")
set(SDK_PATH ${DEFAULT_SDK_PATH})
set(ARM_SDK_PATH "${SDK_PATH}" CACHE PATH "交叉编译SDK路径")

# 显示使用的SDK路径
message(STATUS "使用 SDK 路径: ${ARM_SDK_PATH}")

# 引入cmake模块
include(cmake/GlobalConfig.cmake)
include(cmake/TargetArchConfig.cmake)
include(cmake/LoadCrossSDK.cmake)

# 加载SDK
LoadCrossSDK(${ARM_SDK_PATH})

# 清理并拷贝数据目录；头文件、数据文件等目录
add_custom_target(copyDependFiles ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${INSTALL_PACKAGE_ROOT_PATH}/include
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/include ${INSTALL_PACKAGE_ROOT_PATH}/include
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${INSTALL_PACKAGE_ROOT_PATH}/res
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/res ${INSTALL_PACKAGE_ROOT_PATH}/res
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${INSTALL_PACKAGE_ROOT_PATH}/data
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/data ${INSTALL_PACKAGE_ROOT_PATH}/data
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${INSTALL_PACKAGE_ROOT_PATH}/lib
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPEND_DYNAMIC_LIB_PATH} ${INSTALL_PACKAGE_ROOT_PATH}/lib
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${INSTALL_PACKAGE_ROOT_PATH}/../system
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/system ${INSTALL_PACKAGE_ROOT_PATH}/../system

    COMMAND ${CMAKE_COMMAND} -E remove_directory ${INSTALL_INCLUDE_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${INSTALL_RESOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${INSTALL_DATA_DIR}

    COMMENT "生成数据文件至 ${INSTALL_PACKAGE_ROOT_PATH}"
)

# 添加子目录
add_subdirectory(project/TpCore)
add_subdirectory(project/TpUtils)
add_subdirectory(project/TpExtUtils)
add_subdirectory(project/TpGUI)
add_subdirectory(project/TpWM)
add_subdirectory(project/TpService)
# add_subdirectory(examplesApp)

add_dependencies(TpCore copyDependFiles)
add_dependencies(TpUtils copyDependFiles)
add_dependencies(TpExtUtils copyDependFiles)
add_dependencies(TpGUI copyDependFiles)
add_dependencies(TpWM copyDependFiles)
add_dependencies(TpService copyDependFiles)

# 安装头文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ DESTINATION ${INSTALL_INCLUDE_DIR})
# # 安装资源文件到/usr/res/
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/res/ DESTINATION ${INSTALL_RESOURCE_DIR})
# # 安装数据文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/data/ DESTINATION ${INSTALL_DATA_DIR})
# # 安装TpWM的system文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/system/ DESTINATION ${INSTALL_SYSTEM_DIR})

# 修改bin下文件权限
install(CODE "
    # 权限修复（可选）
    execute_process(COMMAND chmod -R 0777 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    execute_process(COMMAND chmod -R 0777 ${INSTALL_BIN_DIR})
")

# 调安装所有动态库至安装目录，并在/usr/lib下创建软链接
# 构建拷贝命令
set(COPY_DEPEND_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_LIB_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_LIB_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${INSTALL_PACKAGE_ROOT_PATH}/lib ${INSTALL_LIB_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${INSTALL_PACKAGE_ROOT_PATH}/bin ${INSTALL_BIN_DIR}
)

# 创建自定义目标，执行拷贝和创建符号链接的命令
add_custom_target(create_symlinks
    COMMAND ${CMAKE_COMMAND} -E echo "开始处理依赖库与可执行程序..."
    ${COPY_DEPEND_COMMANDS}
    COMMAND ${CMAKE_COMMAND} -E echo "开始创建符号链接..."
    COMMAND ${CMAKE_COMMAND} 
        -DINSTALL_DIR=${INSTALL_LIB_DIR}
        -DLINK_DIR=${INSTALL_LIB_DIR}
        -P "${CMAKE_SOURCE_DIR}/cmake/CreateSymlink.cmake"
    COMMAND ${CMAKE_COMMAND} 
        -DINSTALL_DIR=${INSTALL_BIN_DIR}
        -DLINK_DIR=${LINK_BIN_DIR}
        -P "${CMAKE_SOURCE_DIR}/cmake/CreateSymlink.cmake"
    COMMENT "正在拷贝可执行程序到 ${INSTALL_BIN_DIR} 并创建符号链接到 ${LINK_BIN_DIR}"
    VERBATIM
)

# 添加install后执行的脚本
install(CODE "
    message(STATUS \"安装完成，开始设置依赖库...\")
    execute_process(
        COMMAND \${CMAKE_COMMAND} --build \${CMAKE_BINARY_DIR} --target create_symlinks
        WORKING_DIRECTORY \${CMAKE_BINARY_DIR}
    )
")

# 添加卸载目标
add_custom_target(uninstall
    COMMAND "${CMAKE_COMMAND}" 
    -DINSTALL_LIB_DIR=${INSTALL_LIB_DIR}
    -DINSTALL_INCLUDE_DIR=${INSTALL_INCLUDE_DIR}
    -DINSTALL_RESOURCE_DIR=${INSTALL_RESOURCE_DIR}
    -DINSTALL_DATA_DIR=${INSTALL_DATA_DIR}
    -DINSTALL_BIN_DIR=${INSTALL_BIN_DIR}
    -P "${CMAKE_SOURCE_DIR}/cmake/CmakeUninstall.cmake"
    COMMENT "Uninstalling"
)