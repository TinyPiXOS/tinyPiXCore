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
    ${SOURCE_PATH}/src/SingleGUI/core/tpObject.cpp
    ${SOURCE_PATH}/src/SingleGUI/core/tpChildWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/core/tpEvent.cpp
    ${SOURCE_PATH}/src/SingleGUI/core/tpAutoObject.cpp
    ${SOURCE_PATH}/src/SingleGUI/core/tpApp.cpp
    ${SOURCE_PATH}/src/SingleGUI/screen/tpScreen.cpp
    ${SOURCE_PATH}/src/SingleGUI/screen/tpFixScreen.cpp
    ${SOURCE_PATH}/src/SingleGUI/screen/tpDialog.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpMenu.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpLabel.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpCheckBox.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpIconTopButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpLayout.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpLayoutItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpSpacerItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpBoxLayout.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpHBoxLayout.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpVBoxLayout.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpGridLayout.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpFlexLayout.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpRadioButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpProgressBar.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpSlider.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpLink.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpScroll.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpScrollBar.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpOnOffButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpScrollPanel.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpLineEdit.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpComBox.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpListWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpListWidgetItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpTreeWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpTreeWidgetItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpTabWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpTabBar.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpBattery.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpCarouselButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpSlideProgressBar.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpPanelSwitchButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpNotificationWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpNotificationItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpLine.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpCollapsibleFileWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpCollapsibleFileItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpMenuPanelWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpMenuPanelItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpMediaTileButton.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpFilePathWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpFilePathWidgetItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpListCheckBoxGroup.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpListCheckBoxGroupItem.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpMessageBox.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpPercentProgressBar.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpAnimation.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpVirtualKeyboard.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpImageWidget.cpp
    ${SOURCE_PATH}/src/SingleGUI/widgets/tpRadioButtonGroup.cpp
    ${SOURCE_PATH}/src_p/SingleGUI/core/tpObjectStack.cpp
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
    dl
    pthread
    PiXApi
    PiXUtils
    PiXExternUtils
)

# 如果你需要将动态库安装到特定的目录，可以修改DESTINATION路径
install(TARGETS PiXSingleGUI LIBRARY DESTINATION ${INSTALL_LIB_DIR})

# 创建软链接
cmake_create_core_symlink(
    TARGET PiXSingleGUI
    INSTALL_DIR ${INSTALL_LIB_DIR}
    LINK_DIR ${LINK_LIB_DIR}             
)
