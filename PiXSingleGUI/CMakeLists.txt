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
    ${SOURCE_PATH}/src/SingleGUI/core/TpObject.cpp
    ${SOURCE_PATH}/src/SingleGUI/core/TpChildWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/core/TpEvent.cpp
    ${SOURCE_PATH}/src/SingleGUI/core/TpAutoObject.cpp
    ${SOURCE_PATH}/src/SingleGUI/core/TpApp.cpp
    ${SOURCE_PATH}/src/SingleGUI/screen/TpScreen.cpp
    ${SOURCE_PATH}/src/SingleGUI/screen/TpFixScreen.cpp
    ${SOURCE_PATH}/src/SingleGUI/screen/TpDialog.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpMenu.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpLabel.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpCheckBox.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpIconTopButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpLayout.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpLayoutItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpSpacerItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpBoxLayout.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpHBoxLayout.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpVBoxLayout.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpGridLayout.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpFlexLayout.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpRadioButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpProgressBar.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpSlider.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpLink.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpScroll.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpScrollBar.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpOnOffButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpScrollPanel.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpLineEdit.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpComBox.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpListWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpListWidgetItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpTreeWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpTreeWidgetItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpTabWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpTabBar.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpBattery.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpCarouselButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpSlideProgressBar.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpPanelSwitchButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpNotificationWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpNotificationItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpLine.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpCollapsibleFileWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpCollapsibleFileItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpMenuPanelWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpMenuPanelItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpMediaTileButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpFilePathWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpFilePathWidgetItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpListCheckBoxGroup.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpListCheckBoxGroupItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpMessageBox.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpPercentProgressBar.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpAnimation.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpVirtualKeyboard.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpImageWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/TpRadioButtonGroup.cpp
    ${SOURCE_PATH}/src_p/SingleGUI/core/TpObjectStack.cpp
)

include_directories(/usr/include/cairo)
include_directories(/usr/include/PiXWM)
include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/Utils)
include_directories(${SOURCE_PATH}/include/ExternUtils)
include_directories(${SOURCE_PATH}/include/SingleGUI)
include_directories(${SOURCE_PATH}/include/SingleGUI/core)
include_directories(${SOURCE_PATH}/include/SingleGUI/screen)
include_directories(${SOURCE_PATH}/include/SingleGUI/widgets)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/SingleGUI)
include_directories(${SOURCE_PATH}/include_p/SingleGUI/widgets)
include_directories(${SOURCE_PATH}/include_p/SingleGUI/core)
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
