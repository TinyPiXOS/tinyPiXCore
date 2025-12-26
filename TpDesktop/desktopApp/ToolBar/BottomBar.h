#ifndef __BOTTOM_BAR_H
#define __BOTTOM_BAR_H

#include "TpDialog.h"
#include "TpList.h"
#include "DeskTopGlobal.hpp"
#include "DesktopAppButton.h"

class BottomBar
	: public TpWidget
{
public:
	// 顶层窗口指针
	BottomBar(TpScreen* topScreen);
	virtual ~BottomBar();

	virtual void setVisible(bool visible = true) override;

	// 设置工具栏按钮列表
	void setBottomAppBtn(const TpList<DesktopAppButton*>& appList);

	/// @brief 获取底部工具栏APP按钮列表
	/// @return 
	TpList<DesktopAppButton*> bottomAppBtnList();

	// 卸载应用
	void removeApp(DesktopAppButton* removeBtn);

protected:
	virtual bool onResizeEvent(TpResizeEvent *event) override;
	virtual bool onMoveEvent(TpMoveEvent *event) override;
    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;

private:
	void caculateBottomAppPos();

private:
	TpScreen* topScreen_;

	// 系统底部应用图标信息
	TpList<DesktopAppButton *> systemAppBtnList_;

	// 所有在底部的icon列表
	// TpList<ApplicationInfoSPtr> bottomAppInfoList_;
};

#endif
