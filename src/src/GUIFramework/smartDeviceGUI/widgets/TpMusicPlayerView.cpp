#include "TpMusicPlayerView.h"
#include "tpLabel.h"
#include "tpButton.h"
#include "tpDisplay.h"
#include "tpFont.h"

SMART_DEVICE_GUI_NAMESPACE_BEGIN

struct TpMusicPlayerViewData
{
    tpLabel *musicImageLabel;
    tpLabel *nameLabel;
    tpLabel *authorLabel;
    tpLabel *curWordLabel;

    tpButton *previousBtn;
    tpButton *playPauseBtn;
    tpButton *nextBtn;
};

TpMusicPlayerView::TpMusicPlayerView(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    TpMusicPlayerViewData *musicData = new TpMusicPlayerViewData();
    data_ = musicData;

    musicData->musicImageLabel = new tpLabel(this);

    musicData->nameLabel = new tpLabel(this);
    musicData->nameLabel->setAlign(tinyPiX::AlignCenter);
    musicData->nameLabel->font()->setFontSize(tpDisplay::dp2Px(12));

    musicData->authorLabel = new tpLabel(this);
    musicData->authorLabel->setAlign(tinyPiX::AlignCenter);
    musicData->authorLabel->font()->setFontSize(tpDisplay::dp2Px(10));

    musicData->curWordLabel = new tpLabel(this);
    musicData->curWordLabel->setAlign(tinyPiX::AlignCenter);
    musicData->curWordLabel->font()->setFontSize(tpDisplay::dp2Px(10));
    musicData->curWordLabel->font()->setFontColor(_RGB(181, 181, 181), _RGB(181, 181, 181));

    musicData->previousBtn = new tpButton(this);
    musicData->previousBtn->setButtonStyle(tpButton::IconOnly);
    musicData->previousBtn->setIcon("/usr/res/tinyPiX/smartDeviceGUI/上一首.png");

    musicData->playPauseBtn = new tpButton(this);
    musicData->playPauseBtn->setButtonStyle(tpButton::IconOnly);
    musicData->playPauseBtn->setIcon("/usr/res/tinyPiX/smartDeviceGUI/播放.png");

    musicData->nextBtn = new tpButton(this);
    musicData->nextBtn->setButtonStyle(tpButton::IconOnly);
    musicData->nextBtn->setIcon("/usr/res/tinyPiX/smartDeviceGUI/下一首.png");

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

void TpMusicPlayerView::setName(const tpString &name)
{
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);
    musicData->nameLabel->setText(name);
}

tpString TpMusicPlayerView::name()
{
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);
    return musicData->nameLabel->text();
}

void TpMusicPlayerView::setAuthor(const tpString &author)
{
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);
    musicData->authorLabel->setText(author);
}

tpString TpMusicPlayerView::author()
{
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);
    return musicData->authorLabel->text();
}

void TpMusicPlayerView::setLyric(const tpString &curLyric)
{
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);
    musicData->curWordLabel->setText(curLyric);
}

tpString TpMusicPlayerView::lyric()
{
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);
    return musicData->curWordLabel->text();
}

void TpMusicPlayerView::setImage(const tpString &imagePath)
{
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);
    musicData->musicImageLabel->setBackGroundImage(TpImage(imagePath));
}

void TpMusicPlayerView::setImage(TpImage image)
{
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);
    musicData->musicImageLabel->setBackGroundImage(image);
}

bool TpMusicPlayerView::onPaintEvent(tpObjectPaintEvent *event)
{
    tpChildWidget::onPaintEvent(event);

    return true;
}

bool TpMusicPlayerView::onResizeEvent(tpObjectResizeEvent *event)
{
    // 重新计算子组件布局
    TpMusicPlayerViewData *musicData = static_cast<TpMusicPlayerViewData *>(data_);

    // 图片资源
    int32_t imageSize = width() < height() ? width() : height();
    imageSize = 0.3846 * imageSize;
    musicData->musicImageLabel->setSize(imageSize, imageSize);
    musicData->musicImageLabel->move(tpDisplay::dp2Px(17), tpDisplay::dp2Px(17));

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
