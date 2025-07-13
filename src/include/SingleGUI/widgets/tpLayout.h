#ifndef __TP_LAYOUT_H
#define __TP_LAYOUT_H

#include "tpChildWidget.h"
#include "tpSpacerItem.h"
#include "tpSignalSlot.h"

TP_DEF_VOID_TYPE_VAR(ItpLayoutData);
class tpLayout
	: public tpObject
{
public:
	tpLayout(tpChildWidget *parent = nullptr);

	virtual ~tpLayout();

	/// @brief 向布局内添加窗口
	/// @param widget 窗口指针
	/// @param stretch 缩放比例
	virtual void addWidget(tpChildWidget *widget, int stretch = 1);

	/// @brief 向布局内添加布局
	/// @param layout 布局指针
	/// @param stretch 缩放比例
	virtual void addLayout(tpLayout *layout, int stretch = 1);

	/// @brief 插入一个窗口
	/// @param index 插入索引，从0开始
	/// @param widget 窗口指针
	/// @param stretch 缩放比例
	virtual void insertWidget(uint32_t index, tpChildWidget *widget, int stretch = 1);

	/// @brief 插入一个布局
	/// @param index 插入索引，从0开始
	/// @param layout 布局指针
	/// @param stretch 缩放比例
	virtual void insertLayout(uint32_t index, tpLayout *layout, int stretch = 1);

	/// @brief 从布局移除一个指定widget，不会释放指针
	/// @param widget widget指针
	virtual void removeWidget(tpChildWidget *widget);

	/// @brief 从布局移除一个子布局，不会释放指针
	/// @param layout 布局指针
	virtual void removeLayout(tpLayout *layout);

	/// @brief 添加一个弹簧
	/// @param spacer 弹簧指针
	virtual void addSpacer(tpSpacerItem *spacer);

	/// @brief 设置布局距离外边界距离（px）
	/// @param left 距离左边界距离
	/// @param top 距离上边界距离
	/// @param right 距离右边界距离
	/// @param bottom 距离下边界距离
	virtual void setContentsMargins(int32_t left, int32_t top, int32_t right, int32_t bottom);
	/// @brief 获取布局距离外边界距离（px）
	/// @param left 距离左边界距离
	/// @param top 距离上边界距离
	/// @param right 距离右边界距离
	/// @param bottom 距离下边界距离
	virtual void contentsMargins(int32_t *left, int32_t *top, int32_t *right, int32_t *bottom) const;

	/// @brief 设置间距
	/// @param spacing 间距值，单位px
	virtual void setSpacing(int spacing);
	/// @brief 获取布局间距
	/// @return 间距值，单位px
	virtual int spacing() const;

	/// @brief 布局刷新
	virtual void update();

	/// @brief 获取布局当前行数
	/// @return 布局行数
	virtual uint32_t rowCount();

	/// @brief 获取布局当前列数
	/// @return 布局列数
	virtual uint32_t columnCount();

	/// @brief 清空布局内所有子部件
	virtual void clear();

	/// @brief 获取子控件列表
	/// @return 控件指针列表
	virtual tpVector<tpObject*> children();

	/// @brief 获取布局的最小尺寸；内部根据布局内子控件计算布局的最小尺寸
	/// @return 最小尺寸
	virtual ItpSize minumumSize();

public
signals:
	/// @brief 布局更新事件
	declare_signal(onUpdate);

private:
	ItpLayoutData *data_;
};

#endif
