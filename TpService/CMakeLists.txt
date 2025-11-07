cmake_minimum_required(VERSION 3.20.0)
project(TpService VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../cmake/TargetArchConfig.cmake)

# 添加源码目录
set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/src/TpService/main.cpp
    ${SOURCE_PATH}/src/TpService/TpGatewayServerPUBSUB.cpp
    ${SOURCE_PATH}/src/TpService/TpGatewayServerREP.cpp
)

include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/TpWM)
include_directories(${SOURCE_PATH}/include/TpUtils)
include_directories(${SOURCE_PATH}/include/TpExtUtils)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/TpUtils)

# 添加动态库目标
add_executable(TpService ${SOURCE_FILES})

# 指定依赖动态库目录
if(CMAKE_CROSSCOMPILING)
    link_directories("${LOAD_SDK_LIB_PATH}")
else()
    link_directories("/usr/lib")
endif()

# 添加链接器选项
# target_link_options(TpService PRIVATE
#     "-Wl,-rpath-link,/usr/lib/aarch64-linux-gnu"
#     "-Wl,-rpath-link,${LOAD_SDK_LIB_PATH}"
# )

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
LoadSDKLibrary(JSONC_LIBRARY json-c)
LoadSDKLibrary(ASOUND_LIBRARY asound)
LoadSDKLibrary(AVCODEC_LIBRARY avcodec)
LoadSDKLibrary(AVFILTER_LIBRARY avfilter)
LoadSDKLibrary(AVFORMAT_LIBRARY avformat)
LoadSDKLibrary(AVUTIL_LIBRARY avutil)
LoadSDKLibrary(SWRESAMPLE_LIBRARY swresample)
LoadSDKLibrary(AVDEVICE_LIBRARY avdevice)
LoadSDKLibrary(SWSCALE_LIBRARY swscale)
LoadSDKLibrary(BLUETOOTH_LIBRARY bluetooth)
LoadSDKLibrary(DBUS_LIBRARY dbus-1)
LoadSDKLibrary(USB_LIBRARY usb-1.0)
LoadSDKLibrary(UDEV_LIBRARY udev)

# 指定静态库的路径
set(NANOMSG_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libnanomsg.a)

# 库链接到可执行目标
target_link_libraries(TpService
    dl
    pthread
    ${DEPEND_DYNAMIC_LIB_PATH}/libTpWMEngine.so
    TpGUI
    TpUtils
    TpExtUtils
    ${SDL2_LIBRARY} ${CARIO_LIBRARY} ${PANGO_LIBRARY}
    ${GOBJECT_LIBRARY} ${GLIB_LIBRARY} ${PANGOCAIRO_LIBRARY}
    ${FONTCONFIG_LIBRARY} ${FREETYPE_LIBRARY} ${GBM_LIBRARY}
    ${GLESV2_LIBRARY} ${EGL_LIBRARY} ${GIO_LIBRARY} ${RSVG_LIBRARY}
    ${GLIB_LIBRARY} ${GIO_LIBRARY} ${SDL2_LIBRARY} ${SSL_LIBRARY}
    ${JSONC_LIBRARY} ${ASOUND_LIBRARY} ${AVCODEC_LIBRARY} ${AVFILTER_LIBRARY} ${AVFORMAT_LIBRARY}
    ${AVUTIL_LIBRARY} ${SWRESAMPLE_LIBRARY} ${AVDEVICE_LIBRARY} ${SWSCALE_LIBRARY}
    ${BLUETOOTH_LIBRARY} ${DBUS_LIBRARY} ${USB_LIBRARY} ${UDEV_LIBRARY}
)
