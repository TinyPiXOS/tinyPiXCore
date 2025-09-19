#include "TpLottieAnimation.h"
#include "thorVG/thorvg.h"
#include "TpFileInfo.h"
#include "TpTimer.h"

struct TpLottieAnimationData
{
    tvg::Animation *animation = tvg::Animation::gen();

    TpTimer *updateTimer;

    // 当前帧
    float currentFrames = 0;
    // 总帧数
    float totalFrames = 0;
    // 持续时间
    float duration = 0;

    TpLottieAnimationData()
    {
        updateTimer = new TpTimer(16);
    }

    ~TpLottieAnimationData()
    {
        updateTimer->stop();

        delete updateTimer;
        updateTimer = nullptr;

        delete animation;
        animation = nullptr;
    }
};

TpLottieAnimation::TpLottieAnimation(TpChildWidget *parent) : TpChildWidget(parent)
{
    TpLottieAnimationData *lottieData = new TpLottieAnimationData();
    data_ = lottieData;

    connect(lottieData->updateTimer, timeout, [=]()
            {
                ++lottieData->currentFrames;
                if (lottieData->currentFrames >= lottieData->totalFrames)
                    lottieData->currentFrames = 0;
                lottieData->animation->frame(lottieData->currentFrames); 
                update(); });
}

TpLottieAnimation::TpLottieAnimation(const TpString &lottieFile, TpChildWidget *parent)
{
    TpLottieAnimationData *lottieData = new TpLottieAnimationData();
    data_ = lottieData;
    load(lottieFile);

    connect(lottieData->updateTimer, timeout, [=]()
            {
                ++lottieData->currentFrames;
                if (lottieData->currentFrames >= lottieData->totalFrames)
                    lottieData->currentFrames = 0;
                lottieData->animation->frame(lottieData->currentFrames); 
                update(); });
}

TpLottieAnimation::~TpLottieAnimation()
{
    TpLottieAnimationData *lottieData = static_cast<TpLottieAnimationData *>(data_);
    if (lottieData)
    {
        delete lottieData;
        lottieData = nullptr;
        data_ = nullptr;
    }
}

bool TpLottieAnimation::load(const TpString &lottieFile)
{
    TpLottieAnimationData *lottieData = static_cast<TpLottieAnimationData *>(data_);

    if (lottieFile.empty())
        return false;

    TpFileInfo lottieFileInfo(lottieFile);
    if (!lottieFileInfo.exists())
        return false;

    if (lottieData->animation->picture()->load(lottieFile.c_str()) != tvg::Result::Success)
    {
        std::cout << "Lottie文件加载失败" << std::endl;
        return false;
    }

    lottieData->currentFrames = 0;
    lottieData->totalFrames = lottieData->animation->totalFrame();
    lottieData->duration = lottieData->animation->duration();

    lottieData->animation->frame(lottieData->currentFrames);

    update();

    // std::cout << "总帧数： " << lottieData->totalFrames << std::endl;
    // std::cout << "动画事件： " << lottieData->duration << std::endl;
    // std::cout << "刷新间隔时间： " << 1000.0 * lottieData->duration / lottieData->totalFrames << std::endl;

    // 计算刷新率
    lottieData->updateTimer->setInterval(1000.0 * lottieData->duration / lottieData->totalFrames);
    lottieData->updateTimer->start();

    return true;
}

void TpLottieAnimation::setVisible(bool visible)
{
    TpChildWidget::setVisible(visible);

    TpLottieAnimationData *lottieData = static_cast<TpLottieAnimationData *>(data_);

    if (visible)
        lottieData->updateTimer->start();
    else
        lottieData->updateTimer->stop();
}

bool TpLottieAnimation::onPaintEvent(TpPaintEvent *event)
{
    TpChildWidget::onPaintEvent(event);

    TpLottieAnimationData *lottieData = static_cast<TpLottieAnimationData *>(data_);

    std::pair<void *, void *> canvasPtrPair = TpChildWidget::canvasPtr();
    tvg::Scene *widgetScene = (tvg::Scene *)canvasPtrPair.second;

    tvg::Picture *dupPicture = static_cast<tvg::Picture *>(lottieData->animation->picture()->duplicate());
    dupPicture->size(width(), height());
    dupPicture->translate(pos().x(), pos().y());

    widgetScene->push(std::move(dupPicture));

    return false;
}
