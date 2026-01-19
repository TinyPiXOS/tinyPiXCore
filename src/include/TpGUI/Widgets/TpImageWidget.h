#ifndef __TP_IMAGE_WIDGET_H
#define __TP_IMAGE_WIDGET_H

#include "TpDialog.h"
#include "TpSignalSlot.h"
#include "TpString.h"

TP_DEF_VOID_TYPE_VAR(ITpImageWidgetData);
/// @brief 图片资源查看窗体
class TpImageWidget : public TpDialog
{
public:
    TpImageWidget();

    virtual ~TpImageWidget();

    /// @brief 设置图片资源路径，支持png, jpg, bmp, xpm, web, tiff, SVG
    /// @param filePath 图片资源路径
    void setImageFilePath(const TpString &filePath);

protected:
	virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool onResizeEvent(TpResizeEvent *event) override;

protected:
    virtual TpString pluginType() override { return TO_STRING(TpImageWidget); }

private:
    ITpImageWidgetData *data_;
};

#endif
