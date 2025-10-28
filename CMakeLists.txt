cmake_minimum_required(VERSION 3.20.0)
project(TinyPiXOS VERSION 1.0.0 LANGUAGES C CXX)

# 定义选项控制是否构建子项目
option(BUILD_SUBPROJECTS "Build subprojects from source" OFF)
option(FORCE_REBUILD_SUBPROJECTS "Force rebuild subprojects" OFF)

# 引入cmake模块
include(cmake/GlobalConfig.cmake)
include(cmake/TargetArchConfig.cmake)
include(cmake/CopyDependLib.cmake)

# 添加子目录
add_subdirectory(TpWM)
add_subdirectory(TpUtils)
add_subdirectory(TpExtUtils)
add_subdirectory(TpGUI)
add_subdirectory(TpService)

# add_subdirectory(examplesApp)

# 创建一个虚拟目标用于执行自定义命令，清理安装目录
add_custom_target(clean_install_directory
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_INCLUDE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_INCLUDE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_RESOURCE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_RESOURCE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_DATA_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_DATA_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_BIN_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_BIN_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_PACKAGE_ROOT_PATH}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_PACKAGE_ROOT_PATH}"
    COMMENT "Cleaning install directory for headers and res"
)

# 将清理目标添加为全局安装的依赖
# install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} --build . --target clean_install_directory)")

# 安装整个头文件目录到/usr/include
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ DESTINATION ${INSTALL_INCLUDE_DIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/include DESTINATION ${INSTALL_PACKAGE_ROOT_PATH})

# 安装资源文件到/usr/res/
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/res/ DESTINATION ${INSTALL_RESOURCE_DIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/res DESTINATION ${INSTALL_PACKAGE_ROOT_PATH})

# 安装数据文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/data/ DESTINATION ${INSTALL_DATA_DIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/data DESTINATION ${INSTALL_PACKAGE_ROOT_PATH})

# 安装bin文件
# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/bin/ DESTINATION ${INSTALL_BIN_DIR})
# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/bin DESTINATION ${INSTALL_PACKAGE_ROOT_PATH})

# 安装TpWM的system文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/system/ DESTINATION ${INSTALL_SYSTEM_DIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/system/ DESTINATION ${INSTALL_PACKAGE_ROOT_PATH}/../system)

# 修改bin下文件权限
install(CODE "
    # 权限修复（可选）
    execute_process(COMMAND chmod -R 0777 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    execute_process(COMMAND chmod -R 0777 ${INSTALL_BIN_DIR})
")

# 需要依赖此操作的目标列表
set(TARGET_LIST TpUtils TpExtUtils TpGUI TpService)

# 调用函数;将公共依赖动态库拷贝至指定目录
setup_dependency_libraries(
    "${DEPEND_DYNAMIC_LIB_PATH}"
    "${INSTALL_LIB_DIR}"
    "${LINK_LIB_DIR}"
    "${TARGET_LIST}"
)