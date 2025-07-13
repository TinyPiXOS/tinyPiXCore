/*///------------------------------------------------------------------------------------------------------------------------//
		录音接口
说 明 : 当录音功能以接口调用形式使用的时候调用此接口
日 期 : 2024.1.25

/*///------------------------------------------------------------------------------------------------------------------------//	

#include <iostream>
#include <thread>
#include <stdint.h>
#include "tpRecordInterface.h"
#include "tpAudioDevice.h"
#include "tpRecordDevice.h"



struct tpRecordInfData{
	PIAudioConf *record;
	struct MediaParams *user;
	tpString name;
	std::atomic<bool> running;
	std::thread thread_t;
	tpRecordInfData(){
		running=false;
		user=nullptr;
		record=nullptr;

	};
};


tpRecordInterface::tpRecordInterface(const tpString& device)
{
	data_ = new tpRecordInfData();
	tpRecordInfData *recData = static_cast<tpRecordInfData *>(data_);
	MediaParams *user=media_user_config_creat();
	if(user==NULL)
	{
		std::cerr << "Failed to creat tpAudioInterface" << std::endl;
	}
	recData->user=user;
	size_t pos = device.find(' ');      			// 查找第一个空格位置
	if (pos == std::string::npos) // 无空格时返回整个字符串
        recData->name = device;
	else
   		recData->name = device.substr(0, pos);      // 截取开头到空格前的部分
}

tpRecordInterface::~tpRecordInterface()
{
	tpRecordInfData *recData = static_cast<tpRecordInfData *>(data_);
	if(recData->thread_t.joinable())
	{
		recData->thread_t.join();
	}
	if(!Audio_State_Is_Exit(recData->user))
		Audio_Device_Close(recData->record);
	media_user_config_free(recData->user);
}


int tpRecordInterface::threadRecord()
{
	tpRecordInfData *recData = static_cast<tpRecordInfData *>(data_);
	Audio_Record_Main(recData->record,recData->user);
	return 0;
}


int tpRecordInterface::openDevice()
{
	tpRecordInfData *recData = static_cast<tpRecordInfData *>(data_);
	if(!recData->user)
		return -1;
	if(recData->running)
		return -1;
	PIAudioConf *hard=Audio_Record_Open(recData->name.c_str());	//recData->name.c_str()
	if(hard==NULL)
		return -1;
	recData->record=hard;
	recData->running = true;
	recData->thread_t = std::thread(&tpRecordInterface::threadRecord, this);
	printf("device open ok\n");
	return 0;
}

tpBool tpRecordInterface::isOpen()
{
	tpRecordInfData *recData = static_cast<tpRecordInfData *>(data_);
	return (recData->running==true? TP_TRUE : TP_FALSE);
}

int tpRecordInterface::closeDevice()
{
	tpRecordInfData *recData = static_cast<tpRecordInfData *>(data_);
	Audio_Set_Close(recData->user);
	while(!Audio_State_Is_Exit(recData->user))			//
		usleep(10);
	return Audio_Device_Close(recData->record);
}

int tpRecordInterface::recordStart(tpString& file,AudioType type,AudioBitRate rate)
{
	return recordStart(file.c_str(),type,rate);
}

int tpRecordInterface::recordStart(const char *file,AudioType type,AudioBitRate rate)
{
	tpRecordInfData *recData = static_cast<tpRecordInfData *>(data_);
	if(!recData->user)
		return -1;
	if(Record_Set_Start(recData->user,file)<0)
	{
		std::cerr <<"录音开始错误\n";
		return -1;
	}
	return 0;
}

int tpRecordInterface::recordContinue()
{
	tpRecordInfData *recData = static_cast<tpRecordInfData *>(data_);
	if(!recData->user)
		return -1;
	return Audio_Set_Continue(recData->user);
}

int tpRecordInterface::recordPause()
{
	tpRecordInfData *recData = static_cast<tpRecordInfData *>(data_);
	if(!recData->user)
		return -1;
	return Audio_Set_Suspend(recData->user);
}

int tpRecordInterface::recordStop()
{
	tpRecordInfData *recData = static_cast<tpRecordInfData *>(data_);
	if(!recData->user)
		return -1;
	return Audio_Set_Stop(recData->user);
}

