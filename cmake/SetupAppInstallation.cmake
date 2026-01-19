# cmake/SetupAppInstallation.cmake

function(setup_app_installation PROJECT_UUID)

    # set(APPLICATION_UUID "4a1a4cb1-5d7b-952f-9ddd-416857b44e84")

    # 设置安装路径变量
    set(INSTALL_ROOT_DIR "/System/app/${PROJECT_UUID}")
    set(INSTALL_BIN_DIR "${INSTALL_ROOT_DIR}/bin")
    set(INSTALL_RESOURCE_DIR "${INSTALL_ROOT_DIR}/res")
    set(INSTALL_CONFIG_DIR "${INSTALL_ROOT_DIR}/conf")
    set(INSTALL_DATA_DIR "${INSTALL_ROOT_DIR}/data")
    
    # 平台不同拷贝资源文件到不同安装目录
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86|i686|i386|AMD64")
        set(INSTALL_PACKAGE_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../install/x86_64/app/${PROJECT_UUID}")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
        set(INSTALL_PACKAGE_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../install/arm_64/app/${PROJECT_UUID}")
    endif()

    # 设置安装目录
    set(INSTALL_PACKAGE_BIN_DIR "${INSTALL_PACKAGE_ROOT_PATH}/bin")
    set(INSTALL_PACKAGE_RESOURCE_DIR "${INSTALL_PACKAGE_ROOT_PATH}/res")
    set(INSTALL_PACKAGE_CONFIG_DIR "${INSTALL_PACKAGE_ROOT_PATH}/conf")
    set(INSTALL_PACKAGE_DATA_DIR "${INSTALL_PACKAGE_ROOT_PATH}/data")

    # 示例：创建安装目录
    # file(REMOVE_RECURSE "${INSTALL_ROOT_DIR}")
    # file(MAKE_DIRECTORY "${INSTALL_ROOT_DIR}")

    # file(REMOVE_RECURSE "${INSTALL_PACKAGE_ROOT_PATH}")
    # file(MAKE_DIRECTORY "${INSTALL_PACKAGE_ROOT_PATH}")

    # 创建一个虚拟目标用于执行自定义命令，清理安装目录
    add_custom_target(clean${PROJECT_UUID}Install
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_ROOT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_ROOT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_PACKAGE_ROOT_PATH}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_PACKAGE_ROOT_PATH}"
        COMMENT "清理应用 ${PROJECT_UUID} 安装目录"
    )

    # 将清理目标添加为全局安装的依赖
    install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} --build . --target clean${PROJECT_UUID}Install)")

    # 安装所有文件到系统目录
    # 安装所有文件到tinyPiXApp安装目录
    # 安装可执行文件目录
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bin/)
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/ DESTINATION ${INSTALL_BIN_DIR})
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/ DESTINATION ${INSTALL_PACKAGE_BIN_DIR})
    else()
        message(STATUS "${PROJECT_UUID} /bin 文件夹不存在!")
    endif()

    # 安装资源文件
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/res/)
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/res/ DESTINATION ${INSTALL_RESOURCE_DIR})
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/res/ DESTINATION ${INSTALL_PACKAGE_RESOURCE_DIR})
    else()
        message(STATUS "${PROJECT_UUID} /res 文件夹不存在!")
    endif()

    # 安装配置文件
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/conf/)
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/conf/ DESTINATION ${INSTALL_CONFIG_DIR})
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/conf/ DESTINATION ${INSTALL_PACKAGE_CONFIG_DIR})
    else()
        message(STATUS "${PROJECT_UUID} /conf 文件夹不存在!")
    endif()

    # 安装数据文件
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/data/)
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/ DESTINATION ${INSTALL_DATA_DIR})
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/ DESTINATION ${INSTALL_PACKAGE_DATA_DIR})
    else()
        message(STATUS "${PROJECT_UUID} /data 文件夹不存在!")
    endif()

    # 安装应用运行配置文件Json
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_UUID}.json)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_UUID}.json DESTINATION ${INSTALL_RUN_APP_CONFIG_DIR})
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_UUID}.json DESTINATION ${INSTALL_PACKAGE_ROOT_PATH}/../../conf/app)
    else()
        message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_UUID}.json 应用配置Json不存在!")
    endif()
    
    # 修改安装指令（保持原始功能，仅追加新操作）
    install(CODE "
        # 权限修复（可选）
        execute_process(COMMAND chmod -R 0777 ${INSTALL_BIN_DIR})
        execute_process(COMMAND chmod -R 0777 ${INSTALL_PACKAGE_BIN_DIR})
    ")

    # 全局安装路径标记
    # set_property(GLOBAL PROPERTY APP_INSTALL_INITIALIZED TRUE)
endfunction()