#include "TpGraphicsBlurEffect.h"

struct TpGraphicsBlurEffectData
{
    float blurRadius = 0;
};

TpGraphicsBlurEffect::TpGraphicsBlurEffect()
{
    TpGraphicsBlurEffectData *blurData = new TpGraphicsBlurEffectData();
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

const TpGraphicsBlurEffect &TpGraphicsBlurEffect::operator=(const TpGraphicsBlurEffect &others)
{
    TpGraphicsBlurEffectData *blurData = static_cast<TpGraphicsBlurEffectData *>(data_);
    TpGraphicsBlurEffectData *othersBlurData = static_cast<TpGraphicsBlurEffectData *>(others.data_);

    *blurData = *othersBlurData;

    return *this;
}