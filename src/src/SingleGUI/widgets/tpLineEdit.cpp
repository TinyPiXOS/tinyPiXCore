#include "tpLineEdit.h"
#include "tpCanvas.h"
#include "tpEvent.h"
#include "tpTimer.h"
#include "tpClipboard.h"
#include "tpLabel.h"
#include "tpSurface.h"
#include "tpApp.h"
#include "tpVirtualKeyboard.h"

static const int CURSOR_EDGE_THRESHOLD = 3; // 光标边缘触发阈值(像素)

static uint32_t CaculateTextWidth(tpFont *font, const tpString &text)
{
    if (text.empty())
        return 0;

    font->setText(text);

    // std::cout << "Caculate Text : " << text << "  Width : " << font->layoutWidth() << std::endl;
    uint32_t textWidth = 0;
    for (int i = 0; i < text.logicalLength(); ++i)
    {
        tpString curCharStr = text.mid(i, 1);
        font->setText(curCharStr);
        textWidth += font->layoutWidth();

        // std::cout << "CurChar : " << curCharStr << "  Width : " << font->layoutWidth() << std::endl;
    }

    return textWidth;
}

struct tpLineEditData
{
    // icon的显示label
    tpLabel *iconLabel = nullptr;

    // 有icon时，文本要叠加偏移量
    uint32_t iconOffset = 0;

    // 人输入框文本
    tpString text;

    // 提示文本
    tpString placeholerText;

    // 当前光标偏移量
    int cursorPos;
    bool hasFocus;  // 是否有焦点
    int textOffset; // 文本偏移量，用于处理滚动

    // 选择起始/终止索引
    int selectionStart;
    int selectionEnd;
    // 是否选择
    bool isSelected;

    tpFont *textFont;

    tpTimer *updateCursor;

    // 鼠标左键是否按下
    bool mouseIsPress;

    tinyPiX::AlignmentFlag align = tinyPiX::AlignLeft;

    bool readOnly = false;

    tpLineEditData() : text(""), cursorPos(0), hasFocus(false), textOffset(0), selectionStart(-1), selectionEnd(-1), isSelected(false), mouseIsPress(false)
    {
        textFont = new tpFont();
        textFont->setFontForeColor(_RGB(0, 0, 0));
        textFont->setFontSize(15);

        updateCursor = new tpTimer(800);
    }

    ~tpLineEditData()
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

tpLineEdit::tpLineEdit(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    data_ = new tpLineEditData();
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);

    editData->iconLabel = new tpLabel(this);
    editData->iconLabel->setVisible(false);

    // connect(editData->updateCursor, timeout, [=]()
    // {
    //     editData->
    //      update();
    //      });

    refreshBaseCss();
}

tpLineEdit::~tpLineEdit()
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (editData)
    {
        delete editData;
        editData = nullptr;
        data_ = nullptr;
    }
}

tpFont *tpLineEdit::font()
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    return editData->textFont;
}

void tpLineEdit::setText(const tpString &text)
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData)
        return;

    editData->text = text;
    update();
}

tpString tpLineEdit::text()
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData)
        return "";

    return editData->text;
}

void tpLineEdit::setIcon(const tpString &iconPath)
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData)
        return;

    if (iconPath.empty())
    {
        editData->iconLabel->setVisible(false);
        editData->iconOffset = 0;
        return;
    }

    tpShared<tpSurface> iconSurface = tpMakeShared<tpSurface>();
    iconSurface->fromFile(iconPath);
    editData->iconLabel->setBackGroundImage(iconSurface);

    editData->iconLabel->setVisible(true);
    caculateIconPos();

    update();
}

void tpLineEdit::clear()
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData)
        return;

    editData->text = "";
    editData->textFont->setText(editData->text);
    editData->isSelected = false;

    update();
}

void tpLineEdit::setPlaceholderText(const tpString &text)
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData)
        return;

    editData->placeholerText = text;
}

tpString tpLineEdit::placeholderText()
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData)
        return "";

    return editData->placeholerText;
}

void tpLineEdit::setAlign(const tinyPiX::AlignmentFlag align)
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData)
        return;
    editData->align = align;
    update();
}

void tpLineEdit::setReadOnly(const bool &readOnly)
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    editData->readOnly = readOnly;
}

void tpLineEdit::virtualKeyboardInput(const tinyPiX::VirtualKeyboardInputType &type, const tpString &text)
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (editData->readOnly)
        return;

    if (type == tinyPiX::Delete)
    {
        dealBackSpace();
    }
    else if (type == tinyPiX::Pinyin)
    {
    }
    else if (type == tinyPiX::Chinese || type == tinyPiX::Symbol)
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

bool tpLineEdit::onPaintEvent(tpObjectPaintEvent *event)
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData)
        return true;

    tpCanvas *canvas = event->canvas();
    if (!canvas)
        return true;

    // 父类绘制背景
    tpChildWidget::onPaintEvent(event);

    tpShared<tpCssData> curCssData = currentStatusCss();
    editData->textFont->setFontColor(curCssData->color(), curCssData->color());
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
    uint32_t visibleTextWidth = rect().w - leftMargin - rightMargin;

    // 计算文本偏移
    tpString cursorLeftStr = editData->text.mid(0, editData->cursorPos);
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
        int start = std::min(editData->selectionStart, editData->selectionEnd);
        int end = std::max(editData->selectionStart, editData->selectionEnd);
        tpString selectedText = editData->text.mid(start, end - start);
        uint32_t selectionWidth = CaculateTextWidth(editData->textFont, selectedText);
        uint32_t selectionX = leftMargin + CaculateTextWidth(editData->textFont, editData->text.mid(0, start)) - editData->textOffset;
        uint32_t selectionY = (rect().h - editData->textFont->pixelHeight()) / 2;
        canvas->box(selectionX, selectionY, selectionX + selectionWidth, selectionY + editData->textFont->pixelHeight(), _RGB(0, 120, 215)); // 蓝色背景表示选中
    }

    // 绘制文本
    if (!editData->text.empty())
    {
        editData->textFont->setText(editData->text);
        uint32_t textY = (rect().h - editData->textFont->pixelHeight()) / 2;

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
        canvas->renderText(*editData->textFont, textX, textY, editData->text);
    }
    else
    {
        // 文本为空时绘制提示文本
        tpFont placeholderTextFont;
        placeholderTextFont.setFontColor(_RGB(191, 191, 191), _RGB(191, 191, 191));
        placeholderTextFont.setFontSize(curCssData->fontSize());

        placeholderTextFont.setText(editData->placeholerText);

        uint32_t textY = (rect().h - placeholderTextFont.pixelHeight()) / 2;

        uint32_t placeTextX = leftMargin;
        uint32_t placeholderTextWidth = CaculateTextWidth(&placeholderTextFont, editData->placeholerText);
        if (editData->align == tinyPiX::AlignLeft)
        {
            placeTextX = leftMargin;
        }
        else if (editData->align == tinyPiX::AlignRight)
        {
            placeTextX = rect().w - rightMargin - placeholderTextWidth;
        }
        else if (editData->align == tinyPiX::AlignHCenter || editData->align == tinyPiX::AlignCenter)
        {
            placeTextX = (rect().w - leftMargin - rightMargin - placeholderTextWidth) / 2.0;
        }
        else
        {
            placeTextX = leftMargin;
        }

        canvas->renderText(placeholderTextFont, placeTextX, textY, editData->placeholerText);
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

        canvas->roundedBox(0, 0, blockWidth, blockHeight - 1, blockRound, curCssData->backgroundColor());
        // canvas->roundedBox(0, 0, blockWidth, blockHeight - 1, blockRound, _RGB(255, 0, 0));
    }

    // // 如果有焦点，绘制光标
    if (editData->hasFocus)
    {
        // 绘制一个垂直线作为光标
        // uint32_t cursorrX = alignOffset;
        // if (editData->cursorPos != 0)
        // {
        //     tpString cursorLeftStr = editData->text.mid(0, editData->cursorPos);

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
            tpString cursorLeftStr = editData->text.mid(0, editData->cursorPos);

            // std::cout << " cursorLeftStr " << cursorLeftStr << std::endl;

            cursorrX += CaculateTextWidth(editData->textFont, cursorLeftStr);
        }

        cursorrX -= editData->textOffset;

        canvas->vline(cursorrX, upMargin, rect().h - downMargin, _RGB(0, 0, 0)); // 黑色光标
    }

    return true;
}

bool tpLineEdit::onLeaveEvent(tpObjectLeaveEvent *event)
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData)
        return true;

    if (editData->readOnly)
        return true;

    editData->mouseIsPress = false;

    return true;
}

bool tpLineEdit::onFocusEvent(tpObjectFocusEvent *event)
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData)
        return true;

    if (editData->readOnly)
        return true;
    // std::cout << "tpLineEdit::onFocusEvent  " << event->focused() << std::endl;

    editData->hasFocus = event->focused();
    update();

    if (editData->hasFocus)
        tpApp::Inst()->wakeUpVirtualKeyboard(this);
    else
        tpApp::Inst()->dormantVirtualKeyboard();

    return true;
}

bool tpLineEdit::onKeyPressEvent(tpKeyboardEvent *event)
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
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

bool tpLineEdit::onMousePressEvent(tpMouseEvent *event)
{
    if (event->button() != BUTTON_LEFT)
        return true;

    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData)
        return true;

    if (editData->readOnly)
        return true;

    editData->mouseIsPress = event->state();

    // 转换鼠标坐标为光标位置
    int cursorPos = mousePosToCursorPos(event->pos().x);
    editData->cursorPos = cursorPos;
    editData->selectionStart = cursorPos;
    editData->selectionEnd = cursorPos;
    editData->isSelected = false;

    update();

    return true;
}

bool tpLineEdit::onMouseRleaseEvent(tpMouseEvent *event)
{
    if (event->button() != BUTTON_LEFT)
        return true;

    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData)
        return true;

    if (editData->readOnly)
        return true;

    editData->mouseIsPress = event->state();

    return true;
}

bool tpLineEdit::onMouseMoveEvent(tpMouseEvent *event)
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData)
        return true;

    if (editData->readOnly)
        return true;

    if (editData->mouseIsPress)
    {
        // 获取输入框尺寸和边距
        tpShared<tpCssData> css = currentStatusCss();
        uint32_t leftMargin = css->paddingLeft();
        uint32_t rightMargin = css->paddingRight();
        if (editData->iconLabel->visible())
        {
            leftMargin = editData->iconOffset;
        }

        // 计算可见区域和文本总宽度
        uint32_t visibleWidth = rect().w - leftMargin - rightMargin;
        uint32_t textWidth = CaculateTextWidth(editData->textFont, editData->text);
        uint32_t maxTextOffset = (textWidth > visibleWidth) ? (textWidth - visibleWidth) : 0;

        // 获取鼠标位置并计算是否在边缘
        int mouseX = event->pos().x;
        int edgeThreshold = 15; // 边缘检测阈值（像素）

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
        int endPos = mousePosToCursorPos(event->pos().x);
        if (endPos != editData->selectionEnd)
        {
            editData->selectionEnd = endPos;
            editData->isSelected = true;
            update();
        }
    }

    return true;
}

bool tpLineEdit::onResizeEvent(tpObjectResizeEvent *event)
{
    caculateIconPos();

    return true;
}

void tpLineEdit::insertCharacter(const tpString &insertStr)
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData)
        return;

    editData->text.insert(editData->cursorPos, insertStr);
    editData->cursorPos += insertStr.logicalLength();

    update();
}

// 将鼠标位置转换为光标位置
int tpLineEdit::mousePosToCursorPos(int mouseX)
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
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

    uint32_t visibleTextWidth = rect().w - leftMargin - rightMargin;
    uint32_t alignOffset = cacualteAlignOffset(visibleTextWidth);

    // 调整鼠标X坐标：减去左边距并加上文本滚动偏移
    mouseX -= alignOffset;
    mouseX += editData->textOffset;

    // 计算光标位置
    int cursorPos = 0;
    int currentWidth = 0;
    while (cursorPos < editData->text.logicalLength())
    {
        tpString curCharStr = editData->text.mid(cursorPos, 1);
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
void tpLineEdit::copySelectedText()
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData || !editData->isSelected)
        return;

    // 获取选中的文本
    int start = std::min(editData->selectionStart, editData->selectionEnd);
    int end = std::max(editData->selectionStart, editData->selectionEnd);
    tpString selectedText = editData->text.mid(start, end - start);

    // 将文本复制到剪贴板
    tpClipboard::Inst()->setText(selectedText);
}

// 粘贴文本
void tpLineEdit::pasteText()
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData)
        return;

    // 从剪贴板获取文本
    tpString clipboardText;
    // TODO: 实现从剪贴板获取文本功能
    clipboardText = tpClipboard::Inst()->text();

    // 如果有选中的文本，则替换它
    if (editData->isSelected)
    {
        int start = std::min(editData->selectionStart, editData->selectionEnd);
        int end = std::max(editData->selectionStart, editData->selectionEnd);
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
void tpLineEdit::deleteSelectedText()
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData || !editData->isSelected)
        return;

    int start = std::min(editData->selectionStart, editData->selectionEnd);
    int end = std::max(editData->selectionStart, editData->selectionEnd);

    editData->text.remove(start, end - start);
    editData->cursorPos = start;
    editData->isSelected = false;
    editData->selectionStart = -1;
    editData->selectionEnd = -1;
}

void tpLineEdit::caculateIconPos()
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData)
        return;

    if (editData->iconLabel->visible())
    {
        tpShared<tpCssData> curCssData = currentStatusCss();

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

void tpLineEdit::updateTextOffset()
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData)
        return;

    // 获取布局参数
    const uint32_t visibleWidth = rect().w - editData->iconOffset - currentStatusCss()->paddingRight();
    const uint32_t textWidth = CaculateTextWidth(editData->textFont, editData->text);

    // 计算光标位置相关宽度
    const tpString leftStr = editData->text.mid(0, editData->cursorPos);
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

uint32_t tpLineEdit::cacualteAlignOffset(const uint32_t &visibleTextWidth)
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);
    if (!editData)
        return 0;

    tpShared<tpCssData> curCssData = currentStatusCss();

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
        if (editData->align == tinyPiX::AlignLeft)
        {
            alignOffset = leftMargin;
        }
        else if (editData->align == tinyPiX::AlignRight)
        {
            alignOffset = rect().w - rightMargin - textWidth;
        }
        else if (editData->align == tinyPiX::AlignHCenter || editData->align == tinyPiX::AlignCenter)
        {
            alignOffset = (rect().w - leftMargin - rightMargin - textWidth) / 2.0;
        }
        else
        {
            alignOffset = leftMargin;
        }
    }

    return alignOffset;
}

void tpLineEdit::dealBackSpace()
{
    tpLineEditData *editData = static_cast<tpLineEditData *>(data_);

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
