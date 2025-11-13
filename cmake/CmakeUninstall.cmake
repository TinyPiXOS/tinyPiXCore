# 外部输入安装目录：
#           ${INSTALL_LIB_DIR}
#           ${INSTALL_INCLUDE_DIR}
#           ${INSTALL_RESOURCE_DIR}
#           ${INSTALL_DATA_DIR}
#           ${INSTALL_BIN_DIR}

message(STATUS "删除生成库: ${INSTALL_LIB_DIR}")
message(STATUS "删除头文件: ${INSTALL_INCLUDE_DIR}")
message(STATUS "删除资源文件: ${INSTALL_RESOURCE_DIR}")
message(STATUS "删除资源文件: ${INSTALL_DATA_DIR}")
message(STATUS "删除资源文件: ${INSTALL_BIN_DIR}")

execute_process(
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_LIB_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_INCLUDE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_RESOURCE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_DATA_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_BIN_DIR}"

    # 移除软链接
    COMMAND ${CMAKE_COMMAND} -E remove "/usr/bin/TpService"
    COMMAND ${CMAKE_COMMAND} -E remove "/usr/bin/TpWM"
    RESULT_VARIABLE removeLibResult
)

if(NOT removeLibResult EQUAL 0)
    message(WARNING "删除安装文件失败")
endif()
