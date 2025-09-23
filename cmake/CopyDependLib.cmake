# 拷贝依赖动态库至安装目录；并在/usr/lib下创建软链接

function(setup_dependency_libraries
    DEPEND_LIB_PATH # 依赖库的搜索路径
    INSTALL_LIB_DIR # 安装库的目标目录（拷贝.so文件到此）
    LINK_LIB_DIR # 创建符号链接的目录（通常为/usr/lib或类似系统库目录）
    TARGET_LIST # 需要依赖此操作的目标列表（例如TpUtils TpExtUtils等）
)
    # 查找所有已安装的.so文件（用于后续创建符号链接）
    file(GLOB_RECURSE INSTALL_SO_FILES "${INSTALL_LIB_DIR}/*.so*")

    # 查找所有依赖的.so文件（用于拷贝）
    file(GLOB_RECURSE DEPEND_SO_FILES "${DEPEND_LIB_PATH}/*.so*")

    # 初始化命令列表：先清空目标目录，然后重新创建
    set(COPY_DEPEND_COMMANDS)
    list(APPEND COPY_DEPEND_COMMANDS
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_LIB_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_LIB_DIR}"
    )

    # 遍历所有依赖库文件，生成拷贝命令
    foreach(SO_FILE IN LISTS DEPEND_SO_FILES)
        message(STATUS "拷贝依赖库: ===== ${SO_FILE} =====")
        list(APPEND COPY_DEPEND_COMMANDS
            COMMAND ${CMAKE_COMMAND} -E copy
            "${SO_FILE}"
            "${INSTALL_LIB_DIR}"
        )
    endforeach()

    # 初始化创建符号链接的命令列表
    set(CREATE_LINK_COMMANDS)

    foreach(SO_FILE IN LISTS INSTALL_SO_FILES)
        # 提取文件名（不带路径）
        get_filename_component(FILENAME "${SO_FILE}" NAME)

        # 初始化变量
        set(BASE_NAME_WITHOUT_ANY_VERSION "") # 不带任何版本号的名称
        set(BASE_NAME_WITH_MAJOR_VERSION "") # 只带主版本号的名称

        # 匹配带完整版本号的库文件 (lib.so.x.y.z)
        if("${FILENAME}" MATCHES "^(.*)\\.so\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)$")
            set(LIB_BASE "${CMAKE_MATCH_1}")
            set(MAJOR_VERSION "${CMAKE_MATCH_2}")
            set(MINOR_VERSION "${CMAKE_MATCH_3}")
            set(PATCH_VERSION "${CMAKE_MATCH_4}")

            set(BASE_NAME_WITHOUT_ANY_VERSION "${LIB_BASE}.so")
            set(BASE_NAME_WITH_MAJOR_VERSION "${LIB_BASE}.so.${MAJOR_VERSION}")

        # 匹配带主次版本号的库文件 (lib.so.x.y)
        elseif("${FILENAME}" MATCHES "^(.*)\\.so\\.([0-9]+)\\.([0-9]+)$")
            set(LIB_BASE "${CMAKE_MATCH_1}")
            set(MAJOR_VERSION "${CMAKE_MATCH_2}")
            set(MINOR_VERSION "${CMAKE_MATCH_3}")

            set(BASE_NAME_WITHOUT_ANY_VERSION "${LIB_BASE}.so")
            set(BASE_NAME_WITH_MAJOR_VERSION "${LIB_BASE}.so.${MAJOR_VERSION}")

        # 匹配只带主版本号的库文件 (lib.so.x)
        elseif("${FILENAME}" MATCHES "^(.*)\\.so\\.([0-9]+)$")
            set(LIB_BASE "${CMAKE_MATCH_1}")
            set(MAJOR_VERSION "${CMAKE_MATCH_2}")

            set(BASE_NAME_WITHOUT_ANY_VERSION "${LIB_BASE}.so")
            set(BASE_NAME_WITH_MAJOR_VERSION "${FILENAME}") # 已经是主版本号形式

        # 处理没有版本号后缀的文件
        else()
            set(BASE_NAME_WITHOUT_ANY_VERSION "${FILENAME}")
            set(BASE_NAME_WITH_MAJOR_VERSION "${FILENAME}") # 没有版本则两者相同
        endif()

        # 为库文件创建符号链接
        if("${BASE_NAME_WITH_MAJOR_VERSION}" STREQUAL "${BASE_NAME_WITHOUT_ANY_VERSION}")
            list(APPEND CREATE_LINK_COMMANDS
                COMMAND ${CMAKE_COMMAND} -E create_symlink
                "${SO_FILE}"
                "${LINK_LIB_DIR}/${BASE_NAME_WITHOUT_ANY_VERSION}"
            )
        else()
            # 创建两个符号链接：一个带主版本号，一个不带任何版本号
            list(APPEND CREATE_LINK_COMMANDS
                COMMAND ${CMAKE_COMMAND} -E create_symlink
                "${SO_FILE}"
                "${LINK_LIB_DIR}/${BASE_NAME_WITHOUT_ANY_VERSION}"
            )
            list(APPEND CREATE_LINK_COMMANDS
                COMMAND ${CMAKE_COMMAND} -E create_symlink
                "${SO_FILE}"
                "${LINK_LIB_DIR}/${BASE_NAME_WITH_MAJOR_VERSION}"
            )
        endif()
    endforeach()

    # 创建自定义目标，执行拷贝和创建符号链接的命令
    add_custom_target(create_symlinks
        COMMAND ${CMAKE_COMMAND} -E echo "开始处理依赖库..."
        ${COPY_DEPEND_COMMANDS}
        COMMAND ${CMAKE_COMMAND} -E echo "开始创建符号链接..."
        ${CREATE_LINK_COMMANDS}
        COMMENT "正在拷贝依赖库文件到 ${INSTALL_LIB_DIR} 并创建符号链接到 ${LINK_LIB_DIR}"
        VERBATIM
    )

    # 使指定的目标依赖于此自定义目标
    foreach(TARGET_NAME IN LISTS TARGET_LIST)
        if(TARGET ${TARGET_NAME})
            add_dependencies(${TARGET_NAME} create_symlinks)
        else()
            message(WARNING "目标 ${TARGET_NAME} 不存在，无法添加依赖关系")
        endif()
    endforeach()
endfunction()