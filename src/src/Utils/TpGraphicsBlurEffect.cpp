#include "TpGraphicsBlurEffect.h"

struct TpGraphicsBlurEffectData
{
    float blurRadius = 5;

    TpGraphicsBlurEffect::BlurDirection direction = TpGraphicsBlurEffect::BothDirection;
    TpGraphicsBlurEffect::BlurBorderType borderType = TpGraphicsBlurEffect::CopyBorder;
    int32_t quality = 50;
};

TpGraphicsBlurEffect::TpGraphicsBlurEffect() : TpGraphicsEffect()
{
    TpGraphicsBlurEffectData *blurData = new TpGraphicsBlurEffectData();
    data_ = blurData;
}

TpGraphicsBlurEffect::TpGraphicsBlurEffect(const TpGraphicsBlurEffect &other)
    : TpGraphicsEffect(other)
{
    TpGraphicsBlurEffectData *blurData = new TpGraphicsBlurEffectData();
    TpGraphicsBlurEffectData *othersBlurData = static_cast<TpGraphicsBlurEffectData *>(other.data_);
    *blurData = *blurData;
    data_ = blurData;
}

TpGraphicsBlurEffect::~TpGraphicsBlurEffect()
{
    TpGraphicsBlurEffectData *blurData = static_cast<TpGraphicsBlurEffectData *>(data_);
    if (blurData)
    {
        delete blurData;
        blurData = nullptr;
        data_ = nullptr;
    }
}

void TpGraphicsBlurEffect::setBlurRadius(float blurRadius)
{
    TpGraphicsBlurEffectData *blurData = static_cast<TpGraphicsBlurEffectData *>(data_);
    blurData->blurRadius = blurRadius;
}

float TpGraphicsBlurEffect::blurRadius()
{
    TpGraphicsBlurEffectData *blurData = static_cast<TpGraphicsBlurEffectData *>(data_);
    return blurData->blurRadius;
}

void TpGraphicsBlurEffect::setDirection(TpGraphicsBlurEffect::BlurDirection direction)
{
    TpGraphicsBlurEffectData *blurData = static_cast<TpGraphicsBlurEffectData *>(data_);
    blurData->direction = direction;
}

TpGraphicsBlurEffect::BlurDirection TpGraphicsBlurEffect::direction()
{
    TpGraphicsBlurEffectData *blurData = static_cast<TpGraphicsBlurEffectData *>(data_);
    return blurData->direction;
}

void TpGraphicsBlurEffect::setBorder(TpGraphicsBlurEffect::BlurBorderType borderType)
{
    TpGraphicsBlurEffectData *blurData = static_cast<TpGraphicsBlurEffectData *>(data_);
    blurData->borderType = borderType;
}

TpGraphicsBlurEffect::BlurBorderType TpGraphicsBlurEffect::border()
{
    TpGraphicsBlurEffectData *blurData = static_cast<TpGraphicsBlurEffectData *>(data_);
    return blurData->borderType;
}

void TpGraphicsBlurEffect::setQuality(int32_t quality)
{
    TpGraphicsBlurEffectData *blurData = static_cast<TpGraphicsBlurEffectData *>(data_);
    blurData->quality = quality;
}

int32_t TpGraphicsBlurEffect::quality()
{
    TpGraphicsBlurEffectData *blurData = static_cast<TpGraphicsBlurEffectData *>(data_);
    return blurData->quality;
}

const TpGraphicsBlurEffect &TpGraphicsBlurEffect::operator=(const TpGraphicsBlurEffect &others)
{
    TpGraphicsEffect::operator=(others);

    TpGraphicsBlurEffectData *blurData = static_cast<TpGraphicsBlurEffectData *>(data_);
    TpGraphicsBlurEffectData *othersBlurData = static_cast<TpGraphicsBlurEffectData *>(others.data_);

    *blurData = *othersBlurData;

    return *this;
}