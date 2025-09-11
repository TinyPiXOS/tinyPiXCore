cmake_minimum_required(VERSION 3.20.0)
project(PiXUtils VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../cmake/CreateTinyCoreSymlink.cmake)

# 添加源码目录
set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/src/Utils/Service/TpSystemApi.cpp
    ${SOURCE_PATH}/src/Utils/Service/TpAppConfigIO.cpp
    ${SOURCE_PATH}/src/Utils/Service/TpWidgetsGateway.cpp
    ${SOURCE_PATH}/src/Utils/TpRect.cpp
    ${SOURCE_PATH}/src/Utils/TpRectF.cpp
    ${SOURCE_PATH}/src/Utils/TpPoint.cpp
    ${SOURCE_PATH}/src/Utils/TpPointF.cpp
    ${SOURCE_PATH}/src/Utils/TpSize.cpp
    ${SOURCE_PATH}/src/Utils/TpSizeF.cpp
    ${SOURCE_PATH}/src/Utils/TpSurface.cpp
    ${SOURCE_PATH}/src/Utils/TpFont.cpp
    ${SOURCE_PATH}/src/Utils/TpPainter.cpp
    ${SOURCE_PATH}/src/Utils/TpRange.cpp
    ${SOURCE_PATH}/src/Utils/TpUUID.cpp
    ${SOURCE_PATH}/src/Utils/TpMD5.cpp
    ${SOURCE_PATH}/src/Utils/TpClipboard.cpp
    ${SOURCE_PATH}/src/Utils/TpConfig.cpp
    ${SOURCE_PATH}/src/Utils/TpThread.cpp
    ${SOURCE_PATH}/src/Utils/TpTimer.cpp
    ${SOURCE_PATH}/src/Utils/TpVariant.cpp
    ${SOURCE_PATH}/src/Utils/TpMap.cpp
    ${SOURCE_PATH}/src/Utils/TpString.cpp
    ${SOURCE_PATH}/src/Utils/TpVector.cpp
    ${SOURCE_PATH}/src/Utils/TpStack.cpp
    ${SOURCE_PATH}/src/Utils/TpColors.cpp
    ${SOURCE_PATH}/src/Utils/TpFile.cpp
    ${SOURCE_PATH}/src/Utils/TpDir.cpp
    ${SOURCE_PATH}/src/Utils/TpFileInfo.cpp
    ${SOURCE_PATH}/src/Utils/TpCursor.cpp
    ${SOURCE_PATH}/src/Utils/TpRegex.cpp
    ${SOURCE_PATH}/src/Utils/TpPipe.cpp
    ${SOURCE_PATH}/src/Utils/TpGateway.cpp
    ${SOURCE_PATH}/src/Utils/TpProcess.cpp
    ${SOURCE_PATH}/src/Utils/TpDate.cpp
    ${SOURCE_PATH}/src/Utils/TpTime.cpp
    ${SOURCE_PATH}/src/Utils/TpDateTime.cpp
    ${SOURCE_PATH}/src/Utils/TpInputMethod.cpp
    ${SOURCE_PATH}/src/Utils/TpCssParser.cpp
    ${SOURCE_PATH}/src/Utils/TpCssData.cpp
    ${SOURCE_PATH}/src/Utils/TpJsonDocument.cpp
    ${SOURCE_PATH}/src/Utils/TpJsonArray.cpp
    ${SOURCE_PATH}/src/Utils/TpJsonObject.cpp
    ${SOURCE_PATH}/src/Utils/TpJsonValue.cpp
    ${SOURCE_PATH}/src/Utils/TpThreadPool.cpp
    ${SOURCE_PATH}/src/Utils/TpImage.cpp
    ${SOURCE_PATH}/src/Utils/TpGradient.cpp
    ${SOURCE_PATH}/src/Utils/TpLinearGradient.cpp
    ${SOURCE_PATH}/src/Utils/TpRadialGradient.cpp
    ${SOURCE_PATH}/src/Utils/TpHardwareDevice.cpp
    ${SOURCE_PATH}/src/Utils/TpGraphicsEffect.cpp
    ${SOURCE_PATH}/src/Utils/TpGraphicsBlurEffect.cpp
    ${SOURCE_PATH}/src/Utils/TpPainterPath.cpp
	${SOURCE_PATH}/src/Utils/TpThreadManage.cpp
	${SOURCE_PATH}/src/Utils/TpNetworkDatagram.cpp
	${SOURCE_PATH}/src/Utils/TpSocket.cpp
	${SOURCE_PATH}/src/Utils/TpTcpSocket.cpp
	${SOURCE_PATH}/src/Utils/TpTcpServer.cpp
	${SOURCE_PATH}/src/Utils/TpUdpSocket.cpp
	${SOURCE_PATH}/src/Utils/TpPen.cpp
	${SOURCE_PATH}/src/Utils/TpBrush.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/CSSParser/CSSParser.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/CSSParser/CSSLex.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/CSSParser/Keyword/KeywordItem.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/CSSParser/Selectors/ClassSelector.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/CSSParser/Selectors/PseudoSelector.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/CSSParser/Selectors/SelectorGroup.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/CSSParser/Selectors/SignSelector.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/CSSParser/Selectors/CombineSelector.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/CSSParser/Selectors/AttributeSelector.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/CSSParser/Selectors/SelectorSequence.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/CSSParser/Selectors/UniversalSelector.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/CSSParser/Selectors/TypeSelector.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/CSSParser/Selectors/Selector.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/CSSParser/Selectors/IdSelector.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/Applicability/HTMLCSSRefAdaptor.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/Applicability/PseudoSelectorRefChiefJudge.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/Vendor/gumbo/string_buffer.c
    ${SOURCE_PATH}/src_p/Utils/CSSParser/Vendor/gumbo/util.c
    ${SOURCE_PATH}/src_p/Utils/CSSParser/Vendor/gumbo/vector.c
    ${SOURCE_PATH}/src_p/Utils/CSSParser/Vendor/gumbo/attribute.c
    ${SOURCE_PATH}/src_p/Utils/CSSParser/Vendor/gumbo/UrlUtil.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/Vendor/gumbo/char_ref.c
    ${SOURCE_PATH}/src_p/Utils/CSSParser/Vendor/gumbo/utf8.c
    ${SOURCE_PATH}/src_p/Utils/CSSParser/Vendor/gumbo/error.c
    ${SOURCE_PATH}/src_p/Utils/CSSParser/Vendor/gumbo/tag.c
    ${SOURCE_PATH}/src_p/Utils/CSSParser/Vendor/gumbo/GumboInterface.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/Vendor/gumbo/parser.c
    ${SOURCE_PATH}/src_p/Utils/CSSParser/Vendor/gumbo/tokenizer.c
    ${SOURCE_PATH}/src_p/Utils/CSSParser/Vendor/gumbo/string_piece.c
    ${SOURCE_PATH}/src_p/Utils/CSSParser/Vendor/gumbo/gumbo_edit.c
    ${SOURCE_PATH}/src_p/Utils/CSSParser/Vendor/Utils/ContainerUtil.cpp
    ${SOURCE_PATH}/src_p/Utils/CSSParser/Vendor/Utils/StringUtil.cpp
    ${SOURCE_PATH}/src_p/Utils/tweeny/tweeny.tcc
    ${SOURCE_PATH}/src_p/Utils/tweeny/tween.tcc
    ${SOURCE_PATH}/src_p/Utils/tweeny/tweenone.tcc
    ${SOURCE_PATH}/src_p/Utils/tweeny/tweenpoint.tcc
    ${SOURCE_PATH}/src_p/Utils/TpMessage.cpp
	${SOURCE_PATH}/src_p/Utils/TpHostAddress.cpp
	${SOURCE_PATH}/src_p/Utils/TpSocketNotifier.cpp
	${SOURCE_PATH}/src_p/Utils/TpSocketNotifierManager.cpp
    ${SOURCE_PATH}/src_p/Utils/TpShareTrans.cpp
)

include_directories(/usr/include)
include_directories(/usr/include/PiXWM)
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
include_directories(${SOURCE_PATH}/include/ExternUtils)
include_directories(${SOURCE_PATH}/include/Utils)
include_directories(${SOURCE_PATH}/include/Utils/JsonStructPackage)
include_directories(${SOURCE_PATH}/include/SingleGUI)
include_directories(${SOURCE_PATH}/include/SingleGUI/Core)
include_directories(${SOURCE_PATH}/include/SingleGUI/Screen)
include_directories(${SOURCE_PATH}/include/SingleGUI/Widgets)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/ExternUtils/SystemLib)
include_directories(${SOURCE_PATH}/include_p/Utils)
include_directories(${SOURCE_PATH}/include_p/Api)
include_directories(${SOURCE_PATH}/include_p/SingleGUI)
include_directories(${SOURCE_PATH}/include_p/Utils/CSSParser)
include_directories(${SOURCE_PATH}/include_p/Utils/CSSParser/Applicability)
include_directories(${SOURCE_PATH}/include_p/Utils/CSSParser/Vendor)
include_directories(${SOURCE_PATH}/include_p/Utils/CSSParser/Vendor/Utils)
include_directories(${SOURCE_PATH}/include_p/Utils/CSSParser/Vendor/gumbo)
include_directories(${SOURCE_PATH}/include_p/Utils/CSSParser/CSSParser)
include_directories(${SOURCE_PATH}/include_p/Utils/CSSParser/CSSParser/Keyword)
include_directories(${SOURCE_PATH}/include_p/Utils/CSSParser/CSSParser/Selectors)
include_directories(${SOURCE_PATH}/include_p/Utils/tweeny)
include_directories(${SOURCE_PATH}/include_p/Utils/rlottie)
include_directories(${SOURCE_PATH}/include_p/Utils/rlottie/lottie)
include_directories(${SOURCE_PATH}/include_p/Utils/rlottie/vector)

# 添加动态库目标
add_library(PiXUtils SHARED ${SOURCE_FILES})

# 设置动态库版本 VERSION 是库的当前版本号，而 SOVERSION 是API的兼容性版本号。
# set_target_properties(PiXUtils PROPERTIES VERSION 1.0 SOVERSION 1)

# 指定依赖动态库目录
link_directories("/usr/lib")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86|i686|i386|AMD64")
    set(NANOMSG_STATIC_LIB ${SOURCE_PATH}/depend_lib/static/x86_x64/libnanomsg.a)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
    set(NANOMSG_STATIC_LIB ${SOURCE_PATH}/depend_lib/static/arm_64/libnanomsg.a)
endif()

# 将pthread库链接到可执行目标
target_link_libraries(PiXUtils ${NANOMSG_STATIC_LIB} dl pthread PiXApi SDL2 SDL2_image SDL2_gfx
    cairo pango-1.0 gobject-2.0 glib-2.0 pangocairo-1.0 fontconfig freetype gbm GLESv2 EGL gio-2.0 rsvg-2 rime thorvg
)

# 如果你需要将动态库安装到特定的目录，可以修改DESTINATION路径
install(TARGETS PiXUtils LIBRARY DESTINATION ${INSTALL_LIB_DIR})

# 创建软链接
cmake_create_core_symlink(
    TARGET PiXUtils
    INSTALL_DIR ${INSTALL_LIB_DIR}
    LINK_DIR ${LINK_LIB_DIR}
)
