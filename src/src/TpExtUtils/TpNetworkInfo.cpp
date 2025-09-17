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
#include "TpNetworkInfo.h"
#include "TpSystemDataManage.h"



struct TpNetworkInfoParam
{
	TpString name;
	uint64_t rx_bytes;
	uint64_t tx_bytes;
	double tx_speed;
	double rx_speed;
	TpNetworkInfoParam(const TpString &name) : name(name), rx_bytes(0), tx_bytes(0), tx_speed(0.0), rx_speed(0.0) {}
	TpNetworkInfoParam() {}
};

struct TpNetworkInfoData
{
	TpSystemDataManage data;
	TpNetworkInfoParam param_t; // 线程使用，外部禁止直接使用

	TpNetworkInfoData()
	{
	}
};


TpNetworkInfo::TpNetworkInfo(TpString &name, tpBool enabled, uint16_t samp)
{
	data_ = new TpNetworkInfoData();

	TpNetworkInfoData *netData = static_cast<TpNetworkInfoData *>(data_);

	netData->param_t.name = name;
	// 添加获取其他信息的处理
	if (enabled)
	{
		netData->data.running = true;
		uint16_t time_samp = samp;
		netData->data.thread_t = std::thread(&TpNetworkInfo::threadUpdate, this, time_samp);
	}
}

TpNetworkInfo::~TpNetworkInfo()
{
	TpNetworkInfoData *netData = static_cast<TpNetworkInfoData *>(data_);

	netData->data.running = false;
	if (netData->data.thread_t.joinable())
		netData->data.thread_t.join(); // 等待线程完成
}

void TpNetworkInfo::threadUpdate(uint16_t time_samp)
{
	TpNetworkInfoData *netData = static_cast<TpNetworkInfoData *>(data_);

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

void TpNetworkInfo::updata()
{
	TpNetworkInfoData *netData = static_cast<TpNetworkInfoData *>(data_);
	if(netData->data.running == true)
		return ;
	updateBytes();
}

// 重新读取
int TpNetworkInfo::updateBytes()
{
	TpNetworkInfoData *netData = static_cast<TpNetworkInfoData *>(data_);

	std::ifstream fd("/proc/net/dev");
	if (!fd.is_open())
	{
		return -1;
	}
	TpString line;
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

double TpNetworkInfo::getDownloadSpeed()
{
	TpNetworkInfoData *netData = static_cast<TpNetworkInfoData *>(data_);

	netData->data.dataReadLock();
	double speed = netData->param_t.rx_speed;
	netData->data.dataUnlock();
	return speed;
}

double TpNetworkInfo::getUploadSpeed()
{
	TpNetworkInfoData *netData = static_cast<TpNetworkInfoData *>(data_);

	netData->data.dataReadLock();
	double speed = netData->param_t.tx_speed;
	netData->data.dataUnlock();
	return speed;
}

void TpNetworkInfo::updateInfo(double tx, double rx)
{
	TpNetworkInfoData *netData = static_cast<TpNetworkInfoData *>(data_);

	netData->data.dataWriteLock();
	netData->param_t.tx_speed = tx;
	netData->param_t.rx_speed = rx;
	netData->data.dataUnlock();
}
