cmake_minimum_required(VERSION 3.20.0)
project(PiXExternUtils VERSION 1.0.0 LANGUAGES C CXX)

# 指定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入cmake模块
include(../cmake/CreateTinyCoreSymlink.cmake)

# 添加源码目录
set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# 定义源码路径宏
set(SOURCE_FILES
	${SOURCE_PATH}/src_p/ExternUtils/fileUtils.c
	${SOURCE_PATH}/src_p/ExternUtils/media/media/media.c
	${SOURCE_PATH}/src_p/ExternUtils/media/media/media_timer.c
	${SOURCE_PATH}/src_p/ExternUtils/media/media/media_file_list.c
	${SOURCE_PATH}/src_p/ExternUtils/media/media/media_codec.c
	${SOURCE_PATH}/src_p/ExternUtils/media/media/media_play_temp.c
	${SOURCE_PATH}/src_p/ExternUtils/media/audio/audio_codec.c
	${SOURCE_PATH}/src_p/ExternUtils/media/audio/audio_manage.c
	${SOURCE_PATH}/src_p/ExternUtils/media/audio/audio_play.c
	${SOURCE_PATH}/src_p/ExternUtils/media/audio/audio_record.c
	${SOURCE_PATH}/src_p/ExternUtils/media/audio/hard.c
	${SOURCE_PATH}/src_p/ExternUtils/media/audio/filter.c
	${SOURCE_PATH}/src_p/ExternUtils/media/video/video_display.c
	${SOURCE_PATH}/src_p/ExternUtils/media/video/video_codec.c
	${SOURCE_PATH}/src_p/ExternUtils/media/video/video_manage.c
	${SOURCE_PATH}/src_p/ExternUtils/media/video/video_play.c
	${SOURCE_PATH}/src_p/ExternUtils/network/network_conflib.c
	${SOURCE_PATH}/src_p/ExternUtils/network/network_conf.c
	${SOURCE_PATH}/src_p/ExternUtils/AppManage/dopack/json_conf.c
	${SOURCE_PATH}/src_p/ExternUtils/AppManage/dopack/appm_creat.c
	${SOURCE_PATH}/src_p/ExternUtils/AppManage/dopack/file_creat_lib.c
	${SOURCE_PATH}/src_p/ExternUtils/AppManage/dopack/file_creat.c
	${SOURCE_PATH}/src_p/ExternUtils/AppManage/manage/utilslib.c
	${SOURCE_PATH}/src_p/ExternUtils/AppManage/manage/system_permission.c
	${SOURCE_PATH}/src_p/ExternUtils/AppManage/manage/secret_key.c
	${SOURCE_PATH}/src_p/ExternUtils/AppManage/manage/conf_json.c
	${SOURCE_PATH}/src_p/ExternUtils/AppManage/manage/install_check.c
	${SOURCE_PATH}/src_p/ExternUtils/AppManage/manage/install.c
	${SOURCE_PATH}/src_p/ExternUtils/AppManage/manage/appinfo.c
	${SOURCE_PATH}/src_p/ExternUtils/AppManage/manage/purview.c
	${SOURCE_PATH}/src_p/ExternUtils/AppManage/manage/unload.c
	${SOURCE_PATH}/src_p/ExternUtils/AppManage/manage/unpack.c
	${SOURCE_PATH}/src_p/ExternUtils/AppManage/manage/update.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/lib/agent-helper.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/lib/g_utils.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/lib/bluez_alsa.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/lib/freedesktop_systemd.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/lib/media_transport.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/lib/service.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/lib/bluez/adapter.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/lib/bluez/agent_manager.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/lib/bluez/device.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/lib/bluez/profile.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/lib/bluez/filter.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/lib/obex/obex_agent_manager.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/lib/obex/obex_agent.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/lib/obex/obex_transfer.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/lib/obex/obex_session.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/lib/obex/obex_client.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/lib/obex/obex_object_push.c
#	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/obexnames.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/sdpregister.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/blt_audio.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/blt_agent.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/blt_dbussignal.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/blt_device.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/blt_file.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/blt_gatt.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/blt_glibscan.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/blt_hard.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/blt_hid.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/blt_sdp.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/blt_service.c
	${SOURCE_PATH}/src_p/ExternUtils/bluetooth/src/blt_service_reg.c
	${SOURCE_PATH}/src_p/ExternUtils/utils/utilslib.c
	${SOURCE_PATH}/src_p/ExternUtils/utils/utlist.c
	${SOURCE_PATH}/src_p/ExternUtils/utils/variable_array.c
	${SOURCE_PATH}/src_p/ExternUtils/utils/utthread.c
	${SOURCE_PATH}/src_p/ExternUtils/TpDbus/manager.c
	${SOURCE_PATH}/src_p/ExternUtils/TpDbus/properties.c
	${SOURCE_PATH}/src_p/ExternUtils/TpDbus/connect.c
	${SOURCE_PATH}/src_p/ExternUtils/TpDbus/network_manager.c
	${SOURCE_PATH}/src_p/ExternUtils/TpDbus/freedesktop_upower.c
	${SOURCE_PATH}/src_p/ExternUtils/TpDbus/logind.c
	${SOURCE_PATH}/src_p/ExternUtils/usb/src/usb_manager.c
	${SOURCE_PATH}/src_p/ExternUtils/devmanager/src/dev_manager.c
	${SOURCE_PATH}/src_p/ExternUtils/devmanager/src/dev_monitor.c
    ${SOURCE_PATH}/src_p/ExternUtils/TpSystemDataManage.cpp
    ${SOURCE_PATH}/src_p/ExternUtils/TpFileInfoWR.cpp
	${SOURCE_PATH}/src_p/ExternUtils/TpNetPcap.cpp
	${SOURCE_PATH}/src_p/ExternUtils/TpDbusConnectManage.cpp
	${SOURCE_PATH}/src/ExternUtils/TpAppDataDisk.cpp
#	${SOURCE_PATH}/src/ExternUtils/TpDisplayManage.cpp
	${SOURCE_PATH}/src/ExternUtils/TpDisplay.cpp
	${SOURCE_PATH}/src/ExternUtils/TpCompress.cpp
	${SOURCE_PATH}/src/ExternUtils/TpCpu.cpp
	${SOURCE_PATH}/src/ExternUtils/TpCpuManage.cpp
	${SOURCE_PATH}/src/ExternUtils/TpGpu.cpp
	${SOURCE_PATH}/src/ExternUtils/TpDisk.cpp
	${SOURCE_PATH}/src/ExternUtils/TpDiskManage.cpp
	${SOURCE_PATH}/src/ExternUtils/TpMemory.cpp
	${SOURCE_PATH}/src/ExternUtils/TpNetworkInfo.cpp
	${SOURCE_PATH}/src/ExternUtils/TpNetworkManage.cpp
	${SOURCE_PATH}/src/ExternUtils/TpProcessInfo.cpp
	${SOURCE_PATH}/src/ExternUtils/TpProcessManage.cpp
	${SOURCE_PATH}/src/ExternUtils/TpSystemInfo.cpp
	${SOURCE_PATH}/src/ExternUtils/TpNetworkInterface.cpp
	${SOURCE_PATH}/src/ExternUtils/TpWirelessInfo.cpp
	${SOURCE_PATH}/src/ExternUtils/TpAudioInterface.cpp
	${SOURCE_PATH}/src/ExternUtils/TpVideoInterface.cpp
	${SOURCE_PATH}/src/ExternUtils/TpMediaFile.cpp
	${SOURCE_PATH}/src/ExternUtils/TpRecordInterface.cpp
	${SOURCE_PATH}/src/ExternUtils/TpAppInstall.cpp
	${SOURCE_PATH}/src/ExternUtils/TpLibDopack.cpp
	${SOURCE_PATH}/src/ExternUtils/TpAppDopack.cpp
	${SOURCE_PATH}/src/ExternUtils/TpSystemBatteryInfo.cpp
	${SOURCE_PATH}/src/ExternUtils/TpBluetoothAddress.cpp
	${SOURCE_PATH}/src/ExternUtils/TpBluetoothUuid.cpp
	${SOURCE_PATH}/src/ExternUtils/TpBluetoothLocal.cpp
	${SOURCE_PATH}/src/ExternUtils/TpBluetoothDevice.cpp
	${SOURCE_PATH}/src/ExternUtils/TpBluetoothDiscovery.cpp
	${SOURCE_PATH}/src/ExternUtils/TpBluetoothService.cpp
	${SOURCE_PATH}/src/ExternUtils/TpBluetoothServiceDiscovery.cpp
	${SOURCE_PATH}/src/ExternUtils/TpBluetoothSocket.cpp
	${SOURCE_PATH}/src/ExternUtils/TpBluetoothServer.cpp
	${SOURCE_PATH}/src/ExternUtils/TpBluetoothTransfer.cpp
	${SOURCE_PATH}/src/ExternUtils/TpBluetoothTransferAgent.cpp
	${SOURCE_PATH}/src/ExternUtils/TpBluetoothAudioDevice.cpp
	${SOURCE_PATH}/src/ExternUtils/TpBluetoothAudioManager.cpp
	${SOURCE_PATH}/src/ExternUtils/TpUsbDevice.cpp
	${SOURCE_PATH}/src/ExternUtils/TpUsbDeviceInfo.cpp
	${SOURCE_PATH}/src/ExternUtils/TpUsbManager.cpp
	${SOURCE_PATH}/src/ExternUtils/TpSound.cpp
	${SOURCE_PATH}/src/ExternUtils/TpGpio.cpp
	${SOURCE_PATH}/src/ExternUtils/TpHardwareI2c.cpp
	${SOURCE_PATH}/src/ExternUtils/TpLogin.cpp
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
include_directories(${SOURCE_PATH}/include)
include_directories(${SOURCE_PATH}/include/Api)
include_directories(${SOURCE_PATH}/include/Utils)
include_directories(${SOURCE_PATH}/include/Utils/base)
include_directories(${SOURCE_PATH}/include/ExternUtils)
include_directories(${SOURCE_PATH}/include_p)
include_directories(${SOURCE_PATH}/include_p/Api)
include_directories(${SOURCE_PATH}/include_p/Api/base)
include_directories(${SOURCE_PATH}/include_p/Utils)
include_directories(${SOURCE_PATH}/include_p/ExternUtils)
include_directories(${SOURCE_PATH}/include_p/ExternUtils/SystemLib)
include_directories(${SOURCE_PATH}/src_p/ExternUtils)
include_directories(${SOURCE_PATH}/src_p/ExternUtils/media/include)
include_directories(${SOURCE_PATH}/src_p/ExternUtils/media/audio)
include_directories(${SOURCE_PATH}/src_p/ExternUtils/media/video)
include_directories(${SOURCE_PATH}/src_p/ExternUtils/media/media)
include_directories(${SOURCE_PATH}/src_p/ExternUtils/network)
include_directories(${SOURCE_PATH}/src_p/ExternUtils/AppManage/manage)
include_directories(${SOURCE_PATH}/src_p/ExternUtils/AppManage/dopack)
include_directories(${SOURCE_PATH}/src_p/ExternUtils/AppManage/inc)
include_directories(/usr/include/dbus-1.0/)
include_directories(/usr/lib/x86_64-linux-gnu/dbus-1.0/include/)
include_directories(${SOURCE_PATH}/src_p/ExternUtils/bluetooth/include)
include_directories(${SOURCE_PATH}/src_p/ExternUtils/bluetooth/utils)
include_directories(${SOURCE_PATH}/src_p/ExternUtils/bluetooth)
include_directories(${SOURCE_PATH}/src_p/ExternUtils/bluetooth/include/lib)
include_directories(${SOURCE_PATH}/src_p/ExternUtils/bluetooth/include/lib/bluez)
include_directories(${SOURCE_PATH}/src_p/ExternUtils/bluetooth/include/lib/obex)
include_directories(${SOURCE_PATH}/src_p/ExternUtils/usb/include)
include_directories(${SOURCE_PATH}/src_p/ExternUtils/devmanager/include)

# 添加动态库目标
add_library(PiXExternUtils SHARED ${SOURCE_FILES})

# 设置动态库版本 VERSION 是库的当前版本号，而 SOVERSION 是API的兼容性版本号。
# set_target_properties(PiXExternUtils PROPERTIES VERSION 1.0 SOVERSION 1)

# 指定静态库 pcap 的路径
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86|i686|i386|AMD64")
	set(PCAP_LIB_STATIC_LIB ${SOURCE_PATH}/depend_lib/static/x86_x64/libpcap.a)
	set(TPARCHIVE_LIB_STATIC_LIB ${SOURCE_PATH}/depend_lib/static/x86_x64/libtparchive.a)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
	set(PCAP_LIB_STATIC_LIB ${SOURCE_PATH}/depend_lib/static/arm_64/libpcap.a)
	set(TPARCHIVE_LIB_STATIC_LIB ${SOURCE_PATH}/depend_lib/static/arm_64/libtparchive.a)
endif()

# 将pthread库链接到可执行目标
target_link_libraries(PiXExternUtils ${PCAP_LIB_STATIC_LIB} ${TPARCHIVE_LIB_STATIC_LIB} dl
	pthread PiXApi PiXUtils ssl json-c glib-2.0 
	gio-2.0 asound avcodec avfilter avformat avutil 
	swresample SDL2 avdevice swscale bluetooth dbus-1 usb-1.0 udev)

# 如果你需要将动态库安装到特定的目录，可以修改DESTINATION路径
install(TARGETS PiXExternUtils LIBRARY DESTINATION ${INSTALL_LIB_DIR})

# 创建软链接
cmake_create_core_symlink(
    TARGET PiXExternUtils
    INSTALL_DIR ${INSTALL_LIB_DIR}
    LINK_DIR ${LINK_LIB_DIR}             
)
