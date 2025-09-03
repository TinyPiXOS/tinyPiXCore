cmake_minimum_required(VERSION 3.20.0)
project(tinyPiXCore VERSION 1.0.0 LANGUAGES C CXX)

#[[
# 1. 确保构建目录存在
set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/build")
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR})

# 2. 设置 .config 文件路径
set(CONFIG_FILE "${CMAKE_BINARY_DIR}/.config")

# 3. 如果 .config 不存在，创建默认配置

# 4. 检测配置工具（放在创建默认配置之后）

# ---- nconfig (Python/kconfiglib) 检测 ----
find_program(PYTHON NAMES python python3)
if(PYTHON)
    execute_process(
        COMMAND ${PYTHON} -c "import kconfiglib; print('found')"
        OUTPUT_VARIABLE KCONFIGLIB_OUTPUT
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(KCONFIGLIB_OUTPUT STREQUAL "found")
        set(HAVE_NCONFIG TRUE)
        message(STATUS "支持 nconfig (Python, kconfiglib)")
    else()
        set(HAVE_NCONFIG FALSE)
    endif()
else()
    set(HAVE_NCONFIG FALSE)
endif()

# ---- menuconfig (C 工具) 检测 ----
set(HAVE_MENUCONFIG FALSE)

# 按优先顺序查找：kconfig-mconf -> mconf -> menuconfig
find_program(MENUCONFIG kconfig-mconf)
if(MENUCONFIG)
    set(HAVE_MENUCONFIG TRUE)
else()
    find_program(MENUCONFIG mconf)
    if(MENUCONFIG)
        set(HAVE_MENUCONFIG TRUE)
    else()
        find_program(MENUCONFIG menuconfig)
        if(MENUCONFIG)
            set(HAVE_MENUCONFIG TRUE)
        endif()
    endif()
endif()

if(HAVE_MENUCONFIG)
    message(STATUS "支持 menuconfig: ${MENUCONFIG}")
else()
    message(STATUS "未找到 menuconfig 工具")
endif()

# 5. 添加配置目标
# 构建命令变量
if(HAVE_NCONFIG OR HAVE_MENUCONFIG)
    # 构建命令列表
    if(HAVE_NCONFIG)
        set(config_cmd_list ${PYTHON} -m menuconfig ${CMAKE_SOURCE_DIR}/Kconfig)
    else()
        set(config_cmd_list ${MENUCONFIG} ${CMAKE_SOURCE_DIR}/Kconfig)
    endif()

    # 尝试用独立终端运行
    find_program(XTERM xterm)
    if(XTERM)
        add_custom_target(config
            COMMAND ${XTERM} -e ${config_cmd_list}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "在独立终端运行配置工具"
        )
    else()
        # 没有 xterm，就在当前终端运行
        add_custom_target(config
            COMMAND ${config_cmd_list}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "运行配置工具 (当前终端)"
        )
    endif()
else()
    add_custom_target(config
        COMMAND ${CMAKE_COMMAND} -E echo "错误：未找到配置工具"
        COMMENT "配置工具不可用"
    )
endif()

# 6. 解析 .config 文件（增强版）
if(EXISTS "${CONFIG_FILE}")
    file(STRINGS "${CONFIG_FILE}" CONFIG_LINES)
    foreach(LINE ${CONFIG_LINES})
        # 跳过空行和注释行
        if(LINE MATCHES "^[ \t]*$" OR LINE MATCHES "^#")
            continue()
        endif()
        
        # 处理有效配置行
        if(LINE MATCHES "^CONFIG_([A-Za-z0-9_]+)=(.+)$")
            set(CONFIG_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}")
            # 处理字符串值（移除引号）
            if(CMAKE_MATCH_2 MATCHES "^\"(.*)\"$")
                set(CONFIG_${CMAKE_MATCH_1} "${CMAKE_MATCH_1}")
            endif()
            message(STATUS "配置项: CONFIG_${CMAKE_MATCH_1} = ${CONFIG_${CMAKE_MATCH_1}}")
        else()
            message(WARNING "忽略无效配置行: ${LINE}")
        endif()
    endforeach()
else()
    message(WARNING ".config 文件不存在！")
endif()

# 7. 确保 config.h.in 存在
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/config.h.in")
    file(WRITE "${CMAKE_SOURCE_DIR}/config.h.in" "#pragma once\n")
    message(STATUS "已创建默认的 config.h.in 文件")
endif()

# 8. 生成 config.h
configure_file(
    ${CMAKE_SOURCE_DIR}/config.h.in
    ${CMAKE_BINARY_DIR}/config.h
)

# 9. 添加重新配置目标
add_custom_target(reconfigure
    COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "重新运行 CMake 应用新配置"
)

# 添加依赖关系
add_dependencies(reconfigure config)
]]


# 将包含目录添加到编译器搜索路径
include_directories(${CMAKE_BINARY_DIR})

# 设置构建生成的根目录
set(CMAKE_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")

# 设置不同类型的目标文件的输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/bin")

# 安装目标目录
set(INSTALL_INCLUDE_DIR "/usr/include/tinyPiX")
# 安装资源文件
set(INSTALL_RESOURCE_DIR "/usr/res/tinyPiX")
# 安装数据文件
set(INSTALL_DATA_DIR "/usr/data/tinyPiX")
# 安装bin文件
set(INSTALL_BIN_DIR "/usr/bin/tinyPiX")
# 安装bin文件
set(INSTALL_LIB_DIR "/usr/lib/tinyPiX" CACHE PATH "Global library installation directory")
# 定义可执行程序的软链接目录
set(LINK_BIN_DIR "/usr/bin")
# 定义动态库软链接的目标目录
set(LINK_LIB_DIR "/usr/lib")

# 在自定义目标后添加
add_custom_target(test_config
    COMMAND ${CMAKE_COMMAND} -E echo "===== 配置系统测试成功 ====="
    COMMENT "测试配置目标是否可用"
)
# 添加子目录
add_subdirectory(PiXUtils)
add_subdirectory(PiXExternUtils)
add_subdirectory(PiXSingleGUI)
add_subdirectory(PiXGUIFramework/desktopGUI)
add_subdirectory(PiXGUIFramework/exampleGUI)
add_subdirectory(PiXGUIFramework/SmartDeviceGUI)
add_subdirectory(PiXService)
# add_subdirectory(examplesApp)

# 平台不同拷贝资源文件到不同安装目录；以及准备不同平台的安装依赖库
# 首选方案：使用 CMAKE_CROSSCOMPILING 和自定义变量
if(DEFINED TARGET_ARCH)
    # 显式指定目标架构优先
    if(TARGET_ARCH STREQUAL "x86_64")
        message(STATUS "============ 目标平台: X86_64 ============")
        set(INSTALL_PACKAGE_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/install/x86_64")
        set(DEPEND_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/depend_lib/dynamic/x86_64")
    elseif(TARGET_ARCH STREQUAL "arm64" OR TARGET_ARCH STREQUAL "aarch64")
        message(STATUS "============ 目标平台: ARM64 ============")
        set(INSTALL_PACKAGE_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/install/arm_64")
        set(DEPEND_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/depend_lib/dynamic/arm_64")
    endif()
elseif(CMAKE_CROSSCOMPILING)
    # 交叉编译时获取目标架构
    message(STATUS "============ 交叉编译目标平台: ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR} ============")
    
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
        set(INSTALL_PACKAGE_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/install/arm_64")
        set(DEPEND_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/depend_lib/dynamic/arm_64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86|i686|i386|AMD64")
        set(INSTALL_PACKAGE_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/install/x86_64")
        set(DEPEND_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/depend_lib/dynamic/x86_64")
    else()
        message(WARNING "未知的目标平台架构: ${CMAKE_SYSTEM_PROCESSOR}")
    endif()
else()
    # 本机编译
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86|i686|i386|AMD64")
        message(STATUS "============ 本机平台: X86_64 ============")
        set(INSTALL_PACKAGE_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/install/x86_64")
        set(DEPEND_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/depend_lib/dynamic/x86_64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
        message(STATUS "============ 本机平台: ARM ============")
        set(INSTALL_PACKAGE_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/install/arm_64")
        set(DEPEND_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/depend_lib/dynamic/arm_64")
    endif()
endif()

# 创建一个虚拟目标用于执行自定义命令，清理安装目录
add_custom_target(clean_install_directory
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_INCLUDE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_INCLUDE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_RESOURCE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_RESOURCE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_DATA_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_DATA_DIR}"
    # COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_BIN_DIR}"
    # COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_BIN_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_PACKAGE_ROOT_PATH}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_PACKAGE_ROOT_PATH}"   
    COMMENT "Cleaning install directory for headers and res"
)

# 将清理目标添加为全局安装的依赖
install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} --build . --target clean_install_directory)")

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
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/bin/ DESTINATION ${INSTALL_BIN_DIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/bin DESTINATION ${INSTALL_PACKAGE_ROOT_PATH})

# 修改bin下文件权限
install(CODE "
    # 权限修复（可选）
    execute_process(COMMAND chmod -R 0777 ${INSTALL_PACKAGE_ROOT_PATH}/bin)
    execute_process(COMMAND chmod -R 0777 ${INSTALL_BIN_DIR})
")

# 安装生成库到安装目录
install(TARGETS PiXService RUNTIME DESTINATION ${INSTALL_PACKAGE_ROOT_PATH}/bin)
install(TARGETS PiXSingleGUI LIBRARY DESTINATION ${INSTALL_PACKAGE_ROOT_PATH}/lib)
install(TARGETS PiXExternUtils LIBRARY DESTINATION ${INSTALL_PACKAGE_ROOT_PATH}/lib)
install(TARGETS PiXUtils LIBRARY DESTINATION ${INSTALL_PACKAGE_ROOT_PATH}/lib)
install(TARGETS PiXDesktopGUI LIBRARY DESTINATION ${INSTALL_PACKAGE_ROOT_PATH}/lib)
install(TARGETS exampleGUI LIBRARY DESTINATION ${INSTALL_PACKAGE_ROOT_PATH}/lib)

# 拷贝依赖库到/usr/lib
file(GLOB_RECURSE INSTALL_SO_FILES "${INSTALL_LIB_DIR}/*.so*")
file(GLOB_RECURSE DEPEND_SO_FILES "${DEPEND_LIB_PATH}/*.so*")

set(COPY_DEPEND_COMMANDS)
list(APPEND COPY_DEPEND_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${INSTALL_LIB_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_LIB_DIR}"
)
foreach(SO_FILE IN LISTS DEPEND_SO_FILES)
    message(STATUS "拷贝依赖库: ===== ${SO_FILE} =====")
   
    # 拷贝
    list(APPEND COPY_DEPEND_COMMANDS
        COMMAND ${CMAKE_COMMAND} -E copy
            "${SO_FILE}"
            "${INSTALL_LIB_DIR}"
    )
endforeach()

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

        # 创建两种名称
        set(BASE_NAME_WITHOUT_ANY_VERSION "${LIB_BASE}.so")
        set(BASE_NAME_WITH_MAJOR_VERSION "${LIB_BASE}.so.${MAJOR_VERSION}")

    # 匹配带主次版本号的库文件 (lib.so.x.y)
    elseif("${FILENAME}" MATCHES "^(.*)\\.so\\.([0-9]+)\\.([0-9]+)$")
        set(LIB_BASE "${CMAKE_MATCH_1}")
        set(MAJOR_VERSION "${CMAKE_MATCH_2}")
        set(MINOR_VERSION "${CMAKE_MATCH_3}")

        # 创建两种名称
        set(BASE_NAME_WITHOUT_ANY_VERSION "${LIB_BASE}.so")
        set(BASE_NAME_WITH_MAJOR_VERSION "${LIB_BASE}.so.${MAJOR_VERSION}")

    # 匹配只带主版本号的库文件 (lib.so.x)
    elseif("${FILENAME}" MATCHES "^(.*)\\.so\\.([0-9]+)$")
        set(LIB_BASE "${CMAKE_MATCH_1}")
        set(MAJOR_VERSION "${CMAKE_MATCH_2}")

        # 创建两种名称
        set(BASE_NAME_WITHOUT_ANY_VERSION "${LIB_BASE}.so")
        set(BASE_NAME_WITH_MAJOR_VERSION "${FILENAME}") # 已经是主版本号形式

    # 处理没有版本号后缀的文件
    else()
        set(BASE_NAME_WITHOUT_ANY_VERSION "${FILENAME}")
        set(BASE_NAME_WITH_MAJOR_VERSION "${FILENAME}") # 没有版本则两者相同
    endif()

    # message(STATUS "文件名称: ===== ${FILENAME} =====")
    # message(STATUS "无版本号: ===== ${BASE_NAME_WITHOUT_ANY_VERSION} =====")
    # message(STATUS "带主版本号: ===== ${BASE_NAME_WITH_MAJOR_VERSION} =====")

    # 生成单个 ln -s 命令（注意使用绝对路径）
    if("${BASE_NAME_WITH_MAJOR_VERSION}" STREQUAL "${BASE_NAME_WITHOUT_ANY_VERSION}")
        list(APPEND CREATE_LINK_COMMANDS
            COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${SO_FILE}"
            "${LINK_LIB_DIR}/${BASE_NAME_WITHOUT_ANY_VERSION}"
        )
    else()
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

add_custom_target(create_symlinks
    COMMAND ${CMAKE_COMMAND} -E echo "创建软链接..."
    ${COPY_DEPEND_COMMANDS}
    ${CREATE_LINK_COMMANDS}
    COMMENT "/usr/lib/tinyPiX .so文件创建符号链接"
)

# 确保拷贝在构建前执行
add_dependencies(PiXUtils create_symlinks)
add_dependencies(PiXExternUtils create_symlinks)
add_dependencies(PiXSingleGUI create_symlinks)
add_dependencies(PiXService create_symlinks)
add_dependencies(PiXDesktopGUI create_symlinks)
add_dependencies(exampleGUI create_symlinks)
