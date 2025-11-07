cmake_minimum_required(VERSION 3.20.0)
project(TpExtUtils VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../cmake/TargetArchConfig.cmake)

# 添加源码目录
set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# 定义源码路径宏
set(SOURCE_FILES
	${SOURCE_PATH}/src_p/TpExtUtils/fileUtils.c
	${SOURCE_PATH}/src_p/TpExtUtils/media/media/media.c
	${SOURCE_PATH}/src_p/TpExtUtils/media/media/media_timer.c
	${SOURCE_PATH}/src_p/TpExtUtils/media/media/media_file_list.c
	${SOURCE_PATH}/src_p/TpExtUtils/media/media/media_codec.c
	${SOURCE_PATH}/src_p/TpExtUtils/media/media/media_play_temp.c
	${SOURCE_PATH}/src_p/TpExtUtils/media/audio/audio_codec.c
	${SOURCE_PATH}/src_p/TpExtUtils/media/audio/audio_manage.c
	${SOURCE_PATH}/src_p/TpExtUtils/media/audio/audio_play.c
	${SOURCE_PATH}/src_p/TpExtUtils/media/audio/audio_record.c
	${SOURCE_PATH}/src_p/TpExtUtils/media/audio/hard.c
	${SOURCE_PATH}/src_p/TpExtUtils/media/audio/filter.c
	${SOURCE_PATH}/src_p/TpExtUtils/media/video/video_display.c
	${SOURCE_PATH}/src_p/TpExtUtils/media/video/video_codec.c
	${SOURCE_PATH}/src_p/TpExtUtils/media/video/video_manage.c
	${SOURCE_PATH}/src_p/TpExtUtils/media/video/video_play.c
	${SOURCE_PATH}/src_p/TpExtUtils/network/network_conflib.c
	${SOURCE_PATH}/src_p/TpExtUtils/network/network_conf.c
	${SOURCE_PATH}/src_p/TpExtUtils/AppManage/dopack/json_conf.c
	${SOURCE_PATH}/src_p/TpExtUtils/AppManage/dopack/appm_creat.c
	${SOURCE_PATH}/src_p/TpExtUtils/AppManage/dopack/file_creat_lib.c
	${SOURCE_PATH}/src_p/TpExtUtils/AppManage/dopack/file_creat.c
	${SOURCE_PATH}/src_p/TpExtUtils/AppManage/manage/utilslib.c
	${SOURCE_PATH}/src_p/TpExtUtils/AppManage/manage/system_permission.c
	${SOURCE_PATH}/src_p/TpExtUtils/AppManage/manage/secret_key.c
	${SOURCE_PATH}/src_p/TpExtUtils/AppManage/manage/conf_json.c
	${SOURCE_PATH}/src_p/TpExtUtils/AppManage/manage/install_check.c
	${SOURCE_PATH}/src_p/TpExtUtils/AppManage/manage/install.c
	${SOURCE_PATH}/src_p/TpExtUtils/AppManage/manage/appinfo.c
	${SOURCE_PATH}/src_p/TpExtUtils/AppManage/manage/purview.c
	${SOURCE_PATH}/src_p/TpExtUtils/AppManage/manage/unload.c
	${SOURCE_PATH}/src_p/TpExtUtils/AppManage/manage/unpack.c
	${SOURCE_PATH}/src_p/TpExtUtils/AppManage/manage/update.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/lib/agent-helper.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/lib/g_utils.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/lib/bluez_alsa.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/lib/freedesktop_systemd.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/lib/media_transport.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/lib/service.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/lib/bluez/adapter.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/lib/bluez/agent_manager.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/lib/bluez/device.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/lib/bluez/profile.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/lib/bluez/filter.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/lib/obex/obex_agent_manager.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/lib/obex/obex_agent.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/lib/obex/obex_transfer.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/lib/obex/obex_session.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/lib/obex/obex_client.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/lib/obex/obex_object_push.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/sdpregister.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/blt_audio.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/blt_agent.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/blt_dbussignal.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/blt_device.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/blt_file.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/blt_gatt.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/blt_glibscan.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/blt_hard.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/blt_hid.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/blt_sdp.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/blt_service.c
	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/blt_service_reg.c
	${SOURCE_PATH}/src_p/TpExtUtils/utils/utilslib.c
	${SOURCE_PATH}/src_p/TpExtUtils/utils/utlist.c
	${SOURCE_PATH}/src_p/TpExtUtils/utils/variable_array.c
	${SOURCE_PATH}/src_p/TpExtUtils/utils/utthread.c
	${SOURCE_PATH}/src_p/TpExtUtils/TpDbus/manager.c
	${SOURCE_PATH}/src_p/TpExtUtils/TpDbus/properties.c
	${SOURCE_PATH}/src_p/TpExtUtils/TpDbus/connect.c
	${SOURCE_PATH}/src_p/TpExtUtils/TpDbus/network_manager.c
	${SOURCE_PATH}/src_p/TpExtUtils/TpDbus/freedesktop_upower.c
	${SOURCE_PATH}/src_p/TpExtUtils/TpDbus/logind.c
	${SOURCE_PATH}/src_p/TpExtUtils/usb/src/usb_manager.c
	${SOURCE_PATH}/src_p/TpExtUtils/devmanager/src/dev_manager.c
	${SOURCE_PATH}/src_p/TpExtUtils/devmanager/src/dev_monitor.c
    ${SOURCE_PATH}/src_p/TpExtUtils/TpSystemDataManage.cpp
    ${SOURCE_PATH}/src_p/TpExtUtils/TpFileInfoWR.cpp
	${SOURCE_PATH}/src_p/TpExtUtils/TpNetPcap.cpp
	${SOURCE_PATH}/src_p/TpExtUtils/TpDbusConnectManage.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpAppDataDisk.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpDisplay.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpCompress.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpCpu.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpCpuManage.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpGpu.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpDisk.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpDiskManage.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpMemory.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpNetworkInfo.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpNetworkManage.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpProcessInfo.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpProcessManage.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpSystemInfo.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpNetworkInterface.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpWirelessInfo.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpAudioInterface.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpVideoInterface.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpMediaFile.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpRecordInterface.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpAppInstall.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpLibDopack.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpAppDopack.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpSystemBatteryInfo.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpBluetoothAddress.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpBluetoothUuid.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpBluetoothLocal.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpBluetoothDevice.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpBluetoothDiscovery.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpBluetoothService.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpBluetoothServiceDiscovery.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpBluetoothSocket.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpBluetoothServer.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpBluetoothTransfer.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpBluetoothTransferAgent.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpBluetoothAudioDevice.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpBluetoothAudioManager.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpUsbDevice.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpUsbDeviceInfo.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpUsbManager.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpSound.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpGpio.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpHardwareI2c.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpHardwarePwm.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpLogin.cpp
	${SOURCE_PATH}/src/TpExtUtils/TpSerialPort.cpp
    #	${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/src/obexnames.c
    #	${SOURCE_PATH}/src/TpExtUtils/TpDisplayManage.cpp
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

include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/TpWM)
include_directories(${SOURCE_PATH}/include/TpUtils)
include_directories(${SOURCE_PATH}/include/TpExtUtils)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/TpUtils)
include_directories(${SOURCE_PATH}/include_p/TpExtUtils)
include_directories(${SOURCE_PATH}/include_p/TpExtUtils/SystemLib)
include_directories(${SOURCE_PATH}/src_p/TpExtUtils)
include_directories(${SOURCE_PATH}/src_p/TpExtUtils/media/include)
include_directories(${SOURCE_PATH}/src_p/TpExtUtils/media/audio)
include_directories(${SOURCE_PATH}/src_p/TpExtUtils/media/video)
include_directories(${SOURCE_PATH}/src_p/TpExtUtils/media/media)
include_directories(${SOURCE_PATH}/src_p/TpExtUtils/network)
include_directories(${SOURCE_PATH}/src_p/TpExtUtils/AppManage/manage)
include_directories(${SOURCE_PATH}/src_p/TpExtUtils/AppManage/dopack)
include_directories(${SOURCE_PATH}/src_p/TpExtUtils/AppManage/inc)
include_directories(${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/include)
include_directories(${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/utils)
include_directories(${SOURCE_PATH}/src_p/TpExtUtils/bluetooth)
include_directories(${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/include/lib)
include_directories(${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/include/lib/bluez)
include_directories(${SOURCE_PATH}/src_p/TpExtUtils/bluetooth/include/lib/obex)
include_directories(${SOURCE_PATH}/src_p/TpExtUtils/usb/include)
include_directories(${SOURCE_PATH}/src_p/TpExtUtils/devmanager/include)

# 添加动态库目标
add_library(TpExtUtils SHARED ${SOURCE_FILES})

# 指定依赖动态库目录
if(CMAKE_CROSSCOMPILING)
    link_directories("${LOAD_SDK_LIB_PATH}")
else()
    link_directories("/usr/lib")
endif()

LoadSDKLibrary(SDL2_LIBRARY SDL2)
LoadSDKLibrary(GLIB_LIBRARY glib-2.0)
LoadSDKLibrary(GIO_LIBRARY gio-2.0)
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

# 设置动态库版本 VERSION 是库的当前版本号，而 SOVERSION 是API的兼容性版本号。
# set_target_properties(TpExtUtils PROPERTIES VERSION 1.0 SOVERSION 1)

# 指定静态库 pcap 的路径
set(PCAP_LIB_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libpcap.a)
set(TPARCHIVE_LIB_STATIC_LIB ${DEPEND_STATIC_LIB_PATH}/libtparchive.a)

# 将pthread库链接到可执行目标
target_link_libraries(TpExtUtils PUBLIC
    dl pthread TpUtils
    ${PCAP_LIB_STATIC_LIB} ${TPARCHIVE_LIB_STATIC_LIB} 
    ${GLIB_LIBRARY} ${GIO_LIBRARY} ${SDL2_LIBRARY} ${SSL_LIBRARY}
    ${JSONC_LIBRARY} ${ASOUND_LIBRARY} ${AVCODEC_LIBRARY} ${AVFILTER_LIBRARY} ${AVFORMAT_LIBRARY}
    ${AVUTIL_LIBRARY} ${SWRESAMPLE_LIBRARY} ${AVDEVICE_LIBRARY} ${SWSCALE_LIBRARY}
    ${BLUETOOTH_LIBRARY} ${DBUS_LIBRARY} ${USB_LIBRARY} ${UDEV_LIBRARY}
	${DEPEND_DYNAMIC_LIB_PATH}/libTpWMEngine.so)
