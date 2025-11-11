cmake_minimum_required(VERSION 3.20.0)
project(TpUtils VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../../cmake/TargetArchConfig.cmake)

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/src/TpUtils/Service/TpSystemApi.cpp
    ${SOURCE_PATH}/src/TpUtils/Service/TpAppConfigIO.cpp
    ${SOURCE_PATH}/src/TpUtils/Service/TpWidgetsGateway.cpp
    ${SOURCE_PATH}/src/TpUtils/TpSurface.cpp
    ${SOURCE_PATH}/src/TpUtils/TpFont.cpp
    ${SOURCE_PATH}/src/TpUtils/TpPainter.cpp
    ${SOURCE_PATH}/src/TpUtils/TpUuid.cpp
    ${SOURCE_PATH}/src/TpUtils/TpMD5.cpp
    ${SOURCE_PATH}/src/TpUtils/TpClipboard.cpp
    ${SOURCE_PATH}/src/TpUtils/TpConfig.cpp
    ${SOURCE_PATH}/src/TpUtils/TpThread.cpp
    ${SOURCE_PATH}/src/TpUtils/TpColors.cpp
    ${SOURCE_PATH}/src/TpUtils/TpFile.cpp
    ${SOURCE_PATH}/src/TpUtils/TpDir.cpp
    ${SOURCE_PATH}/src/TpUtils/TpFileInfo.cpp
    ${SOURCE_PATH}/src/TpUtils/TpCursor.cpp
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

# 添加包含目录
include_directories(${SYSTEM_INCLUDE_PATH})
include_directories(${SYSTEM_INCLUDE_PATH}/SDL2)
include_directories(${SYSTEM_INCLUDE_PATH}/freetype2)
include_directories(${SYSTEM_INCLUDE_PATH}/cairo)
include_directories(${SYSTEM_INCLUDE_PATH}/pango-1.0)
include_directories(${SYSTEM_INCLUDE_PATH}/harfbuzz)
include_directories(${SYSTEM_INCLUDE_PATH}/librsvg-2.0/librsvg/)
include_directories(${SYSTEM_INCLUDE_PATH}/librsvg-2.0)
include_directories(${SYSTEM_INCLUDE_PATH}/gdk-pixbuf-2.0)
include_directories(${SYSTEM_INCLUDE_PATH}/harfbuzz)

include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/TpCore)
include_directories(${SOURCE_PATH}/include/TpWM)
include_directories(${SOURCE_PATH}/include/TpExtUtils)
include_directories(${SOURCE_PATH}/include/TpUtils)
include_directories(${SOURCE_PATH}/include/TpUtils/JsonStructPackage)
include_directories(${SOURCE_PATH}/include/TpGUI)
include_directories(${SOURCE_PATH}/include/TpGUI/Core)
include_directories(${SOURCE_PATH}/include/TpGUI/Screen)
include_directories(${SOURCE_PATH}/include/TpGUI/Widgets)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/TpCore)
include_directories(${SOURCE_PATH}/include_p/TpUtils)
include_directories(${SOURCE_PATH}/include_p/TpGUI)
include_directories(${SOURCE_PATH}/include_p/TpGUI/Core)
include_directories(${SOURCE_PATH}/include_p/TpGUI/Screen)
include_directories(${SOURCE_PATH}/include_p/TpExtUtils/SystemLib)
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
if(CMAKE_CROSSCOMPILING)
    link_directories("${LOAD_SDK_LIB_PATH}")
else()
    link_directories("/usr/lib")
endif()

LoadSDKLibrary(SDL2_LIBRARY SDL2)
LoadSDKLibrary(CAIRO_LIBRARY cairo)
LoadSDKLibrary(PANGO_LIBRARY pango-1.0)
LoadSDKLibrary(GOBJECT_LIBRARY gobject-2.0)
LoadSDKLibrary(GLIB_LIBRARY glib-2.0)
LoadSDKLibrary(PANGOCAIRO_LIBRARY pangocairo-1.0)
LoadSDKLibrary(FONTCONFIG_LIBRARY fontconfig)
LoadSDKLibrary(FREETYPE_LIBRARY freetype)
LoadSDKLibrary(GBM_LIBRARY gbm)
LoadSDKLibrary(GLESV2_LIBRARY GLESv2)
LoadSDKLibrary(EGL_LIBRARY EGL)
LoadSDKLibrary(GIO_LIBRARY gio-2.0)
LoadSDKLibrary(RSVG_LIBRARY rsvg-2)

# 依赖的静态库
set(NANOMSG_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libnanomsg.a)
set(THORVG_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libthorvg.a)

target_link_libraries(TpUtils PUBLIC 
    dl pthread 
    ${NANOMSG_STATIC_LIB} ${THORVG_STATIC_LIB} 
    ${SDL2_LIBRARY} ${CARIO_LIBRARY} ${PANGO_LIBRARY}
    ${GOBJECT_LIBRARY} ${GLIB_LIBRARY} ${PANGOCAIRO_LIBRARY}
    ${FONTCONFIG_LIBRARY} ${FREETYPE_LIBRARY} ${GBM_LIBRARY}
    ${GLESV2_LIBRARY} ${EGL_LIBRARY} ${GIO_LIBRARY} ${RSVG_LIBRARY}
    ${DEPEND_DYNAMIC_LIB_PATH}/librime.so.1.5.3
    ${DEPEND_DYNAMIC_LIB_PATH}/libTpWMEngine.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpCore.so
)
