#include "TpCssData.h"
#include "TpUtils.h"
#include "TpLinearGradient.h"
#include "TpRadialGradient.h"

// 颜色属性封装
class ColorProperty
{
public:
    void parse(const TpString &value)
    {
        if (value.contains(","))
        {
            TpList<TpString> parts = value.split(',');
            if (parts.size() < 3)
                return;

            if (parts.front().compare("linear-gradient") == 0)
            {
                tpShared<TpLinearGradient> lineGradient = tpMakeShared<TpLinearGradient>();
                // 解析角度
                lineGradient->setAngle(parts.at(1).toDouble());

                gradient_ = lineGradient;

                // 解析颜色节点
                for (int i = 2; i < parts.size(); i++)
                {
                    TpList<TpString> point = parts[i].split('|');
                    if (point.size() >= 2)
                    {
                        gradient_->setColorAt(point.at(1).toDouble(),
                                              point.at(0).toInt());
                    }
                }

                isGradient_ = true;

                TpList<std::pair<float, int32_t>> colorAtList = gradient_->getColors();
                if (colorAtList.size() > 0)
                    solidColor_ = colorAtList.front().second;
            }
            else if (parts.front().compare("radial-gradient") == 0)
            {
                solidColor_ = _RGB(0, 0, 0);
                isGradient_ = false;
                gradient_.reset();
            }
            else
            {
                solidColor_ = value.toInt();
                isGradient_ = false;
                gradient_.reset();
            }
        }
        else
        {
            solidColor_ = value.toInt();
            isGradient_ = false;
            gradient_.reset();
        }
        parsed_ = true;
    }

    int32_t solidColor() const { return solidColor_; }
    bool isGradient() const { return isGradient_; }
    TpGradient *gradient() const { return gradient_.get(); }
    bool parsed() const { return parsed_; }
    void reset()
    {
        parsed_ = false;
        gradient_.reset();
    }

private:
    int32_t solidColor_ = 0;
    bool isGradient_ = false;
    tpShared<TpGradient> gradient_;
    bool parsed_ = false;
};

struct TpCssDataData
{
    TpHash<TpString, TpString> cssDataMap;

    // 颜色属性实例
    ColorProperty color_;
    ColorProperty subColor_;
    ColorProperty backgroundColor_;
    ColorProperty borderColor_;
    ColorProperty iconBackground_;

    // 初始化颜色属性
    void initColorProperty(ColorProperty &prop, const TpString &key)
    {
        if (cssDataMap.contains(key))
        {
            prop.parse(cssDataMap.value(key));
        }
    }
};

TpCssData::TpCssData(const TpHash<TpString, TpString> &cssDataMap)
{
    TpCssDataData *cssData = new TpCssDataData();
    cssData->cssDataMap = cssDataMap;

    cssData->initColorProperty(cssData->color_, "color");
    cssData->initColorProperty(cssData->subColor_, "sub-color");
    cssData->initColorProperty(cssData->borderColor_, "border-color");
    cssData->initColorProperty(cssData->iconBackground_, "icon-background");

    TpString bgCssKeyStr = "background-color";
    if (cssData->cssDataMap.contains("background"))
        bgCssKeyStr = "background";

    cssData->initColorProperty(cssData->backgroundColor_, bgCssKeyStr);

    data_ = cssData;
}

TpCssData::~TpCssData()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    if (cssData)
    {
        delete cssData;
        cssData = nullptr;
        data_ = nullptr;
    }
}
// 修改所有函数，使用data_访问TpCssDataData成员
int32_t TpCssData::width()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    if (cssData->cssDataMap.contains("width"))
        return cssData->cssDataMap.value("width").toInt();
    return 10;
}

int32_t TpCssData::minimumWidth()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    if (cssData->cssDataMap.contains("min-width"))
        return cssData->cssDataMap.value("min-width").toInt();
    return 0;
}

int32_t TpCssData::maximumWidth()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    if (cssData->cssDataMap.contains("max-width"))
        return cssData->cssDataMap.value("max-width").toInt();
    return WIDGET_MAX_WIDTH;
}

int32_t TpCssData::height()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    if (cssData->cssDataMap.contains("height"))
        return cssData->cssDataMap.value("height").toInt();
    return 10;
}

int32_t TpCssData::minimumHeight()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    if (cssData->cssDataMap.contains("min-height"))
        return cssData->cssDataMap.value("min-height").toInt();
    return 0;
}

int32_t TpCssData::maximumHeight()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    if (cssData->cssDataMap.contains("max-height"))
        return cssData->cssDataMap.value("max-height").toInt();
    return WIDGET_MAX_HEIGHT;
}

int32_t TpCssData::color()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    return cssData->color_.solidColor();
}

bool TpCssData::colorIsGradient()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    return cssData->color_.isGradient();
}

TpGradient *TpCssData::colorGradiant()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    return cssData->color_.gradient();
}

int32_t TpCssData::subColor()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    return cssData->subColor_.solidColor();
}

bool TpCssData::subColorIsGradient()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    return cssData->subColor_.isGradient();
}

TpGradient *TpCssData::subColorGradiant()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    return cssData->subColor_.gradient();
}

int32_t TpCssData::backgroundColor()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    return cssData->backgroundColor_.solidColor();
}

void TpCssData::setBackgroundColor(const int32_t &color)
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    cssData->backgroundColor_.parse(TpString::number(color));
}

bool TpCssData::backgroundColorIsGradient()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    return cssData->backgroundColor_.isGradient();
}

TpGradient *TpCssData::backgroundColorGradiant()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    return cssData->backgroundColor_.gradient();
}

void TpCssData::setBackgroundColor(TpGradient *color)
{
}

int32_t TpCssData::borderColor()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    return cssData->borderColor_.solidColor();
}

void TpCssData::setBorderColor(const int32_t &color)
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    cssData->cssDataMap["border-color"] = TpString::number(color);
    cssData->borderColor_.parse(cssData->cssDataMap["border-color"]);
}

bool TpCssData::borderColorIsGradient()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    return cssData->borderColor_.isGradient();
}

TpGradient *TpCssData::borderColorGradiant()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    return cssData->borderColor_.gradient();
}

int32_t TpCssData::borderWidth()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    if (cssData->cssDataMap.contains("border-width"))
        return cssData->cssDataMap.value("border-width").toInt();
    return 0;
}

int32_t TpCssData::fontSize()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    if (cssData->cssDataMap.contains("font-size"))
        return cssData->cssDataMap.value("font-size").toInt();
    return 10;
}

int32_t TpCssData::gap()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    if (cssData->cssDataMap.contains("gap"))
        return cssData->cssDataMap.value("gap").toInt();
    return 5;
}

int32_t TpCssData::padding()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    if (cssData->cssDataMap.contains("padding"))
        return cssData->cssDataMap.value("padding").toInt();
    return 0;
}

int32_t TpCssData::paddingLeft()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    if (cssData->cssDataMap.contains("padding"))
        return cssData->cssDataMap.value("padding").toInt();

    if (cssData->cssDataMap.contains("padding-left"))
        return cssData->cssDataMap.value("padding-left").toInt();
    return 0;
}

int32_t TpCssData::paddingRight()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    if (cssData->cssDataMap.contains("padding"))
        return cssData->cssDataMap.value("padding").toInt();

    if (cssData->cssDataMap.contains("padding-right"))
        return cssData->cssDataMap.value("padding-right").toInt();
    return 0;
}

int32_t TpCssData::paddingTop()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    if (cssData->cssDataMap.contains("padding"))
        return cssData->cssDataMap.value("padding").toInt();

    if (cssData->cssDataMap.contains("padding-top"))
        return cssData->cssDataMap.value("padding-top").toInt();
    return 0;
}

int32_t TpCssData::paddingBottom()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    if (cssData->cssDataMap.contains("padding"))
        return cssData->cssDataMap.value("padding").toInt();

    if (cssData->cssDataMap.contains("padding-bottom"))
        return cssData->cssDataMap.value("padding-bottom").toInt();
    return 0;
}

uint32_t TpCssData::roundCorners()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    if (cssData->cssDataMap.contains("border-radius"))
        return cssData->cssDataMap.value("border-radius").toInt();
    return 0;
}

void TpCssData::setRoundCorners(const uint32_t &corners)
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    cssData->cssDataMap["border-radius"] = TpString::number(corners);
}

int32_t TpCssData::iconSize()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    if (cssData->cssDataMap.contains("icon-size"))
        return cssData->cssDataMap.value("icon-size").toInt();
    return 10;
}

int32_t TpCssData::iconBackground()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    return cssData->iconBackground_.solidColor();
}

bool TpCssData::iconBackgroundIsGradient()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    return cssData->iconBackground_.isGradient();
}

TpGradient *TpCssData::iconBackgroundGradiant()
{
    TpCssDataData *cssData = static_cast<TpCssDataData *>(data_);
    return cssData->iconBackground_.gradient();
}