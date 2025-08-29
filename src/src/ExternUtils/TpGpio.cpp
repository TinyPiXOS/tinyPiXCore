/*///------------------------------------------------------------------------------------------------------------------------//
		GPIO控制接口
说 明 : 
日 期 : 2025.08.28

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "TpGpio.h"

#define GPIO_DEVICE_PATH "/sys/class/gpio/gpio"		//后面拼接gpio编号


struct TpGpioData{
	TpString path;			//gpio路径
	tpUInt16 number;		//gpio编号
	tpBool is_open;
	TpString direction_path;	//输入输出模式设置
	TpString edge_path;			//中断触发模式
	TpString value_path;		//输入输出电平
	TpGpio::GpioInterruptType edge;
	TpGpioData(){
		is_open=TP_FALSE;

		edge=TpGpio::EDGE_NONE;
	}
};


TpGpio::TpGpio(tpUInt16 number)
{
	const TpString name=GPIO_DEVICE_PATH+std::to_string(static_cast<int>(number));
	data_ = new TpGpioData();
	TpGpioData *data = static_cast<TpGpioData *>(data_);
	data->number=number;
	data->path=name;
	data->direction_path=name+"/direction";
	data->edge_path=name+"/edge";
	data->value_path=name+"/value";

}

TpGpio::~TpGpio()
{
	TpGpioData *data = static_cast<TpGpioData *>(data_);
	if(!data)
		return ;
	if(data->is_open)
		close();
	delete(data);
}

tpBool TpGpio::open()
{
	TpGpioData *data = static_cast<TpGpioData *>(data_);
	if(data->is_open)
		return TP_TRUE;
	// 检查是否已导出
	if(isExportGpio())
		return TP_TRUE;
	if(!exportGpio())
		return TP_FALSE;
	
	int err=0;
	while(1)
	{
		struct stat st;
		if(!isExportGpio())
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

void TpGpio::close()
{
	TpGpioData *data = static_cast<TpGpioData *>(data_);
	unexportGpio();
}

ssize_t TpGpio::read(uint8_t* buffer, size_t size) 
{
	TpGpioData *data = static_cast<TpGpioData *>(data_);
	if(!buffer)
		return -1;
	if(getLevel())
		buffer[0]='1';
	else
		buffer[0]='0';
    return 1;
}

ssize_t TpGpio::write(const uint8_t* buffer, size_t size) 
{
	TpGpioData *data = static_cast<TpGpioData *>(data_);
	if(!buffer)
		return -1;
	if(buffer[0]=='0')
		setLow();
	if(buffer[0]=='1')
		setHeight();
	else
		return -1;
	return 1;
}

int TpGpio::setDirection(TpGpio::GpioDirectionType type)
{
	TpGpioData *data = static_cast<TpGpioData *>(data_);
	if(type==TpGpio::INPUT)
		return (writeToFile(data->direction_path,TpString("in"))? 0: -1);
	else
		return (writeToFile(data->direction_path,TpString("out"))? 0: -1);
}


TpGpio::GpioDirectionType TpGpio::getDirection()
{
	TpGpioData *data = static_cast<TpGpioData *>(data_);
	TpString type=readFromFile(data->direction_path);
	if(type==TpString("in") || type==TpString("IN"))
		return TpGpio::INPUT;
	else	
		return TpGpio::OUTPUT;
}


int TpGpio::setHeight()
{
	TpGpioData *data = static_cast<TpGpioData *>(data_);
	return (writeToFile(data->value_path,TpString("1"))? 0: -1);
}

int TpGpio::setLow()
{
	TpGpioData *data = static_cast<TpGpioData *>(data_);
	return (writeToFile(data->value_path,TpString("0"))? 0: -1);
}

tpBool TpGpio::getLevel()
{
	TpGpioData *data = static_cast<TpGpioData *>(data_);
	TpString level=readFromFile(data->value_path);
	return (level==TpString("0"))?TP_FALSE:TP_TRUE;
}


//导出gpio端口
bool TpGpio::exportGpio()
{
	TpGpioData *data = static_cast<TpGpioData *>(data_);
	return writeToFile("/sys/class/gpio/export", std::to_string(data->number));
}

//是否已经导出
bool TpGpio::isExportGpio()
{
	TpGpioData *data = static_cast<TpGpioData *>(data_);
	struct stat st;
	if (stat(data->value_path.c_str(), &st) == 0) {
		return true; // 已导出
	}
	return false;
}

//取消导出gpio端口
bool TpGpio::unexportGpio()
{
	TpGpioData *data = static_cast<TpGpioData *>(data_);
	return writeToFile("/sys/class/gpio/unexport", std::to_string(data->number));
}

//向文件中写入值
bool TpGpio::writeToFile(const TpString& path, const TpString& value)
{
	std::ofstream file(path);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << path << std::endl;
		return false;
	}

	file << value;
	file.close();

	return !file.fail();
}

//从文件中读取值
TpString TpGpio::readFromFile(const TpString& path)
{
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << path << std::endl;
		return "";
	}

	std::string value;
	file >> value;
	file.close();

	return value;
}

    