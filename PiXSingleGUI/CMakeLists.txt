cmake_minimum_required(VERSION 3.20.0)
project(PiXSingleGUI VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../cmake/CreateTinyCoreSymlink.cmake)

# 添加源码目录
set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/src/SingleGUI/Core/TpObject.cpp
    ${SOURCE_PATH}/src/SingleGUI/Core/TpChildWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/Core/TpEvent.cpp
    ${SOURCE_PATH}/src/SingleGUI/Core/TpAutoObject.cpp
    ${SOURCE_PATH}/src/SingleGUI/Core/TpApp.cpp
    ${SOURCE_PATH}/src/SingleGUI/Screen/TpScreen.cpp
    ${SOURCE_PATH}/src/SingleGUI/Screen/TpFixScreen.cpp
    ${SOURCE_PATH}/src/SingleGUI/Screen/TpDialog.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpMenu.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpLabel.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpCheckBox.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpIconTopButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpLayout.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpLayoutItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpSpacerItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpBoxLayout.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpHBoxLayout.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpVBoxLayout.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpGridLayout.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpFlexLayout.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpRadioButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpProgressBar.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpSlider.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpLink.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpScroll.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpScrollBar.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpOnOffButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpScrollPanel.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpLineEdit.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpComBox.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpListWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpListWidgetItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpTreeWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpTreeWidgetItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpTabWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpTabBar.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpBattery.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpCarouselButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpSlideProgressBar.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpPanelSwitchButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpNotificationWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpNotificationItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpLine.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpCollapsibleFileWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpCollapsibleFileItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpMenuPanelWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpMenuPanelItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpMediaTileButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpFilePathWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpFilePathWidgetItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpListCheckBoxGroup.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpListCheckBoxGroupItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpMessageBox.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpPercentProgressBar.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpAnimation.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpVirtualKeyboard.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpImageWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/Widgets/TpRadioButtonGroup.cpp
    ${SOURCE_PATH}/src_p/SingleGUI/Core/TpObjectStack.cpp
)

include_directories(/usr/include/cairo)
include_directories(/usr/include/PiXWM)
include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/Utils)
include_directories(${SOURCE_PATH}/include/ExternUtils)
include_directories(${SOURCE_PATH}/include/SingleGUI)
include_directories(${SOURCE_PATH}/include/SingleGUI/Core)
include_directories(${SOURCE_PATH}/include/SingleGUI/Screen)
include_directories(${SOURCE_PATH}/include/SingleGUI/Widgets)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/SingleGUI)
include_directories(${SOURCE_PATH}/include_p/SingleGUI/Widgets)
include_directories(${SOURCE_PATH}/include_p/SingleGUI/Core)
include_directories(${SOURCE_PATH}/include_p/Api)
include_directories(${SOURCE_PATH}/include_p/Utils)

# 添加动态库目标
add_library(PiXSingleGUI SHARED ${SOURCE_FILES})

# 设置动态库版本 VERSION 是库的当前版本号，而 SOVERSION 是API的兼容性版本号。
# set_target_properties(PiXSingleGUI PROPERTIES VERSION 1.0 SOVERSION 1)

# 指定依赖动态库目录
link_directories("/usr/lib")

# 直接指定静态库的路径
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86|i686|i386|AMD64")
    set(SQLITE3_STATIC_LIB ${SOURCE_PATH}/depend_lib/static/x86_x64/libsqlite3.a)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
    set(SQLITE3_STATIC_LIB ${SOURCE_PATH}/depend_lib/static/arm_64/libsqlite3.a)
endif()

# 库链接到可执行目标
target_link_libraries(PiXSingleGUI
    ${SQLITE3_STATIC_LIB}
    dl pthread PiXApi PiXUtils PiXExternUtils
)

# 如果你需要将动态库安装到特定的目录，可以修改DESTINATION路径
install(TARGETS PiXSingleGUI LIBRARY DESTINATION ${INSTALL_LIB_DIR})

# 创建软链接
cmake_create_core_symlink(
    TARGET PiXSingleGUI
    INSTALL_DIR ${INSTALL_LIB_DIR}
    LINK_DIR ${LINK_LIB_DIR}             
)
