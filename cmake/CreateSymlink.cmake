# 将安装目录的库创建软链接
# file(GLOB INSTALLED_SO_FILES "${INSTALL_DIR}/*.so*")
file(GLOB INSTALLED_SO_FILES "${INSTALL_DIR}/*")
foreach(SO_FILE IN LISTS INSTALLED_SO_FILES)
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
        # 没有版本号则直接返回不处理
        set(BASE_NAME_WITHOUT_ANY_VERSION "${FILENAME}")
        set(BASE_NAME_WITH_MAJOR_VERSION "${FILENAME}") # 没有版本则两者相同
    endif()

    # 创建无版本号的软链接
    # 先删除已存在的符号链接（如果存在）
    if(EXISTS "${LINK_DIR}/${BASE_NAME_WITHOUT_ANY_VERSION}" AND 
            IS_SYMLINK "${LINK_DIR}/${BASE_NAME_WITHOUT_ANY_VERSION}")
    
        message(STATUS "删除软链接: ${LINK_DIR}/${BASE_NAME_WITHOUT_ANY_VERSION}")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E remove "${LINK_DIR}/${BASE_NAME_WITHOUT_ANY_VERSION}"
            RESULT_VARIABLE remove_result
        )

        if(NOT remove_result EQUAL 0)
            message(WARNING "删除软链接失败: ${remove_result}")
        endif()

    endif()

    if(NOT "${SO_FILE}" STREQUAL "${LINK_DIR}/${BASE_NAME_WITHOUT_ANY_VERSION}")

        message(STATUS "创建符号链接: ${BASE_NAME_WITHOUT_ANY_VERSION}")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E create_symlink
                "${SO_FILE}"
                "${LINK_DIR}/${BASE_NAME_WITHOUT_ANY_VERSION}"
            RESULT_VARIABLE result
        )
        if(NOT result EQUAL 0)
            message(WARNING "创建符号链接失败: ${result}")
        endif()
    endif()

    if(NOT "${SO_FILE}" STREQUAL "${LINK_DIR}/${BASE_NAME_WITH_MAJOR_VERSION}"
        AND NOT "${BASE_NAME_WITHOUT_ANY_VERSION}" STREQUAL "${BASE_NAME_WITH_MAJOR_VERSION}")

        message(STATUS "创建符号链接: ${BASE_NAME_WITH_MAJOR_VERSION}")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E create_symlink
                "${SO_FILE}"
                "${LINK_DIR}/${BASE_NAME_WITH_MAJOR_VERSION}"
            RESULT_VARIABLE result
        )
        if(NOT result EQUAL 0)
            message(WARNING "创建符号链接失败: ${result}")
        endif()
    endif()

endforeach()