/*///------------------------------------------------------------------------------------------------------------------------//
		硬件PWM
说 明 : 使用此功能需要内核启用pwm功能，如果没有需要修改内核设备树以及驱动(如有需要)以支持硬件PWM。如不想修改内核可以使用软件PWM接口，但会牺牲性能
日 期 : 2025.09.05

/*///------------------------------------------------------------------------------------------------------------------------//

#include <dirent.h>
#include <algorithm>
#include "TpHardwarePwm.h"

#define  PATH_PWM_DEVICE	"/sys/class/pwm/pwmchip"


struct TpHardwarePwmData{
	TpString path;
	tpUInt8 channel;
	tpBool is_open;
	TpString period_path;
	TpString duty_path;
	tpUInt32 period;
	float duty_cycle;
	TpHardwarePwmData(){
		is_open=TP_FALSE;
		channel=0;
	}
};



TpHardwarePwm::TpHardwarePwm(const TpString& name, tpUInt8 channel)
{
	data_ = new TpHardwarePwmData();
	TpHardwarePwmData *data = static_cast<TpHardwarePwmData *>(data_);
	data->path=name;
	data->duty_path=data->path+"/duty_cycle";
	data->period_path=data->path+"/period";
}

/// @brief 
/// @param num pwm编号
TpHardwarePwm::TpHardwarePwm(tpUInt8 num, tpUInt8 channel): TpHardwarePwm(
        TpString(PATH_PWM_DEVICE) + std::to_string(static_cast<int>(num))+TpString("/pwm")+std::to_string(static_cast<int>(channel)),channel)
{
}

TpHardwarePwm::~TpHardwarePwm()
{
	TpHardwarePwmData *data = static_cast<TpHardwarePwmData *>(data_);
	if(data)
		delete(data);
}


TpList<tpUInt8> TpHardwarePwm::getPwmNumbers()
{
	const std::string pwm_dir = "/sys/class/pwm";
	TpList<tpUInt8> controllers;

	// 打开 PWM 目录
	DIR* dir = opendir(pwm_dir.c_str());
	if (!dir) {
		fprintf(stderr,"[Error]: get pwm numbers error\n");
		return controllers;
	}

	// 遍历目录项
	struct dirent* entry;
	while ((entry = readdir(dir)) != nullptr) {
		std::string name(entry->d_name);
		
		// 检查是否是 pwmchip 设备
		if (name.find("pwmchip") == 0) {
			// 提取编号部分
			std::string num_str = name.substr(7); // "pwmchip" 长度是 7
			
			// 验证是否为纯数字
			if (!num_str.empty() && 
				std::all_of(num_str.begin(), num_str.end(), ::isdigit)) {
				try {
					int controller_num = std::stoi(num_str);
					controllers.push_back((tpUInt8)controller_num);
				} catch (const std::exception& e) {
					// 忽略转换失败的项目
					std::cerr << "Warning: Invalid controller number in " 
								<< name << ": " << e.what() << std::endl;
				}
			}
		}
	}

	// 关闭目录
	closedir(dir);

	return controllers;
}

/// @brief 打开设备
/// @return 
tpBool TpHardwarePwm::open()
{
	TpHardwarePwmData *data = static_cast<TpHardwarePwmData *>(data_);
	if(data->is_open)
		return TP_TRUE;
	// 检查是否已导出
	if(isExportPwm())
		return TP_TRUE;
	if(!exportPwm())
		return TP_FALSE;
	
	int err=0;
	while(1)
	{
		struct stat st;
		if(!isExportPwm())
		{
			usleep(10000);
		}
		else
			break; 
		err++;
		if(err>10)
			return TP_FALSE;
	}
    
	data->is_open=TP_TRUE;
	return TP_TRUE;
}

/// @brief 关闭设备
void TpHardwarePwm::close()
{
	TpHardwarePwmData *data = static_cast<TpHardwarePwmData *>(data_);
	if(!data->is_open)
		return ;
	unexportPwm();
}

int TpHardwarePwm::setDutyCycle(float duty)
{
	TpHardwarePwmData *data = static_cast<TpHardwarePwmData *>(data_);
	tpUInt32 duty_cycle=(tpUInt32)(duty*data->period/100.0);
	if(!writeToFile(data->period_path,std::to_string(duty_cycle)))
		return -1;
	data->duty_cycle=duty;
	return 0;
}

int TpHardwarePwm::setPeriod(tpUInt32 ns)
{
	TpHardwarePwmData *data = static_cast<TpHardwarePwmData *>(data_);
	if(!writeToFile(data->period_path,std::to_string(ns)))
		return -1;
	data->period=ns;
	return 0;
}



//导出gpio端口
bool TpHardwarePwm::exportPwm()
{
	TpHardwarePwmData *data = static_cast<TpHardwarePwmData *>(data_);
	return writeToFile(data->path, std::to_string(data->channel));
}

//是否已经导出
bool TpHardwarePwm::isExportPwm()
{
	TpHardwarePwmData *data = static_cast<TpHardwarePwmData *>(data_);
	struct stat st;
	if (stat(data->path.c_str(), &st) == 0) {
		return true; // 已导出
	}
	return false;
}

//取消导出gpio端口
bool TpHardwarePwm::unexportPwm()
{
	TpHardwarePwmData *data = static_cast<TpHardwarePwmData *>(data_);
	return writeToFile(data->path, std::to_string(data->channel));
}


ssize_t TpHardwarePwm::read(uint8_t* buffer, size_t length)		//禁止使用
{
	throw std::logic_error("This function is disabled for this");
}
ssize_t TpHardwarePwm::write(const uint8_t* data, size_t length)	//禁止使用
{
	throw std::logic_error("This function is disabled for this");
}