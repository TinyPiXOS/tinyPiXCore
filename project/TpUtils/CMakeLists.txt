cmake_minimum_required(VERSION 3.20.0)
project(TpUtils VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../../cmake/TargetArchConfig.cmake)

# 定义源码路径宏
set(SOURCE_FILES
    ${SOURCE_PATH}/src/TpUtils/TpInputMethod.cpp
    ${SOURCE_PATH}/src/TpUtils/TpSVG.cpp
    ${SOURCE_PATH}/src/TpUtils/TpChineseCalendar.cpp
    ${SOURCE_PATH}/src/TpUtils/TpHardwareDevice.cpp
    ${SOURCE_PATH}/src/TpUtils/TpGateway.cpp
    ${SOURCE_PATH}/src/TpUtils/TpSystemInfo.cpp

    ${SOURCE_PATH}/src/TpUtils/Service/TpSystemApi.cpp
    ${SOURCE_PATH}/src/TpUtils/Service/TpAppConfigIO.cpp
    ${SOURCE_PATH}/src/TpUtils/Service/TpWidgetsGateway.cpp

    ${SOURCE_PATH}/src/TpUtils/Network/TpNetworkDatagram.cpp
    ${SOURCE_PATH}/src/TpUtils/Network/TpSocket.cpp
    ${SOURCE_PATH}/src/TpUtils/Network/TpTcpSocket.cpp
    ${SOURCE_PATH}/src/TpUtils/Network/TpTcpServer.cpp
    ${SOURCE_PATH}/src/TpUtils/Network/TpUdpSocket.cpp
    ${SOURCE_PATH}/src/TpUtils/Network/TpNetworkInfo.cpp
    ${SOURCE_PATH}/src/TpUtils/Network/TpNetworkManage.cpp
    ${SOURCE_PATH}/src/TpUtils/Network/TpNetworkInterface.cpp
    ${SOURCE_PATH}/src/TpUtils/Network/TpWirelessInfo.cpp

    ${SOURCE_PATH}/src/TpUtils/SystemDevice/TpCpu.cpp
    ${SOURCE_PATH}/src/TpUtils/SystemDevice/TpCpuManage.cpp
    ${SOURCE_PATH}/src/TpUtils/SystemDevice/TpGpu.cpp
    ${SOURCE_PATH}/src/TpUtils/SystemDevice/TpDisk.cpp
    ${SOURCE_PATH}/src/TpUtils/SystemDevice/TpDiskManage.cpp
    ${SOURCE_PATH}/src/TpUtils/SystemDevice/TpMemory.cpp
    ${SOURCE_PATH}/src/TpUtils/SystemDevice/TpProcessInfo.cpp
    ${SOURCE_PATH}/src/TpUtils/SystemDevice/TpProcessManage.cpp

    ${SOURCE_PATH}/src/TpUtils/Bluetooth/TpBluetoothAddress.cpp
    ${SOURCE_PATH}/src/TpUtils/Bluetooth/TpBluetoothUuid.cpp
    ${SOURCE_PATH}/src/TpUtils/Bluetooth/TpBluetoothLocal.cpp
    ${SOURCE_PATH}/src/TpUtils/Bluetooth/TpBluetoothDevice.cpp
    ${SOURCE_PATH}/src/TpUtils/Bluetooth/TpBluetoothDiscovery.cpp
    ${SOURCE_PATH}/src/TpUtils/Bluetooth/TpBluetoothService.cpp
    ${SOURCE_PATH}/src/TpUtils/Bluetooth/TpBluetoothServiceDiscovery.cpp
    ${SOURCE_PATH}/src/TpUtils/Bluetooth/TpBluetoothSocket.cpp
    ${SOURCE_PATH}/src/TpUtils/Bluetooth/TpBluetoothServer.cpp
    ${SOURCE_PATH}/src/TpUtils/Bluetooth/TpBluetoothTransfer.cpp
    ${SOURCE_PATH}/src/TpUtils/Bluetooth/TpBluetoothTransferAgent.cpp
    ${SOURCE_PATH}/src/TpUtils/Bluetooth/TpBluetoothAudioDevice.cpp
    ${SOURCE_PATH}/src/TpUtils/Bluetooth/TpBluetoothAudioManager.cpp

    ${SOURCE_PATH}/src_p/TpUtils/TpShareTrans.cpp
    ${SOURCE_PATH}/src_p/TpUtils/TpThreadManage.cpp
    ${SOURCE_PATH}/src_p/TpUtils/TpDbusConnectManage.cpp
    ${SOURCE_PATH}/src_p/TpUtils/TpSystemDataManage.cpp
    ${SOURCE_PATH}/src_p/TpUtils/TpSocketNotifier.cpp
    ${SOURCE_PATH}/src_p/TpUtils/TpSocketNotifierManager.cpp

    ${SOURCE_PATH}/src_p/TpUtils/tweeny/tweeny.tcc
    ${SOURCE_PATH}/src_p/TpUtils/tweeny/tween.tcc
    ${SOURCE_PATH}/src_p/TpUtils/tweeny/tweenone.tcc
    ${SOURCE_PATH}/src_p/TpUtils/tweeny/tweenpoint.tcc

    ${SOURCE_PATH}/src_p/TpUtils/Network/TpHostAddress.cpp
    ${SOURCE_PATH}/src_p/TpUtils/Network/NetworkConflib.c
    ${SOURCE_PATH}/src_p/TpUtils/Network/NetworkConf.c
    ${SOURCE_PATH}/src_p/TpUtils/Network/TpNetPcap.cpp

    ${SOURCE_PATH}/src/TpUtils/Media/TpAudioInterface.cpp
    ${SOURCE_PATH}/src/TpUtils/Media/TpVideoInterface.cpp
    ${SOURCE_PATH}/src/TpUtils/Media/TpMediaFile.cpp
    ${SOURCE_PATH}/src/TpUtils/Media/TpSound.cpp
    ${SOURCE_PATH}/src/TpUtils/Media/TpRecordInterface.cpp

    ${SOURCE_PATH}/src_p/TpUtils/Media/Media/media.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Media/media_timer.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Media/media_file_list.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Media/media_codec.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Media/media_play_temp.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Audio/audio_codec.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Audio/audio_manage.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Audio/audio_play.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Audio/audio_record.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Audio/hard.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Audio/filter.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Video/video_display.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Video/video_codec.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Video/video_manage.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Video/video_play.c

    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/lib/agent-helper.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/lib/g_utils.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/lib/bluez_alsa.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/lib/freedesktop_systemd.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/lib/media_transport.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/lib/service.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/lib/bluez/adapter.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/lib/bluez/agent_manager.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/lib/bluez/device.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/lib/bluez/profile.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/lib/bluez/filter.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/lib/obex/obex_agent_manager.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/lib/obex/obex_agent.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/lib/obex/obex_transfer.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/lib/obex/obex_session.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/lib/obex/obex_client.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/lib/obex/obex_object_push.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/sdpregister.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/blt_audio.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/blt_agent.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/blt_dbussignal.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/blt_device.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/blt_file.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/blt_gatt.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/blt_glibscan.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/blt_hard.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/blt_hid.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/blt_sdp.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/blt_service.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/src/blt_service_reg.c

    ${SOURCE_PATH}/src_p/TpUtils/TpDbus/manager.c
    ${SOURCE_PATH}/src_p/TpUtils/TpDbus/properties.c
    ${SOURCE_PATH}/src_p/TpUtils/TpDbus/connect.c
    ${SOURCE_PATH}/src_p/TpUtils/TpDbus/network_manager.c
    ${SOURCE_PATH}/src_p/TpUtils/TpDbus/freedesktop_upower.c
    ${SOURCE_PATH}/src_p/TpUtils/TpDbus/logind.c

    ${SOURCE_PATH}/src_p/TpUtils/utils/utilslib.c
    ${SOURCE_PATH}/src_p/TpUtils/utils/utlist.c
    ${SOURCE_PATH}/src_p/TpUtils/utils/variable_array.c
    ${SOURCE_PATH}/src_p/TpUtils/utils/utthread.c



    ${SOURCE_PATH}/src_p/TpUtils/fileUtils.c

    ${SOURCE_PATH}/src_p/TpUtils/AppManage/dopack/json_conf.c
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/dopack/appm_creat.c
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/dopack/file_creat_lib.c
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/dopack/file_creat.c
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/manage/utilslib.c
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/manage/system_permission.c
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/manage/secret_key.c
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/manage/conf_json.c
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/manage/install_check.c
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/manage/install.c
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/manage/appinfo.c
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/manage/purview.c
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/manage/unload.c
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/manage/unpack.c
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/manage/update.c

    ${SOURCE_PATH}/src_p/TpUtils/usb/src/usb_manager.c
    ${SOURCE_PATH}/src_p/TpUtils/devmanager/src/dev_manager.c
    ${SOURCE_PATH}/src_p/TpUtils/devmanager/src/dev_monitor.c
    ${SOURCE_PATH}/src/TpUtils/TpAppDataDisk.cpp
    ${SOURCE_PATH}/src/TpUtils/TpDisplay.cpp
    ${SOURCE_PATH}/src/TpUtils/TpCompress.cpp

    ${SOURCE_PATH}/src/TpUtils/TpAppInstall.cpp
    ${SOURCE_PATH}/src/TpUtils/TpLibDopack.cpp
    ${SOURCE_PATH}/src/TpUtils/TpAppDopack.cpp
    ${SOURCE_PATH}/src/TpUtils/TpSystemBatteryInfo.cpp
    ${SOURCE_PATH}/src/TpUtils/TpUsbDevice.cpp
    ${SOURCE_PATH}/src/TpUtils/TpUsbDeviceInfo.cpp
    ${SOURCE_PATH}/src/TpUtils/TpUsbManager.cpp
    ${SOURCE_PATH}/src/TpUtils/TpGpio.cpp
    ${SOURCE_PATH}/src/TpUtils/TpHardwareI2c.cpp
    ${SOURCE_PATH}/src/TpUtils/TpHardwarePwm.cpp
    ${SOURCE_PATH}/src/TpUtils/TpLogin.cpp
    ${SOURCE_PATH}/src/TpUtils/TpSerialPort.cpp

    # ${SOURCE_PATH}/src_p/TpUtils/bluetooth/src/obexnames.c
    # ${SOURCE_PATH}/src/TpUtils/TpDisplayManage.cpp
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
include_directories(${SYSTEM_INCLUDE_PATH}/dbus-1.0)
include_directories(${SYSTEM_INCLUDE_PATH}/librsvg-2.0/librsvg/)
include_directories(${SYSTEM_INCLUDE_PATH}/librsvg-2.0)
include_directories(${SYSTEM_INCLUDE_PATH}/gdk-pixbuf-2.0)
include_directories(${SYSTEM_INCLUDE_PATH}/harfbuzz)
include_directories(${SYSTEM_INCLUDE_PATH}/json-c)

include_directories(/usr/lib/x86_64-linux-gnu/dbus-1.0/include)

include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/TpCore)
include_directories(${SOURCE_PATH}/include/TpWM)
include_directories(${SOURCE_PATH}/include/TpUtils)
include_directories(${SOURCE_PATH}/include/TpUtils)
include_directories(${SOURCE_PATH}/include/TpUtils/JsonStructPackage)

include_directories(${SOURCE_PATH}/include/TpUtils/Network)

include_directories(${SOURCE_PATH}/include/TpUtils/SystemDevice)

include_directories(${SOURCE_PATH}/include/TpUtils/Bluetooth)
include_directories(${SOURCE_PATH}/include/TpUtils/Media)

include_directories(${SOURCE_PATH}/include/TpGUI)
include_directories(${SOURCE_PATH}/include/TpGUI/GUI)
include_directories(${SOURCE_PATH}/include/TpGUI/Widgets)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/TpUtils)
include_directories(${SOURCE_PATH}/include_p/TpCore)
include_directories(${SOURCE_PATH}/include_p/TpGUI)
include_directories(${SOURCE_PATH}/include_p/TpGUI/GUI)

include_directories(${SOURCE_PATH}/include_p/TpUtils/Media)

include_directories(${SOURCE_PATH}/include_p/TpUtils/SystemLib)
include_directories(${SOURCE_PATH}/include_p/TpUtils/tweeny)
include_directories(${SOURCE_PATH}/include_p/TpUtils/utils)
include_directories(${SOURCE_PATH}/include_p/TpUtils/Network)
include_directories(${SOURCE_PATH}/include_p/TpUtils/TpDbus)

include_directories(${SOURCE_PATH}/src_p/TpUtils/Bluetooth)
include_directories(${SOURCE_PATH}/src_p/TpUtils/Bluetooth/include)
include_directories(${SOURCE_PATH}/src_p/TpUtils/Bluetooth/include/lib)
include_directories(${SOURCE_PATH}/src_p/TpUtils/Bluetooth/include/lib/bluez)
include_directories(${SOURCE_PATH}/src_p/TpUtils/Bluetooth/include/lib/obex)

include_directories(${SOURCE_PATH}/src_p/TpUtils/AppManage/manage)
include_directories(${SOURCE_PATH}/src_p/TpUtils/AppManage/dopack)
include_directories(${SOURCE_PATH}/src_p/TpUtils/AppManage/inc)
include_directories(${SOURCE_PATH}/src_p/TpUtils/usb/include)
include_directories(${SOURCE_PATH}/src_p/TpUtils/devmanager/include)

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
LoadSDKLibrary(SSL_LIBRARY ssl)
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

# 依赖的静态库
set(NANOMSG_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libnanomsg.a)
set(THORVG_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libthorvg.a)
set(PCAP_LIB_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libpcap.a)
set(TPARCHIVE_LIB_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libtparchive.a)

target_link_libraries(TpUtils PUBLIC
    dl pthread
    ${PCAP_LIB_STATIC_LIB} ${TPARCHIVE_LIB_STATIC_LIB} 
    ${NANOMSG_STATIC_LIB} ${THORVG_STATIC_LIB}
    ${SDL2_LIBRARY} ${CARIO_LIBRARY} ${PANGO_LIBRARY}
    ${GOBJECT_LIBRARY} ${GLIB_LIBRARY} ${PANGOCAIRO_LIBRARY}
    ${FONTCONFIG_LIBRARY} ${FREETYPE_LIBRARY} ${GBM_LIBRARY}
    ${GLESV2_LIBRARY} ${EGL_LIBRARY} ${GIO_LIBRARY} ${RSVG_LIBRARY}
    ${PCAP_LIB_STATIC_LIB} ${TPARCHIVE_LIB_STATIC_LIB} 
    ${SSL_LIBRARY} ${AVFILTER_LIBRARY} ${AVFORMAT_LIBRARY}
    ${JSONC_LIBRARY} ${ASOUND_LIBRARY} ${AVCODEC_LIBRARY} ${SWSCALE_LIBRARY}
    ${AVUTIL_LIBRARY} ${SWRESAMPLE_LIBRARY} ${AVDEVICE_LIBRARY} 
    ${BLUETOOTH_LIBRARY} ${DBUS_LIBRARY} ${USB_LIBRARY} ${UDEV_LIBRARY}
    ${DEPEND_DYNAMIC_LIB_PATH}/librime.so.1.5.3
    ${DEPEND_DYNAMIC_LIB_PATH}/libTpWMEngine.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpCore.so
)
