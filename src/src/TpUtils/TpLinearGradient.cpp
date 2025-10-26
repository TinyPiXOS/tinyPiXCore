#include "TpLinearGradient.h"
#include "TpGradient_p.h"

TpLinearGradient::TpLinearGradient() : TpGradient()
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    gradientData->type = TpGradient::LinearGradient;
}

TpLinearGradient::TpLinearGradient(float x1, float y1, float x2, float y2) : TpGradient()
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    gradientData->type = TpGradient::LinearGradient;

    gradientData->lineStartPos.rx() = x1;
    gradientData->lineStartPos.ry() = y1;
    gradientData->lineStopPos.rx() = x2;
    gradientData->lineStopPos.ry() = y2;
}

TpLinearGradient::TpLinearGradient(const TpPointF &start, const TpPointF &finalStop) : TpGradient()
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    gradientData->type = TpGradient::LinearGradient;

    gradientData->lineStartPos = start;
    gradientData->lineStopPos = finalStop;
}

TpLinearGradient::~TpLinearGradient()
{
}

void TpLinearGradient::setAngle(float angle)
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return;
    gradientData->angle = angle;
    gradientData->hasAngle = true;
}

float TpLinearGradient::angle()
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return 0.0f;
    return gradientData->angle;
}

bool TpLinearGradient::hasAngle()
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return false;
    return gradientData->hasAngle;
}

void TpLinearGradient::setStart(const TpPointF &start)
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return;

    gradientData->lineStartPos = start;
}

void TpLinearGradient::setStart(float x, float y)
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return;

    setStart(TpPointF(x, y));
}

TpPointF TpLinearGradient::start() const
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return TpPointF();

    return gradientData->lineStartPos;
}

void TpLinearGradient::setFinalStop(const TpPointF &stop)
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return;

    gradientData->lineStopPos = stop;
}

void TpLinearGradient::setFinalStop(float x, float y)
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return;

    setFinalStop(TpPointF(x, y));
}

TpPointF TpLinearGradient::finalStop() const
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return TpPointF();

    return gradientData->lineStopPos;
}
