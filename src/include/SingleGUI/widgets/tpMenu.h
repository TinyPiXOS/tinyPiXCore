
#ifndef __TP_VMENU_H
#define __TP_VMENU_H

#include "tpDialog.h"
#include "tpString.h"
#include "tpSignalSlot.h"

TP_DEF_VOID_TYPE_VAR(ItpMenuData);
/// @brief 弹出菜单窗体
class tpMenu : public tpDialog
{
public:
	tpMenu();
	virtual ~tpMenu();

	/// @brief 非模态顶层显示menu菜单
	/// @param globalPos 屏幕全局X，Y坐标
	void exec(const ItpPoint &globalPos);

	/// @brief 顶层显示menu菜单
	/// @param globalPosX 屏幕全局X坐标
	/// @param globalPosY 屏幕全局Y坐标
	void exec(const int32_t &globalPosX, const int32_t &globalPosY);

	/// @brief 添加一个操作按钮
	/// @param text 按钮文本
	/// @return 返回按钮ID，当前menu下唯一
	uint32_t addItem(const tpString &text);

	/// @brief 添加一个操作按钮
	/// @param text 按钮文本
	/// @param iconPath 按钮图标绝对路径
	/// @return 返回按钮ID，当前menu下唯一
	uint32_t addItem(const tpString &text, const tpString &iconPath);

	/// @brief 添加子菜单
	/// @param menuName 子菜单名称
	/// @param menu 子菜单指针，外部无需释放
	void addMenu(const tpString &menuName, tpMenu *menu);

	/// @brief 添加子菜单
	/// @param menuName 子菜单名称
	/// @return 返回子菜单指针，子菜单指针，外部无需释放
	tpMenu *addMenu(const tpString &menuName);

	/// @brief 获取单个item的高度
	/// @return 返回item高度
	uint32_t itemHeight();

	/// @brief 设置item高度
	/// @param height 高度值
	void setItemHeight(const uint32_t &height);

	/// @brief 设置鼠标悬停item的背景颜色
	/// @param color 颜色_RGB
	void setItemHoverColor(const int32_t &color);

	/// @brief 清空菜单，所有子菜单指针均会被释放
	void clear();

	/// @brief 指定索引值获取item的文本
	/// @param index 索引值
	/// @return 文本字符串
	tpString itemText(const uint32_t &index);

public
signals:
	/// @brief 点击item
	/// @param uint32_t 当前下标索引
	declare_signal(onClicked, uint32_t);

protected:
	virtual bool onFocusEvent(tpObjectFocusEvent *event) override;
	virtual bool onMousePressEvent(tpMouseEvent *event) override;
	virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
	virtual bool onMouseMoveEvent(tpMouseEvent *event) override;
	virtual bool onLeaveEvent(tpObjectLeaveEvent *event) override;
	virtual bool onPaintEvent(tpObjectPaintEvent *event) override;

protected:
	virtual tpString pluginType() override { return TO_STRING(tpMenu); }

private:
	ItpMenuData *data_;
};

#endif
