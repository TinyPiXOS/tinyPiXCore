#ifndef __TP_IMAGE_WIDGET_H
#define __TP_IMAGE_WIDGET_H

#include "tpDialog.h"
#include "tpSignalSlot.h"
#include "tpString.h"

TP_DEF_VOID_TYPE_VAR(ItpImageWidgetData);
/// @brief 图片资源查看窗体
class tpImageWidget : public tpDialog
{
public:
    tpImageWidget();

    virtual ~tpImageWidget();

    /// @brief 设置图片资源路径，支持png, jpg, bmp, xpm, web, tiff, SVG
    /// @param filePath 图片资源路径
    void setImageFilePath(const tpString &filePath);

protected:
	virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;

protected:
    virtual tpString pluginType() override { return TO_STRING(tpImageWidget); }

private:
    ItpImageWidgetData *data_;
};

#endif
