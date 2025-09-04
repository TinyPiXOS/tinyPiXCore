#include "TpMusicPlayerView.h"
#include "TpLabel.h"
#include "TpButton.h"
#include "TpDisplay.h"
#include "TpFont.h"

SMART_DEVICE_GUI_NAMESPACE_BEGIN

struct TpMusicPlayerViewData
{
    TpLabel *musicImageLabel;
    TpLabel *nameLabel;
    TpLabel *authorLabel;
    TpLabel *curWordLabel;

    ::TpButton *previousBtn;
    ::TpButton *playPauseBtn;
    ::TpButton *nextBtn;

    bool isPlaying = false;
};

TpMusicPlayerView::TpMusicPlayerView(TpChildWidget *parent)
    : TpChildWidget(parent)
{
    TpMusicPlayerViewData *musicData = new TpMusicPlayerViewData();
    data_ = musicData;

    musicData->musicImageLabel = new TpLabel(this);

    musicData->nameLabel = new TpLabel(this);
    musicData->nameLabel->setAlign(tinyPiX::AlignCenter);
    musicData->nameLabel->font()->setFontSize(TpDisplay::dp2Px(12));

    musicData->authorLabel = new TpLabel(this);
    musicData->authorLabel->setAlign(tinyPiX::AlignCenter);
    musicData->authorLabel->font()->setFontSize(TpDisplay::dp2Px(10));

    musicData->curWordLabel = new TpLabel(this);
    musicData->curWordLabel->setAlign(tinyPiX::AlignCenter);
    musicData->curWordLabel->font()->setFontSize(TpDisplay::dp2Px(10));
    musicData->curWordLabel->font()->setFontColor(_RGB(181, 181, 181), _RGB(181, 181, 181));

    musicData->previousBtn = new ::TpButton(this);
    musicData->previousBtn->setButtonStyle(TpButton::IconOnly);
    musicData->previousBtn->setBackGroundImage(TpImage("/usr/res/tinyPiX/SmartDeviceGUI/TpMusicPlayerView/上一首.png"));
    musicData->previousBtn->setFixedSize(TpDisplay::dp2Px(19), TpDisplay::dp2Px(19));
    connect(musicData->previousBtn, onClicked, [=](bool)
            { onPreviousMusic.emit(); });

    musicData->playPauseBtn = new ::TpButton(this);
    musicData->playPauseBtn->setButtonStyle(TpButton::IconOnly);
    musicData->playPauseBtn->setBackGroundImage(TpImage("/usr/res/tinyPiX/SmartDeviceGUI/TpMusicPlayerView/播放.png"));
    musicData->playPauseBtn->setFixedSize(TpDisplay::dp2Px(19), TpDisplay::dp2Px(19));
    connect(musicData->playPauseBtn, onClicked, [=](bool)
            { 
                std::cout << " 播放按钮点击 " << std::endl;
                if (musicData->isPlaying)
                {
                    musicData->playPauseBtn->setBackGroundImage(TpImage("/usr/res/tinyPiX/SmartDeviceGUI/TpMusicPlayerView/暂停.png"));
                    onPauseMusic.emit();
                }
                else
                {
                    musicData->playPauseBtn->setBackGroundImage(TpImage("/usr/res/tinyPiX/SmartDeviceGUI/TpMusicPlayerView/播放.png"));
                    onPlayingMusic.emit();
                } 
                musicData->isPlaying = !musicData->isPlaying; });

    musicData->nextBtn = new ::TpButton(this);
    musicData->nextBtn->setButtonStyle(TpButton::IconOnly);
    musicData->nextBtn->setBackGroundImage(TpImage("/usr/res/tinyPiX/SmartDeviceGUI/TpMusicPlayerView/下一首.png"));
    musicData->nextBtn->setFixedSize(TpDisplay::dp2Px(19), TpDisplay::dp2Px(19));
    connect(musicData->nextBtn, onClicked, [=](bool)
            { onNextMusic.emit(); });

    setBackGroundColor(_RGB(255, 255, 255));
    setRoundCorners(20);
}

TpMusicPlayerView::~TpMusicPlayerView()
{
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);
    if (musicData)
    {
        delete musicData;
        musicData = nullptr;
    }
}

void TpMusicPlayerView::setName(const TpString &name)
{
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);
    musicData->nameLabel->setText(name);
}

TpString TpMusicPlayerView::name()
{
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);
    return musicData->nameLabel->text();
}

void TpMusicPlayerView::setAuthor(const TpString &author)
{
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);
    musicData->authorLabel->setText(author);
}

TpString TpMusicPlayerView::author()
{
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);
    return musicData->authorLabel->text();
}

void TpMusicPlayerView::setLyric(const TpString &curLyric)
{
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);
    musicData->curWordLabel->setText(curLyric);
}

TpString TpMusicPlayerView::lyric()
{
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);
    return musicData->curWordLabel->text();
}

void TpMusicPlayerView::setImage(const TpString &imagePath)
{
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);
    musicData->musicImageLabel->setBackGroundImage(TpImage(imagePath));
}

void TpMusicPlayerView::setImage(TpImage image)
{
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);
    musicData->musicImageLabel->setBackGroundImage(image);
}

bool TpMusicPlayerView::onPaintEvent(TpObjectPaintEvent *event)
{
    TpChildWidget::onPaintEvent(event);

    return true;
}

bool TpMusicPlayerView::onResizeEvent(TpObjectResizeEvent *event)
{
    // 重新计算子组件布局
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);

    // 图片资源
    int32_t imageSize = width() < height() ? width() : height();
    imageSize = 0.3846 * imageSize;
    musicData->musicImageLabel->setSize(imageSize, imageSize);
    musicData->musicImageLabel->move(TpDisplay::dp2Px(17), TpDisplay::dp2Px(17));

    // 调整标题位置
    int32_t nameX = musicData->musicImageLabel->pos().x() + musicData->musicImageLabel->width();
    int32_t nameY = musicData->musicImageLabel->pos().y() + (musicData->musicImageLabel->height() / 2.0 - musicData->nameLabel->font()->fontSize()) / 2.0;
    musicData->nameLabel->setRect(nameX, nameY, width() - nameX, musicData->nameLabel->font()->fontSize());

    // 歌手位置
    int32_t authorY = musicData->musicImageLabel->pos().y() + (musicData->musicImageLabel->height() / 2.0) + 5;
    musicData->authorLabel->setRect(nameX, authorY, width() - nameX, musicData->authorLabel->font()->fontSize());

    // 当前歌词位置
    int32_t wordY = musicData->musicImageLabel->pos().y() + musicData->musicImageLabel->height() + 10;
    musicData->curWordLabel->setRect(0, wordY, width(), musicData->curWordLabel->font()->fontSize());

    // 操作按钮位置
    // 播放/暂停按钮在水平中央位置；垂直在歌词下空白位置的中央位置
    int32_t platBtnX = (width() - musicData->playPauseBtn->width()) / 2.0;
    int32_t platBtnY = height() - musicData->curWordLabel->pos().y() - musicData->curWordLabel->height();
    platBtnY = (platBtnY - musicData->playPauseBtn->height()) / 2.0;
    platBtnY = musicData->curWordLabel->pos().y() + musicData->curWordLabel->height() + platBtnY;

    musicData->playPauseBtn->move(platBtnX, platBtnY);

    musicData->previousBtn->move(musicData->playPauseBtn->pos().x() - musicData->previousBtn->width() - 30, platBtnY);
    musicData->nextBtn->move(musicData->playPauseBtn->pos().x() + musicData->playPauseBtn->width() + 30, platBtnY);

    return true;
}

SMART_DEVICE_GUI_NAMESPACE_END
