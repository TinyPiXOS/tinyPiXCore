#include "tpAnimation.h"
#include "tpVariant.h"
#include "tpTimer.h"
#include "tpMap.h"

static uint32_t globalTimerInterval = 16;

struct tpAnimationData
{
    tpAnimation::AnimationType animationType;
    tpAnimation::CustomAnimationFunc customFunc;

    uint32_t durationMs = 0;

    tpVariant startValue;
    tpVariant endValue;
    tpVariant curValue;

    // 当前执行动画次数
    int32_t curLoopIndex = 0;
    // 动画循环次数
    int32_t loopCount = 1;

    // 每个时间间隔的差值
    tpVariant subValue;

    tpTimer animationTimer;
    uint32_t curTimeMs = 0;

    tpChildWidget *targetWidget = nullptr;

    tpAnimation::DeletionPolicy deleteMode = tpAnimation::DeleteWhenStopped;

    // 关键帧<百分比，对应值>
    tpVector<std::pair<float, tpVariant>> keyFrameValueList;

    // 当前要到的关键帧
    int32_t curTargetKeyIndex = 0;

    tpAnimationData()
    {
    }
};

tpAnimation::tpAnimation(tpChildWidget *target, const AnimationType &propertyType)
    : tpObject(nullptr)
{
    tpAnimationData *animationData = new tpAnimationData();
    data_ = animationData;

    animationData->targetWidget = target;
    animationData->animationType = propertyType;

    animationData->animationTimer.setInterval(globalTimerInterval);

    connect(&animationData->animationTimer, timeout, this, tpAnimation::AnimationRun);
}

tpAnimation::~tpAnimation()
{
    tpAnimationData *animationData = static_cast<tpAnimationData *>(data_);
    if (animationData)
    {
        delete animationData;
        animationData = nullptr;
        data_ = nullptr;
    }
}

void tpAnimation::setTargetWidget(tpChildWidget *target)
{
    tpAnimationData *animationData = static_cast<tpAnimationData *>(data_);
    animationData->targetWidget = target;
}

tpChildWidget *tpAnimation::targetWidget()
{
    tpAnimationData *animationData = static_cast<tpAnimationData *>(data_);
    return animationData->targetWidget;
}

void tpAnimation::setLoopCount(const int32_t count)
{
    tpAnimationData *animationData = static_cast<tpAnimationData *>(data_);
    animationData->loopCount = count;
}

int32_t tpAnimation::loopCount()
{
    tpAnimationData *animationData = static_cast<tpAnimationData *>(data_);
    return animationData->loopCount;
}

void tpAnimation::setCustomAnimationFunc(CustomAnimationFunc func)
{
    tpAnimationData *animationData = static_cast<tpAnimationData *>(data_);
    animationData->customFunc = func;
    animationData->targetWidget = nullptr;
}

void tpAnimation::setDuration(const uint32_t &timeMs)
{
    tpAnimationData *animationData = static_cast<tpAnimationData *>(data_);
    animationData->durationMs = timeMs;
}

uint32_t tpAnimation::duration()
{
    tpAnimationData *animationData = static_cast<tpAnimationData *>(data_);
    return animationData->durationMs;
}

void tpAnimation::setStartValue(const tpVariant &value)
{
    tpAnimationData *animationData = static_cast<tpAnimationData *>(data_);
    animationData->startValue = value;
}

tpVariant tpAnimation::startValue()
{
    tpAnimationData *animationData = static_cast<tpAnimationData *>(data_);
    return animationData->startValue;
}

void tpAnimation::setEndValue(const tpVariant &value)
{
    tpAnimationData *animationData = static_cast<tpAnimationData *>(data_);
    animationData->endValue = value;
}

tpVariant tpAnimation::endValue()
{
    tpAnimationData *animationData = static_cast<tpAnimationData *>(data_);
    return animationData->endValue;
}

void tpAnimation::start(const DeletionPolicy &runMode)
{
    tpAnimationData *animationData = static_cast<tpAnimationData *>(data_);

    animationData->curTimeMs = 0;
    animationData->curLoopIndex = 0;
    animationData->curTargetKeyIndex = 0;
    animationData->animationTimer.start();
}

void tpAnimation::pause()
{
    tpAnimationData *animationData = static_cast<tpAnimationData *>(data_);
    animationData->animationTimer.stop();
}

void tpAnimation::stop()
{
    tpAnimationData *animationData = static_cast<tpAnimationData *>(data_);
    animationData->animationTimer.stop();

    finished.emit(); // 触发完成信号

    if (animationData->deleteMode == tpAnimation::DeleteWhenStopped)
    {
        deleteLater();
    }
}

void tpAnimation::setKeyValueAt(const float &percent, const tpVariant &value)
{
    tpAnimationData *animationData = static_cast<tpAnimationData *>(data_);

    float inputPercent = percent;
    if (inputPercent > 1)
        inputPercent = 1;
    if (inputPercent < 0)
        inputPercent = 0;

    if (inputPercent == 0)
    {
        setStartValue(value);
    }
    else if (inputPercent == 1)
    {
        setEndValue(value);
    }
    else
    {
        animationData->keyFrameValueList.emplace_back(std::make_pair(inputPercent, value));
    }
}

// 新增插值模板函数（头文件内声明）
template <typename T>
T lerp(const T &start, const T &end, float progress)
{
    T finalValue = start + (end - start) * progress;
    if (start > end)
    {
        if (finalValue < end)
            finalValue = end;
    }
    else
    {
        if (finalValue > end)
            finalValue = end;
    }

    return finalValue;
}

// 特化Uint类型
template <>
uint32_t lerp<uint32_t>(const uint32_t &start, const uint32_t &end, float progress)
{
    int64_t finalValue = 0;
    if (start > end)
    {
        finalValue = start - (start - end) * progress;
        if (finalValue < end)
            finalValue = end;
    }
    else
    {
        finalValue = start + (end - start) * progress;
        if (finalValue > end)
            finalValue = end;
    }

    return finalValue;
};

int32_t lerpColor(int32_t start, int32_t end, float progress)
{
    return _RGB(
        _R(start) + (int32_t)((_R(end) - _R(start)) * progress),
        _G(start) + (int32_t)((_G(end) - _G(start)) * progress),
        _B(start) + (int32_t)((_B(end) - _B(start)) * progress));
}

void tpAnimation::AnimationRun()
{
    tpAnimationData *animationData = static_cast<tpAnimationData *>(data_);

    // 计算动画总进度（0.0~1.0）
    const float progress = static_cast<float>(animationData->curTimeMs) / animationData->durationMs;

    // 关键帧进度  keyProgress = （curTime - lastKey） / (curKey - lastKey)
    // 注意关键帧时间单位为 s秒，当前时间单位为 ms毫秒
    float keyProgress = progress;

    tpVariant startValue = animationData->startValue;
    tpVariant endValue = animationData->endValue;

    if (animationData->keyFrameValueList.size() > 0)
    {
        if (animationData->curTargetKeyIndex == 0)
        {
            // 第一个关键帧
            auto &keyValueIter = animationData->keyFrameValueList.at(animationData->curTargetKeyIndex);
            endValue = keyValueIter.second;
            keyProgress = 1.0 * animationData->curTimeMs / (keyValueIter.first * animationData->durationMs);
        }
        else if (animationData->curTargetKeyIndex == animationData->keyFrameValueList.size())
        {
            // 动画关键帧已结束，向最终值计算
            // 上一个关键帧的数据作为开始数据
            int32_t lastKeyIndex = animationData->curTargetKeyIndex - 1;
            if (lastKeyIndex >= 0)
            {
                auto &keyValueIter = animationData->keyFrameValueList.at(lastKeyIndex);

                startValue = keyValueIter.second;
                // 最终值的关键帧为1
                keyProgress = 1.0 * (animationData->curTimeMs - (keyValueIter.first * animationData->durationMs)) / (animationData->durationMs - (keyValueIter.first * animationData->durationMs));
            }
        }
        else
        {
            // 上一个关键帧的数据作为开始数据
            float lastKey = 0;
            int32_t lastKeyIndex = animationData->curTargetKeyIndex - 1;
            if (lastKeyIndex >= 0)
            {
                startValue = animationData->keyFrameValueList.at(lastKeyIndex).second;
                lastKey = animationData->keyFrameValueList.at(lastKeyIndex).first;
            }

            // 当前关键帧为目标数据
            endValue = animationData->keyFrameValueList.at(animationData->curTargetKeyIndex).second;
            float curKey = animationData->keyFrameValueList.at(animationData->curTargetKeyIndex).first;

            keyProgress = 1.0 * (animationData->curTimeMs - (lastKey * animationData->durationMs)) / (curKey * animationData->durationMs - lastKey * animationData->durationMs);
        }

        // 切换到下一个关键帧
        if (keyProgress >= 1.0f)
        {
            animationData->curTargetKeyIndex += 1;
            if (animationData->curTargetKeyIndex >= animationData->keyFrameValueList.size())
                animationData->curTargetKeyIndex = animationData->keyFrameValueList.size();
        }
    }

    // 处理不同动画类型
    switch (animationData->animationType)
    {
    case Geometry:
    {
        if (startValue.isRect() && endValue.isRect())
        {
            const ItpRect start = startValue.toRect();
            const ItpRect end = endValue.toRect();
            const ItpRect current = ItpRect(
                lerp(start.x, end.x, keyProgress),
                lerp(start.y, end.y, keyProgress),
                lerp(start.w, end.w, keyProgress),
                lerp(start.h, end.h, keyProgress));
            animationData->targetWidget->setRect(current);
        }
        break;
    }
    case Pos:
    {
        if (startValue.isPoint() && endValue.isPoint())
        {
            const ItpPoint start = startValue.toPoint();
            const ItpPoint end = endValue.toPoint();
            animationData->targetWidget->move(
                lerp(start.x, end.x, keyProgress),
                lerp(start.y, end.y, keyProgress));
        }
        break;
    }
    case Size:
        if (startValue.isSize() && endValue.isSize())
        {
            const ItpSize start = startValue.toSize();
            const ItpSize end = endValue.toSize();
            animationData->targetWidget->setSize(
                lerp(start.w, end.w, keyProgress),
                lerp(start.h, end.h, keyProgress));
        }
        break;
    case Height:
        if (startValue.isInt32() && endValue.isInt32())
        {
            const int32_t start = startValue.toInt32();
            const int32_t end = endValue.toInt32();
            animationData->targetWidget->setHeight(lerp(start, end, keyProgress));
        }
        break;
    case Width:
        if (startValue.isInt32() && endValue.isInt32())
        {
            const int32_t start = startValue.toInt32();
            const int32_t end = endValue.toInt32();
            animationData->targetWidget->setWidth(lerp(start, end, keyProgress));
        }
        break;
    case WindowOpacity:
    {
        if (startValue.isInt32() && endValue.isInt32())
        {
            const double opacity = lerp(
                startValue.toInt32(),
                endValue.toInt32(),
                keyProgress);
            animationData->targetWidget->setAlpha(opacity);
        }
        break;
    }
    case BackgroundColor:
    {
        if (startValue.isInt32() && endValue.isInt32())
        {
            const int32_t start = startValue.toInt32();
            const int32_t end = endValue.toInt32();
            animationData->targetWidget->setBackGroundColor(lerpColor(start, end, keyProgress));
        }
        break;
    }
    case CustomAnimation:
    {
        // if (animationData->customFunc)
        // {
        //     const tpVariant value = lerp(
        //         startValue,
        //         endValue,
        //         progress);
        //     animationData->customFunc(value);
        // }
        break;
    }
    default:
        break;
    }

    if (animationData->targetWidget)
    {
        animationData->targetWidget->update();

        // 同步更新目标窗口的父窗口；避免在父窗口产生阴影残留
        tpChildWidget *targetParent = dynamic_cast<tpChildWidget *>(animationData->targetWidget->parent());
        if (targetParent)
        {
            targetParent->update();
        }
    }

    animationData->curTimeMs += globalTimerInterval;

    // 动画完成判断
    if (progress >= 1.0f)
    {
        animationData->curLoopIndex += 1;

        // 是否已完成动画循环次数
        if ((animationData->loopCount != -1) && (animationData->curLoopIndex >= animationData->loopCount))
        {
            animationData->animationTimer.stop();
            finished.emit(); // 触发完成信号

            if (animationData->deleteMode == tpAnimation::DeleteWhenStopped)
            {
                deleteLater();
            }
        }
        else
        {
            // 继续循环动画
            animationData->curTimeMs = 0;
            animationData->curTargetKeyIndex = 0;
        }
    }
}
