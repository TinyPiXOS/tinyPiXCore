# 指定SDK目录
function(LoadCrossSDK
    SDK_PATH                    # SDK根目录
)
    # 默认包含目录
    set(SYSTEM_INCLUDE_PATH "/usr/include" PARENT_SCOPE)
    
    if(CMAKE_CROSSCOMPILING)
        # 验证SDK路径是否存在
        if(NOT EXISTS ${SDK_PATH})
            message(WARNING "SDK路径不存在: ${SDK_PATH}")
            return()
        endif()

        # 设置全局变量，使用PARENT_SCOPE让变量在函数外部可见
        set(LOAD_SDK_LIB_PATH "${SDK_PATH}/lib" PARENT_SCOPE)
        set(LOAD_SDK_INCLUDE_PATH "${SDK_PATH}/include" PARENT_SCOPE)
        set(SYSTEM_INCLUDE_PATH "${SDK_PATH}/include" PARENT_SCOPE)
        message(STATUS "已加载交叉编译 SDK: ${SDK_PATH}")
    else()
        message(STATUS "非交叉编译模式, SDK 未加载")
    endif()
endfunction()

# 加载指定动态库；必须在加载SDK目录之后使用
function(LoadSDKLibrary
    VAR_NAME                    # 库自定义名称
    LIB_NAME                    # 库名
)
    if(CMAKE_CROSSCOMPILING)
        # 检查是否已设置SDK路径
        if(NOT DEFINED LOAD_SDK_LIB_PATH)
            message(FATAL_ERROR "请先调用 LoadCrossSDK 设置 SDK 路径")
        endif()

        find_library(TEMP_${VAR_NAME}
            NAMES ${LIB_NAME}
            PATHS ${LOAD_SDK_LIB_PATH}
            NO_DEFAULT_PATH
            NO_CMAKE_FIND_ROOT_PATH
        )

        # 将结果设置到父作用域变量
        if(TEMP_${VAR_NAME})
            set(${VAR_NAME} ${TEMP_${VAR_NAME}} PARENT_SCOPE)
            message(STATUS "加载 ${LIB_NAME} 库: ${TEMP_${VAR_NAME}}")
        else()
            # 尝试在系统路径中查找作为fallback
            find_library(TEMP_${VAR_NAME} ${LIB_NAME})
            if(TEMP_${VAR_NAME})
                set(${VAR_NAME} ${TEMP_${VAR_NAME}} PARENT_SCOPE)
                message(STATUS "在系统路径找到 ${LIB_NAME} 库: ${TEMP_${VAR_NAME}}")
            else()
                message(FATAL_ERROR "${LIB_NAME} 库未找到, 请检查SDK路径: ${LOAD_SDK_LIB_PATH}")
            endif()
        endif()
    else()
       # 本地编译使用默认查找
        find_library(TEMP_${VAR_NAME} ${LIB_NAME})
        if(TEMP_${VAR_NAME})
            set(${VAR_NAME} ${TEMP_${VAR_NAME}} PARENT_SCOPE)
            message(STATUS "本地编译找到 ${LIB_NAME} 库: ${TEMP_${VAR_NAME}}")
        else()
            message(FATAL_ERROR "本地编译未找到 ${LIB_NAME} 库")
        endif()
    endif()
endfunction()

