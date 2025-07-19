cmake_minimum_required(VERSION 3.20.0)
project(exampleGUI VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../../cmake/CreateTinyCoreSymlink.cmake)

# 添加源码目录
set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../src")

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/src/GUIFramework/exampleGUI/widgets/testLight.cpp
)

include_directories(/usr/include/cairo)
include_directories(/usr/include/PiXWM)
include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/Utils)
include_directories(${SOURCE_PATH}/include/ExternUtils)
include_directories(${SOURCE_PATH}/include/GUIFramework)
include_directories(${SOURCE_PATH}/include/GUIFramework/exampleGUI)
include_directories(${SOURCE_PATH}/include/GUIFramework/exampleGUI/widgets)
include_directories(${SOURCE_PATH}/include/SingleGUI)
include_directories(${SOURCE_PATH}/include/SingleGUI/core)
include_directories(${SOURCE_PATH}/include/SingleGUI/screen)
include_directories(${SOURCE_PATH}/include/SingleGUI/widgets)

# 添加动态库目标
add_library(exampleGUI SHARED ${SOURCE_FILES})

# 设置动态库版本 VERSION 是库的当前版本号，而 SOVERSION 是API的兼容性版本号。
# set_target_properties(PiXDesktopGUI PROPERTIES VERSION 1.0 SOVERSION 1)

# 指定依赖动态库目录
link_directories("/usr/lib")

# 将pthread库链接到可执行目标
target_link_libraries(exampleGUI
    dl
    pthread
    PiXApi
    PiXUtils
    PiXExternUtils
    PiXSingleGUI
)

# 如果你需要将动态库安装到特定的目录，可以修改DESTINATION路径
install(TARGETS exampleGUI LIBRARY DESTINATION ${INSTALL_LIB_DIR})

# 创建软链接
cmake_create_core_symlink(
    TARGET exampleGUI
    INSTALL_DIR ${INSTALL_LIB_DIR}
    LINK_DIR ${LINK_LIB_DIR}             
)