#include "TpSlideProgressBar.h"
#include "TpImage.h"
#include "SystemInfo/TpDisplay.h"
#include "TpPainter.h"

struct TpSlideProgressBarData
{
    // 内部填充距外边界间距像素
    int32_t margin = 2;

    int32_t minValue = 0;
    int32_t maxValue = 0;
    double curValue = 0;

    TpImage iconImage;

    TpPoint pressPoint;
    bool mouseLeftPress = false;

    TpSlideProgressBarData()
    {
    }

    ~TpSlideProgressBarData()
    {
    }
};

enum SectorType
{
    SECTOR_S1, // 正西到正北
    SECTOR_S2, // 正南到正西
    SECTOR_S3, // 正北到正东
    SECTOR_S4  // 正东到正南
};
struct CircleResult
{
    TpPoint intersection; // 交点坐标
    double angle;         // 与正西/东方向的角度（弧度）
};

// 计算四个角的扇形；切线的XY坐标和 圆心和切线与圆弧交点连线 与正东/正西方向夹角
CircleResult calculateSectorIntersection(SectorType sectorType, TpPoint center, double radius, double lineX)
{
    CircleResult result;

    // 计算垂直线与圆的交点
    double dx = lineX - center.x();

    // 检查垂直线是否与圆相交
    if (std::abs(dx) > radius)
    {
        // 无交点
        result.intersection = TpPoint(lineX, center.y());
        result.angle = 0;
        return result;
    }

    // 计算交点y坐标
    double dy = std::sqrt(radius * radius - dx * dx);

    // 根据扇形类型选择正确的交点位置
    switch (sectorType)
    {
    case SECTOR_S1: // 正西到正北，上半圆
        result.intersection = TpPoint(lineX, center.y() - dy);
        break;
    case SECTOR_S2: // 正南到正西，下半圆
        result.intersection = TpPoint(lineX, center.y() + dy);
        break;
    case SECTOR_S3: // 正北到正东，上半圆
        result.intersection = TpPoint(lineX, center.y() - dy);
        break;
    case SECTOR_S4: // 正东到正南，下半圆
        result.intersection = TpPoint(lineX, center.y() + dy);
        break;
    }

    // 计算从圆心到交点的向量
    double vecX = result.intersection.x() - center.x();
    double vecY = result.intersection.y() - center.y();

    // 计算与参考方向的夹角
    double dotProduct, referenceX, referenceY;

    switch (sectorType)
    {
    case SECTOR_S1: // 参考方向为正西(-1, 0)
    case SECTOR_S2:
        referenceX = -1;
        referenceY = 0;
        break;
    case SECTOR_S3: // 参考方向为正东(1, 0)
    case SECTOR_S4:
        referenceX = 1;
        referenceY = 0;
        break;
    }

    dotProduct = vecX * referenceX + vecY * referenceY;
    double magnitude = std::sqrt(vecX * vecX + vecY * vecY);

    // 计算夹角（弧度）
    double angle = std::acos(dotProduct / magnitude);

    // 根据扇形类型调整角度范围
    switch (sectorType)
    {
    case SECTOR_S1: // 正西到正北，角度范围0到π/2
        // 不需要调整，点积计算的角度已经是正确的
        break;
    case SECTOR_S2: // 正南到正西，角度范围3π/2到2π
        if (vecY < 0)
        { // 如果向量指向上方
            angle = 2 * M_PI - angle;
        }
        break;
    case SECTOR_S3: // 正北到正东，角度范围π/2到π
        if (vecY > 0)
        { // 如果向量指向下方
            angle = 2 * M_PI - angle;
        }
        break;
    case SECTOR_S4: // 正东到正南，角度范围π到3π/2
        if (vecY < 0)
        { // 如果向量指向上方
            angle = 2 * M_PI - angle;
        }
        break;
    }

    result.angle = angle;
    return result;
}

TpSlideProgressBar::TpSlideProgressBar(TpWidget *parent)
    : TpWidget(parent)
{
    // 注册属性
    // TP_PROPERTY(int32_t, value, value, updateValue);

    data_ = new TpSlideProgressBarData();

    refreshBaseCss();
}

TpSlideProgressBar::~TpSlideProgressBar()
{
    TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);
    if (progressData)
    {
        delete progressData;
        progressData = nullptr;
        data_ = nullptr;
    }
}

void TpSlideProgressBar::setIcon(const TpString &iconPath)
{
    TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);
    progressData->iconImage.load(iconPath);
}

void TpSlideProgressBar::setRange(int32_t minValue, int32_t maxValue)
{
    TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);

    progressData->minValue = minValue;
    progressData->maxValue = maxValue;

    if (progressData->maxValue <= progressData->minValue)
    {
        progressData->maxValue = progressData->minValue + 1;
    }
}

void TpSlideProgressBar::setValue(int32_t value)
{
    TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);
    progressData->curValue = value;

    if (progressData->curValue < progressData->minValue)
        progressData->curValue = progressData->minValue;
    else if (progressData->curValue > progressData->maxValue)
        progressData->curValue = progressData->maxValue;
    else
    {
    }

    // onValueChanged.emit(progressData->curValue);

    update();
}

int32_t TpSlideProgressBar::value()
{
    TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);
    return progressData->curValue;
}

bool TpSlideProgressBar::onMousePressEvent(TpMouseEvent *event)
{
    TpWidget::onMousePressEvent(event);

    TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);

    if (event->button() == BUTTON_LEFT)
    {
        progressData->mouseLeftPress = event->state();
        progressData->pressPoint = event->globalPos();
    }

    return true;
}

bool TpSlideProgressBar::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpWidget::onMouseRleaseEvent(event);

    TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);

    if (event->button() == BUTTON_LEFT)
    {
        progressData->mouseLeftPress = event->state();
    }

    return true;
}

bool TpSlideProgressBar::onMouseMoveEvent(TpMouseEvent *event)
{
    TpWidget::onMouseMoveEvent(event);

    TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);
    if (progressData->mouseLeftPress)
    {
        TpPoint curPos = event->globalPos();
        int32_t offsetX = curPos.x() - progressData->pressPoint.x();

        progressData->pressPoint = curPos;

        // value偏移对应像素  (1.0 / (100 - 0))
        // double curValue = value() + (1.0 * offsetX) / ((width() - 4) / (progressData->maxValue - progressData->minValue));
        // setValue(curValue);

        progressData->curValue += (1.0 * offsetX) / ((width() - 4) / (progressData->maxValue - progressData->minValue));
        if (progressData->curValue < progressData->minValue)
            progressData->curValue = progressData->minValue;
        else if (progressData->curValue > progressData->maxValue)
            progressData->curValue = progressData->maxValue;
        else
        {
        }

        onValueChanged.emit(progressData->curValue);
        update();

        // std::cout << "progressData->curValue  " << progressData->curValue << std::endl;
    }
    return true;
}

bool TpSlideProgressBar::onPaintEvent(TpPaintEvent *event)
{
    TpWidget::onPaintEvent(event);

    TpSlideProgressBarData *progressData = static_cast<TpSlideProgressBarData *>(data_);

    tpShared<TpCssData> curCssData = enabledCss();

    TpPainter *painter = event->painter();

    // 绘制填充
    double valuePercent = 1.0 * (progressData->curValue - progressData->minValue) / (progressData->maxValue - progressData->minValue);
    int32_t actualWidth = width() - progressData->margin * 2;
    int32_t actualHeight = height() - progressData->margin * 2;

    uint32_t valueWidth = valuePercent * actualWidth;
    if (valueWidth > actualWidth)
        valueWidth = actualWidth;

    uint32_t roundCorners = this->roundCorners();

    painter->setPen(curCssData->color());
    painter->setBrush(TpBrush(curCssData->color()));

    if (valueWidth > 0)
    {
        if (valueWidth <= roundCorners)
        {
            // 只填充起始圆角部分

            // // 绘制上半圆;先绘制整个扇形，然后镂空
            TpPoint topPiePoint(progressData->margin + roundCorners, progressData->margin + roundCorners);

            // // 上半圆计算弧上交点Y坐标及交点扇形
            int32_t lineX = valueWidth;
            int32_t radius = roundCorners;

            CircleResult topCacuRes = calculateSectorIntersection(SECTOR_S1, topPiePoint, radius, lineX);

            // 构建三角形镂空
            TpHollowMask topHollowMask;
            TpHollowMask::PolygonHollow polygonHollow;
            polygonHollow.posintList.emplace_back(TpPoint(topCacuRes.intersection.x(), radius));
            polygonHollow.posintList.emplace_back(TpPoint(topCacuRes.intersection.x(), topCacuRes.intersection.y() - 2));
            polygonHollow.posintList.emplace_back(TpPoint(radius + 2, radius));
            topHollowMask.addPolygonHollow(polygonHollow);

            painter->drawPie(topPiePoint, radius, 180, 180 + (topCacuRes.angle * 180.0 / M_PI), topHollowMask);

            // 绘制左侧中间矩形；宽度绘制实际宽度
            painter->drawRect(progressData->margin, progressData->margin + roundCorners,
                              valueWidth, actualHeight - roundCorners * 2);

            // 绘制左侧下半圆
            TpPoint downPiePoint(progressData->margin + roundCorners, height() - roundCorners - progressData->margin);
            CircleResult downCacuRes = calculateSectorIntersection(SECTOR_S2, downPiePoint, radius, lineX);

            // 构建三角形镂空
            TpHollowMask downHollowMask;
            // TpHollowMask::PolygonHollow polygonHollow;
            polygonHollow.posintList.clear();
            polygonHollow.posintList.emplace_back(TpPoint(lineX, radius));
            polygonHollow.posintList.emplace_back(TpPoint(lineX, downCacuRes.intersection.y() + 2));
            polygonHollow.posintList.emplace_back(TpPoint(radius + 2, radius));
            downHollowMask.addPolygonHollow(polygonHollow);

            // painter->drawEllipse(downPiePoint, radius, radius, downHollowMask);
            painter->drawPie(downPiePoint, radius, 180 - (downCacuRes.angle * 180.0 / M_PI), 180, downHollowMask);
        }
        else if (valueWidth > (actualWidth - roundCorners))
        {
            // 在末尾圆角部分
            // 绘制左侧上半圆
            painter->drawPie(TpPoint(progressData->margin + roundCorners, progressData->margin + roundCorners),
                             roundCorners, 180, 270);

            // 绘制左侧中间矩形
            painter->drawRect(progressData->margin, progressData->margin + roundCorners,
                              roundCorners, actualHeight - roundCorners * 2);

            // 绘制左侧下半圆
            painter->drawPie(TpPoint(progressData->margin + roundCorners, height() - roundCorners - progressData->margin),
                             roundCorners, 90, 180);

            // 绘制右半区矩形
            int32_t middleRectWidth = actualWidth - roundCorners * 2;
            painter->drawRect(progressData->margin + roundCorners, progressData->margin,
                              middleRectWidth, actualHeight);

            // 绘制右边区域填充
            // 右侧弧形区域宽度
            int32_t rightAreaWidth = valueWidth - middleRectWidth - roundCorners;

            // // 上半圆计算弧上交点Y坐标及交点扇形
            int32_t lineX = valueWidth + progressData->margin;
            int32_t radius = roundCorners;

            // // 绘制上半圆;先绘制整个扇形，然后镂空切线连接点的扇形；最后补充绘制一个三角形
            TpPoint topPiePoint(width() - progressData->margin - radius, progressData->margin + radius);
            TpPoint downPiePoint(width() - progressData->margin - radius, height() - progressData->margin - radius);

            // 如果当前是最大值，无需计算，直接绘制填满即可
            if (tpFuzzyCompare(progressData->curValue, progressData->maxValue))
            {
                painter->drawPie(topPiePoint, radius, -90, 0);
                painter->drawPie(downPiePoint, radius, 0, 90);
                painter->drawRect(topPiePoint.x(), progressData->margin + radius, radius, actualHeight - radius * 2);
            }
            else
            {
                CircleResult topCacuRes = calculateSectorIntersection(SECTOR_S3, topPiePoint, radius, lineX);

                // std::cout << "topCacuRes.X/ Y " << topCacuRes.intersection.x() << " , " << topCacuRes.intersection.y() << std::endl;
                // std::cout << "topCacuRes.angle " << topCacuRes.angle * 180.0 / M_PI << std::endl;

                // 构建填充三角形
                TpVector<TpPoint> angleShape;
                angleShape.emplace_back(TpPoint(topPiePoint.x() - 1, topPiePoint.y()));
                angleShape.emplace_back(TpPoint(topCacuRes.intersection.x(), topCacuRes.intersection.y()));
                angleShape.emplace_back(TpPoint(topCacuRes.intersection.x(), topPiePoint.y()));

                // painter->setBrush(TpBrush(_RGB(255, 150, 150)));

                painter->drawPie(topPiePoint, radius, 270, 270 + (90 - topCacuRes.angle * 180.0 / M_PI) + 1);
                painter->drawPolygon(angleShape);

                // 绘制中间矩形
                painter->drawRect(topPiePoint.x(), progressData->margin + radius, rightAreaWidth, actualHeight - radius * 2);

                // 绘制右侧下半圆
                CircleResult downCacuRes = calculateSectorIntersection(SECTOR_S4, downPiePoint, radius, lineX);

                // std::cout << "downCacuRes.X/ Y " << downCacuRes.intersection.x() << " , " << downCacuRes.intersection.y() << std::endl;
                // std::cout << "downCacuRes.angle " << downCacuRes.angle * 180.0 / M_PI << std::endl;

                painter->drawPie(downPiePoint, radius, downCacuRes.angle * 180.0 / M_PI - 1, 90);

                TpVector<TpPoint> downAngleShape;
                downAngleShape.emplace_back(TpPoint(downPiePoint.x() - 1, downPiePoint.y()));
                downAngleShape.emplace_back(TpPoint(downCacuRes.intersection.x(), downPiePoint.y()));
                downAngleShape.emplace_back(TpPoint(downCacuRes.intersection.x(), downCacuRes.intersection.y()));
                painter->drawPolygon(downAngleShape);
            }
        }
        else
        {
            /*
                进度在中间
            */
            // 绘制左侧上半圆
            painter->drawPie(TpPoint(progressData->margin + roundCorners, progressData->margin + roundCorners),
                             roundCorners, 180, 270);

            // 绘制左侧中间矩形
            painter->drawRect(progressData->margin, progressData->margin + roundCorners,
                              roundCorners, actualHeight - roundCorners * 2);

            // 绘制左侧下半圆
            painter->drawPie(TpPoint(progressData->margin + roundCorners, height() - roundCorners - progressData->margin),
                             roundCorners, 90, 180);

            // 绘制右半区矩形
            int32_t rightRectWidth = valueWidth - roundCorners;
            painter->drawRect(progressData->margin + roundCorners, progressData->margin,
                              rightRectWidth, actualHeight);
        }
    }

    // 绘制图标
    if (!progressData->iconImage.isNull())
    {
        TpImage drawSurface = progressData->iconImage.scaled(curCssData->iconSize(), curCssData->iconSize());

        int32_t imageWidth = drawSurface.width();
        int32_t imageHeight = drawSurface.height();

        TpPainter *canvas = event->painter();

        int32_t cy = (rect().height() - imageHeight) / 2;

        painter->drawImage(cy, cy, drawSurface);
    }

    return true;
}

void TpSlideProgressBar::onThemeChangeEvent(TpThemeChangeEvent *event)
{
    update();
}
