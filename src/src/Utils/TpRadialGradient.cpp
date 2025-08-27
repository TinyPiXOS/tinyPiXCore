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

    gradientData->center.x = cx;
    gradientData->center.y = cy;
    gradientData->centerRadius = centerRadius;

    gradientData->focalPoint.x = fx;
    gradientData->focalPoint.y = fy;
    gradientData->focalRadius = focalRadius;
}

TpRadialGradient::TpRadialGradient(const ItpPointF &center, float centerRadius, const ItpPointF &focalPoint, float focalRadius) : TpGradient()
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

    gradientData->center.x = cx;
    gradientData->center.y = cy;
    gradientData->centerRadius = radius;

    gradientData->focalPoint.x = cx;
    gradientData->focalPoint.y = cy;
    gradientData->focalRadius = 0;
}

TpRadialGradient::TpRadialGradient(const ItpPointF &center, float radius) : TpGradient()
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

    gradientData->center.x = cx;
    gradientData->center.y = cy;
    gradientData->centerRadius = radius;

    gradientData->focalPoint.x = fx;
    gradientData->focalPoint.y = fy;
    gradientData->focalRadius = 0;
}

TpRadialGradient::TpRadialGradient(const ItpPointF &center, float radius, const ItpPointF &focalPoint) : TpGradient()
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

ItpPointF TpRadialGradient::center() const
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return ItpPointF();
    return gradientData->center;
}

void TpRadialGradient::setCenter(const ItpPointF &center)
{
    setCenter(center.x, center.y);
}

void TpRadialGradient::setCenter(float x, float y)
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return;
    gradientData->center.x = x;
    gradientData->center.y = y;
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

ItpPointF TpRadialGradient::focalPoint() const
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return ItpPointF();
    return gradientData->focalPoint;
}

void TpRadialGradient::setFocalPoint(const ItpPointF &focalPoint)
{
    setFocalPoint(focalPoint.x, focalPoint.y);
}

void TpRadialGradient::setFocalPoint(float x, float y)
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return;
    gradientData->focalPoint.x = x;
    gradientData->focalPoint.y = y;
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
