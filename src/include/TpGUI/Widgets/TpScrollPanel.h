#ifndef __TP_VSCROLL_PANEL_H
#define __TP_VSCROLL_PANEL_H

#include "TpWidget.h"
#include "TpSignalSlot.h"
#include "TpColors.h"
#include "TpEvent.h"
#include "TpVector.h"

TP_DEF_VOID_TYPE_VAR(ItpScrollPanelData);

class TpScrollPanel : public TpWidget
{
public:
	TpScrollPanel(TpWidget *parent = nullptr);
	virtual ~TpScrollPanel();

public:
	/// @brief 获取水平方向滚动条的值
	/// @return 取值范围为[0,100]
	uint32_t horizontalScrollBarValue();
	/// @brief 设置水平方向滚动条的值
	/// @param value 取值范围为[0,100]
	void setHorizontalScrollBarValue(const uint32_t &value);

	/// @brief 获取垂直方向滚动条的值
	/// @return 取值范围为[0,100]
	uint32_t verticalScrollBarValue();
	/// @brief 设置垂直方向滚动条的值
	/// @param value 取值范围为[0,100]
	void setVerticalScrollBarValue(const uint32_t &value);

	/// @brief 水平滚动条距离偏移像素量
	/// @return 偏移量值
	int32_t horizontalPostion();
	/// @brief 设置水平偏移量
	/// @param value 偏移量值
	void setHorizontalPostion(const int32_t &value);

	/// @brief 垂直滚动条距离偏移像素量
	/// @return 偏移量值
	int32_t verticalPostion();
	/// @brief 设置垂直偏移量
	/// @param value 偏移量值
	void setVerticalPostion(const int32_t &value);

	/// @brief 获取当前滚动模式
	/// @return 垂直滚动返回true，否则返回false
	bool scrollMode();
	/// @brief 设置滚轮滚动模式，默认滚轮滚动滚纵向
	/// @param isVertical 垂直滚动返回true，否则返回false
	void setScrollMode(const bool &isVertical = true);

public:
	/// @brief 向滚动窗口添加组件，组件坐标、宽高需外部手动设置
	/// @param object 组件指针
	/// @return 返回添加结果
	virtual bool addObject(TpWidget *object); // object must position (x>=0, y>=0)
	virtual bool delObject(TpWidget *object);

	/// @brief 设置滚动区域的窗口，内部会调整窗口充满滚动区域
	/// @param widget 窗口指针
	/// @return 添加结果
	virtual bool setWidget(TpWidget *widget);

	/// @brief 获取设置的滚动窗口
	/// @return 窗口指针，不存在则返回空
	TpWidget *widget();

	/// @brief 获取滚动窗体内所有添加的子组件
	/// @return 子组件列表
	TpVector<TpWidget *> children();

	// 清除所有object，内部会直接释放指针
	bool clearObject();

public:
	virtual bool recal(bool enableOffset = false); // when child rect be changed, call this function

    virtual TpString pluginType() { return TO_STRING(TpScrollPanel); }

public:
	virtual bool onPaintEvent(TpPaintEvent *event) override;
	virtual bool eventFilter(TpObject *watched, TpEvent *event) override;
	virtual bool onMousePressEvent(TpMouseEvent *event) override;
	virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
	virtual bool onMouseMoveEvent(TpMouseEvent *event) override;
    virtual bool onWheelEvent(TpWheelEvent *event) override;
	virtual bool onResizeEvent(TpResizeEvent *event) override;
	virtual bool onLeaveEvent(TpLeaveEvent *event) override;

private:
	ItpScrollPanelData *data_;
};

#endif
