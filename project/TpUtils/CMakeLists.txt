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
    ${SOURCE_PATH}/src/TpUtils/TpGateway.cpp
    ${SOURCE_PATH}/src/TpUtils/TpGatewayClientREQ.cpp

    ${SOURCE_PATH}/src/TpUtils/Network/TpNetworkDatagram.cpp
    ${SOURCE_PATH}/src/TpUtils/Network/TpSocket.cpp
    ${SOURCE_PATH}/src/TpUtils/Network/TpTcpSocket.cpp
    ${SOURCE_PATH}/src/TpUtils/Network/TpTcpServer.cpp
    ${SOURCE_PATH}/src/TpUtils/Network/TpUdpSocket.cpp
    ${SOURCE_PATH}/src/TpUtils/Network/TpNetworkInfo.cpp
    ${SOURCE_PATH}/src/TpUtils/Network/TpNetworkManage.cpp
    ${SOURCE_PATH}/src/TpUtils/Network/TpNetworkInterface.cpp
    ${SOURCE_PATH}/src/TpUtils/Network/TpWirelessInfo.cpp

    ${SOURCE_PATH}/src/TpUtils/SystemInfo/TpCpu.cpp
    ${SOURCE_PATH}/src/TpUtils/SystemInfo/TpCpuManage.cpp
    ${SOURCE_PATH}/src/TpUtils/SystemInfo/TpGpu.cpp
    ${SOURCE_PATH}/src/TpUtils/SystemInfo/TpDisk.cpp
    ${SOURCE_PATH}/src/TpUtils/SystemInfo/TpDiskManage.cpp
    ${SOURCE_PATH}/src/TpUtils/SystemInfo/TpMemory.cpp
    ${SOURCE_PATH}/src/TpUtils/SystemInfo/TpProcessInfo.cpp
    ${SOURCE_PATH}/src/TpUtils/SystemInfo/TpProcessManage.cpp
    ${SOURCE_PATH}/src/TpUtils/SystemInfo/TpDisplay.cpp
    ${SOURCE_PATH}/src/TpUtils/SystemInfo/TpSystemInfo.cpp

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

    ${SOURCE_PATH}/src_p/TpUtils/Network/TpHostAddress.cpp
    ${SOURCE_PATH}/src_p/TpUtils/Network/NetworkConflib.c
    ${SOURCE_PATH}/src_p/TpUtils/Network/NetworkConf.c
    ${SOURCE_PATH}/src_p/TpUtils/Network/TpNetPcap.cpp

    ${SOURCE_PATH}/src/TpUtils/Media/TpAudioOutput.cpp
	${SOURCE_PATH}/src/TpUtils/Media/TpVideoOutput.cpp
    ${SOURCE_PATH}/src/TpUtils/Media/TpVideoFrame.cpp
    ${SOURCE_PATH}/src/TpUtils/Media/TpMediaFile.cpp
    ${SOURCE_PATH}/src/TpUtils/Media/TpMediaPlayer.cpp
    ${SOURCE_PATH}/src/TpUtils/Media/TpSound.cpp
    ${SOURCE_PATH}/src/TpUtils/Media/TpRecordInterface.cpp

    ${SOURCE_PATH}/src/TpUtils/AppManage/TpAppDataDisk.cpp
    ${SOURCE_PATH}/src/TpUtils/AppManage/TpAppInstall.cpp
    ${SOURCE_PATH}/src/TpUtils/AppManage/TpAppDopack.cpp
    ${SOURCE_PATH}/src/TpUtils/AppManage/TpLibDopack.cpp
    ${SOURCE_PATH}/src/TpUtils/AppManage/TpAppConfigIO.cpp

    ${SOURCE_PATH}/src/TpUtils/Hardware/TpUsbDevice.cpp
    ${SOURCE_PATH}/src/TpUtils/Hardware/TpUsbDeviceInfo.cpp
    ${SOURCE_PATH}/src/TpUtils/Hardware/TpUsbManager.cpp
    ${SOURCE_PATH}/src/TpUtils/Hardware/TpGpio.cpp
    ${SOURCE_PATH}/src/TpUtils/Hardware/TpHardwareI2c.cpp
    ${SOURCE_PATH}/src/TpUtils/Hardware/TpHardwarePwm.cpp
    ${SOURCE_PATH}/src/TpUtils/Hardware/TpSerialPort.cpp
    ${SOURCE_PATH}/src/TpUtils/Hardware/TpHardwareDevice.cpp
    ${SOURCE_PATH}/src/TpUtils/TpLogin.cpp

    ${SOURCE_PATH}/src_p/TpUtils/Media/TpMediaDevice.cpp
    ${SOURCE_PATH}/src_p/TpUtils/Media/Media/media.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Media/media_timer.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Media/media_file_list.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Media/media_codec.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Media/media_play.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Audio/audio_play.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Audio/audio_record.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Audio/hard.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Audio/filter.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Video/video_display.c
    ${SOURCE_PATH}/src_p/TpUtils/Media/Video/video_play.c

    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/lib/AgentHelper.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/lib/GUtils.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/lib/BluezAlsa.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/lib/FreedesktopSystemd.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/lib/MediaTransport.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/lib/Service.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/lib/bluez/Adapter.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/lib/bluez/AgentManager.cpp
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/lib/bluez/device.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/lib/bluez/Profile.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/lib/bluez/filter.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/lib/obex/ObexAgentManager.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/lib/obex/obex_agent.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/lib/obex/obex_transfer.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/lib/obex/obex_session.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/lib/obex/obex_client.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/lib/obex/obex_object_push.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/sdpregister.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/blt_audio.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/blt_agent.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/blt_dbussignal.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/blt_device.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/blt_file.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/blt_gatt.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/blt_glibscan.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/blt_hard.cpp
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/blt_hid.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/blt_sdp.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/blt_service.c
    ${SOURCE_PATH}/src_p/TpUtils/Bluetooth/blt_service_reg.c

    ${SOURCE_PATH}/src_p/TpUtils/Dbus/manager.c
    ${SOURCE_PATH}/src_p/TpUtils/Dbus/properties.c
    ${SOURCE_PATH}/src_p/TpUtils/Dbus/connect.c
    ${SOURCE_PATH}/src_p/TpUtils/Dbus/network_manager.c
    ${SOURCE_PATH}/src_p/TpUtils/Dbus/freedesktop_upower.c
    ${SOURCE_PATH}/src_p/TpUtils/Dbus/logind.c

    ${SOURCE_PATH}/src_p/TpUtils/tools/utilslib.c
    ${SOURCE_PATH}/src_p/TpUtils/tools/utlist.c
    ${SOURCE_PATH}/src_p/TpUtils/tools/variable_array.c
    ${SOURCE_PATH}/src_p/TpUtils/tools/utthread.c

    ${SOURCE_PATH}/src_p/TpUtils/AppManage/JsonConf.cpp
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/AppmCreat.cpp
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/TpFileCreat.cpp
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/AppmUtils.cpp
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/SystemPermission.c
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/SecretKey.cpp
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/ConfJson.cpp
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/InstallCheck.cpp
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/Install.cpp
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/Purview.cpp
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/Unload.cpp
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/Unpack.cpp
    ${SOURCE_PATH}/src_p/TpUtils/AppManage/Update.cpp

    ${SOURCE_PATH}/src_p/TpUtils/usb/usb_manager.c
    ${SOURCE_PATH}/src_p/TpUtils/devmanager/dev_manager.c
    ${SOURCE_PATH}/src_p/TpUtils/devmanager/dev_monitor.c

    ${SOURCE_PATH}/src_p/TpUtils/Log/src/elog.c
    ${SOURCE_PATH}/src_p/TpUtils/Log/src/elog_async.c
    ${SOURCE_PATH}/src_p/TpUtils/Log/src/elog_buf.c
    ${SOURCE_PATH}/src_p/TpUtils/Log/src/elog_utils.c
    ${SOURCE_PATH}/src_p/TpUtils/Log/port/elog_port.c

    ${SOURCE_PATH}/src/TpUtils/TpCompress.cpp
    ${SOURCE_PATH}/src/TpUtils/TpSystemBatteryInfo.cpp

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
include_directories(${SYSTEM_INCLUDE_PATH}/dbus-1.0)
include_directories(${SYSTEM_INCLUDE_PATH}/librsvg-2.0/librsvg/)
include_directories(${SYSTEM_INCLUDE_PATH}/librsvg-2.0)
include_directories(${SYSTEM_INCLUDE_PATH}/gdk-pixbuf-2.0)

include_directories(/usr/lib/x86_64-linux-gnu/dbus-1.0/include)
include_directories(/usr/lib/aarch64-linux-gnu/dbus-1.0/include)

include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/TpCore)
include_directories(${SOURCE_PATH}/include/TpWM)
include_directories(${SOURCE_PATH}/include/TpUtils)
include_directories(${SOURCE_PATH}/include/TpUtils/Service)
include_directories(${SOURCE_PATH}/include/TpUtils/JsonStructPackage)
include_directories(${SOURCE_PATH}/include/TpUtils/Network)
include_directories(${SOURCE_PATH}/include/TpUtils/SystemInfo)
include_directories(${SOURCE_PATH}/include/TpUtils/Bluetooth)
include_directories(${SOURCE_PATH}/include/TpUtils/Media)
include_directories(${SOURCE_PATH}/include/TpUtils/AppManage)
include_directories(${SOURCE_PATH}/include/TpUtils/Hardware)
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
include_directories(${SOURCE_PATH}/include_p/TpUtils/tools)
include_directories(${SOURCE_PATH}/include_p/TpUtils/Network)
include_directories(${SOURCE_PATH}/include_p/TpUtils/Dbus)
include_directories(${SOURCE_PATH}/include_p/TpUtils/AppManage)
include_directories(${SOURCE_PATH}/include_p/TpUtils/Bluetooth)
include_directories(${SOURCE_PATH}/include_p/TpUtils/Bluetooth/bluez)
include_directories(${SOURCE_PATH}/include_p/TpUtils/Bluetooth/obex)


include_directories(${SOURCE_PATH}/include_p/TpUtils/erpc)
include_directories(${SOURCE_PATH}/src_p/TpUtils/Service)

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
LoadSDKLibrary(GOBJECT_LIBRARY gobject-2.0)
LoadSDKLibrary(GLIB_LIBRARY glib-2.0)
LoadSDKLibrary(GBM_LIBRARY gbm)
LoadSDKLibrary(GLESV2_LIBRARY GLESv2)
LoadSDKLibrary(EGL_LIBRARY EGL)
LoadSDKLibrary(GIO_LIBRARY gio-2.0)
LoadSDKLibrary(RSVG_LIBRARY rsvg-2)
LoadSDKLibrary(SSL_LIBRARY ssl)
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
LoadSDKLibrary(expat_LIBRARY expat)
LoadSDKLibrary(bz2_LIBRARY bz2)

# 依赖的静态库
set(NANOMSG_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libnanomsg.a)
set(PCAP_LIB_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libpcap.a)
set(TPARCHIVE_LIB_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libtparchive.a)
set(OPENSSL3_LIB_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libssl.a)
set(CRYPTO3_LIB_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libcrypto.a)
set(ERPC_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/liberpc.a)

target_link_libraries(TpUtils PUBLIC
    dl pthread 
    ${ERPC_STATIC_LIB}
    ${OPENSSL3_LIB_STATIC_LIB} ${CRYPTO3_LIB_STATIC_LIB} 
    ${PCAP_LIB_STATIC_LIB} ${TPARCHIVE_LIB_STATIC_LIB} 
    ${NANOMSG_STATIC_LIB} 
    ${SDL2_LIBRARY} ${GOBJECT_LIBRARY} ${GLIB_LIBRARY} ${GBM_LIBRARY}
    ${GLESV2_LIBRARY} ${EGL_LIBRARY} ${GIO_LIBRARY} ${RSVG_LIBRARY}
    ${AVFILTER_LIBRARY} ${AVFORMAT_LIBRARY}
    ${ASOUND_LIBRARY} ${AVCODEC_LIBRARY} ${SWSCALE_LIBRARY}
    ${AVUTIL_LIBRARY} ${SWRESAMPLE_LIBRARY} ${AVDEVICE_LIBRARY}
    ${BLUETOOTH_LIBRARY} 
    ${expat_LIBRARY}
    ${bz2_LIBRARY}
    ${DBUS_LIBRARY} ${USB_LIBRARY} ${UDEV_LIBRARY}
    ${DEPEND_DYNAMIC_LIB_PATH}/librime.so.1.5.3
    ${DEPEND_DYNAMIC_LIB_PATH}/libTpWMEngine.so
    ${INSTALL_PACKAGE_ROOT_PATH}/lib/libTpCore.so
)
