/*///------------------------------------------------------------------------------------------------------------------------//
		硬件通信接口的擦欧在哦基类
说 明 : 
日 期 : 2025.08.28

/*///------------------------------------------------------------------------------------------------------------------------//、

#include <iostream>
#include <fstream>
#include "TpHardwareDevice.h"

//向文件中写入值
bool TpHardwareDevice::writeToFile(const TpString& path, const TpString& value)
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
TpString TpHardwareDevice::readFromFile(const TpString& path)
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
