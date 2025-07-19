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
    ${SOURCE_PATH}/src/Utils/tpRect.cpp
    ${SOURCE_PATH}/src/Utils/tpPoint.cpp
    ${SOURCE_PATH}/src/Utils/tpSize.cpp
    ${SOURCE_PATH}/src/Utils/tpSurface.cpp
    ${SOURCE_PATH}/src/Utils/tpFont.cpp
    ${SOURCE_PATH}/src/Utils/tpCanvas.cpp
    ${SOURCE_PATH}/src/Utils/tpRange.cpp
    ${SOURCE_PATH}/src/Utils/tpUUID.cpp
    ${SOURCE_PATH}/src/Utils/tpMD5.cpp
    ${SOURCE_PATH}/src/Utils/tpClipboard.cpp
    ${SOURCE_PATH}/src/Utils/tpConfig.cpp
    ${SOURCE_PATH}/src/Utils/tpThread.cpp
    ${SOURCE_PATH}/src/Utils/tpTimer.cpp
    ${SOURCE_PATH}/src/Utils/tpVariant.cpp
    ${SOURCE_PATH}/src/Utils/tpMap.cpp
    ${SOURCE_PATH}/src/Utils/tpString.cpp
    ${SOURCE_PATH}/src/Utils/tpVector.cpp
    ${SOURCE_PATH}/src/Utils/tpStack.cpp
    ${SOURCE_PATH}/src/Utils/tpColors.cpp
    ${SOURCE_PATH}/src/Utils/tpFile.cpp
    ${SOURCE_PATH}/src/Utils/tpDir.cpp
    ${SOURCE_PATH}/src/Utils/tpFileInfo.cpp
    ${SOURCE_PATH}/src/Utils/tpCursor.cpp
    ${SOURCE_PATH}/src/Utils/tpRegex.cpp
    ${SOURCE_PATH}/src/Utils/tpPipe.cpp
    ${SOURCE_PATH}/src/Utils/tpGateway.cpp
    ${SOURCE_PATH}/src/Utils/tpProcess.cpp
    ${SOURCE_PATH}/src/Utils/tpDate.cpp
    ${SOURCE_PATH}/src/Utils/tpTime.cpp
    ${SOURCE_PATH}/src/Utils/tpDateTime.cpp
    ${SOURCE_PATH}/src/Utils/tpInputMethod.cpp
    ${SOURCE_PATH}/src/Utils/tpSystemApi.cpp
    ${SOURCE_PATH}/src/Utils/tpCssParser.cpp
	${SOURCE_PATH}/src/Utils/tpCssData.cpp
	${SOURCE_PATH}/src/Utils/tpJsonDocument.cpp
	${SOURCE_PATH}/src/Utils/tpJsonArray.cpp
	${SOURCE_PATH}/src/Utils/tpJsonObject.cpp
	${SOURCE_PATH}/src/Utils/tpJsonValue.cpp
    ${SOURCE_PATH}/src/Utils/tpAppConfigIO.cpp
    ${SOURCE_PATH}/src/Utils/tpThreadPool.cpp
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
    ${SOURCE_PATH}/src_p/Utils/tpMessage.cpp
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
include_directories(${SOURCE_PATH}/include/SingleGUI)
include_directories(${SOURCE_PATH}/include/SingleGUI/core)
include_directories(${SOURCE_PATH}/include/SingleGUI/screen)
include_directories(${SOURCE_PATH}/include/SingleGUI/widgets)
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
target_link_libraries(PiXUtils ${NANOMSG_STATIC_LIB} dl pthread PiXApi SDL2 SDL2_image SDL2_gfx cairo pango-1.0 gobject-2.0
    glib-2.0 pangocairo-1.0 fontconfig freetype gbm GLESv2 EGL gio-2.0 rsvg-2 rime
)

# 如果你需要将动态库安装到特定的目录，可以修改DESTINATION路径
install(TARGETS PiXUtils LIBRARY DESTINATION ${INSTALL_LIB_DIR})

# 创建软链接
cmake_create_core_symlink(
    TARGET PiXUtils
    INSTALL_DIR ${INSTALL_LIB_DIR}
    LINK_DIR ${LINK_LIB_DIR}             
)
