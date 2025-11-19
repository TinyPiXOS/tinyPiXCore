#include "TpRadialGradient.h"
#include "TpGradient_p.h"

TpRadialGradient::TpRadialGradient() : TpGradient()
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    gradientData->type = TpGradient::RadialGradient;
}

TpRadialGradient::TpRadialGradient(float cx, float cy, float centerRadius, float fx, float fy, float focalRadius) : TpGradient()
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    gradientData->type = TpGradient::RadialGradient;

    gradientData->center = TpPointF(cx, cy);
    gradientData->centerRadius = centerRadius;

    gradientData->focalPoint = TpPointF(fx, fy);
    gradientData->focalRadius = focalRadius;
}

TpRadialGradient::TpRadialGradient(const TpPointF &center, float centerRadius, const TpPointF &focalPoint, float focalRadius) : TpGradient()
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    gradientData->type = TpGradient::RadialGradient;

    gradientData->center = center;
    gradientData->centerRadius = centerRadius;

    gradientData->focalPoint = focalPoint;
    gradientData->focalRadius = focalRadius;
}

TpRadialGradient::TpRadialGradient(float cx, float cy, float radius) : TpGradient()
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    gradientData->type = TpGradient::RadialGradient;

    gradientData->center = TpPointF(cx, cy);
    gradientData->centerRadius = radius;

    gradientData->focalPoint = TpPointF(cx, cy);
    gradientData->focalRadius = 0;
}

TpRadialGradient::TpRadialGradient(const TpPointF &center, float radius) : TpGradient()
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    gradientData->type = TpGradient::RadialGradient;

    gradientData->center = center;
    gradientData->centerRadius = radius;

    gradientData->focalPoint = center;
    gradientData->focalRadius = 0;
}

TpRadialGradient::TpRadialGradient(float cx, float cy, float radius, float fx, float fy) : TpGradient()
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    gradientData->type = TpGradient::RadialGradient;

    gradientData->center = TpPointF(cx, cy);
    gradientData->centerRadius = radius;

    gradientData->focalPoint = TpPointF(fx, fy);
    gradientData->focalRadius = 0;
}

TpRadialGradient::TpRadialGradient(const TpPointF &center, float radius, const TpPointF &focalPoint) : TpGradient()
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    gradientData->type = TpGradient::RadialGradient;

    gradientData->center = center;
    gradientData->centerRadius = radius;

    gradientData->focalPoint = focalPoint;
    gradientData->focalRadius = 0;
}

TpRadialGradient::~TpRadialGradient()
{
}

TpPointF TpRadialGradient::center() const
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return TpPointF();
    return gradientData->center;
}

void TpRadialGradient::setCenter(const TpPointF &center)
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return;
    gradientData->center = center;
}

void TpRadialGradient::setCenter(float x, float y)
{
    setCenter(TpPointF(x, y));
}

float TpRadialGradient::centerRadius() const
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return 0.0;
    return gradientData->centerRadius;
}

void TpRadialGradient::setCenterRadius(float radius)
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return;
    gradientData->centerRadius = radius;
}

TpPointF TpRadialGradient::focalPoint() const
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return TpPointF();
    return gradientData->focalPoint;
}

void TpRadialGradient::setFocalPoint(const TpPointF &focalPoint)
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return;
    gradientData->focalPoint = focalPoint;
}

void TpRadialGradient::setFocalPoint(float x, float y)
{
    setFocalPoint(TpPointF(x, y));
}

float TpRadialGradient::focalRadius() const
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return 0.0;
    return gradientData->focalRadius;
}

void TpRadialGradient::setFocalRadius(float radius)
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return;
    gradientData->focalRadius = radius;
}
