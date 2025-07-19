cmake_minimum_required(VERSION 3.20.0)
project(tinyPiXCore VERSION 1.0.0 LANGUAGES C CXX)

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

# 添加子目录
add_subdirectory(PiXUtils)
add_subdirectory(PiXExternUtils)
add_subdirectory(PiXSingleGUI)
add_subdirectory(PiXGUIFramework/desktopGUI)
add_subdirectory(PiXGUIFramework/exampleGUI)
add_subdirectory(PiXService)

# 平台不同拷贝资源文件到不同安装目录；以及准备不同平台的安装依赖库
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86|i686|i386|AMD64")
    message(STATUS "============X86 平台============")
    set(INSTALL_PACKAGE_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/install/x86_64")
    set(DEPEND_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/depend_lib/dynamic/x86_64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
    message(STATUS "============ARM 平台============")
    set(INSTALL_PACKAGE_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/install/arm_64")
    set(DEPEND_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/depend_lib/dynamic/arm_64")
endif()

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
    # message(STATUS "拷贝依赖库: ===== ${SO_FILE} =====")
   
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
