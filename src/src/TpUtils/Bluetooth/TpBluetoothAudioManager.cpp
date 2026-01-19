/*///------------------------------------------------------------------------------------------------------------------------//
        蓝牙音频服务管理相关接口
说 明 :
日 期 : 2025.5.8

/*/
//------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include "bluetooth_inc.h"
#include "blt_audio.h"
#include "TpBluetoothAudioManager.h"
#include "TpDbusConnectManage.h"

struct TpBluetoothAudioManagerData
{
    DesktopSystem *system;
    TpBluetoothAudioManagerData()
    {
        system = NULL;
    };
};

TpBluetoothAudioManager::TpBluetoothAudioManager()
{
    data_ = new TpBluetoothAudioManagerData();
    TpBluetoothAudioManagerData *data = static_cast<TpBluetoothAudioManagerData *>(data_);
    if (!data)
        return;
    if (TpDbusConnectManage::instance().connection() != TP_TRUE)
    {
        fprintf(stderr, "connect to dbus error\n");
        return;
    }
    data->system = desktop_system_creat();
    if (!data->system)
    {
        fprintf(stderr, "构造tpBluetoothAudio失败\n");
        return;
    }
    if (bluet_audio_blue_alsa_is_runing(data->system, NULL) == 0)
    {
        bluet_audio_start_blue_alsa(data->system, NULL);
    }
}

TpBluetoothAudioManager::~TpBluetoothAudioManager()
{
    TpBluetoothAudioManagerData *data = static_cast<TpBluetoothAudioManagerData *>(data_);
    if (!data)
        return;

    desktop_system_delete(data->system);
    delete (data);
}

int TpBluetoothAudioManager::startService()
{
    TpBluetoothAudioManagerData *data = static_cast<TpBluetoothAudioManagerData *>(data_);
    return bluet_audio_start_blue_alsa(data->system, NULL);
}

int TpBluetoothAudioManager::stopService()
{
    TpBluetoothAudioManagerData *data = static_cast<TpBluetoothAudioManagerData *>(data_);
    return bluet_audio_stop_blue_alsa(data->system, NULL);
}

int TpBluetoothAudioManager::restartService()
{
    TpBluetoothAudioManagerData *data = static_cast<TpBluetoothAudioManagerData *>(data_);
    return bluet_audio_restart_blue_alsa(data->system, NULL);
}

tpBool TpBluetoothAudioManager::isRuning()
{
    TpBluetoothAudioManagerData *data = static_cast<TpBluetoothAudioManagerData *>(data_);
    if (bluet_audio_blue_alsa_is_runing(data->system, NULL) == 0)
        return TP_FALSE;
    return TP_TRUE;
}
