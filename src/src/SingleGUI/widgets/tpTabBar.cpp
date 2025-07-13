#include "tpTabBar.h"
#include "tpLabel.h"
#include "tpVector.h"
#include "tpSurface.h"
#include "tpEvent.h"
#include "tpCanvas.h"
#include "tpFont.h"

struct singleButtonData
{
    tpLabel *iconLabel = nullptr;
    tpLabel *textLabel = nullptr;

    tpShared<tpSurface> normalIcon = tpMakeShared<tpSurface>();
    tpShared<tpSurface> selectedIcon = tpMakeShared<tpSurface>();

    bool isSelected = false;

    singleButtonData()
    {
    }
    ~singleButtonData()
    {
    }
};

struct tpTabBarData
{
    tpVector<singleButtonData> buttonGroup;
};

tpTabBar::tpTabBar(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    tpTabBarData *menuData = new tpTabBarData();
    data_ = menuData;

    refreshBaseCss();

    // tpChildWidget *parentPtr = dynamic_cast<tpChildWidget *>(parent);
    // if (parentPtr)
    // {
    //     setWidth(parentPtr->width());
    // }

    setBackGroundColor(enabledCss()->backgroundColor());
    setEnabledBorderColor(false);
}

tpTabBar::~tpTabBar()
{
    tpTabBarData *menuData = static_cast<tpTabBarData *>(data_);
    if (menuData)
    {
        menuData->buttonGroup.clear();
        delete menuData;
        menuData = nullptr;
    }
}

uint32_t tpTabBar::addButton(const tpString &text)
{
    tpTabBarData *menuData = static_cast<tpTabBarData *>(data_);

    singleButtonData buttonData;
    buttonData.iconLabel = new tpLabel(this);
    buttonData.iconLabel->installEventFilter(this);
    buttonData.iconLabel->setEnableBackGroundColor(false);
    buttonData.iconLabel->setEnabledBorderColor(false);
    //  buttonData.iconLabel->setBackGroundColor(_RGB(255, 0, 0));
    //  buttonData.iconLabel->setMinimumSize(25, 25);

    buttonData.textLabel = new tpLabel(this);
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

uint32_t tpTabBar::addButton(const tpString &text, const tpString &normalIconPath, const tpString &selectIconPath)
{
    tpTabBarData *menuData = static_cast<tpTabBarData *>(data_);

    uint32_t curIndex = addButton(text);
    setIcon(curIndex, normalIconPath, selectIconPath);

    return curIndex;
}

uint32_t tpTabBar::insertButton(const uint32_t &index, const tpString &text)
{
    tpTabBarData *menuData = static_cast<tpTabBarData *>(data_);

    singleButtonData buttonData;
    buttonData.iconLabel = new tpLabel(this);
    buttonData.iconLabel->installEventFilter(this);
    buttonData.iconLabel->setEnableBackGroundColor(false);
    buttonData.iconLabel->setEnabledBorderColor(false);
    //  buttonData.iconLabel->setMinimumSize(25, 25);

    buttonData.textLabel = new tpLabel(this);
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

void tpTabBar::setIcon(const uint32_t &buttonIndex, const tpString &normalIconPath, const tpString &selectIconPath)
{
    tpTabBarData *menuData = static_cast<tpTabBarData *>(data_);

    if (buttonIndex >= menuData->buttonGroup.size())
        return;

    singleButtonData &findData = menuData->buttonGroup[buttonIndex];

    if (!normalIconPath.empty())
        findData.normalIcon->fromFile(normalIconPath);

    if (!selectIconPath.empty())
        findData.selectedIcon->fromFile(selectIconPath);
}

tpString tpTabBar::tabText(int32_t index) const
{
    tpTabBarData *menuData = static_cast<tpTabBarData *>(data_);
    if (index >= menuData->buttonGroup.size())
        return "";

    return menuData->buttonGroup.at(index).textLabel->text();
}

void tpTabBar::setTabText(int32_t index, const tpString &text)
{
    tpTabBarData *menuData = static_cast<tpTabBarData *>(data_);
    if (index >= menuData->buttonGroup.size())
        return;

    menuData->buttonGroup.at(index).textLabel->setText(text);
}

void tpTabBar::setCurrentIndex(const uint32_t &buttonIndex)
{
    tpTabBarData *menuData = static_cast<tpTabBarData *>(data_);

    for (int i = 0; i < menuData->buttonGroup.size(); ++i)
    {
        singleButtonData &curButtonData = menuData->buttonGroup[i];
        if (i == buttonIndex)
            curButtonData.isSelected = true;
        else
            curButtonData.isSelected = false;
    }

    update();
}

uint32_t tpTabBar::currendIndex()
{
    tpTabBarData *menuData = static_cast<tpTabBarData *>(data_);

    for (int i = 0; i < menuData->buttonGroup.size(); ++i)
    {
        const singleButtonData &curData = menuData->buttonGroup[i];
        if (curData.isSelected)
            return i;
    }

    return 0;
}

void tpTabBar::removeButton(const uint32_t &index)
{
    tpTabBarData *menuData = static_cast<tpTabBarData *>(data_);

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

uint32_t tpTabBar::count()
{
    tpTabBarData *menuData = static_cast<tpTabBarData *>(data_);
    return menuData->buttonGroup.size();
}

void tpTabBar::clear()
{
    tpTabBarData *menuData = static_cast<tpTabBarData *>(data_);
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

bool tpTabBar::onMousePressEvent(tpMouseEvent *event)
{
    uint32_t button = event->button();

    if (button != BUTTON_LEFT)
        return true;

    // 鼠标按下，根据当前点击坐标，计算是否切换当前选中的按钮
    uint32_t curPressX = event->globalPos().x;

    tpTabBarData *menuData = static_cast<tpTabBarData *>(data_);
    double singleButtonWidth = 1.0 * width() / menuData->buttonGroup.size();

    uint32_t pressIndex = curPressX / singleButtonWidth;
    uint32_t pressButtonId = 0;

    bool isUpdate = false;
    for (int i = 0; i < menuData->buttonGroup.size(); ++i)
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

bool tpTabBar::onResizeEvent(tpObjectResizeEvent *event)
{
    // tpChildWidget *parentPtr = dynamic_cast<tpChildWidget *>(parent());

    // if (parentPtr)
    // {
    //     setWidth(parentPtr->width());
    // }

    caculateButtonSize();

    return true;
}

bool tpTabBar::onPaintEvent(tpObjectPaintEvent *event)
{
    tpChildWidget::onPaintEvent(event);

    tpTabBarData *menuData = static_cast<tpTabBarData *>(data_);

    tpShared<tpCssData> normalCss = enabledCss();
    tpShared<tpCssData> focusCss = checkedCss();
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

bool tpTabBar::eventFilter(tpObject *watched, tpEvent *event)
{
    if (event->eventType() == tpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        tpMouseEvent *mouseEvent = dynamic_cast<tpMouseEvent *>(event);
        onMousePressEvent(mouseEvent);
    }

    return false;
}

void tpTabBar::caculateButtonSize()
{
    tpTabBarData *menuData = static_cast<tpTabBarData *>(data_);
    if (menuData->buttonGroup.size() == 0)
        return;

    tpShared<tpCssData> normalCss = enabledCss();

    // 一个按钮占的宽度
    uint32_t singleButtonWidth = width() / menuData->buttonGroup.size();

    // 计算按钮的宽度
    uint32_t iconWidthHeight = height() - normalCss->paddingTop() - normalCss->paddingBottom() - normalCss->gap() - normalCss->fontSize();

    uint32_t buttonX = (singleButtonWidth - iconWidthHeight) / 2.0;

    // 调整所有按钮布局
    for (int i = 0; i < menuData->buttonGroup.size(); ++i)
    {
        singleButtonData curButtonData = menuData->buttonGroup.at(i);

        curButtonData.iconLabel->setSize(iconWidthHeight, iconWidthHeight);
        curButtonData.textLabel->setSize(iconWidthHeight, curButtonData.textLabel->font()->pixelHeight());

        if (curButtonData.normalIcon->hasSurface())
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
