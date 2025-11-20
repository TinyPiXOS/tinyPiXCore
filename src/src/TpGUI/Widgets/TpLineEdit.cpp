#include "TpLineEdit.h"
#include "TpPainter.h"
#include "TpEvent.h"
#include "TpTimer.h"
#include "TpClipboard.h"
#include "TpLabel.h"
#include "TpImage.h"
#include "TpApp.h"
#include "TpVirtualKeyboard.h"

static const int32_t CURSOR_EDGE_THRESHOLD = 3; // 光标边缘触发阈值(像素)

static uint32_t CaculateTextWidth(TpFont *font, const TpString &text)
{
    if (text.empty())
        return 0;

    font->setText(text);

    // std::cout << "Caculate Text : " << text << "  Width : " << font->layoutWidth() << std::endl;
    uint32_t textWidth = 0;
    for (int32_t i = 0; i < text.logicalLength(); ++i)
    {
        TpString curCharStr = text.mid(i, 1);
        font->setText(curCharStr);
        textWidth += font->layoutWidth();

        // std::cout << "CurChar : " << curCharStr << "  Width : " << font->layoutWidth() << std::endl;
    }

    return textWidth;
}

struct TpLineEditData
{
    // icon的显示label
    TpLabel *iconLabel = nullptr;

    // 有icon时，文本要叠加偏移量
    uint32_t iconOffset = 0;

    // 人输入框文本
    TpString text;

    // 提示文本
    TpString placeholerText;

    // 当前光标偏移量
    int32_t cursorPos;
    bool hasFocus;      // 是否有焦点
    int32_t textOffset; // 文本偏移量，用于处理滚动

    // 选择起始/终止索引
    int32_t selectionStart;
    int32_t selectionEnd;
    // 是否选择
    bool isSelected;

    TpFont *textFont;

    TpTimer *updateCursor;

    // 鼠标左键是否按下
    bool mouseIsPress;

    Tp::Alignment align = Tp::AlignLeft;

    bool readOnly = false;

    TpLineEditData() : text(""), cursorPos(0), hasFocus(false), textOffset(0), selectionStart(-1), selectionEnd(-1), isSelected(false), mouseIsPress(false)
    {
        textFont = new TpFont();
        textFont->setFontForeColor(_RGB(0, 0, 0));
        textFont->setFontSize(15);

        updateCursor = new TpTimer(800);
    }

    ~TpLineEditData()
    {
        delete textFont;
        textFont = nullptr;

        if (updateCursor->isActive())
            updateCursor->stop();

        delete updateCursor;
        updateCursor = nullptr;

        if (iconLabel)
        {
            iconLabel->setParent(nullptr);
            delete iconLabel;
            iconLabel = nullptr;
        }
    }
};

TpLineEdit::TpLineEdit(TpWidget *parent)
    : TpWidget(parent)
{
    data_ = new TpLineEditData();
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);

    editData->iconLabel = new TpLabel(this);
    editData->iconLabel->setVisible(false);

    // connect(editData->updateCursor, timeout, [=]()
    // {
    //     editData->
    //      update();
    //      });

    refreshBaseCss();
}

TpLineEdit::~TpLineEdit()
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (editData)
    {
        delete editData;
        editData = nullptr;
        data_ = nullptr;
    }
}

TpFont *TpLineEdit::font()
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    return editData->textFont;
}

void TpLineEdit::setText(const TpString &text)
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return;

    editData->text = text;
    update();
}

TpString TpLineEdit::text()
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return "";

    return editData->text;
}

void TpLineEdit::setIcon(const TpString &iconPath)
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return;

    if (iconPath.empty())
    {
        editData->iconLabel->setVisible(false);
        editData->iconOffset = 0;
        return;
    }

    editData->iconLabel->setBackGroundImage(TpImage(iconPath));

    editData->iconLabel->setVisible(true);
    caculateIconPos();

    update();
}

void TpLineEdit::clear()
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return;

    editData->text = "";
    editData->textFont->setText(editData->text);
    editData->isSelected = false;

    update();
}

void TpLineEdit::setPlaceholderText(const TpString &text)
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return;

    editData->placeholerText = text;
}

TpString TpLineEdit::placeholderText()
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return "";

    return editData->placeholerText;
}

void TpLineEdit::setAlign(const Tp::Alignment align)
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return;
    editData->align = align;
    update();
}

void TpLineEdit::setReadOnly(const bool &readOnly)
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    editData->readOnly = readOnly;
}

void TpLineEdit::virtualKeyboardInput(const Tp::VirtualKeyboardInputType &type, const TpString &text)
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (editData->readOnly)
        return;

    if (type == Tp::Delete)
    {
        dealBackSpace();
    }
    else if (type == Tp::Pinyin)
    {
    }
    else if (type == Tp::Chinese || type == Tp::Symbol)
    {
        // 如果有选中文本，先删除选中的文本
        if (editData->isSelected)
        {
            deleteSelectedText();
        }

        insertCharacter(text);
    }
    else
    {
    }
}

bool TpLineEdit::onPaintEvent(TpPaintEvent *event)
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return true;

    TpPainter *canvas = event->painter();
    if (!canvas)
        return true;

    // 父类绘制背景
    TpWidget::onPaintEvent(event);

    tpShared<TpCssData> curCssData = currentStatusCss();
    editData->textFont->setFontColor(curCssData->color());
    editData->textFont->setFontSize(curCssData->fontSize());

    uint32_t leftMargin = curCssData->paddingLeft();
    uint32_t rightMargin = curCssData->paddingRight();
    uint32_t upMargin = curCssData->paddingTop();
    uint32_t downMargin = curCssData->paddingBottom();

    if (!editData->iconLabel->visible())
    {
        editData->iconOffset = 0;
    }
    else
    {
        leftMargin = editData->iconOffset;
    }

    // 计算可见文本宽度
    uint32_t visibleTextWidth = rect().width() - leftMargin - rightMargin;

    // 计算文本偏移
    TpString cursorLeftStr = editData->text.mid(0, editData->cursorPos);
    uint32_t cursorLeftWidth = CaculateTextWidth(editData->textFont, cursorLeftStr);
    if (cursorLeftWidth > visibleTextWidth)
    {
        editData->textOffset = cursorLeftWidth - visibleTextWidth;
    }
    else
    {
        editData->textOffset = leftMargin;
    }

    // 默认左对齐偏移
    uint32_t alignOffset = cacualteAlignOffset(visibleTextWidth);

    // 绘制选中的文本背景
    if (editData->isSelected)
    {
        int32_t start = std::min(editData->selectionStart, editData->selectionEnd);
        int32_t end = std::max(editData->selectionStart, editData->selectionEnd);
        TpString selectedText = editData->text.mid(start, end - start);
        uint32_t selectionWidth = CaculateTextWidth(editData->textFont, selectedText);
        uint32_t selectionX = leftMargin + CaculateTextWidth(editData->textFont, editData->text.mid(0, start)) - editData->textOffset;
        uint32_t selectionY = (rect().height() - editData->textFont->pixelHeight()) / 2;

        canvas->setBrush(TpBrush(_RGB(0, 120, 215)));
        canvas->drawRect(selectionX, selectionY, selectionWidth, editData->textFont->pixelHeight()); // 蓝色背景表示选中
    }

    // 绘制文本
    if (!editData->text.empty())
    {
        editData->textFont->setText(editData->text);
        uint32_t textY = (rect().height() - editData->textFont->pixelHeight()) / 2;

        uint32_t textX = alignOffset - editData->textOffset;

        // if (cursorLeftWidth > visibleTextWidth)
        // {
        //     textX = alignOffset - editData->textOffset;
        // }
        // else
        // {
        //     textX = editData->iconOffset;
        // }

        textX = leftMargin - editData->textOffset;

        // std::cout << "text  X" << textX << std::endl;

        // std::cout << "editData->text  " << editData->text << std::endl;
        canvas->drawText(*editData->textFont, textX, textY, editData->text);
    }
    else
    {
        // 文本为空时绘制提示文本
        TpFont placeholderTextFont;
        placeholderTextFont.setFontColor(_RGB(191, 191, 191));
        placeholderTextFont.setFontSize(curCssData->fontSize());

        placeholderTextFont.setText(editData->placeholerText);

        uint32_t textY = (rect().height() - placeholderTextFont.pixelHeight()) / 2;

        uint32_t placeTextX = leftMargin;
        uint32_t placeholderTextWidth = CaculateTextWidth(&placeholderTextFont, editData->placeholerText);
        if (editData->align == Tp::AlignLeft)
        {
            placeTextX = leftMargin;
        }
        else if (editData->align == Tp::AlignRight)
        {
            placeTextX = rect().width() - rightMargin - placeholderTextWidth;
        }
        else if (editData->align == Tp::AlignHCenter || editData->align == Tp::AlignCenter)
        {
            placeTextX = (rect().width() - leftMargin - rightMargin - placeholderTextWidth) / 2.0;
        }
        else
        {
            placeTextX = leftMargin;
        }

        canvas->drawText(placeholderTextFont, placeTextX, textY, editData->placeholerText);
    }

    // 如果有图标，绘制一个遮罩层
    if (editData->iconLabel->visible())
    {
        /*
            绘制一个与背景同色遮罩层，避免文本偏移后，会显示在图标的底部
            用圆角接口，避免边缘位置对不齐
        */
        uint32_t blockWidth = editData->iconOffset;
        uint32_t blockHeight = height();

        uint32_t blockRound = curCssData->roundCorners();

        canvas->setBrush(TpBrush(curCssData->backgroundColor()));
        canvas->drawRect(0, 0, blockWidth, blockHeight, blockRound);
        // canvas->roundedBox(0, 0, blockWidth, blockHeight - 1, blockRound, _RGB(255, 0, 0));
    }

    // // 如果有焦点，绘制光标
    if (editData->hasFocus)
    {
        // 绘制一个垂直线作为光标
        // uint32_t cursorrX = alignOffset;
        // if (editData->cursorPos != 0)
        // {
        //     TpString cursorLeftStr = editData->text.mid(0, editData->cursorPos);

        //     // std::cout << " cursorLeftStr " << cursorLeftStr << std::endl;

        //     cursorrX += CaculateTextWidth(editData->textFont, cursorLeftStr);
        // }

        // if (cursorLeftWidth > visibleTextWidth)
        // {
        //     cursorrX -= editData->textOffset;
        // }

        uint32_t cursorrX = leftMargin;
        if (editData->cursorPos != 0)
        {
            TpString cursorLeftStr = editData->text.mid(0, editData->cursorPos);

            // std::cout << " cursorLeftStr " << cursorLeftStr << std::endl;

            cursorrX += CaculateTextWidth(editData->textFont, cursorLeftStr);
        }

        cursorrX -= editData->textOffset;

        canvas->setPen(_RGB(0, 0, 0));
        canvas->drawVLine(cursorrX, upMargin, rect().height() - downMargin); // 黑色光标
    }

    return true;
}

bool TpLineEdit::onLeaveEvent(TpLeaveEvent *event)
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return true;

    if (editData->readOnly)
        return true;

    editData->mouseIsPress = false;

    return true;
}

bool TpLineEdit::onFocusEvent(TpFocusEvent *event)
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return true;

    if (editData->readOnly)
        return true;
    // std::cout << "TpLineEdit::onFocusEvent  " << event->focused() << std::endl;

    editData->hasFocus = event->focused();
    update();

    if (editData->hasFocus)
        TpApp::Inst()->wakeUpVirtualKeyboard(this);
    else
        TpApp::Inst()->dormantVirtualKeyboard();

    return true;
}

bool TpLineEdit::onKeyPressEvent(TpKeyboardEvent *event)
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return true;

    if (editData->readOnly)
        return true;

    // std::cout << "event->scancode() " << event->scancode() << std::endl;
    switch (event->scancode())
    {
    case TP_SCANCODE_BACKSPACE:
        dealBackSpace();
        break;
    case TP_SCANCODE_DELETE:
        if (editData->isSelected)
        {
            // 如果有选中文本，删除选中的文本
            deleteSelectedText();
        }
        else if (editData->cursorPos < editData->text.logicalLength())
        {
            // 没有选中文本，删除光标后的字符
            editData->text.remove(editData->cursorPos);
        }
        else
        {
        }
        break;
    case TP_SCANCODE_LEFT:
        if (editData->cursorPos > 0)
        {
            editData->cursorPos--;
        }
        break;
    case TP_SCANCODE_RIGHT:
        if (editData->cursorPos < editData->text.logicalLength())
        {
            editData->cursorPos++;
        }
        break;
    default:
        // 获取是否是可打印字符
        if (event->isPrintable())
        {
            // 是否按住了ctrl
            // std::cout << "keyMod " << event->keyMod() << std::endl;
            if (event->keyMod() == TP_KMOD_LCTRL || event->keyMod() == TP_KMOD_RCTRL)
            {
                // std::cout << "PressCtrl " << std::endl;
                if (event->scancode() == TP_SCANCODE_C)
                {
                    copySelectedText();
                }
                else if (event->scancode() == TP_SCANCODE_V)
                {
                    pasteText();
                }
                else
                {
                }
            }
            else
            {
                // 如果有选中文本，先删除选中的文本
                if (editData->isSelected)
                {
                    deleteSelectedText();
                }

                insertCharacter(event->symbol());
            }
        }
        break;
    }

    update();

    return true;
}

bool TpLineEdit::onMousePressEvent(TpMouseEvent *event)
{
    if (event->button() != BUTTON_LEFT)
        return true;

    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return true;

    if (editData->readOnly)
        return true;

    editData->mouseIsPress = event->state();

    // 转换鼠标坐标为光标位置
    int32_t cursorPos = mousePosToCursorPos(event->pos().x());
    editData->cursorPos = cursorPos;
    editData->selectionStart = cursorPos;
    editData->selectionEnd = cursorPos;
    editData->isSelected = false;

    update();

    return true;
}

bool TpLineEdit::onMouseRleaseEvent(TpMouseEvent *event)
{
    if (event->button() != BUTTON_LEFT)
        return true;

    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return true;

    if (editData->readOnly)
        return true;

    editData->mouseIsPress = event->state();

    return true;
}

bool TpLineEdit::onMouseMoveEvent(TpMouseEvent *event)
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return true;

    if (editData->readOnly)
        return true;

    if (editData->mouseIsPress)
    {
        // 获取输入框尺寸和边距
        tpShared<TpCssData> css = currentStatusCss();
        uint32_t leftMargin = css->paddingLeft();
        uint32_t rightMargin = css->paddingRight();
        if (editData->iconLabel->visible())
        {
            leftMargin = editData->iconOffset;
        }

        // 计算可见区域和文本总宽度
        uint32_t visibleWidth = rect().width() - leftMargin - rightMargin;
        uint32_t textWidth = CaculateTextWidth(editData->textFont, editData->text);
        uint32_t maxTextOffset = (textWidth > visibleWidth) ? (textWidth - visibleWidth) : 0;

        // 获取鼠标位置并计算是否在边缘
        int32_t mouseX = event->pos().x();
        int32_t edgeThreshold = 15; // 边缘检测阈值（像素）

        // 左边缘滚动（文本向右滚动）
        if (mouseX < leftMargin + edgeThreshold && editData->textOffset > 0)
        {
            editData->cursorPos--;
        }
        // 右边缘滚动（文本向左滚动）
        else if (mouseX > leftMargin + visibleWidth - edgeThreshold && editData->textOffset < maxTextOffset)
        {
            editData->cursorPos++;
        }

        // 更新选择结束位置
        int32_t endPos = mousePosToCursorPos(event->pos().x());
        if (endPos != editData->selectionEnd)
        {
            editData->selectionEnd = endPos;
            editData->isSelected = true;
            update();
        }
    }

    return true;
}

bool TpLineEdit::onResizeEvent(TpResizeEvent *event)
{
    caculateIconPos();

    return true;
}

void TpLineEdit::insertCharacter(const TpString &insertStr)
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return;

    editData->text.insert(editData->cursorPos, insertStr);
    editData->cursorPos += insertStr.logicalLength();

    update();
}

// 将鼠标位置转换为光标位置
int32_t TpLineEdit::mousePosToCursorPos(int32_t mouseX)
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return 0;

    uint32_t leftMargin = currentStatusCss()->paddingLeft();
    uint32_t rightMargin = currentStatusCss()->paddingRight();
    if (editData->iconLabel->visible())
    {
        leftMargin = editData->iconOffset;
    }

    // 调整鼠标X坐标以考虑文本偏移
    // mouseX = mouseX + editData->textOffset;

    uint32_t visibleTextWidth = rect().width() - leftMargin - rightMargin;
    uint32_t alignOffset = cacualteAlignOffset(visibleTextWidth);

    // 调整鼠标X坐标：减去左边距并加上文本滚动偏移
    mouseX -= alignOffset;
    mouseX += editData->textOffset;

    // 计算光标位置
    int32_t cursorPos = 0;
    int32_t currentWidth = 0;
    while (cursorPos < editData->text.logicalLength())
    {
        TpString curCharStr = editData->text.mid(cursorPos, 1);
        uint32_t charWidth = CaculateTextWidth(editData->textFont, curCharStr);

        // 判断鼠标位置是否在当前字符的中间位置之前
        if (currentWidth + charWidth / 2 > mouseX)
            break;

        currentWidth += charWidth;
        cursorPos += 1;
    }

    return cursorPos;
}

// 复制选中的文本
void TpLineEdit::copySelectedText()
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData || !editData->isSelected)
        return;

    // 获取选中的文本
    int32_t start = std::min(editData->selectionStart, editData->selectionEnd);
    int32_t end = std::max(editData->selectionStart, editData->selectionEnd);
    TpString selectedText = editData->text.mid(start, end - start);

    // 将文本复制到剪贴板
    TpClipboard::Inst()->setText(selectedText);
}

// 粘贴文本
void TpLineEdit::pasteText()
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return;

    // 从剪贴板获取文本
    TpString clipboardText;
    // TODO: 实现从剪贴板获取文本功能
    clipboardText = TpClipboard::Inst()->text();

    // 如果有选中的文本，则替换它
    if (editData->isSelected)
    {
        int32_t start = std::min(editData->selectionStart, editData->selectionEnd);
        int32_t end = std::max(editData->selectionStart, editData->selectionEnd);
        editData->text.replace(start, end - start, clipboardText);
        editData->cursorPos = start + clipboardText.logicalLength();
    }
    else
    {
        // 插入文本
        editData->text.insert(editData->cursorPos, clipboardText);
        editData->cursorPos += clipboardText.logicalLength();
    }

    // 取消选择
    editData->isSelected = false;
    editData->selectionStart = -1;
    editData->selectionEnd = -1;

    update();
}

// 删除选中的文本
void TpLineEdit::deleteSelectedText()
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData || !editData->isSelected)
        return;

    int32_t start = std::min(editData->selectionStart, editData->selectionEnd);
    int32_t end = std::max(editData->selectionStart, editData->selectionEnd);

    editData->text.remove(start, end - start);
    editData->cursorPos = start;
    editData->isSelected = false;
    editData->selectionStart = -1;
    editData->selectionEnd = -1;
}

void TpLineEdit::caculateIconPos()
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return;

    if (editData->iconLabel->visible())
    {
        tpShared<TpCssData> curCssData = currentStatusCss();

        uint32_t iconHeight = height() - curCssData->paddingTop() - curCssData->paddingBottom();

        editData->iconLabel->setSize(iconHeight, iconHeight);
        editData->iconLabel->move(curCssData->paddingLeft(), curCssData->paddingTop());

        editData->iconOffset = curCssData->paddingLeft() + iconHeight + curCssData->gap();

        editData->iconLabel->update();
    }
    else
    {
        editData->iconOffset = 0;
    }
}

void TpLineEdit::updateTextOffset()
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return;

    // 获取布局参数
    const uint32_t visibleWidth = rect().width() - editData->iconOffset - currentStatusCss()->paddingRight();
    const uint32_t textWidth = CaculateTextWidth(editData->textFont, editData->text);

    // 计算光标位置相关宽度
    const TpString leftStr = editData->text.mid(0, editData->cursorPos);
    const uint32_t cursorLeftWidth = CaculateTextWidth(editData->textFont, leftStr);
    const uint32_t cursorRightWidth = textWidth - cursorLeftWidth;

    // 动态调整偏移量
    uint32_t newOffset = editData->textOffset;

    // 右边界检测（光标即将移出可视区右侧）
    if (cursorLeftWidth - newOffset > visibleWidth - CURSOR_EDGE_THRESHOLD)
    {
        newOffset = cursorLeftWidth - visibleWidth + CURSOR_EDGE_THRESHOLD;
    }
    // 左边界检测（光标即将移出可视区左侧）
    else if (cursorLeftWidth - newOffset < CURSOR_EDGE_THRESHOLD)
    {
        newOffset = cursorLeftWidth - CURSOR_EDGE_THRESHOLD;
    }

    // 边界约束
    newOffset = std::min(newOffset, textWidth > visibleWidth ? textWidth - visibleWidth : 0);
    newOffset = std::max(newOffset, 0u);

    editData->textOffset = newOffset;
}

uint32_t TpLineEdit::cacualteAlignOffset(const uint32_t &visibleTextWidth)
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);
    if (!editData)
        return 0;

    tpShared<TpCssData> curCssData = currentStatusCss();

    uint32_t leftMargin = curCssData->paddingLeft();
    uint32_t rightMargin = curCssData->paddingRight();

    if (editData->iconLabel->visible())
    {
        leftMargin = editData->iconOffset;
    }

    uint32_t alignOffset = leftMargin;
    // 只有当文本需要滚动时才调整 textOffset
    uint32_t textWidth = CaculateTextWidth(editData->textFont, editData->text);
    if (textWidth < visibleTextWidth)
    {
        if (editData->align == Tp::AlignLeft)
        {
            alignOffset = leftMargin;
        }
        else if (editData->align == Tp::AlignRight)
        {
            alignOffset = rect().width() - rightMargin - textWidth;
        }
        else if (editData->align == Tp::AlignHCenter || editData->align == Tp::AlignCenter)
        {
            alignOffset = (rect().width() - leftMargin - rightMargin - textWidth) / 2.0;
        }
        else
        {
            alignOffset = leftMargin;
        }
    }

    return alignOffset;
}

void TpLineEdit::dealBackSpace()
{
    TpLineEditData *editData = static_cast<TpLineEditData *>(data_);

    if (editData->isSelected)
    {
        // 如果有选中文本，删除选中的文本
        deleteSelectedText();
    }
    else if (editData->cursorPos > 0)
    {
        // 没有选中文本，删除光标前的字符
        // std::cout << "before Remove " << editData->text << std::endl;
        editData->text.remove(editData->cursorPos - 1);
        // std::cout << "after Remove " << editData->text << std::endl;

        editData->cursorPos--;
    }
    else
    {
    }

    update();
}
