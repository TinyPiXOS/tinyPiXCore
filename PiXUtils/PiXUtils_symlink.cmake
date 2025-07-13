
        # 在安装时计算实际文件名
        set(target_file "/usr/lib/tinyPiX/libPiXUtils.so")
        
        # 计算链接名（支持普通字符串和生成器表达式）
        if("" STREQUAL "")
            set(link_name "libPiXUtils.so")
        else()
            set(link_name "")
        endif()
        
        # 创建链接完整路径
        set(link_path "/usr/lib/${link_name}")
        
        # 删除旧链接（如果存在）
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E rm -f "${link_path}"
            RESULT_VARIABLE rm_result
        )
        
        # 创建新链接
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E create_symlink "${target_file}" "${link_path}"
            RESULT_VARIABLE link_result
        )
        
        # 结果处理
        if(link_result EQUAL 0)
            message(STATUS "Created symlink: ${link_path} -> ${target_file}")
        else()
            message(WARNING "Failed to create symlink ${link_path}: error ${link_result}")
        endif()
    