#ifndef __TP_NETWORK_INFO_H
#define __TP_NETWORK_INFO_H

#include "tpUtils.h"
#include "tpThreadManage.h"

TP_DEF_VOID_TYPE_VAR(ItpNetworkInfoData);

class tpNetworkInfo
{
public:
	tpNetworkInfo(tpString &name, tpBool enabled = TP_TRUE, uint16_t samp = 500);
	~tpNetworkInfo();

public:
	/// @brief 获取网卡下载速度
	/// @return Byte/s
	double getDownloadSpeed();
	/// @brief 获取网卡上传速度
	/// @return Byte/s
	double getUploadSpeed();
	/// @brief 更新信息(当使能自动更新的时候不需要使用)
	void updata();

private:
	int updateBytes();
	void threadUpdate(uint16_t time_samp); // 更新信息线程
	void updateInfo(double tx, double rx);

private:
	ItpNetworkInfoData* data_;

};


#endif
