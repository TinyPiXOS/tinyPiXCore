/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙音频服务管理相关接口
说 明 : 
日 期 : 2025.5.8

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include "bluetooth/include/bluetooth_inc.h"
#include "bluetooth/include/blt_audio.h"
#include "tpBluetoothAudioManager.h"
#include "tpDbusConnectManage.h"


struct tpBluetoothAudioManagerData{
	DesktopSystem *system;
	tpBluetoothAudioManagerData(){
		system=NULL;
	};
};


tpBluetoothAudioManager::tpBluetoothAudioManager()
{
	data_ = new tpBluetoothAudioManagerData();
	tpBluetoothAudioManagerData *data = static_cast<tpBluetoothAudioManagerData *>(data_);
	if(!data)
		return;
	if(tpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		fprintf(stderr,"connect to dbus error\n");
		return ;
	}
	data->system=desktop_system_creat();
	if(!data->system)
	{
		fprintf(stderr,"构造tpBluetoothAudio失败\n");
		return ;
	}
	if(bluet_audio_blue_alsa_is_runing(data->system,NULL)==0)
	{
		bluet_audio_start_blue_alsa(data->system,NULL);
	}
}

tpBluetoothAudioManager::~tpBluetoothAudioManager()
{
	tpBluetoothAudioManagerData *data = static_cast<tpBluetoothAudioManagerData *>(data_);
	if(!data)
		return;
	
	desktop_system_delete(data->system);
	delete(data);
}

int tpBluetoothAudioManager::startService()
{
	tpBluetoothAudioManagerData *data = static_cast<tpBluetoothAudioManagerData *>(data_);
	return bluet_audio_start_blue_alsa(data->system,NULL);
}

int tpBluetoothAudioManager::stopService()
{
	tpBluetoothAudioManagerData *data = static_cast<tpBluetoothAudioManagerData *>(data_);
	return bluet_audio_stop_blue_alsa(data->system,NULL);
}

int tpBluetoothAudioManager::restartService()
{
	tpBluetoothAudioManagerData *data = static_cast<tpBluetoothAudioManagerData *>(data_);
	return bluet_audio_restart_blue_alsa(data->system,NULL);
}

tpBool tpBluetoothAudioManager::isRuning()
{
	tpBluetoothAudioManagerData *data = static_cast<tpBluetoothAudioManagerData *>(data_);
	if(bluet_audio_blue_alsa_is_runing(data->system,NULL)==0)
		return TP_FALSE;
	return TP_TRUE;
}

