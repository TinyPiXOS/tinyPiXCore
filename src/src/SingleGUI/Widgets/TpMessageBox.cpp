#include "TpMessageBox.h"
#include "TpEvent.h"
#include "TpPainter.h"
#include "TpDisplay.h"

static int32_t BtnFontColor = _RGB(38, 38, 38);

struct TpMessageBoxData
{
    TpMessageBox::MessageType type = TpMessageBox::Information;

    TpVector<TpString> btnList;
    TpVector<TpRect> btnRect;

    TpFont *font = new TpFont();
    TpFont *btnFont = new TpFont();

    TpString text = "";

    // 窗口关闭时点击按钮索引
    uint32_t clickedIndex = 0;

    ~TpMessageBoxData()
    {
        delete font;
        font = nullptr;

        delete btnFont;
        btnFont = nullptr;
    }
};

TpMessageBox::TpMessageBox(const char *type)
    : TpDialog(type)
{
    TpMessageBoxData *messageData = new TpMessageBoxData();

    messageData->font->setFontSize(20);
    messageData->font->setFontColor(_RGB(38, 38, 38), _RGB(38, 38, 38));

    messageData->btnFont->setFontSize(17);
    messageData->btnFont->setFontColor(BtnFontColor, BtnFontColor);

    data_ = messageData;

    setBackGroundColor(_RGBA(0, 0, 0, 153));

    refreshBaseCss();
}

TpMessageBox::~TpMessageBox()
{
    TpMessageBoxData *messageData = static_cast<TpMessageBoxData *>(data_);
    if (messageData)
    {
        delete messageData;
        messageData = nullptr;
        data_ = nullptr;
    }
}

uint32_t TpMessageBox::exec()
{
    TpMessageBoxData *messageData = static_cast<TpMessageBoxData *>(data_);

    setSize(TpScreen::screenWidth(), TpScreen::screenHeight());

    TpDialog::exec();

    return messageData->clickedIndex;
}

void TpMessageBox::setMessageType(const MessageType &type)
{
    TpMessageBoxData *messageData = static_cast<TpMessageBoxData *>(data_);
    messageData->type = type;
}

void TpMessageBox::setButtonList(const TpVector<TpString> &buttonList)
{
    TpMessageBoxData *messageData = static_cast<TpMessageBoxData *>(data_);
    messageData->btnList.clear();
    messageData->btnList = buttonList;
}

uint32_t TpMessageBox::information(const TpString &text, const TpString &btnText)
{
    TpMessageBox msg;
    msg.setMessageType(TpMessageBox::Information);
    msg.setText(text);

    TpVector<TpString> btnList;
    btnList.emplace_back(btnText);

    msg.setButtonList(btnList);
    return msg.exec();
}

uint32_t TpMessageBox::question(const TpString &text, const TpString &yesBtnText, const TpString &cancleBtnText)
{
    TpMessageBox msg;
    msg.setMessageType(TpMessageBox::Question);
    msg.setText(text);

    TpVector<TpString> btnList;
    btnList.emplace_back(cancleBtnText);
    btnList.emplace_back(yesBtnText);

    msg.setButtonList(btnList);
    return msg.exec();
}

uint32_t TpMessageBox::error(const TpString &text, const TpString &yesBtnText)
{
    TpMessageBox msg;
    msg.setMessageType(TpMessageBox::Error);
    msg.setText(text);

    TpVector<TpString> btnList;
    btnList.emplace_back(yesBtnText);

    msg.setButtonList(btnList);
    return msg.exec();
}

uint32_t TpMessageBox::warning(const TpString &text, const TpString &yesBtnText)
{
    TpMessageBox msg;
    msg.setMessageType(TpMessageBox::Warning);
    msg.setText(text);

    TpVector<TpString> btnList;
    btnList.emplace_back(yesBtnText);

    msg.setButtonList(btnList);
    return msg.exec();
}

void TpMessageBox::setText(const TpString &text)
{
    TpMessageBoxData *messageData = static_cast<TpMessageBoxData *>(data_);
    messageData->text = text;
    update();
}

TpFont *TpMessageBox::font()
{
    return nullptr;
}

void TpMessageBox::setVisible(bool visible)
{
    TpDialog::setVisible(visible);
}

bool TpMessageBox::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpMessageBoxData *messageData = static_cast<TpMessageBoxData *>(data_);

    int btnIndex = 0;
    for (auto btnRect : messageData->btnRect)
    {
        if (btnRect.contains(event->pos()))
        {
            messageData->clickedIndex = btnIndex;
            close();
        }

        btnIndex++;
    }

    return true;
}

bool TpMessageBox::onPaintEvent(TpPaintEvent *event)
{
    TpDialog::onPaintEvent(event);

    TpMessageBoxData *messageData = static_cast<TpMessageBoxData *>(data_);
    if (messageData->text.empty())
        return true;

    TpPainter *paintCanvas = event->canvas();

    uint32_t msgWidth = TpDisplay::dp2Px(450);
    uint32_t msgHeight = TpDisplay::dp2Px(160);

    uint32_t msgX = (width() - msgWidth) / 2.0;
    uint32_t msgY = (height() - msgHeight) / 2.0;

    paintCanvas->pen().setColor(_RGBA(255, 255, 255, 230));
    paintCanvas->setBrush(TpBrush(_RGBA(255, 255, 255, 230)));
    paintCanvas->drawRect(msgX, msgY, msgWidth, msgHeight, 25);

    // 分割提示信息
    uint32_t titleHeight = msgHeight * 0.6;
    uint32_t btnHeight = msgHeight - titleHeight;

    uint32_t paddingLeftRight = 60;

    // 绘制标题和按钮分割线
    paintCanvas->pen().setColor(_RGB(190, 196, 202));
    paintCanvas->setBrush(TpBrush(tinyPiX::NoBrush));
    paintCanvas->drawHLine(msgX + paddingLeftRight, msgX + msgWidth - paddingLeftRight, msgY + titleHeight);

    // 文字行间距
    uint32_t textGap = 5;

    TpList<TpString> msgTextList = messageData->text.split('\n');

    // 标题文本起始Y坐标
    int32_t titleStartY = (titleHeight - (msgTextList.size() * messageData->font->pixelHeight() + (msgTextList.size() - 1) * textGap)) / 2.0;

    // 绘制标题
    for (int i = 0; i < msgTextList.size(); ++i)
    {
        messageData->font->setText(msgTextList.at(i));

        int32_t curLineX = (msgWidth - messageData->font->pixelWidth()) / 2.0;

        paintCanvas->drawText(*messageData->font, msgX + curLineX, msgY + titleStartY + i * (messageData->font->pixelHeight() + textGap));
    }

    if (messageData->btnList.size() == 0)
        return true;

    // 按钮均分窗口宽度
    uint32_t btnWidth = msgWidth / messageData->btnList.size();

    // 清空按钮rect
    messageData->btnRect.clear();

    messageData->btnFont->setText("确定");
    int32_t btnTextY = msgY + titleHeight + (btnHeight - messageData->btnFont->pixelHeight()) / 2.0;

    paintCanvas->pen().setColor(_RGB(190, 196, 202));
    paintCanvas->setBrush(TpBrush(tinyPiX::NoBrush));

    for (int i = 0; i < messageData->btnList.size(); ++i)
    {
        messageData->btnFont->setText(messageData->btnList.at(i));

        int32_t btnTextX = msgX + btnWidth * i + (btnWidth - messageData->btnFont->pixelWidth()) / 2.0;

        // 绘制按钮文本
        if ((i == (messageData->btnList.size() - 1)) && messageData->type == TpMessageBox::Question)
        {
            messageData->btnFont->setFontColor(_RGB(255, 77, 79), _RGB(255, 77, 79));
        }
        else
        {
            messageData->btnFont->setFontColor(BtnFontColor, BtnFontColor);
        }
        paintCanvas->drawText(*messageData->btnFont, btnTextX, btnTextY);

        // 记录按钮rect
        TpRect btnRect(btnTextX, msgY + titleHeight, btnWidth, btnHeight);

        messageData->btnRect.emplace_back(btnRect);

        if (i != (messageData->btnList.size() - 1))
        {
            // 绘制分割线
            paintCanvas->drawVLine(msgX + btnWidth * (i + 1), btnTextY, btnTextY + messageData->btnFont->pixelHeight());
        }
    }

    return true;
}

bool TpMessageBox::onResizeEvent(TpResizeEvent *event)
{
    return true;
}

bool TpMessageBox::eventFilter(TpObject *watched, TpEvent *event)
{
    return false;
}
