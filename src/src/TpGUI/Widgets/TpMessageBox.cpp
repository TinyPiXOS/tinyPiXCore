#include "TpMessageBox.h"
#include "TpEvent.h"
#include "TpPainter.h"
#include "SystemInfo/TpDisplay.h"

static int32_t BtnFontColor = _RGB(38, 38, 38);

struct TpMessageBoxData
{
    uint32_t msgWidth = TpDisplay::dp2Px(430);
    uint32_t msgHeight = TpDisplay::dp2Px(160);

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

TpMessageBox::TpMessageBox(MessageType type)
    : TpDialog()
{
    TpMessageBoxData *messageData = new TpMessageBoxData();

    messageData->font->setFontSize(20);
    messageData->font->setFontColor(_RGB(38, 38, 38));

    messageData->btnFont->setFontSize(17);
    messageData->btnFont->setFontColor(BtnFontColor);

    data_ = messageData;

    refreshBaseCss();

    setBackGroundColor(_RGBA(255, 255, 255, 230));
    setRoundCorners(25);

    setMessageType(type);
}

TpMessageBox::TpMessageBox(const TpString &text, MessageType type)
    : TpDialog()
{
    TpMessageBoxData *messageData = new TpMessageBoxData();

    messageData->font->setFontSize(20);
    messageData->font->setFontColor(_RGB(38, 38, 38));

    messageData->btnFont->setFontSize(17);
    messageData->btnFont->setFontColor(BtnFontColor);

    data_ = messageData;

    refreshBaseCss();

    setBackGroundColor(_RGBA(255, 255, 255, 230));
    setRoundCorners(25);

    setText(text);
    setMessageType(type);
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

void TpMessageBox::exec()
{
    TpMessageBoxData *messageData = static_cast<TpMessageBoxData *>(data_);
    setSize(messageData->msgWidth, messageData->msgHeight);

    TpDialog::exec();
}

void TpMessageBox::setText(const TpString &text)
{
    TpMessageBoxData *messageData = static_cast<TpMessageBoxData *>(data_);
    messageData->text = text;
    update();
}

void TpMessageBox::setMessageType(MessageType type)
{
    TpMessageBoxData *messageData = static_cast<TpMessageBoxData *>(data_);
    messageData->type = type;

    switch (messageData->type)
    {
    case Information:
        setButtonList(TpVector<TpString>{"确认"});
        break;
    case Question:
        setButtonList(TpVector<TpString>{"确认", "取消"});
        break;
    case Warning:
        setButtonList(TpVector<TpString>{"确认"});
        break;
    case Error:
        setButtonList(TpVector<TpString>{"确认"});
        break;
    default:
        break;
    }
}

void TpMessageBox::setButtonList(const TpVector<TpString> &buttonList)
{
    TpMessageBoxData *messageData = static_cast<TpMessageBoxData *>(data_);
    messageData->btnList.clear();
    messageData->btnList = buttonList;
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
            onClose.emit(messageData->clickedIndex);
            close();
            break;
        }

        btnIndex++;
    }

    return true;
}

bool TpMessageBox::onPaintEvent(TpPaintEvent *event)
{
    TpMessageBoxData *messageData = static_cast<TpMessageBoxData *>(data_);
    if (messageData->text.empty())
        return true;

    TpDialog::onPaintEvent(event);

    TpPainter *painter = event->painter();

    painter->setPen(backGroundColor());
    painter->setBrush(TpBrush(backGroundColor()));
    painter->drawRect(0, 0, width(), height(), roundCorners());

    // 分割提示信息
    uint32_t titleHeight = messageData->msgHeight * 0.6;
    uint32_t btnHeight = messageData->msgHeight - titleHeight;
    uint32_t paddingLeftRight = 60;

    // 绘制标题和按钮分割线
    painter->pen().setColor(_RGB(190, 196, 202));
    painter->setBrush(TpBrush(Tp::NoBrush));
    painter->drawHLine(paddingLeftRight, messageData->msgWidth - paddingLeftRight, titleHeight);

    // 文字行间距
    uint32_t textGap = 5;

    TpList<TpString> msgTextList = messageData->text.split('\n');

    // 标题文本起始Y坐标
    int32_t titleStartY = (titleHeight - (msgTextList.size() * messageData->font->pixelHeight() + (msgTextList.size() - 1) * textGap)) / 2.0;

    // 绘制标题
    for (int i = 0; i < msgTextList.size(); ++i)
    {
        messageData->font->setText(msgTextList.at(i));

        int32_t curLineX = (messageData->msgWidth - messageData->font->pixelWidth()) / 2.0;

        painter->drawText(*messageData->font, curLineX, titleStartY + i * (messageData->font->pixelHeight() + textGap));
    }

    if (messageData->btnList.size() == 0)
        return true;

    // 按钮均分窗口宽度
    uint32_t btnWidth = messageData->msgWidth / messageData->btnList.size();

    // 清空按钮rect
    messageData->btnRect.clear();

    messageData->btnFont->setText("确定");
    int32_t btnTextY = titleHeight + (btnHeight - messageData->btnFont->pixelHeight()) / 2.0;

    painter->pen().setColor(_RGB(190, 196, 202));
    painter->setBrush(TpBrush(Tp::NoBrush));

    for (int i = 0; i < messageData->btnList.size(); ++i)
    {
        messageData->btnFont->setText(messageData->btnList.at(i));

        int32_t btnTextX = btnWidth * i + (btnWidth - messageData->btnFont->pixelWidth()) / 2.0;

        // 绘制按钮文本
        if ((i == (messageData->btnList.size() - 1)) && messageData->type == TpMessageBox::Question)
        {
            messageData->btnFont->setFontColor(_RGB(255, 77, 79));
        }
        else
        {
            messageData->btnFont->setFontColor(BtnFontColor);
        }
        painter->drawText(*messageData->btnFont, btnTextX, btnTextY);

        // 记录按钮rect
        TpRect btnRect(btnWidth * i, titleHeight, btnWidth, btnHeight);
        messageData->btnRect.emplace_back(btnRect);

        if (i != (messageData->btnList.size() - 1))
        {
            // 绘制分割线
            painter->drawVLine(btnWidth * (i + 1), btnTextY, btnTextY + messageData->btnFont->pixelHeight());
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
