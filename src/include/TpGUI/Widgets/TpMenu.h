
#ifndef __TP_VMENU_H
#define __TP_VMENU_H

#include "TpDialog.h"
#include "TpString.h"
#include "TpSignalSlot.h"

TP_DEF_VOID_TYPE_VAR(ITpMenuData);
/// @brief 弹出菜单窗体
class TpMenu : public TpDialog
{
public:
	TpMenu();
	virtual ~TpMenu();

	/// @brief 非模态顶层显示menu菜单
	/// @param globalPos 屏幕全局X，Y坐标
	void exec(const TpPoint &globalPos);

	/// @brief 顶层显示menu菜单
	/// @param globalPosX 屏幕全局X坐标
	/// @param globalPosY 屏幕全局Y坐标
	void exec(const int32_t &globalPosX, const int32_t &globalPosY);

	/// @brief 添加一个操作按钮
	/// @param text 按钮文本
	/// @return 返回按钮ID，当前menu下唯一
	uint32_t addItem(const TpString &text);

	/// @brief 添加一个操作按钮
	/// @param text 按钮文本
	/// @param iconPath 按钮图标绝对路径
	/// @return 返回按钮ID，当前menu下唯一
	uint32_t addItem(const TpString &text, const TpString &iconPath);

	/// @brief 添加子菜单;暂未实现
	/// @param menuName 子菜单名称
	/// @param menu 子菜单指针，外部无需释放
	void addMenu(const TpString &menuName, TpMenu *menu);

	/// @brief 添加子菜单;暂未实现
	/// @param menuName 子菜单名称
	/// @return 返回子菜单指针，子菜单指针，外部无需释放
	TpMenu *addMenu(const TpString &menuName);

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
	TpString itemText(const uint32_t &index);

public
signals:
	/// @brief 点击item
	/// @param uint32_t 当前下标索引
	declare_signal(onClicked, uint32_t);

protected:
	virtual bool onFocusEvent(TpFocusEvent *event) override;
	virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
	virtual bool onMouseMoveEvent(TpMouseEvent *event) override;
	virtual bool onLeaveEvent(TpLeaveEvent *event) override;
	virtual bool onPaintEvent(TpPaintEvent *event) override;

protected:
	virtual TpString pluginType() override { return TO_STRING(TpMenu); }

private:
	ITpMenuData *data_;
};

#endif
