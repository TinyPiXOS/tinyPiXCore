cmake_minimum_required(VERSION 3.5.0)
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
	${SOURCE_PATH}/src_p/ExternUtils/media/audio/audio_codec.c
	${SOURCE_PATH}/src_p/ExternUtils/media/audio/audio_manage.c
	${SOURCE_PATH}/src_p/ExternUtils/media/audio/audio_play.c
	${SOURCE_PATH}/src_p/ExternUtils/media/audio/audio_record.c
	${SOURCE_PATH}/src_p/ExternUtils/media/audio/hard.c
	${SOURCE_PATH}/src_p/ExternUtils/media/audio/filter.c
	${SOURCE_PATH}/src_p/ExternUtils/media/video/video_display.c
	${SOURCE_PATH}/src_p/ExternUtils/media/video/media_timer.c
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
	${SOURCE_PATH}/src_p/ExternUtils/utils/utilslib.c
	${SOURCE_PATH}/src_p/ExternUtils/utils/utlist.c
	${SOURCE_PATH}/src_p/ExternUtils/utils/variable_array.c
	${SOURCE_PATH}/src_p/ExternUtils/utils/utthread.c
	${SOURCE_PATH}/src_p/ExternUtils/tpdbus/manager.c
	${SOURCE_PATH}/src_p/ExternUtils/tpdbus/properties.c
	${SOURCE_PATH}/src_p/ExternUtils/tpdbus/connect.c
	${SOURCE_PATH}/src_p/ExternUtils/tpdbus/network_manager.c
	${SOURCE_PATH}/src_p/ExternUtils/tpdbus/freedesktop_upower.c
	${SOURCE_PATH}/src_p/ExternUtils/tpdbus/logind.c
	${SOURCE_PATH}/src_p/ExternUtils/usb/src/usb_manager.c
	${SOURCE_PATH}/src_p/ExternUtils/devmanager/src/dev_manager.c
	${SOURCE_PATH}/src_p/ExternUtils/devmanager/src/dev_monitor.c
    ${SOURCE_PATH}/src_p/ExternUtils/tpSystemDataManage.cpp
    ${SOURCE_PATH}/src_p/ExternUtils/tpFileInfoWR.cpp
	${SOURCE_PATH}/src_p/ExternUtils/tpNetPcap.cpp
	${SOURCE_PATH}/src_p/ExternUtils/tpHostAddress.cpp
	${SOURCE_PATH}/src_p/ExternUtils/tpDbusConnectManage.cpp
	${SOURCE_PATH}/src_p/ExternUtils/tpSocketNotifier.cpp
	${SOURCE_PATH}/src_p/ExternUtils/tpSocketNotifierManager.cpp
	${SOURCE_PATH}/src/ExternUtils/tpAppDataDisk.cpp
	${SOURCE_PATH}/src/ExternUtils/tpDisplayManage.cpp
	${SOURCE_PATH}/src/ExternUtils/tpDisplay.cpp
	${SOURCE_PATH}/src/ExternUtils/tpCompress.cpp
	${SOURCE_PATH}/src/ExternUtils/tpCpu.cpp
	${SOURCE_PATH}/src/ExternUtils/tpCpuManage.cpp
	${SOURCE_PATH}/src/ExternUtils/tpGpu.cpp
	${SOURCE_PATH}/src/ExternUtils/tpDisk.cpp
	${SOURCE_PATH}/src/ExternUtils/tpDiskManage.cpp
	${SOURCE_PATH}/src/ExternUtils/tpMemory.cpp
	${SOURCE_PATH}/src/ExternUtils/tpNetworkInfo.cpp
	${SOURCE_PATH}/src/ExternUtils/tpNetworkManage.cpp
	${SOURCE_PATH}/src/ExternUtils/tpProcessInfo.cpp
	${SOURCE_PATH}/src/ExternUtils/tpProcessManage.cpp
	${SOURCE_PATH}/src/ExternUtils/tpSystemInfo.cpp
	${SOURCE_PATH}/src/ExternUtils/tpThreadManage.cpp
	${SOURCE_PATH}/src/ExternUtils/tpNetworkInterface.cpp
	${SOURCE_PATH}/src/ExternUtils/tpWirelessInfo.cpp
	${SOURCE_PATH}/src/ExternUtils/tpNetworkDatagram.cpp
	${SOURCE_PATH}/src/ExternUtils/tpSocket.cpp
	${SOURCE_PATH}/src/ExternUtils/tpTcpSocket.cpp
	${SOURCE_PATH}/src/ExternUtils/tpTcpServer.cpp
	${SOURCE_PATH}/src/ExternUtils/tpUdpSocket.cpp
	${SOURCE_PATH}/src/ExternUtils/tpAudioInterface.cpp
	${SOURCE_PATH}/src/ExternUtils/tpVideoInterface.cpp
	${SOURCE_PATH}/src/ExternUtils/tpMediaFile.cpp
	${SOURCE_PATH}/src/ExternUtils/tpRecordInterface.cpp
	${SOURCE_PATH}/src/ExternUtils/tpAppInstall.cpp
	${SOURCE_PATH}/src/ExternUtils/tpLibDopack.cpp
	${SOURCE_PATH}/src/ExternUtils/tpAppDopack.cpp
	${SOURCE_PATH}/src/ExternUtils/tpSystemBatteryInfo.cpp
	${SOURCE_PATH}/src/ExternUtils/tpLogin.cpp
	${SOURCE_PATH}/src/ExternUtils/tpBluetoothAddress.cpp
	${SOURCE_PATH}/src/ExternUtils/tpBluetoothLocal.cpp
	${SOURCE_PATH}/src/ExternUtils/tpBluetoothDevice.cpp
	${SOURCE_PATH}/src/ExternUtils/tpBluetoothDiscovery.cpp
	${SOURCE_PATH}/src/ExternUtils/tpBluetoothTransfer.cpp
	${SOURCE_PATH}/src/ExternUtils/tpBluetoothTransferAgent.cpp
	${SOURCE_PATH}/src/ExternUtils/tpBluetoothAudioDevice.cpp
	${SOURCE_PATH}/src/ExternUtils/tpBluetoothAudioManager.cpp
	${SOURCE_PATH}/src/ExternUtils/tpUsbDevice.cpp
	${SOURCE_PATH}/src/ExternUtils/tpUsbDeviceInfo.cpp
	${SOURCE_PATH}/src/ExternUtils/tpUsbManager.cpp
	${SOURCE_PATH}/src/ExternUtils/tpSound.cpp
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
