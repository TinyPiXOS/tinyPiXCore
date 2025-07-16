#include "tpMessageBox.h"
#include "tpEvent.h"
#include "tpCanvas.h"
#include "tpDisplay.h"

static int32_t BtnFontColor = _RGB(38, 38, 38);

struct tpMessageBoxData
{
    tpMessageBox::MessageType type = tpMessageBox::Information;

    tpVector<tpString> btnList;
    tpVector<ItpRect> btnRect;

    tpFont *font = new tpFont();
    tpFont *btnFont = new tpFont();

    tpString text = "";

    // 窗口关闭时点击按钮索引
    uint32_t clickedIndex = 0;

    ~tpMessageBoxData()
    {
        delete font;
        font = nullptr;

        delete btnFont;
        btnFont = nullptr;
    }
};

tpMessageBox::tpMessageBox(const char *type)
    : tpDialog(type)
{
    tpMessageBoxData *messageData = new tpMessageBoxData();

    messageData->font->setFontSize(20);
    messageData->font->setFontColor(_RGB(38, 38, 38), _RGB(38, 38, 38));

    messageData->btnFont->setFontSize(17);
    messageData->btnFont->setFontColor(BtnFontColor, BtnFontColor);

    data_ = messageData;

    setBackGroundColor(_RGBA(0, 0, 0, 153));

    refreshBaseCss();
}

tpMessageBox::~tpMessageBox()
{
    tpMessageBoxData *messageData = static_cast<tpMessageBoxData *>(data_);
    if (messageData)
    {
        delete messageData;
        messageData = nullptr;
        data_ = nullptr;
    }
}

uint32_t tpMessageBox::exec()
{
    tpMessageBoxData *messageData = static_cast<tpMessageBoxData *>(data_);

    setSize(tpScreen::screenWidth(), tpScreen::screenHeight());

    tpDialog::exec();

    return messageData->clickedIndex;
}

void tpMessageBox::setMessageType(const MessageType &type)
{
    tpMessageBoxData *messageData = static_cast<tpMessageBoxData *>(data_);
    messageData->type = type;
}

void tpMessageBox::setButtonList(const tpVector<tpString> &buttonList)
{
    tpMessageBoxData *messageData = static_cast<tpMessageBoxData *>(data_);
    messageData->btnList.clear();
    messageData->btnList = buttonList;
}

uint32_t tpMessageBox::information(const tpString &text, const tpString &btnText)
{
    tpMessageBox msg;
    msg.setMessageType(tpMessageBox::Information);
    msg.setText(text);

    tpVector<tpString> btnList;
    btnList.emplace_back(btnText);

    msg.setButtonList(btnList);
    return msg.exec();
}

uint32_t tpMessageBox::question(const tpString &text, const tpString &yesBtnText, const tpString &cancleBtnText)
{
    tpMessageBox msg;
    msg.setMessageType(tpMessageBox::Question);
    msg.setText(text);

    tpVector<tpString> btnList;
    btnList.emplace_back(cancleBtnText);
    btnList.emplace_back(yesBtnText);

    msg.setButtonList(btnList);
    return msg.exec();
}

uint32_t tpMessageBox::error(const tpString &text, const tpString &yesBtnText)
{
    tpMessageBox msg;
    msg.setMessageType(tpMessageBox::Error);
    msg.setText(text);

    tpVector<tpString> btnList;
    btnList.emplace_back(yesBtnText);

    msg.setButtonList(btnList);
    return msg.exec();
}

uint32_t tpMessageBox::warning(const tpString &text, const tpString &yesBtnText)
{
    tpMessageBox msg;
    msg.setMessageType(tpMessageBox::Warning);
    msg.setText(text);

    tpVector<tpString> btnList;
    btnList.emplace_back(yesBtnText);

    msg.setButtonList(btnList);
    return msg.exec();
}

void tpMessageBox::setText(const tpString &text)
{
    tpMessageBoxData *messageData = static_cast<tpMessageBoxData *>(data_);
    messageData->text = text;
    update();
}

tpFont *tpMessageBox::font()
{
    return nullptr;
}

void tpMessageBox::setVisible(bool visible)
{
    tpDialog::setVisible(visible);
}

bool tpMessageBox::onMouseRleaseEvent(tpMouseEvent *event)
{
    tpMessageBoxData *messageData = static_cast<tpMessageBoxData *>(data_);

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

bool tpMessageBox::onPaintEvent(tpObjectPaintEvent *event)
{
    tpDialog::onPaintEvent(event);

    tpMessageBoxData *messageData = static_cast<tpMessageBoxData *>(data_);
    if (messageData->text.empty())
        return true;

    tpCanvas *paintCanvas = event->canvas();

    uint32_t msgWidth = tpDisplay::dp2Px(450);
    uint32_t msgHeight = tpDisplay::dp2Px(160);

    uint32_t msgX = (width() - msgWidth) / 2.0;
    uint32_t msgY = (height() - msgHeight) / 2.0;

    paintCanvas->roundedBox(msgX, msgY, msgX + msgWidth, msgY + msgHeight, 25, _RGBA(255, 255, 255, 230));

    // 分割提示信息
    uint32_t titleHeight = msgHeight * 0.6;
    uint32_t btnHeight = msgHeight - titleHeight;

    uint32_t paddingLeftRight = 60;

    // 绘制标题和按钮分割线
    paintCanvas->hline(msgX + paddingLeftRight, msgX + msgWidth - paddingLeftRight, msgY + titleHeight, _RGB(190, 196, 202));

    // 文字行间距
    uint32_t textGap = 5;

    tpList<tpString> msgTextList = messageData->text.split('\n');

    // 标题文本起始Y坐标
    int32_t titleStartY = (titleHeight - (msgTextList.size() * messageData->font->pixelHeight() + (msgTextList.size() - 1) * textGap)) / 2.0;

    // 绘制标题
    for (int i = 0; i < msgTextList.size(); ++i)
    {
        messageData->font->setText(msgTextList.at(i));

        int32_t curLineX = (msgWidth - messageData->font->pixelWidth()) / 2.0;

        paintCanvas->renderText(*messageData->font, msgX + curLineX, msgY + titleStartY + i * (messageData->font->pixelHeight() + textGap));
    }

    if (messageData->btnList.size() == 0)
        return true;

    // 按钮均分窗口宽度
    uint32_t btnWidth = msgWidth / messageData->btnList.size();

    // 清空按钮rect
    messageData->btnRect.clear();

    messageData->btnFont->setText("确定");
    int32_t btnTextY = msgY + titleHeight + (btnHeight - messageData->btnFont->pixelHeight()) / 2.0;

    for (int i = 0; i < messageData->btnList.size(); ++i)
    {
        messageData->btnFont->setText(messageData->btnList.at(i));

        int32_t btnTextX = msgX + btnWidth * i + (btnWidth - messageData->btnFont->pixelWidth()) / 2.0;

        // 绘制按钮文本
        if ((i == (messageData->btnList.size() - 1)) && messageData->type == tpMessageBox::Question)
        {
            messageData->btnFont->setFontColor(_RGB(255, 77, 79), _RGB(255, 77, 79));
        }
        else
        {
            messageData->btnFont->setFontColor(BtnFontColor, BtnFontColor);
        }
        paintCanvas->renderText(*messageData->btnFont, btnTextX, btnTextY);

        // 记录按钮rect
        ItpRect btnRect;
        btnRect.x = btnTextX;
        btnRect.y = msgY + titleHeight;
        btnRect.w = btnWidth;
        btnRect.h = btnHeight;

        messageData->btnRect.emplace_back(btnRect);

        if (i != (messageData->btnList.size() - 1))
        {
            // 绘制分割线
            paintCanvas->vline(msgX + btnWidth * (i + 1), btnTextY, btnTextY + messageData->btnFont->pixelHeight(), _RGB(190, 196, 202));
        }
    }

    return true;
}

bool tpMessageBox::onResizeEvent(tpObjectResizeEvent *event)
{
    return true;
}

bool tpMessageBox::eventFilter(tpObject *watched, tpEvent *event)
{
    return false;
}
