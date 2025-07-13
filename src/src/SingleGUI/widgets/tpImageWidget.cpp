#include "tpImageWidget.h"
#include "tpSurface.h"
#include "tpFileInfo.h"
#include "tpEvent.h"
#include "tpCanvas.h"

struct tpImageWidgetData
{
    tpString filePath = "";
    tpShared<tpSurface> fileSurface = nullptr;

    tpShared<tpSurface> cachedScaledSurface = nullptr;
    uint32_t cachedWidth = 0;
    uint32_t cachedHeight = 0;
};

tpImageWidget::tpImageWidget()
    : tpDialog()
{
    tpImageWidgetData *imageData = new tpImageWidgetData();
    imageData->fileSurface = tpMakeShared<tpSurface>();
    data_ = imageData;

    setBackGroundColor(_RGBA(0, 0, 0, 200));
}

tpImageWidget::~tpImageWidget()
{
    tpImageWidgetData *imageData = static_cast<tpImageWidgetData *>(data_);
    if (imageData)
    {
        delete imageData;
        imageData = nullptr;
        data_ = nullptr;
    }
}

void tpImageWidget::setImageFilePath(const tpString &filePath)
{
    tpFileInfo imageFile(filePath);
    if (!imageFile.exists())
        return;

    tpImageWidgetData *imageData = static_cast<tpImageWidgetData *>(data_);
    imageData->filePath = filePath;
    imageData->fileSurface->fromFile(filePath);
    update();
}

bool tpImageWidget::onMouseRleaseEvent(tpMouseEvent *event)
{
    tpDialog::onMouseRleaseEvent(event);

    close();

    return true;
}

bool tpImageWidget::onPaintEvent(tpObjectPaintEvent *event)
{
    tpImageWidgetData *imageData = static_cast<tpImageWidgetData *>(data_);

    tpDialog::onPaintEvent(event);

    tpCanvas *painter = event->canvas();

    uint32_t windowWidth = width();
    uint32_t windowHeight = height();

    // 获取图像尺寸
    uint32_t imageWidth = imageData->fileSurface->width();
    uint32_t imageHeight = imageData->fileSurface->height();

    // 计算缩放比例和最终尺寸
    double scaleRatio = 1.0;
    uint32_t finalWidth = imageWidth;
    uint32_t finalHeight = imageHeight;
    bool needsScaling = false;

    // 情况1：只有宽度超过窗口
    if (imageWidth > windowWidth && imageHeight <= windowHeight)
    {
        scaleRatio = static_cast<double>(windowWidth) / imageWidth;
        needsScaling = true;
    }
    // 情况2：只有高度超过窗口
    else if (imageHeight > windowHeight && imageWidth <= windowWidth)
    {
        scaleRatio = static_cast<double>(windowHeight) / imageHeight;
        needsScaling = true;
    }
    // 情况3：宽高都超过窗口
    else if (imageWidth > windowWidth && imageHeight > windowHeight)
    {
        double widthRatio = static_cast<double>(windowWidth) / imageWidth;
        double heightRatio = static_cast<double>(windowHeight) / imageHeight;
        scaleRatio = std::min(widthRatio, heightRatio); // 取最小值保证完整显示
        needsScaling = true;
    }

    // 计算最终尺寸
    if (needsScaling)
    {
        finalWidth = static_cast<uint32_t>(imageWidth * scaleRatio);
        finalHeight = static_cast<uint32_t>(imageHeight * scaleRatio);
    }

    // 计算居中位置
    uint32_t x = (windowWidth - finalWidth) / 2;
    uint32_t y = (windowHeight - finalHeight) / 2;

    // 绘制处理
    if (needsScaling)
    {
        // 创建缩放后的表面
        tpShared<tpSurface> scaledSurface;

        // 检查是否需要重新创建缩放表面
        if (!imageData->cachedScaledSurface ||
            imageData->cachedWidth != finalWidth ||
            imageData->cachedHeight != finalHeight)
        {
            scaledSurface = imageData->fileSurface->scaled(finalWidth, finalHeight);

            // 更新缓存
            imageData->cachedScaledSurface = scaledSurface;
            imageData->cachedWidth = finalWidth;
            imageData->cachedHeight = finalHeight;
        }
        else
        {
            // 使用缓存
            scaledSurface = imageData->cachedScaledSurface;
        }

        painter->paintSurface(x, y, scaledSurface);
    }
    else
    {
        // 无需缩放，直接绘制
        painter->paintSurface(x, y, imageData->fileSurface);
    }

    return true;
}

bool tpImageWidget::onResizeEvent(tpObjectResizeEvent *event)
{
    tpDialog::onResizeEvent(event);
    return true;
}