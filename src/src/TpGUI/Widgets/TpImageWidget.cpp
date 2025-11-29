#include "TpImageWidget.h"
#include "TpImage.h"
#include "TpFileInfo.h"
#include "TpEvent.h"
#include "TpPainter.h"

struct TpImageWidgetData
{
    TpString filePath = "";
    TpImage fileImage;

    TpImage cachedScaledImage;
    uint32_t cachedWidth = 0;
    uint32_t cachedHeight = 0;
};

TpImageWidget::TpImageWidget()
    : TpDialog()
{
    TpImageWidgetData *imageData = new TpImageWidgetData();
    data_ = imageData;

    setBackGroundColor(_RGBA(0, 0, 0, 200));
}

TpImageWidget::~TpImageWidget()
{
    TpImageWidgetData *imageData = static_cast<TpImageWidgetData *>(data_);
    if (imageData)
    {
        delete imageData;
        imageData = nullptr;
        data_ = nullptr;
    }
}

void TpImageWidget::setImageFilePath(const TpString &filePath)
{
    TpFileInfo imageFile(filePath);
    if (!imageFile.exists())
        return;

    TpImageWidgetData *imageData = static_cast<TpImageWidgetData *>(data_);
    imageData->cachedScaledImage.setNull();
    imageData->filePath = filePath;
    imageData->fileImage.load(filePath);
    update();
}

bool TpImageWidget::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpDialog::onMouseRleaseEvent(event);

    close();

    return true;
}

bool TpImageWidget::onPaintEvent(TpPaintEvent *event)
{
    TpImageWidgetData *imageData = static_cast<TpImageWidgetData *>(data_);

    TpDialog::onPaintEvent(event);

    TpPainter *painter = event->painter();

    uint32_t windowWidth = width();
    uint32_t windowHeight = height();

    // 获取图像尺寸
    uint32_t imageWidth = imageData->fileImage.width();
    uint32_t imageHeight = imageData->fileImage.height();

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
        TpImage scaledSurface;

        // 检查是否需要重新创建缩放表面
        if (imageData->cachedScaledImage.isNull() ||
            imageData->cachedWidth != finalWidth ||
            imageData->cachedHeight != finalHeight)
        {
            scaledSurface = imageData->fileImage.scaled(finalWidth, finalHeight);

            // 更新缓存
            imageData->cachedScaledImage = scaledSurface;
            imageData->cachedWidth = finalWidth;
            imageData->cachedHeight = finalHeight;
        }
        else
        {
            // 使用缓存
            scaledSurface = imageData->cachedScaledImage;
        }

        painter->drawImage(x, y, scaledSurface);
    }
    else
    {
        // 无需缩放，直接绘制
        painter->drawImage(x, y, imageData->fileImage);
    }

    return true;
}

bool TpImageWidget::onResizeEvent(TpResizeEvent *event)
{
    TpDialog::onResizeEvent(event);
    return true;
}