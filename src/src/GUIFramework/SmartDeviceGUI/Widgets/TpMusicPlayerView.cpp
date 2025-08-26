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

    TpButton *previousBtn;
    TpButton *playPauseBtn;
    TpButton *nextBtn;
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

    musicData->previousBtn = new TpButton(this);
    musicData->previousBtn->setButtonStyle(TpButton::IconOnly);
    musicData->previousBtn->setIcon("/usr/res/tinyPiX/SmartDeviceGUI/TpMusicPlayerView/上一首.png");

    musicData->playPauseBtn = new TpButton(this);
    musicData->playPauseBtn->setButtonStyle(TpButton::IconOnly);
    musicData->playPauseBtn->setIcon("/usr/res/tinyPiX/SmartDeviceGUI/TpMusicPlayerView/播放.png");

    musicData->nextBtn = new TpButton(this);
    musicData->nextBtn->setButtonStyle(TpButton::IconOnly);
    musicData->nextBtn->setIcon("/usr/res/tinyPiX/SmartDeviceGUI/TpMusicPlayerView/下一首.png");

    setBackGroundColor(_RGB(255, 255, 255));
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
    // TpChildWidget::onPaintEvent(event);

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
    int32_t nameX = musicData->musicImageLabel->pos().x + musicData->musicImageLabel->width();
    int32_t nameY = musicData->musicImageLabel->pos().y + (musicData->musicImageLabel->height() / 2.0 - musicData->nameLabel->font()->fontSize()) / 2.0;
    musicData->nameLabel->setRect(nameX, nameY, width() - nameX, musicData->nameLabel->font()->fontSize());

    // 歌手位置
    int32_t authorY = musicData->musicImageLabel->pos().y + (musicData->musicImageLabel->height() / 2.0) + 5;
    musicData->authorLabel->setRect(nameX, authorY, width() - nameX, musicData->authorLabel->font()->fontSize());

    // 当前歌词位置
    int32_t wordY = musicData->musicImageLabel->pos().y + musicData->musicImageLabel->height();
    musicData->curWordLabel->setRect(0, wordY, width(), musicData->curWordLabel->font()->fontSize());

    return true;
}

SMART_DEVICE_GUI_NAMESPACE_END
