cmake_minimum_required(VERSION 3.20.0)
project(TpDesktop VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加源码目录
set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/AppManage/AppRunnerManage.cpp

    ${SOURCE_PATH}/DeskEntry.cpp
    ${SOURCE_PATH}/DeskScreen.cpp
    ${SOURCE_PATH}/ToolBar/StatusBar.cpp
    ${SOURCE_PATH}/ToolBar/BottomBar.cpp
    ${SOURCE_PATH}/ToolBar/AppSettingBar.cpp
    ${SOURCE_PATH}/ToolBar/PowerManage.cpp
    ${SOURCE_PATH}/ToolBar/NavigationBar.cpp
    ${SOURCE_PATH}/ToolWindow/SysLockWindow.cpp
    ${SOURCE_PATH}/ToolWindow/AppTaskManageWindow.cpp
    ${SOURCE_PATH}/ToolWindow/AppOperateMaskWindow.cpp
    ${SOURCE_PATH}/ToolWindow/AppPreviewWidget.cpp
    ${SOURCE_PATH}/ToolWindow/SplashScreen.cpp
    ${SOURCE_PATH}/ToolModule/MainAppScrollPanel.cpp
    ${SOURCE_PATH}/ToolModule/CaculateAppPos.cpp
    ${SOURCE_PATH}/ToolModule/DesktopAppButton.cpp
)

include_directories(${SOURCE_PATH})
include_directories(${SOURCE_PATH}/ToolBar)
include_directories(${SOURCE_PATH}/ToolModule)
include_directories(${SOURCE_PATH}/ToolWindow)

include_directories(${CMAKE_SOURCE_DIR}/TpDesktop/include)
include_directories(${CMAKE_SOURCE_DIR}/TpDesktop/include/Service)
include_directories(${CMAKE_SOURCE_DIR}/TpDesktop/include/InteractData)

include_directories(${CMAKE_SOURCE_DIR}/src/include)
include_directories(${CMAKE_SOURCE_DIR}/src/include/TpService)
include_directories(${CMAKE_SOURCE_DIR}/src/include/TpCore)
include_directories(${CMAKE_SOURCE_DIR}/src/include/TpWM)
include_directories(${CMAKE_SOURCE_DIR}/src/include/TpUtils)
include_directories(${CMAKE_SOURCE_DIR}/src/include/TpUtils/AppManage)
include_directories(${CMAKE_SOURCE_DIR}/src/include/TpUtils/Bluetooth)
include_directories(${CMAKE_SOURCE_DIR}/src/include/TpUtils/Network)
include_directories(${CMAKE_SOURCE_DIR}/src/include/TpUtils/Media)
include_directories(${CMAKE_SOURCE_DIR}/src/include/TpGUI)
include_directories(${CMAKE_SOURCE_DIR}/src/include/TpGUI/GUI)
include_directories(${CMAKE_SOURCE_DIR}/src/include/TpGUI/Widgets)

# 添加动态库目标
# add_library(TpDesktop SHARED ${SOURCE_FILES})
add_executable(TpDesktop ${SOURCE_FILES})

# 设置目标属性，抑制RPATH警告
set_target_properties(TpDesktop PROPERTIES
    SKIP_BUILD_RPATH TRUE
    BUILD_WITH_INSTALL_RPATH FALSE
    INSTALL_RPATH ""
    MACOSX_RPATH OFF
)

# 设置动态库版本 VERSION 是库的当前版本号，而 SOVERSION 是API的兼容性版本号。
# set_target_properties(TpDesktop PROPERTIES VERSION 1.0 SOVERSION 1)

# 指定依赖动态库目录
if(CMAKE_CROSSCOMPILING)
    target_link_directories(TpDesktop PRIVATE
    ${LOAD_SDK_LIB_PATH}
)
else()
    target_link_directories(TpDesktop PRIVATE
    /usr/lib/TinyPiX
)
endif()

# 在find_library之前设置
set(CMAKE_PREFIX_PATH
    "/usr/lib"
    "/usr/lib/TinyPiX"
    ${CMAKE_PREFIX_PATH}
)

LoadSDKLibrary(WEBP_LIBRARY webp)
LoadSDKLibrary(PNG_LIBRARY png)
#LoadSDKLibrary(GOMP_LIBRARY gomp)

# 将pthread库链接到可执行目标
target_link_libraries(TpDesktop
    ${DEPEND_DYNAMIC_LIB_PATH}/libTpWMEngine.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpCore.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpGUI.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpUtils.so
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libTpDesktop.so
    ${WEBP_LIBRARY}
    ${PNG_LIBRARY}
    gomp
)

# 包含共享模块
# include(${CMAKE_SOURCE_DIR}/cmake/SetupAppInstallation.cmake)

# 初始化应用安装信息
# setup_app_installation("desk0000-0000-0000-0000-0000000000000")
