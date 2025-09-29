#include "TpAnimation.h"
#include "TpVariant.h"
#include "TpTimer.h"
#include "TpMap.h"

static uint32_t globalTimerInterval = 16;

struct TpAnimationData
{
    TpAnimation::AnimationType animationType;
    TpAnimation::CustomAnimationFunc customFunc;

    uint32_t durationMs = 0;

    TpVariant startValue;
    TpVariant endValue;
    TpVariant curValue;

    // 当前执行动画次数
    int32_t curLoopIndex = 0;
    // 动画循环次数
    int32_t loopCount = 1;

    // 每个时间间隔的差值
    TpVariant subValue;

    TpTimer animationTimer;
    uint32_t curTimeMs = 0;

    TpChildWidget *targetWidget = nullptr;

    TpAnimation::DeletionPolicy deleteMode = TpAnimation::DeleteWhenStopped;

    // 关键帧<百分比，对应值>
    TpVector<std::pair<float, TpVariant>> keyFrameValueList;

    // 当前要到的关键帧
    int32_t curTargetKeyIndex = 0;

    // 动画是否停止标志
    std::atomic<bool> stopped;
    std::atomic<bool> isDelete;

    TpAnimationData()
    {
    }
};

TpAnimation::TpAnimation(TpChildWidget *target, const AnimationType &propertyType)
    : TpObject(nullptr)
{
    TpAnimationData *animationData = new TpAnimationData();
    data_ = animationData;

    animationData->targetWidget = target;
    animationData->animationType = propertyType;
    animationData->stopped.store(true);
    animationData->isDelete.store(false);

    animationData->animationTimer.setInterval(globalTimerInterval);

    connect(&animationData->animationTimer, timeout, this, &TpAnimation::AnimationRun);
}

TpAnimation::~TpAnimation()
{
    TpAnimationData *animationData = static_cast<TpAnimationData *>(data_);
    if (animationData)
    {
        delete animationData;
        animationData = nullptr;
        data_ = nullptr;
    }
}

void TpAnimation::setTargetWidget(TpChildWidget *target)
{
    TpAnimationData *animationData = static_cast<TpAnimationData *>(data_);
    animationData->targetWidget = target;
}

TpChildWidget *TpAnimation::targetWidget()
{
    TpAnimationData *animationData = static_cast<TpAnimationData *>(data_);
    return animationData->targetWidget;
}

void TpAnimation::setLoopCount(const int32_t count)
{
    TpAnimationData *animationData = static_cast<TpAnimationData *>(data_);
    animationData->loopCount = count;
}

int32_t TpAnimation::loopCount()
{
    TpAnimationData *animationData = static_cast<TpAnimationData *>(data_);
    return animationData->loopCount;
}

void TpAnimation::setCustomAnimationFunc(CustomAnimationFunc func)
{
    TpAnimationData *animationData = static_cast<TpAnimationData *>(data_);
    animationData->customFunc = func;
    animationData->targetWidget = nullptr;
}

void TpAnimation::setDuration(const uint32_t &timeMs)
{
    TpAnimationData *animationData = static_cast<TpAnimationData *>(data_);
    animationData->durationMs = timeMs;
}

uint32_t TpAnimation::duration()
{
    TpAnimationData *animationData = static_cast<TpAnimationData *>(data_);
    return animationData->durationMs;
}

void TpAnimation::setStartValue(const TpVariant &value)
{
    TpAnimationData *animationData = static_cast<TpAnimationData *>(data_);
    animationData->startValue = value;
}

TpVariant TpAnimation::startValue()
{
    TpAnimationData *animationData = static_cast<TpAnimationData *>(data_);
    return animationData->startValue;
}

void TpAnimation::setEndValue(const TpVariant &value)
{
    TpAnimationData *animationData = static_cast<TpAnimationData *>(data_);
    animationData->endValue = value;
}

TpVariant TpAnimation::endValue()
{
    TpAnimationData *animationData = static_cast<TpAnimationData *>(data_);
    return animationData->endValue;
}

void TpAnimation::start(const DeletionPolicy &runMode)
{
    TpAnimationData *animationData = static_cast<TpAnimationData *>(data_);

    animationData->stopped.store(false);

    animationData->curTimeMs = 0;
    animationData->curLoopIndex = 0;
    animationData->curTargetKeyIndex = 0;
    animationData->animationTimer.start();
}

void TpAnimation::pause()
{
    TpAnimationData *animationData = static_cast<TpAnimationData *>(data_);
    animationData->stopped.store(true);
    animationData->animationTimer.stop();
}

void TpAnimation::stop()
{
    TpAnimationData *animationData = static_cast<TpAnimationData *>(data_);

    animationData->stopped.store(true);

    animationData->animationTimer.stop();

    finished.emit(); // 触发完成信号

    if (animationData->deleteMode == TpAnimation::DeleteWhenStopped && !animationData->isDelete.load())
    {
        disconnect(&animationData->animationTimer, timeout, this, &TpAnimation::AnimationRun);
        animationData->isDelete.store(true);
        deleteLater();
    }
}

void TpAnimation::setKeyValueAt(const float &percent, const TpVariant &value)
{
    TpAnimationData *animationData = static_cast<TpAnimationData *>(data_);

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

void TpAnimation::AnimationRun()
{
    // std::cout << "动画线程开始 "  << std::endl;

    TpAnimationData *animationData = static_cast<TpAnimationData *>(data_);

    if (animationData->stopped.load())
        return;

    // 计算动画总进度（0.0~1.0）
    const float progress = static_cast<float>(animationData->curTimeMs) / animationData->durationMs;

    // 关键帧进度  keyProgress = （curTime - lastKey） / (curKey - lastKey)
    // 注意关键帧时间单位为 s秒，当前时间单位为 ms毫秒
    float keyProgress = progress;

    TpVariant startValue = animationData->startValue;
    TpVariant endValue = animationData->endValue;

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
            const TpRect start = startValue.toRect();
            const TpRect end = endValue.toRect();
            const TpRect current = TpRect(
                lerp(start.x(), end.x(), keyProgress),
                lerp(start.y(), end.y(), keyProgress),
                lerp(start.width(), end.width(), keyProgress),
                lerp(start.height(), end.height(), keyProgress));
            animationData->targetWidget->setRect(current);
        }
        break;
    }
    case Pos:
    {
        if (startValue.isPoint() && endValue.isPoint())
        {
            const TpPoint start = startValue.toPoint();
            const TpPoint end = endValue.toPoint();
            animationData->targetWidget->move(
                lerp(start.x(), end.x(), keyProgress),
                lerp(start.y(), end.y(), keyProgress));
        }
        break;
    }
    case Size:
        if (startValue.isSize() && endValue.isSize())
        {
            const TpSize start = startValue.toSize();
            const TpSize end = endValue.toSize();
            animationData->targetWidget->setSize(
                lerp(start.width(), end.width(), keyProgress),
                lerp(start.height(), end.height(), keyProgress));
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
        //     const TpVariant value = lerp(
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
        TpChildWidget *targetParent = dynamic_cast<TpChildWidget *>(animationData->targetWidget->parent());
        if (targetParent)
        {
            // std::cout << "更新父窗口" << std::endl;
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

            // 没有被外部停止才释放，stop函数已经释放，避免重复释放
            if (!animationData->stopped.load())
            {
                if (animationData->deleteMode == TpAnimation::DeleteWhenStopped && !animationData->isDelete.load())
                {
                    // std::cout << "动画终止 " <<std::endl;
                    disconnect(&animationData->animationTimer, timeout, this, &TpAnimation::AnimationRun);
                    animationData->isDelete.store(true);
                    deleteLater();
                    // std::cout << "动画终止2222 " <<std::endl;
                }

                animationData->stopped.store(true);
            }
        }
        else
        {
            // 继续循环动画
            animationData->curTimeMs = 0;
            animationData->curTargetKeyIndex = 0;
        }
    }

    // std::cout << "动画线程结束 "  << std::endl;
}
