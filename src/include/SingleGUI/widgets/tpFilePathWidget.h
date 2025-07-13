#ifndef __TP_FILE_PATH_WIDGET_H
#define __TP_FILE_PATH_WIDGET_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpString.h"
#include "tpFilePathWidgetItem.h"

TP_DEF_VOID_TYPE_VAR(ItpFilePathWidgetData);

/// @brief 路径显示按钮类
class tpFilePathWidget : public tpChildWidget
{
public:
    tpFilePathWidget(tpChildWidget *parent = nullptr);

    virtual ~tpFilePathWidget();

public:
    /// @brief 设置根目录，根目录之前不会在ui上显示，必须先设置根目录，在设置setPath
    /// @param rootPath 根路径
    virtual void setRootPath(const tpString &rootPath);

    /// @brief 获取根目录
    /// @return 根目录字符串
    virtual tpString rootPath();

    /// @brief 设置路径
    /// @param path 路径
    virtual void setPath(const tpString &path);

    /// @brief 获取当前路径
    /// @return 当前路径字符串
    virtual tpString path();

public
signals:
    /// @brief 点击切换路径信号，如果点击的路径和当前显示路径一致则不会触发该信号
    /// @param tpString 切换之前的路径
    /// @param tpString 切换之后当前路径
    declare_signal(onPathChanged, const tpString &, const tpString &);

protected:
	virtual bool onMousePressEvent(tpMouseEvent *event) override;
	virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;

    virtual bool eventFilter(tpObject *watched, tpEvent *event) override;

    /// @brief
    /// @param event
    virtual void onThemeChangeEvent(tpThemeChangeEvent *event) override;

protected:
    virtual tpString pluginType() override { return TO_STRING(tpFilePathWidget); }

private:
    ItpFilePathWidgetData *data_;
};

#endif
