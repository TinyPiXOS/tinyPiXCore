cmake_minimum_required(VERSION 3.20.0)
project(TpUtils VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../cmake/TargetArchConfig.cmake)

# 添加源码目录
set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/src/TpUtils/Service/TpSystemApi.cpp
    ${SOURCE_PATH}/src/TpUtils/Service/TpAppConfigIO.cpp
    ${SOURCE_PATH}/src/TpUtils/Service/TpWidgetsGateway.cpp
    ${SOURCE_PATH}/src/TpUtils/TpRect.cpp
    ${SOURCE_PATH}/src/TpUtils/TpRectF.cpp
    ${SOURCE_PATH}/src/TpUtils/TpPoint.cpp
    ${SOURCE_PATH}/src/TpUtils/TpPointF.cpp
    ${SOURCE_PATH}/src/TpUtils/TpSize.cpp
    ${SOURCE_PATH}/src/TpUtils/TpSizeF.cpp
    ${SOURCE_PATH}/src/TpUtils/TpSurface.cpp
    ${SOURCE_PATH}/src/TpUtils/TpFont.cpp
    ${SOURCE_PATH}/src/TpUtils/TpPainter.cpp
    ${SOURCE_PATH}/src/TpUtils/TpRange.cpp
    ${SOURCE_PATH}/src/TpUtils/TpUuid.cpp
    ${SOURCE_PATH}/src/TpUtils/TpMD5.cpp
    ${SOURCE_PATH}/src/TpUtils/TpClipboard.cpp
    ${SOURCE_PATH}/src/TpUtils/TpConfig.cpp
    ${SOURCE_PATH}/src/TpUtils/TpThread.cpp
    ${SOURCE_PATH}/src/TpUtils/TpTimer.cpp
    ${SOURCE_PATH}/src/TpUtils/TpVariant.cpp
    ${SOURCE_PATH}/src/TpUtils/TpMap.cpp
    ${SOURCE_PATH}/src/TpUtils/TpString.cpp
    ${SOURCE_PATH}/src/TpUtils/TpVector.cpp
    ${SOURCE_PATH}/src/TpUtils/TpStack.cpp
    ${SOURCE_PATH}/src/TpUtils/TpColors.cpp
    ${SOURCE_PATH}/src/TpUtils/TpFile.cpp
    ${SOURCE_PATH}/src/TpUtils/TpDir.cpp
    ${SOURCE_PATH}/src/TpUtils/TpFileInfo.cpp
    ${SOURCE_PATH}/src/TpUtils/TpCursor.cpp
    ${SOURCE_PATH}/src/TpUtils/TpRegex.cpp
    ${SOURCE_PATH}/src/TpUtils/TpPipe.cpp
    ${SOURCE_PATH}/src/TpUtils/TpGateway.cpp
    ${SOURCE_PATH}/src/TpUtils/TpProcess.cpp
    ${SOURCE_PATH}/src/TpUtils/TpDate.cpp
    ${SOURCE_PATH}/src/TpUtils/TpTime.cpp
    ${SOURCE_PATH}/src/TpUtils/TpDateTime.cpp
    ${SOURCE_PATH}/src/TpUtils/TpInputMethod.cpp
    ${SOURCE_PATH}/src/TpUtils/TpCssParser.cpp
    ${SOURCE_PATH}/src/TpUtils/TpCssData.cpp
    ${SOURCE_PATH}/src/TpUtils/TpJsonDocument.cpp
    ${SOURCE_PATH}/src/TpUtils/TpJsonArray.cpp
    ${SOURCE_PATH}/src/TpUtils/TpJsonObject.cpp
    ${SOURCE_PATH}/src/TpUtils/TpJsonValue.cpp
    ${SOURCE_PATH}/src/TpUtils/TpThreadPool.cpp
    ${SOURCE_PATH}/src/TpUtils/TpImage.cpp
    ${SOURCE_PATH}/src/TpUtils/TpGradient.cpp
    ${SOURCE_PATH}/src/TpUtils/TpLinearGradient.cpp
    ${SOURCE_PATH}/src/TpUtils/TpRadialGradient.cpp
    ${SOURCE_PATH}/src/TpUtils/TpHardwareDevice.cpp
    ${SOURCE_PATH}/src/TpUtils/TpGraphicsEffect.cpp
    ${SOURCE_PATH}/src/TpUtils/TpGraphicsBlurEffect.cpp
    ${SOURCE_PATH}/src/TpUtils/TpPainterPath.cpp
	${SOURCE_PATH}/src/TpUtils/TpThreadManage.cpp
	${SOURCE_PATH}/src/TpUtils/TpNetworkDatagram.cpp
	${SOURCE_PATH}/src/TpUtils/TpSocket.cpp
	${SOURCE_PATH}/src/TpUtils/TpTcpSocket.cpp
	${SOURCE_PATH}/src/TpUtils/TpTcpServer.cpp
	${SOURCE_PATH}/src/TpUtils/TpUdpSocket.cpp
	${SOURCE_PATH}/src/TpUtils/TpPen.cpp
	${SOURCE_PATH}/src/TpUtils/TpBrush.cpp
    ${SOURCE_PATH}/src/TpUtils/TpSVG.cpp
    ${SOURCE_PATH}/src/TpUtils/TpChineseCalendar.cpp
    ${SOURCE_PATH}/src/TpUtils/TpShareMemory.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/CSSParser/CSSParser.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/CSSParser/CSSLex.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/CSSParser/Keyword/KeywordItem.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/CSSParser/Selectors/ClassSelector.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/CSSParser/Selectors/PseudoSelector.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/CSSParser/Selectors/SelectorGroup.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/CSSParser/Selectors/SignSelector.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/CSSParser/Selectors/CombineSelector.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/CSSParser/Selectors/AttributeSelector.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/CSSParser/Selectors/SelectorSequence.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/CSSParser/Selectors/UniversalSelector.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/CSSParser/Selectors/TypeSelector.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/CSSParser/Selectors/Selector.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/CSSParser/Selectors/IdSelector.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/Applicability/HTMLCSSRefAdaptor.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/Applicability/PseudoSelectorRefChiefJudge.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/Vendor/gumbo/string_buffer.c
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/Vendor/gumbo/util.c
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/Vendor/gumbo/vector.c
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/Vendor/gumbo/attribute.c
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/Vendor/gumbo/UrlUtil.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/Vendor/gumbo/char_ref.c
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/Vendor/gumbo/utf8.c
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/Vendor/gumbo/error.c
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/Vendor/gumbo/tag.c
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/Vendor/gumbo/GumboInterface.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/Vendor/gumbo/parser.c
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/Vendor/gumbo/tokenizer.c
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/Vendor/gumbo/string_piece.c
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/Vendor/gumbo/gumbo_edit.c
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/Vendor/Utils/ContainerUtil.cpp
    ${SOURCE_PATH}/src_p/TpUtils/CSSParser/Vendor/Utils/StringUtil.cpp
    ${SOURCE_PATH}/src_p/TpUtils/tweeny/tweeny.tcc
    ${SOURCE_PATH}/src_p/TpUtils/tweeny/tween.tcc
    ${SOURCE_PATH}/src_p/TpUtils/tweeny/tweenone.tcc
    ${SOURCE_PATH}/src_p/TpUtils/tweeny/tweenpoint.tcc
    ${SOURCE_PATH}/src_p/TpUtils/TpMessage.cpp
	${SOURCE_PATH}/src_p/TpUtils/TpHostAddress.cpp
	${SOURCE_PATH}/src_p/TpUtils/TpSocketNotifier.cpp
	${SOURCE_PATH}/src_p/TpUtils/TpSocketNotifierManager.cpp
    ${SOURCE_PATH}/src_p/TpUtils/TpShareTrans.cpp
)

include_directories(/usr/include)
include_directories(/usr/include/SDL2)
include_directories(/usr/include/freetype2)
include_directories(/usr/include/cairo)
include_directories(/usr/include/pango-1.0)
include_directories(/usr/include/harfbuzz)
include_directories(/usr/include/glib-2.0)
include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include)
include_directories(/usr/lib/aarch64-linux-gnu/glib-2.0/include)
include_directories(/usr/include/glib-2.0)
include_directories(/usr/include/librsvg-2.0/librsvg/)
include_directories(/usr/include/librsvg-2.0)
include_directories(/usr/include/gdk-pixbuf-2.0)
include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/TpWM)
include_directories(${SOURCE_PATH}/include/TpExtUtils)
include_directories(${SOURCE_PATH}/include/TpUtils)
include_directories(${SOURCE_PATH}/include/TpUtils/JsonStructPackage)
include_directories(${SOURCE_PATH}/include/TpGUI)
include_directories(${SOURCE_PATH}/include/TpGUI/Core)
include_directories(${SOURCE_PATH}/include/TpGUI/Screen)
include_directories(${SOURCE_PATH}/include/TpGUI/Widgets)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/TpExtUtils/SystemLib)
include_directories(${SOURCE_PATH}/include_p/TpUtils)
include_directories(${SOURCE_PATH}/include_p/TpGUI)
include_directories(${SOURCE_PATH}/include_p/TpGUI/Core)
include_directories(${SOURCE_PATH}/include_p/TpGUI/Screen)
include_directories(${SOURCE_PATH}/include_p/TpUtils/CSSParser)
include_directories(${SOURCE_PATH}/include_p/TpUtils/CSSParser/Applicability)
include_directories(${SOURCE_PATH}/include_p/TpUtils/CSSParser/Vendor)
include_directories(${SOURCE_PATH}/include_p/TpUtils/CSSParser/Vendor/Utils)
include_directories(${SOURCE_PATH}/include_p/TpUtils/CSSParser/Vendor/gumbo)
include_directories(${SOURCE_PATH}/include_p/TpUtils/CSSParser/CSSParser)
include_directories(${SOURCE_PATH}/include_p/TpUtils/CSSParser/CSSParser/Keyword)
include_directories(${SOURCE_PATH}/include_p/TpUtils/CSSParser/CSSParser/Selectors)
include_directories(${SOURCE_PATH}/include_p/TpUtils/tweeny)

# 添加动态库目标
add_library(TpUtils SHARED ${SOURCE_FILES})

# 设置动态库版本 VERSION 是库的当前版本号，而 SOVERSION 是API的兼容性版本号。
# set_target_properties(TpUtils PROPERTIES VERSION 1.0 SOVERSION 1)

# 指定依赖动态库目录
link_directories("/usr/lib")

# 依赖的静态库
set(NANOMSG_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libnanomsg.a)
set(THORVG_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libthorvg.a)

target_link_libraries(TpUtils ${NANOMSG_STATIC_LIB} ${THORVG_STATIC_LIB} dl pthread SDL2
    cairo pango-1.0 gobject-2.0 glib-2.0 pangocairo-1.0 fontconfig freetype gbm GLESv2 EGL gio-2.0 rsvg-2
    ${DEPEND_DYNAMIC_LIB_PATH}/librime.so.1.5.3
    ${DEPEND_DYNAMIC_LIB_PATH}/libTpWMEngine.so
)
