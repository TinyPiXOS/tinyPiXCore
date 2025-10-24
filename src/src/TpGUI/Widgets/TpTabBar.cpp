#include "TpTabBar.h"
#include "TpLabel.h"
#include "TpVector.h"
#include "TpImage.h"
#include "TpEvent.h"
#include "TpPainter.h"
#include "TpFont.h"

struct singleButtonData
{
    TpLabel *iconLabel = nullptr;
    TpLabel *textLabel = nullptr;

    TpImage normalIcon;
    TpImage selectedIcon;

    bool isSelected = false;

    singleButtonData()
    {
    }
    ~singleButtonData()
    {
    }
};

struct TpTabBarData
{
    TpVector<singleButtonData> buttonGroup;
};

TpTabBar::TpTabBar(TpWidget *parent)
    : TpWidget(parent)
{
    TpTabBarData *menuData = new TpTabBarData();
    data_ = menuData;

    refreshBaseCss();

    // TpWidget *parentPtr = dynamic_cast<TpWidget *>(parent);
    // if (parentPtr)
    // {
    //     setWidth(parentPtr->width());
    // }

    setBackGroundColor(enabledCss()->backgroundColor());
    setEnabledBorderColor(false);
}

TpTabBar::~TpTabBar()
{
    TpTabBarData *menuData = static_cast<TpTabBarData *>(data_);
    if (menuData)
    {
        menuData->buttonGroup.clear();
        delete menuData;
        menuData = nullptr;
    }
}

uint32_t TpTabBar::addButton(const TpString &text)
{
    TpTabBarData *menuData = static_cast<TpTabBarData *>(data_);

    singleButtonData buttonData;
    buttonData.iconLabel = new TpLabel(this);
    buttonData.iconLabel->installEventFilter(this);
    buttonData.iconLabel->setEnableBackGroundColor(false);
    buttonData.iconLabel->setEnabledBorderColor(false);
    //  buttonData.iconLabel->setBackGroundColor(_RGB(255, 0, 0));
    //  buttonData.iconLabel->setMinimumSize(25, 25);

    buttonData.textLabel = new TpLabel(this);
    buttonData.textLabel->installEventFilter(this);
    buttonData.textLabel->setEnableBackGroundColor(false);
    buttonData.textLabel->setEnabledBorderColor(false);

    buttonData.textLabel->setText(text);

    if (menuData->buttonGroup.size() == 0)
    {
        buttonData.isSelected = true;
        // indexChanged.emit(menuData->buttonGroup.size());
    }

    menuData->buttonGroup.emplace_back(buttonData);

    // 添加按钮后重新计算布局
    caculateButtonSize();

    return (menuData->buttonGroup.size() - 1);
}

uint32_t TpTabBar::addButton(const TpString &text, const TpString &normalIconPath, const TpString &selectIconPath)
{
    TpTabBarData *menuData = static_cast<TpTabBarData *>(data_);

    uint32_t curIndex = addButton(text);
    setIcon(curIndex, normalIconPath, selectIconPath);

    return curIndex;
}

uint32_t TpTabBar::insertButton(const uint32_t &index, const TpString &text)
{
    TpTabBarData *menuData = static_cast<TpTabBarData *>(data_);

    singleButtonData buttonData;
    buttonData.iconLabel = new TpLabel(this);
    buttonData.iconLabel->installEventFilter(this);
    buttonData.iconLabel->setEnableBackGroundColor(false);
    buttonData.iconLabel->setEnabledBorderColor(false);
    //  buttonData.iconLabel->setMinimumSize(25, 25);

    buttonData.textLabel = new TpLabel(this);
    buttonData.textLabel->installEventFilter(this);
    buttonData.textLabel->setEnableBackGroundColor(false);
    buttonData.textLabel->setEnabledBorderColor(false);

    buttonData.textLabel->setText(text);

    if (menuData->buttonGroup.size() == 0)
    {
        buttonData.isSelected = true;
        // indexChanged.emit(index);
    }

    menuData->buttonGroup.insertData(index, buttonData);

    // 添加按钮后重新计算布局
    caculateButtonSize();

    return index;
}

void TpTabBar::setIcon(const uint32_t &buttonIndex, const TpString &normalIconPath, const TpString &selectIconPath)
{
    TpTabBarData *menuData = static_cast<TpTabBarData *>(data_);

    if (buttonIndex >= menuData->buttonGroup.size())
        return;

    singleButtonData &findData = menuData->buttonGroup[buttonIndex];

    if (!normalIconPath.empty())
        findData.normalIcon.load(normalIconPath);

    if (!selectIconPath.empty())
        findData.selectedIcon.load(selectIconPath);
}

TpString TpTabBar::tabText(int32_t index) const
{
    TpTabBarData *menuData = static_cast<TpTabBarData *>(data_);
    if (index >= menuData->buttonGroup.size())
        return "";

    return menuData->buttonGroup.at(index).textLabel->text();
}

void TpTabBar::setTabText(int32_t index, const TpString &text)
{
    TpTabBarData *menuData = static_cast<TpTabBarData *>(data_);
    if (index >= menuData->buttonGroup.size())
        return;

    menuData->buttonGroup.at(index).textLabel->setText(text);
}

void TpTabBar::setCurrentIndex(const uint32_t &buttonIndex)
{
    TpTabBarData *menuData = static_cast<TpTabBarData *>(data_);

    for (int32_t i = 0; i < menuData->buttonGroup.size(); ++i)
    {
        singleButtonData &curButtonData = menuData->buttonGroup[i];
        if (i == buttonIndex)
            curButtonData.isSelected = true;
        else
            curButtonData.isSelected = false;
    }

    update();
}

uint32_t TpTabBar::currendIndex()
{
    TpTabBarData *menuData = static_cast<TpTabBarData *>(data_);

    for (int32_t i = 0; i < menuData->buttonGroup.size(); ++i)
    {
        const singleButtonData &curData = menuData->buttonGroup[i];
        if (curData.isSelected)
            return i;
    }

    return 0;
}

void TpTabBar::removeButton(const uint32_t &index)
{
    TpTabBarData *menuData = static_cast<TpTabBarData *>(data_);

    if (index >= menuData->buttonGroup.size())
        return;

    singleButtonData curData = menuData->buttonGroup[index];
    menuData->buttonGroup.remove(index);

    if (curData.iconLabel)
    {
        curData.iconLabel->setParent(nullptr);
        delete curData.iconLabel;
        curData.iconLabel = nullptr;
    }
    if (curData.textLabel)
    {
        curData.textLabel->setParent(nullptr);
        delete curData.textLabel;
        curData.textLabel = nullptr;
    }

    // 删除按钮后重新计算布局
    caculateButtonSize();
}

uint32_t TpTabBar::count()
{
    TpTabBarData *menuData = static_cast<TpTabBarData *>(data_);
    return menuData->buttonGroup.size();
}

void TpTabBar::clear()
{
    TpTabBarData *menuData = static_cast<TpTabBarData *>(data_);
    for (auto &data : menuData->buttonGroup)
    {
        if (data.textLabel)
        {
            data.textLabel->setParent(nullptr);
            delete data.textLabel;
            data.textLabel = nullptr;
        }

        if (data.iconLabel)
        {
            data.iconLabel->setParent(nullptr);
            delete data.iconLabel;
            data.iconLabel = nullptr;
        }
    }

    menuData->buttonGroup.clear();
}

bool TpTabBar::onMousePressEvent(TpMouseEvent *event)
{
    uint32_t button = event->button();

    if (button != BUTTON_LEFT)
        return true;

    // 鼠标按下，根据当前点击坐标，计算是否切换当前选中的按钮
    uint32_t curPressX = event->globalPos().x();

    TpTabBarData *menuData = static_cast<TpTabBarData *>(data_);
    double singleButtonWidth = 1.0 * width() / menuData->buttonGroup.size();

    uint32_t pressIndex = curPressX / singleButtonWidth;
    uint32_t pressButtonId = 0;

    bool isUpdate = false;
    for (int32_t i = 0; i < menuData->buttonGroup.size(); ++i)
    {
        auto &curButtonData = menuData->buttonGroup[i];
        if (i == pressIndex)
        {
            // 如果该index本来就是选中的，不在重复刷新
            if (curButtonData.isSelected == true)
                break;

            isUpdate = true;
            pressButtonId = i;
            curButtonData.isSelected = true;
        }
        else
        {
            curButtonData.isSelected = false;
        }
    }

    if (isUpdate)
    {
        update();
        indexChanged.emit(pressButtonId);
    }

    return true;
}

bool TpTabBar::onResizeEvent(TpResizeEvent *event)
{
    // TpWidget *parentPtr = dynamic_cast<TpWidget *>(parent());

    // if (parentPtr)
    // {
    //     setWidth(parentPtr->width());
    // }

    caculateButtonSize();

    return true;
}

bool TpTabBar::onPaintEvent(TpPaintEvent *event)
{
    TpWidget::onPaintEvent(event);

    TpTabBarData *menuData = static_cast<TpTabBarData *>(data_);

    tpShared<TpCssData> normalCss = enabledCss();
    tpShared<TpCssData> focusCss = checkedCss();
    for (const auto &buttonData : menuData->buttonGroup)
    {
        buttonData.textLabel->font()->setFontSize(normalCss->fontSize());

        if (buttonData.isSelected)
        {
            buttonData.textLabel->font()->setFontColor(focusCss->color(), focusCss->color());
            buttonData.iconLabel->setBackGroundImage(buttonData.selectedIcon);
        }
        else
        {
            buttonData.textLabel->font()->setFontColor(normalCss->color(), normalCss->color());
            buttonData.iconLabel->setBackGroundImage(buttonData.normalIcon);
        }
    }

    return true;
}

bool TpTabBar::eventFilter(TpObject *watched, TpEvent *event)
{
    if (event->eventType() == TpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        onMousePressEvent(mouseEvent);
    }

    return false;
}

void TpTabBar::caculateButtonSize()
{
    TpTabBarData *menuData = static_cast<TpTabBarData *>(data_);
    if (menuData->buttonGroup.size() == 0)
        return;

    tpShared<TpCssData> normalCss = enabledCss();

    // 一个按钮占的宽度
    uint32_t singleButtonWidth = width() / menuData->buttonGroup.size();

    // 计算按钮的宽度
    uint32_t iconWidthHeight = height() - normalCss->paddingTop() - normalCss->paddingBottom() - normalCss->gap() - normalCss->fontSize();

    uint32_t buttonX = (singleButtonWidth - iconWidthHeight) / 2.0;

    // 调整所有按钮布局
    for (int32_t i = 0; i < menuData->buttonGroup.size(); ++i)
    {
        singleButtonData curButtonData = menuData->buttonGroup.at(i);

        curButtonData.iconLabel->setSize(iconWidthHeight, iconWidthHeight);
        curButtonData.textLabel->setSize(iconWidthHeight, curButtonData.textLabel->font()->pixelHeight());

        if (!curButtonData.normalIcon.isNull())
        {
            curButtonData.iconLabel->setVisible(true);
            curButtonData.iconLabel->move(buttonX + i * singleButtonWidth, normalCss->paddingTop());
            curButtonData.textLabel->move(buttonX + i * singleButtonWidth, normalCss->paddingTop() + iconWidthHeight + normalCss->gap());
        }
        else
        {
            curButtonData.iconLabel->setVisible(false);
            curButtonData.textLabel->setHeight(height());
            curButtonData.textLabel->move(buttonX + i * singleButtonWidth, normalCss->paddingTop());
        }
    }
}
