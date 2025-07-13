/*///------------------------------------------------------------------------------------------------------------------------//
		网卡速率信息
说 明 :
日 期 : 2024.11.06

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "tpNetworkInfo.h"
#include "tpSystemDataManage.h"



struct tpNetworkInfoParam
{
	tpString name;
	uint64_t rx_bytes;
	uint64_t tx_bytes;
	double tx_speed;
	double rx_speed;
	tpNetworkInfoParam(const tpString &name) : name(name), rx_bytes(0), tx_bytes(0), tx_speed(0.0), rx_speed(0.0) {}
	tpNetworkInfoParam() {}
};

struct tpNetworkInfoData
{
	tpSystemDataManage data;
	tpNetworkInfoParam param_t; // 线程使用，外部禁止直接使用

	tpNetworkInfoData()
	{
	}
};


tpNetworkInfo::tpNetworkInfo(tpString &name, tpBool enabled, uint16_t samp)
{
	data_ = new tpNetworkInfoData();

	tpNetworkInfoData *netData = static_cast<tpNetworkInfoData *>(data_);

	netData->param_t.name = name;
	// 添加获取其他信息的处理
	if (enabled)
	{
		netData->data.running = true;
		uint16_t time_samp = samp;
		netData->data.thread_t = std::thread(&tpNetworkInfo::threadUpdate, this, time_samp);
	}
}

tpNetworkInfo::~tpNetworkInfo()
{
	tpNetworkInfoData *netData = static_cast<tpNetworkInfoData *>(data_);

	netData->data.running = false;
	if (netData->data.thread_t.joinable())
		netData->data.thread_t.join(); // 等待线程完成
}

void tpNetworkInfo::threadUpdate(uint16_t time_samp)
{
	tpNetworkInfoData *netData = static_cast<tpNetworkInfoData *>(data_);

	//	uint16_t time_samp=*(uint16_t*)(arg);		//采样时间
	double time_s = (double)time_samp * 0.001;
	uint64_t rx_bytes_l, tx_bytes_l;
	double tx_speed, rx_speed;
	updateBytes();
	rx_bytes_l = netData->param_t.rx_bytes;
	tx_bytes_l = netData->param_t.tx_bytes;
	while (netData->data.running)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(time_samp)); //
		updateBytes();
		rx_speed = (double)(netData->param_t.rx_bytes - rx_bytes_l) / time_s;
		tx_speed = (double)(netData->param_t.tx_bytes - tx_bytes_l) / time_s;
		// printf("rx:%ld  rx_l:%ld   tx:%ld  tx_l:%ld\n",netData->param_t.rx_bytes,rx_bytes_l,netData->param_t.tx_bytes,tx_bytes_l);
		rx_bytes_l = netData->param_t.rx_bytes;
		tx_bytes_l = netData->param_t.tx_bytes;

		updateInfo(tx_speed, rx_speed);
	}
}

void tpNetworkInfo::updata()
{
	tpNetworkInfoData *netData = static_cast<tpNetworkInfoData *>(data_);
	if(netData->data.running == true)
		return ;
	updateBytes();
}

// 重新读取
int tpNetworkInfo::updateBytes()
{
	tpNetworkInfoData *netData = static_cast<tpNetworkInfoData *>(data_);

	std::ifstream fd("/proc/net/dev");
	if (!fd.is_open())
	{
		return -1;
	}
	tpString line;
	std::getline(fd, line);
	std::getline(fd, line);
	while (std::getline(fd, line))
	{
		if (line.find(netData->param_t.name) != std::string::npos)
		{
			std::stringstream ss(line);
			std::string iface;
			uint64_t receiveBytes, receivePackets, transmitBytes, transmitPackets;
			ss >> iface >> receiveBytes >> receivePackets; // Receive stats
			ss.ignore(5, ' ');
			ss >> transmitBytes >> transmitPackets; // Transmit stats

			netData->param_t.rx_bytes = receiveBytes;
			netData->param_t.tx_bytes = transmitBytes;
		}
	}
	return 0;
}

double tpNetworkInfo::getDownloadSpeed()
{
	tpNetworkInfoData *netData = static_cast<tpNetworkInfoData *>(data_);

	netData->data.dataReadLock();
	double speed = netData->param_t.rx_speed;
	netData->data.dataUnlock();
	return speed;
}

double tpNetworkInfo::getUploadSpeed()
{
	tpNetworkInfoData *netData = static_cast<tpNetworkInfoData *>(data_);

	netData->data.dataReadLock();
	double speed = netData->param_t.tx_speed;
	netData->data.dataUnlock();
	return speed;
}

void tpNetworkInfo::updateInfo(double tx, double rx)
{
	tpNetworkInfoData *netData = static_cast<tpNetworkInfoData *>(data_);

	netData->data.dataWriteLock();
	netData->param_t.tx_speed = tx;
	netData->param_t.rx_speed = rx;
	netData->data.dataUnlock();
}
