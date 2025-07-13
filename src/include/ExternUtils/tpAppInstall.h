#ifndef __TP_APP_MANEGE_H
#define __TP_APP_MANAGE_H

#include "tpUtils.h"
#include "tpUUID.h"

TP_DEF_VOID_TYPE_VAR(ItpAppInstallData);

class tpAppInstall
{
public:
	enum tpInstallErrorCode{
		TP_INSTALL_SUCCESS=0,
		TP_INSTALL_ERROR_ARCK=-1,
		TP_INSTALL_ERROR_SPACE=-2,
		TP_INSTALL_ERROR_MD5=-3,
		TP_INSTALL_ERROR_VERSION=-4,
		TP_INSTALL_ERROR_NONE_FILE=-9,
		TP_INSTALL_ERROR_OTHER=-10,
	};

	/// 安装包类型
	enum tpAppPackageType{
		TP_PACKAGE_TYPE_NONE,
		TP_PACKAGE_TYPE_LIB,
		TP_PACKAGE_TYPE_SYSAPP,
		TP_PACKAGE_TYPE_USRAPP,
	};
	/// 硬件架构（计划增加此枚举或类）
	enum tpArch
	{

	};

public:
	tpAppInstall(const tpString &path);
	tpAppInstall();
	~tpAppInstall();

public:
	/// @brief 获取安装包类型(system_app,user_app,lib,none)
	/// @return
	tpAppPackageType getPackageType();

	/// @brief 硬件架构检查
	/// @return
	int archCheck();

	/// @brief 空间检查
	/// @return
	int spaceCheck();

	/// @brief 版本检查
	/// @return
	int versionCheck();

	/// @brief MD5校验
	/// @return
	int completeCheck();

	/// @brief 安装环境整体检查(硬件架构，剩余空间，版本)
	/// @return
	tpAppInstall::tpInstallErrorCode checkAll();

	/// @brief 设置安装包路径
	/// @param path 安装包路径
	/// @return 
	int setPath(const tpString &path);

	/// @brief 获取安装包版本
	/// @return
	tpString getPackVersion();

	/// @brief 获取安装包支持的硬件架构
	/// @return
	tpString getPackArch();

	/// @brief 获取安装包需要的空间大小
	/// @return
	int getPackSpace();

	/// @brief 获取已安装的版本
	/// @return
	tpString getNowVersion();

	/// @brief 获取安装包的应用UUID
	/// @return
	tpString getAppUUID();

	/// @brief 获取安装包的应用名字
	/// @return 
	tpString getAppName();

	/// @brief 获取app的图标路径
	/// @return 
	tpString getIcon();

	/// @brief 查看应用是否已安装
	/// @return 已安装返回TP_TRUE
	tpBool isInstall();

	/// @brief 安装应用
	/// @return
	tpAppInstall::tpInstallErrorCode install();
	tpAppInstall::tpInstallErrorCode installTest();

	/// @brief 获取安装进度
	/// @return
	int getInstallSchedule();

	/// @brief 卸载应用
	/// @param uuid 应用uuid
	/// @return 
	static int remove(tpString& uuid);
	/// @brief 应用升级
	/// @return
	int update();

private:
	int threadInstall();
	int threadInstallTest();
	ItpAppInstallData *data_;
};

#endif
