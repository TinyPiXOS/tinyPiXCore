#include "TpGradient.h"
#include "TpGradient_p.h"

TpGradient::TpGradient()
{
    TpGradientData *gradientData = new TpGradientData();
    data_ = gradientData;
}

TpGradient::~TpGradient()
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (gradientData)
    {
        delete gradientData;
        gradientData = nullptr;
        data_ = nullptr;
    }
}

void TpGradient::setColorAt(float position, int32_t color)
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return;

    if (position < 0)
        position = 0;
    if (position > 1)
        position = 1;

    int32_t colorSize = gradientData->colorInfo.size();

    if (colorSize == 0)
    {
        gradientData->colorInfo.emplace_back(ColorPosInfo(position, colorSize));
    }
    else
    {
        for (int i = 0; i < colorSize; ++i)
        {
            ColorPosInfo curColorInfo = gradientData->colorInfo.at(i);

            if (tpFuzzyCompare(position, curColorInfo.pos))
            {
                // 给入偏移量和当前相等，替换颜色数据
                gradientData->colorInfo[i] = ColorPosInfo(position, colorSize);
            }
            else if (position < curColorInfo.pos)
            {
                // 给入偏移量比当前偏移量小，向前插入
                gradientData->colorInfo.insertData(i, ColorPosInfo(position, colorSize));
                break;
            }
            else
            {
                // 给入偏移量比当前偏移量大，找下一次循环
                // 如果当前已经是最后一次循环了，插入尾部
                if (i == (colorSize - 1))
                {
                    gradientData->colorInfo.emplace_back(ColorPosInfo(position, colorSize));
                    break;
                }
                else
                {
                    continue;
                }
            }
        }
    }
}

void TpGradient::setColorAt(float position, const TpColors &color)
{
    setColorAt(position, color.rgba());
}
