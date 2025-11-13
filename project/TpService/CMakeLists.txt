cmake_minimum_required(VERSION 3.20.0)
project(TpService VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../../cmake/TargetArchConfig.cmake)

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/src/TpService/main.cpp
    ${SOURCE_PATH}/src/TpService/TpGatewayServerPUBSUB.cpp
    # ${SOURCE_PATH}/src/TpService/TpGatewayServerREP.cpp
)

include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/TpCore)
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

# 指定静态库的路径
set(NANOMSG_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libnanomsg.a)

# 或者为特定目标设置
target_link_options(TpService PRIVATE
    "LINKER:-rpath,${INSTALL_LIB_DIR}"
    "LINKER:-rpath-link,${ARM_SDK_PATH}/lib"
)

find_library(GLOG_LIBRARY glog HINTS ${ARM_SDK_PATH}/lib NO_DEFAULT_PATH)
find_library(YAML_CPP_LIBRARY yaml-cpp HINTS ${ARM_SDK_PATH}/lib NO_DEFAULT_PATH)
find_library(LEVELDB_LIBRARY leveldb HINTS ${ARM_SDK_PATH}/lib NO_DEFAULT_PATH)
find_library(MARISA_LIBRARY marisa HINTS ${ARM_SDK_PATH}/lib NO_DEFAULT_PATH)
find_library(OPENCC_LIBRARY opencc HINTS ${ARM_SDK_PATH}/lib NO_DEFAULT_PATH)
set(OPENSSL_CRYPTO_LIBRARY ${ARM_SDK_PATH}/lib/libcrypto.so)
set(OPENSSL_SSL_LIBRARY ${ARM_SDK_PATH}/lib/libssl.so)
# 库链接到可执行目标
target_link_libraries(TpService
    dl
    pthread
    ${NANOMSG_STATIC_LIB}
    ${DEPEND_DYNAMIC_LIB_PATH}/librime.so.1.5.3
    ${GLOG_LIBRARY}
    ${YAML_CPP_LIBRARY}
    ${LEVELDB_LIBRARY}
    ${MARISA_LIBRARY}
    ${OPENCC_LIBRARY}
    ${DEPEND_DYNAMIC_LIB_PATH}/libTpWMEngine.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpCore.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpGUI.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpUtils.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpExtUtils.so
        ${OPENSSL_CRYPTO_LIBRARY}
    ${OPENSSL_SSL_LIBRARY}
    # ${SDL2_LIBRARY} ${CARIO_LIBRARY} ${PANGO_LIBRARY}
    # ${GOBJECT_LIBRARY} ${GLIB_LIBRARY} ${PANGOCAIRO_LIBRARY}
    # ${FONTCONFIG_LIBRARY} ${FREETYPE_LIBRARY} ${GBM_LIBRARY}
    # ${GLESV2_LIBRARY} ${EGL_LIBRARY} ${GIO_LIBRARY} ${RSVG_LIBRARY}
    # ${GLIB_LIBRARY} ${GIO_LIBRARY} ${SDL2_LIBRARY} ${SSL_LIBRARY}
    # ${JSONC_LIBRARY} ${ASOUND_LIBRARY} ${AVCODEC_LIBRARY} ${AVFILTER_LIBRARY} ${AVFORMAT_LIBRARY}
    # ${AVUTIL_LIBRARY} ${SWRESAMPLE_LIBRARY} ${AVDEVICE_LIBRARY} ${SWSCALE_LIBRARY}
    # ${BLUETOOTH_LIBRARY} ${DBUS_LIBRARY} ${USB_LIBRARY} ${UDEV_LIBRARY}
)
