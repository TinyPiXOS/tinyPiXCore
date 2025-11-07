cmake_minimum_required(VERSION 3.20.0)
project(TpGUI VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../cmake/TargetArchConfig.cmake)

# 添加源码目录
set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/src/TpGUI/Core/TpObject.cpp
    ${SOURCE_PATH}/src/TpGUI/Core/TpWidget.cpp
    ${SOURCE_PATH}/src/TpGUI/Core/TpEvent.cpp
    ${SOURCE_PATH}/src/TpGUI/Core/TpApp.cpp
    ${SOURCE_PATH}/src/TpGUI/Screen/TpScreen.cpp
    ${SOURCE_PATH}/src/TpGUI/Screen/TpMainWindow.cpp
    ${SOURCE_PATH}/src/TpGUI/Screen/TpMainWindow.cpp
    ${SOURCE_PATH}/src/TpGUI/Screen/TpDialog.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpMenu.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpLabel.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpCheckBox.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpButton.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpIconTopButton.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpLayout.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpLayoutItem.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpSpacerItem.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpBoxLayout.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpHBoxLayout.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpVBoxLayout.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpGridLayout.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpFlexLayout.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpRadioButton.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpProgressBar.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpSlider.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpOnOffButton.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpScrollPanel.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpLineEdit.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpComBox.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpListWidget.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpListWidgetItem.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpTabWidget.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpTabBar.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpBattery.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpCarouselButton.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpSlideProgressBar.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpPanelSwitchButton.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpNotificationWidget.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpNotificationItem.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpLine.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpCollapsibleFileWidget.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpCollapsibleFileItem.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpMenuPanelWidget.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpMenuPanelItem.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpMediaTileButton.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpFilePathWidget.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpFilePathWidgetItem.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpMessageBox.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpPercentProgressBar.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpAnimation.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpVirtualKeyboard.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpImageWidget.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpRadioButtonGroup.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpCircularProgressBar.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpMusicPlayerView.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpWeatherInfoPanel.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpButton.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpTemperatureWidget.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpHumidityWidget.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpLottieAnimation.cpp
    ${SOURCE_PATH}/src_p/TpGUI/Core/TpObjectStack.cpp
    ${SOURCE_PATH}/src_p/TpGUI/Core/TpAutoObject.cpp
    ${SOURCE_PATH}/src_p/TpGUI/Screen/TpFixScreen.cpp
)

include_directories(${SYSTEM_INCLUDE_PATH}/cairo)

include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/TpWM)
include_directories(${SOURCE_PATH}/include/TpUtils)
include_directories(${SOURCE_PATH}/include/TpExtUtils)
include_directories(${SOURCE_PATH}/include/TpGUI)
include_directories(${SOURCE_PATH}/include/TpGUI/Core)
include_directories(${SOURCE_PATH}/include/TpGUI/Screen)
include_directories(${SOURCE_PATH}/include/TpGUI/Widgets)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/TpGUI)
include_directories(${SOURCE_PATH}/include_p/TpGUI/Widgets)
include_directories(${SOURCE_PATH}/include_p/TpGUI/Core)
include_directories(${SOURCE_PATH}/include_p/TpGUI/Screen)
include_directories(${SOURCE_PATH}/include_p/TpUtils)

# 添加动态库目标
add_library(TpGUI SHARED ${SOURCE_FILES})

# 设置动态库版本 VERSION 是库的当前版本号，而 SOVERSION 是API的兼容性版本号。
# set_target_properties(TpGUI PROPERTIES VERSION 1.0 SOVERSION 1)

# 指定依赖动态库目录
if(CMAKE_CROSSCOMPILING)
    link_directories("${LOAD_SDK_LIB_PATH}")
else()
    link_directories("/usr/lib")
endif()

# 指定静态库的路径
set(SQLITE3_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libsqlite3.a)
set(THORVG_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libthorvg.a)

# 库链接到可执行目标
target_link_libraries(TpGUI PUBLIC
    ${SQLITE3_STATIC_LIB} ${THORVG_STATIC_LIB}
    dl pthread
    ${DEPEND_DYNAMIC_LIB_PATH}/libTpWMEngine.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpUtils.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpExtUtils.so
)
