#ifndef __TP_DIALOG_H
#define __TP_DIALOG_H

#include "tpScreen.h"

TP_DEF_VOID_TYPE_VAR(ItpDialogData);
class tpDialog
	: public tpScreen
{
public:
	tpDialog(const char *type = "tinyPiX_USE_Float");

	virtual ~tpDialog();

	/// @brief 模态显示
	/// @return 返回点击ID索引
	virtual uint32_t exec();

	/// @brief 关闭窗口
	virtual void close() override;

public:
	virtual ItpObjectType objectType() final;

	/// @brief 组件类名，子类实现，返回子类类名字符串，用于匹配CSS中对应样式
	/// @return 类名字符串
	virtual tpString pluginType() override { return TO_STRING(tpDialog); }

private:
	ItpDialogData *data_;
};

#endif
