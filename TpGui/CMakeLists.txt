cmake_minimum_required(VERSION 3.20.0)
project(TpGui VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../cmake/CreateTinyCoreSymlink.cmake)

# 添加源码目录
set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/src/TpGui/Core/TpObject.cpp
    ${SOURCE_PATH}/src/TpGui/Core/TpChildWidget.cpp
    ${SOURCE_PATH}/src/TpGui/Core/TpEvent.cpp
    ${SOURCE_PATH}/src/TpGui/Core/TpApp.cpp
    ${SOURCE_PATH}/src/TpGui/Screen/TpScreen.cpp
    ${SOURCE_PATH}/src/TpGui/Screen/TpFixScreen.cpp
    ${SOURCE_PATH}/src/TpGui/Screen/TpDialog.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpMenu.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpLabel.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpCheckBox.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpButton.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpIconTopButton.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpLayout.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpLayoutItem.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpSpacerItem.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpBoxLayout.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpHBoxLayout.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpVBoxLayout.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpGridLayout.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpFlexLayout.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpRadioButton.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpProgressBar.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpSlider.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpOnOffButton.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpScrollPanel.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpLineEdit.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpComBox.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpListWidget.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpListWidgetItem.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpTabWidget.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpTabBar.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpBattery.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpCarouselButton.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpSlideProgressBar.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpPanelSwitchButton.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpNotificationWidget.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpNotificationItem.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpLine.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpCollapsibleFileWidget.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpCollapsibleFileItem.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpMenuPanelWidget.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpMenuPanelItem.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpMediaTileButton.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpFilePathWidget.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpFilePathWidgetItem.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpMessageBox.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpPercentProgressBar.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpAnimation.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpVirtualKeyboard.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpImageWidget.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpRadioButtonGroup.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpCircularProgressBar.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpMusicPlayerView.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpWeatherInfoPanel.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpButton.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpTemperatureWidget.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpHumidityWidget.cpp
    ${SOURCE_PATH}/src/TpGui/Widgets/TpLottieAnimation.cpp
    ${SOURCE_PATH}/src_p/TpGui/Core/TpObjectStack.cpp
    ${SOURCE_PATH}/src_p/TpGui/Core/TpAutoObject.cpp
)

include_directories(/usr/include/cairo)
include_directories(/usr/include/PiXWM)
include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/TpUtils)
include_directories(${SOURCE_PATH}/include/TpExtUtils)
include_directories(${SOURCE_PATH}/include/TpGui)
include_directories(${SOURCE_PATH}/include/TpGui/Core)
include_directories(${SOURCE_PATH}/include/TpGui/Screen)
include_directories(${SOURCE_PATH}/include/TpGui/Widgets)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/TpGui)
include_directories(${SOURCE_PATH}/include_p/TpGui/Widgets)
include_directories(${SOURCE_PATH}/include_p/TpGui/Core)
include_directories(${SOURCE_PATH}/include_p/TpUtils)

# 添加动态库目标
add_library(TpGui SHARED ${SOURCE_FILES})

# 设置动态库版本 VERSION 是库的当前版本号，而 SOVERSION 是API的兼容性版本号。
# set_target_properties(TpGui PROPERTIES VERSION 1.0 SOVERSION 1)

# 指定依赖动态库目录
link_directories("/usr/lib")

# 直接指定静态库的路径
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86|i686|i386|AMD64")
    set(SQLITE3_STATIC_LIB ${SOURCE_PATH}/depend_lib/static/x86_x64/libsqlite3.a)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
    set(SQLITE3_STATIC_LIB ${SOURCE_PATH}/depend_lib/static/arm_64/libsqlite3.a)
endif()

# 库链接到可执行目标
target_link_libraries(TpGui
    ${SQLITE3_STATIC_LIB}
    dl pthread PiXApi TpUtils TpExtUtils
)

# 如果你需要将动态库安装到特定的目录，可以修改DESTINATION路径
install(TARGETS TpGui LIBRARY DESTINATION ${INSTALL_LIB_DIR})

# 创建软链接
cmake_create_core_symlink(
    TARGET TpGui
    INSTALL_DIR ${INSTALL_LIB_DIR}
    LINK_DIR ${LINK_LIB_DIR}             
)
