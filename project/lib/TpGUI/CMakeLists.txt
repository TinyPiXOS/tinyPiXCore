cmake_minimum_required(VERSION 3.20.0)
project(TpGUI VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../../../cmake/TargetArchConfig.cmake)

# 定义源码路径宏
set(SOURCE_FILES
    # ${SOURCE_PATH}/src_p/TpService/DeskRPC/DeskService_server.cpp
    # ${SOURCE_PATH}/src_p/TpService/DeskRPC/DeskService_interface.cpp
    # ${SOURCE_PATH}/src_p/TpService/DeskRPC/DeskService_server.cpp

    ${SOURCE_PATH}/src/TpGUI/GUI/TpApp.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpWidget.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpEvent.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpScreen.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpMainWindow.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpMainWindow.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpDialog.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpPen.cpp
	${SOURCE_PATH}/src/TpGUI/GUI/TpBrush.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpImage.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpGradient.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpLinearGradient.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpRadialGradient.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpGraphicsEffect.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpGraphicsBlurEffect.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpPainterPath.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpCssParser.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpCssData.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpColors.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpCursor.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpPainter.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpFont.cpp
    ${SOURCE_PATH}/src/TpGUI/GUI/TpClipboard.cpp
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
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpSwitchButton.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpScrollPanel.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpLineEdit.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpComBox.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpListWidget.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpListWidgetItem.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpTabWidget.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpTabBar.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpBattery.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpCarouselButton.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpIconSlider.cpp
    ${SOURCE_PATH}/src/TpGUI/Widgets/TpSliderProgressBar.cpp
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
    ${SOURCE_PATH}/src_p/TpGUI/GUI/TpObjectStack.cpp
    ${SOURCE_PATH}/src_p/TpGUI/GUI/TpSurface.cpp
    ${SOURCE_PATH}/src_p/TpGUI/GUI/TpFixScreen.cpp
    ${SOURCE_PATH}/src_p/TpGUI/GUI/TpFontConfig.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/CSSParser/CSSParser.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/CSSParser/CSSLex.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/CSSParser/Keyword/KeywordItem.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/CSSParser/Selectors/ClassSelector.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/CSSParser/Selectors/PseudoSelector.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/CSSParser/Selectors/SelectorGroup.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/CSSParser/Selectors/SignSelector.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/CSSParser/Selectors/CombineSelector.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/CSSParser/Selectors/AttributeSelector.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/CSSParser/Selectors/SelectorSequence.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/CSSParser/Selectors/UniversalSelector.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/CSSParser/Selectors/TypeSelector.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/CSSParser/Selectors/Selector.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/CSSParser/Selectors/IdSelector.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/Applicability/HTMLCSSRefAdaptor.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/Applicability/PseudoSelectorRefChiefJudge.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/Vendor/gumbo/string_buffer.c
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/Vendor/gumbo/util.c
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/Vendor/gumbo/vector.c
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/Vendor/gumbo/attribute.c
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/Vendor/gumbo/UrlUtil.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/Vendor/gumbo/char_ref.c
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/Vendor/gumbo/utf8.c
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/Vendor/gumbo/error.c
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/Vendor/gumbo/tag.c
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/Vendor/gumbo/GumboInterface.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/Vendor/gumbo/parser.c
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/Vendor/gumbo/tokenizer.c
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/Vendor/gumbo/string_piece.c
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/Vendor/gumbo/gumbo_edit.c
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/Vendor/Utils/ContainerUtil.cpp
    ${SOURCE_PATH}/src_p/TpGUI/CSSParser/Vendor/Utils/StringUtil.cpp
)

if(CMAKE_CROSSCOMPILING)
    include_directories(${LOAD_SDK_INCLUDE_PATH}/glib-2.0)
    include_directories(${LOAD_SDK_LIB_PATH}/glib-2.0/include)
else()
    # 查找 pkg-config
    find_package(PkgConfig REQUIRED)
    # 查找 glib-2.0
    pkg_check_modules(GLIB REQUIRED glib-2.0)

    include_directories(${GLIB_INCLUDE_DIRS})
endif()

include_directories(${SYSTEM_INCLUDE_PATH})

include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/TpCore)
include_directories(${SOURCE_PATH}/include/TpWM)
include_directories(${SOURCE_PATH}/include/TpUtils)
include_directories(${SOURCE_PATH}/include_p/TpUtils/erpc)
include_directories(${SOURCE_PATH}/include/TpGUI)
include_directories(${SOURCE_PATH}/include/TpGUI/Widgets)
include_directories(${SOURCE_PATH}/include/TpGUI/GUI)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/TpCore)
include_directories(${SOURCE_PATH}/include_p/TpGUI)
include_directories(${SOURCE_PATH}/include_p/TpGUI/Widgets)
include_directories(${SOURCE_PATH}/include_p/TpGUI/GUI)
include_directories(${SOURCE_PATH}/include_p/TpGUI)
include_directories(${SOURCE_PATH}/include_p/TpGUI/CSSParser)
include_directories(${SOURCE_PATH}/include_p/TpGUI/CSSParser/Applicability)
include_directories(${SOURCE_PATH}/include_p/TpGUI/CSSParser/Vendor)
include_directories(${SOURCE_PATH}/include_p/TpGUI/CSSParser/Vendor/Utils)
include_directories(${SOURCE_PATH}/include_p/TpGUI/CSSParser/Vendor/gumbo)
include_directories(${SOURCE_PATH}/include_p/TpGUI/CSSParser/CSSParser)
include_directories(${SOURCE_PATH}/include_p/TpGUI/CSSParser/CSSParser/Keyword)
include_directories(${SOURCE_PATH}/include_p/TpGUI/CSSParser/CSSParser/Selectors)

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
set(THORVG_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libthorvg.a)
# set(ERPC_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/liberpc.a)

# 库链接到可执行目标
target_link_libraries(TpGUI PUBLIC
    dl pthread
    ${THORVG_STATIC_LIB}
    # ${ERPC_STATIC_LIB}
    ${DEPEND_DYNAMIC_LIB_PATH}/libTpWMEngine.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpCore.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpUtils.so
)
