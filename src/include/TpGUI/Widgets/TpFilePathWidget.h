#ifndef __TP_FILE_PATH_WIDGET_H
#define __TP_FILE_PATH_WIDGET_H

#include "TpWidget.h"
#include "TpSignalSlot.h"
#include "TpString.h"
#include "TpFilePathWidgetItem.h"

TP_DEF_VOID_TYPE_VAR(ItpFilePathWidgetData);

/// @brief 路径显示按钮类
class TpFilePathWidget : public TpWidget
{
public:
    TpFilePathWidget(TpWidget *parent = nullptr);

    virtual ~TpFilePathWidget();

public:
    /// @brief 设置根目录，根目录之前不会在ui上显示，必须先设置根目录，在设置setPath
    /// @param rootPath 根路径
    virtual void setRootPath(const TpString &rootPath);

    /// @brief 获取根目录
    /// @return 根目录字符串
    virtual TpString rootPath();

    /// @brief 设置路径
    /// @param path 路径
    virtual void setPath(const TpString &path);

    /// @brief 获取当前路径
    /// @return 当前路径字符串
    virtual TpString path();

public
signals:
    /// @brief 点击切换路径信号，如果点击的路径和当前显示路径一致则不会触发该信号
    /// @param TpString 切换之前的路径
    /// @param TpString 切换之后当前路径
    declare_signal(onPathChanged, const TpString &, const TpString &);

protected:
	virtual bool onMousePressEvent(TpMouseEvent *event) override;
	virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool onResizeEvent(TpResizeEvent *event) override;

    virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

    /// @brief
    /// @param event
    virtual void onThemeChangeEvent(TpThemeChangeEvent *event) override;

protected:
    virtual TpString pluginType() override { return TO_STRING(TpFilePathWidget); }

private:
    ItpFilePathWidgetData *data_;
};

#endif
