#include "TpLinearGradient.h"
#include "TpGradient_p.h"

TpLinearGradient::TpLinearGradient() : TpGradient()
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    gradientData->type = TpGradient::LinearGradient;
}

TpLinearGradient::TpLinearGradient(float x1, float y1, float x2, float y2)
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    gradientData->type = TpGradient::LinearGradient;

    gradientData->lineStartPos.x = x1;
    gradientData->lineStartPos.y = y1;
    gradientData->lineStopPos.x = x2;
    gradientData->lineStopPos.y = y2;
}

TpLinearGradient::TpLinearGradient(const ItpPointF &start, const ItpPointF &finalStop)
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return;

    gradientData->lineStartPos = start;
    gradientData->lineStopPos = finalStop;
}

TpLinearGradient::~TpLinearGradient()
{
}

void TpLinearGradient::setStart(const ItpPointF &start)
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

    gradientData->lineStartPos.x = x;
    gradientData->lineStartPos.y = y;
}

ItpPointF TpLinearGradient::start() const
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return ItpPointF();

    return gradientData->lineStartPos;
}

void TpLinearGradient::setFinalStop(const ItpPointF &stop)
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

    gradientData->lineStopPos.x = x;
    gradientData->lineStopPos.y = y;
}

ItpPointF TpLinearGradient::finalStop() const
{
    TpGradientData *gradientData = static_cast<TpGradientData *>(data_);
    if (!gradientData)
        return ItpPointF();

    return gradientData->lineStopPos;
}
