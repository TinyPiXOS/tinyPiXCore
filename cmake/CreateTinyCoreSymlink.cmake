# tinyPiXCore 生成库创建软链接
# CreateSymlink.cmake - 可重用的软链接创建模块
#
# 用法:
#   cmake_create_core_symlink(TARGET <target> 
#       [INSTALL_DIR <install_dir>]
#       [LINK_DIR <link_dir>]
#       [LINK_NAME <link_name>]
#   )
#
# 参数:
#   TARGET        CMake目标名
#   INSTALL_DIR   库安装目录 (默认: /usr/lib/tinyPiX)
#   LINK_DIR      软链接创建目录 (默认: /usr/lib)
#   LINK_NAME     自定义链接名 (默认为目标文件名)

function(cmake_create_core_symlink)
    # 解析参数
    set(options)
    set(oneValueArgs TARGET INSTALL_DIR LINK_DIR LINK_NAME)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "" ${ARGN})

    # 检查必填参数
    if(NOT ARG_TARGET)
        message(FATAL_ERROR "TARGET argument is required")
    endif()

    # 设置默认值
    if(NOT ARG_INSTALL_DIR)
        set(ARG_INSTALL_DIR "/usr/lib/tinyPiX")
    endif()

    if(NOT ARG_LINK_DIR)
        set(ARG_LINK_DIR "/usr/lib")
    endif()

    # 设置链接名称
    if(ARG_LINK_NAME)
        set(link_name "${ARG_LINK_NAME}")
    else()
        set(link_name "$<TARGET_FILE_NAME:${ARG_TARGET}>")
    endif()

        # 关键修复：使用 FILE(GENERATE) 预解析生成器表达式
    set(config_file "${CMAKE_CURRENT_BINARY_DIR}/${ARG_TARGET}_symlink.cmake")
    
    file(GENERATE OUTPUT ${config_file} CONTENT "
        # 在安装时计算实际文件名
        set(target_file \"${ARG_INSTALL_DIR}/$<TARGET_FILE_NAME:${ARG_TARGET}>\")
        
        # 计算链接名（支持普通字符串和生成器表达式）
        if(\"${ARG_LINK_NAME}\" STREQUAL \"\")
            set(link_name \"$<TARGET_FILE_NAME:${ARG_TARGET}>\")
        else()
            set(link_name \"${ARG_LINK_NAME}\")
        endif()
        
        # 创建链接完整路径
        set(link_path \"${ARG_LINK_DIR}/\${link_name}\")
        
        # 删除旧链接（如果存在）
        execute_process(
            COMMAND \${CMAKE_COMMAND} -E rm -f \"\${link_path}\"
            RESULT_VARIABLE rm_result
        )
        
        # 创建新链接
        execute_process(
            COMMAND \${CMAKE_COMMAND} -E create_symlink \"\${target_file}\" \"\${link_path}\"
            RESULT_VARIABLE link_result
        )
        
        # 结果处理
        if(link_result EQUAL 0)
            message(STATUS \"Created symlink: \${link_path} -> \${target_file}\")
        else()
            message(WARNING \"Failed to create symlink \${link_path}: error \${link_result}\")
        endif()
    ")
    
    # 使用生成的脚本文件
    install(SCRIPT ${config_file})
    
    # # 创建软链接的安装脚本
    # install(CODE "
    #     # 获取目标文件完整路径
    #     set(target_file \"${ARG_INSTALL_DIR}/$<TARGET_FILE_NAME:${ARG_TARGET}>\")
        
    #     # 创建软链接完整路径
    #     set(link_path \"${ARG_LINK_DIR}/${link_name}\")
        
    #     # 删除旧链接（如果存在）
    #     execute_process(
    #         COMMAND \${CMAKE_COMMAND} -E rm -f \"\${link_path}\"
    #         RESULT_VARIABLE rm_result
    #     )
        
    #     # 创建新链接
    #     execute_process(
    #         COMMAND \${CMAKE_COMMAND} -E create_symlink \"\${target_file}\" \"\${link_path}\"
    #         RESULT_VARIABLE link_result
    #         OUTPUT_VARIABLE link_output
    #         ERROR_VARIABLE link_error
    #     )
        
    #     # 错误处理
    #     if(NOT link_result EQUAL 0)
    #         message(WARNING \"Failed to create symlink \${link_path}: \${link_error}\")
    #     else()
    #         message(STATUS \"Created symlink: \${link_path} -> \${target_file}\")
    #     endif()
    # ")
endfunction()