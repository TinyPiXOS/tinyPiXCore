#ifndef __TP_DIAPLAY_MANAGE_H
#define __TP_DIAPLAY_MANAGE_H

#include "tpDisplay.h"
#include "tpString.h"
#include "tpList.h"

TP_DEF_VOID_TYPE_VAR(ItpDisplayManageData);
class tpDisplayManage
{
public:
	tpDisplayManage();
	~tpDisplayManage();

public:
	/// @brief 获取显示器列表
	/// @return 
	const tpList<tpShared<tpDisplay>> &getDeviceList() const;
	/// @brief 使用显示器名称来获取显示器
	/// @param name 显示器名称
	/// @return 
	tpShared<tpDisplay> getDevice(const tpString &name);
	/// @brief 使用编号来获取单个物理屏幕(显示器)
	/// @param num 显示器编号
	/// @return 
	tpShared<tpDisplay> getDevice(tpInt32 num);
	/// @brief 获取逻辑屏幕高度(注意：逻辑屏幕不是物理屏幕)
	/// @return 
	tpInt32 getPhysicsHeight();
	/// @brief 获取逻辑屏幕宽度(注意：逻辑屏幕不是物理屏幕)
	/// @return 
	tpInt32 getPhysicsWidth();
	/// @brief 获取逻辑屏幕分辨率高度(注意：逻辑屏幕不是物理屏幕)
	/// @return 
	tpInt32 getResolutionHeight();
	/// @brief 获取逻辑屏幕分辨率宽度(注意：逻辑屏幕不是物理屏幕)
	/// @return 
	tpInt32 getResolutionWidth();
	/// @brief 获取PiXWM物理高度
	/// @return 
	tpInt32 getPiXWMPhysicsHeight();
	/// @brief 获获取PiXWM物理宽度
	/// @return 
	tpInt32 getPiXWMPhysicsWidth();
	/// @brief 重置鼠标指针位置，用于在修改分辨率使用，但是目前此函数存在问题
	/// @return 
	tpInt32 correctMousePosition();

private:
	ItpDisplayManageData* data_;
};

#endif
