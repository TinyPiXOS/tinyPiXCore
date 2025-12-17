#include "TpMenu.h"
#include "TpEvent.h"
#include "TpPainter.h"
#include "TpDef.h"
#include "TpImage.h"
#include "TpFont.h"
#include "TpApp.h"

struct MenuItemData
{
    uint32_t id;
    TpString text;
    TpImage iconImage;
};

struct TpMenuData
{
    TpVector<MenuItemData> itemList;

    TpFont *font = new TpFont();

    // ID索引，使用后+1
    uint32_t idIndex = 0;

    uint32_t itemHeight = 0;
    uint32_t itemWidth = 0;

    bool isHover;
    uint32_t hoverIndex = 0;

    int32_t itemHoverColor = _RGB(247, 237, 255);

    TpMenuData()
    {
    }

    ~TpMenuData()
    {
        delete font;
        font = nullptr;
    }
};

TpMenu::TpMenu() : TpDialog(), data_(nullptr)
{
    TpMenuData *menuData = new TpMenuData();
    data_ = menuData;

    refreshBaseCss();

    tpShared<TpCssData> normalCss = currentStatusCss();

    menuData->font->setText("TpMenu");
    menuData->font->setFontSize(normalCss->fontSize());
    menuData->font->setFontColor(normalCss->color());

    menuData->itemHeight = menuData->font->pixelHeight() + normalCss->paddingTop() + normalCss->paddingBottom();

    setVisible(false);

    setEnabledBorderColor(false);

    // setFixedSize(150, 150);
    // setBackGroundColor(_RGB(255, 0, 0));
}

TpMenu::~TpMenu()
{
    TpMenuData *menuData = static_cast<TpMenuData *>(data_);

    if (menuData)
    {
        delete menuData;
        menuData = nullptr;
        data_ = nullptr;
    }
}

void TpMenu::exec(const TpPoint &globalPos)
{
    exec(globalPos.x(), globalPos.y());
}

void TpMenu::exec(const int32_t &globalPosX, const int32_t &globalPosY)
{
    TpMenuData *menuData = static_cast<TpMenuData *>(data_);
    if (!menuData)
        return;

    TpDialog::exec();

    uint32_t menuHeight = menuData->itemList.size() * (menuData->itemHeight);
    setSize(menuData->itemWidth, menuHeight);

    int32_t mainWindowWidth = TpApp::Inst()->mainWindow()->width();
    int32_t mainWindowHeight = TpApp::Inst()->mainWindow()->height();

    int32_t posX = globalPosX;
    int32_t posY = globalPosY;

    if ((posX + width()) > mainWindowWidth)
    {
        posX = posX - width();
    }

    if ((posY + height()) > mainWindowHeight)
    {
        posY = posY - height();
    }

    move(posX, posY);
    setVisible(true);
    update();
}

uint32_t TpMenu::addItem(const TpString &text)
{
    TpMenuData *menuData = static_cast<TpMenuData *>(data_);

    MenuItemData itemData;
    itemData.id = menuData->idIndex++;
    itemData.text = text;

    menuData->itemList.emplace_back(itemData);

    // 计算item的最大宽度
    tpShared<TpCssData> normalCss = currentStatusCss();
    TpFont tmpFont;
    tmpFont.setText(text);
    tmpFont.setFontSize(normalCss->fontSize());

    uint32_t curItemWidth = tmpFont.pixelWidth() + normalCss->paddingLeft() + normalCss->paddingRight();
    if (curItemWidth > menuData->itemWidth)
        menuData->itemWidth = curItemWidth;

    uint32_t menuHeight = menuData->itemList.size() * (menuData->itemHeight);
    setHeight(menuHeight);
    update();

    return itemData.id;
}

uint32_t TpMenu::addItem(const TpString &text, const TpString &iconPath)
{
    TpMenuData *menuData = static_cast<TpMenuData *>(data_);

    MenuItemData itemData;
    itemData.id = menuData->idIndex++;
    itemData.text = text;

    itemData.iconImage.load(iconPath);

    menuData->itemList.emplace_back(itemData);

    // 计算item的最大宽度
    tpShared<TpCssData> normalCss = currentStatusCss();
    TpFont tmpFont;
    tmpFont.setText(text);
    tmpFont.setFontSize(normalCss->fontSize());

    // 要叠加icon的大小和间距
    uint32_t curItemWidth = normalCss->fontSize() + normalCss->gap() + tmpFont.pixelWidth() + normalCss->paddingLeft() + normalCss->paddingRight();
    if (curItemWidth > menuData->itemWidth)
        menuData->itemWidth = curItemWidth;

    update();

    return itemData.id;
}

void TpMenu::addMenu(const TpString &menuName, TpMenu *menu)
{
}

TpMenu *TpMenu::addMenu(const TpString &menuName)
{
    return nullptr;
}

uint32_t TpMenu::itemHeight()
{
    TpMenuData *menuData = static_cast<TpMenuData *>(data_);

    return menuData->itemHeight;
}

void TpMenu::setItemHeight(const uint32_t &height)
{
    TpMenuData *menuData = static_cast<TpMenuData *>(data_);
    menuData->itemHeight = height;

    uint32_t menuHeight = menuData->itemList.size() * (menuData->itemHeight);
    setHeight(menuHeight);
    update();
}

void TpMenu::setItemHoverColor(const int32_t &color)
{
    TpMenuData *menuData = static_cast<TpMenuData *>(data_);
    menuData->itemHoverColor = color;
}

void TpMenu::clear()
{
    TpMenuData *menuData = static_cast<TpMenuData *>(data_);
    menuData->itemList.clear();

    // ID索引，使用后+1
    menuData->idIndex = 0;
    menuData->itemWidth = 0;
}

TpString TpMenu::itemText(const uint32_t &index)
{
    TpMenuData *menuData = static_cast<TpMenuData *>(data_);

    if (index >= menuData->itemList.size())
        return "";

    return menuData->itemList.at(index).text;
}

bool TpMenu::onFocusEvent(TpFocusEvent *event)
{
    // std::cout << "TpMenu::onFocusEvent " << std::endl;
    // if (!event->focused())
    // {
    // 	close();
    // }

    return true;
}

bool TpMenu::onMouseRleaseEvent(TpMouseEvent *event)
{
    if (event->button() != BUTTON_LEFT)
        return true;

    TpMenuData *menuData = static_cast<TpMenuData *>(data_);

    int32_t clickY = event->pos().y();
    uint32_t clickIndex = clickY / menuData->itemHeight;

    onClicked.emit(clickIndex);

    close();

    return true;
}

bool TpMenu::onMouseMoveEvent(TpMouseEvent *event)
{
    TpMenuData *menuData = static_cast<TpMenuData *>(data_);

    int32_t motionY = event->pos().y();

    menuData->hoverIndex = motionY / menuData->itemHeight;
    update();

    return true;
}

bool TpMenu::onLeaveEvent(TpLeaveEvent *event)
{
    TpMenuData *menuData = static_cast<TpMenuData *>(data_);
    menuData->isHover = event->leave();

    // update();

    return true;
}

bool TpMenu::onPaintEvent(TpPaintEvent *event)
{
    TpDialog::onPaintEvent(event);

    TpPainter *painter = event->painter();

    TpMenuData *menuData = static_cast<TpMenuData *>(data_);
    if (!menuData)
        return true;

    tpShared<TpCssData> normalCss = currentStatusCss();
    menuData->font->setFontSize(normalCss->fontSize());
    menuData->font->setFontColor(normalCss->color());

    for (int i = 0; i < menuData->itemList.size(); ++i)
    {
        MenuItemData curItem = menuData->itemList.at(i);

        menuData->font->setText(curItem.text);

        uint32_t curItemX = (width() - menuData->font->pixelWidth()) / 2.0;
        if (!curItem.iconImage.isNull())
        {
            curItemX = (width() - menuData->font->fontSize() - normalCss->gap() - menuData->font->pixelWidth()) / 2.0;
        }

        // 悬停item
        if (menuData->hoverIndex == i)
        {
            painter->setBrush(TpBrush(menuData->itemHoverColor));
            painter->drawRect(0, i * menuData->itemHeight, width(), menuData->itemHeight, roundCorners());
        }

        uint32_t curItemY = (i * menuData->itemHeight) + ((menuData->itemHeight - menuData->font->pixelHeight()) / 2.0);
        painter->drawText(*menuData->font, curItemX, curItemY);

        // item之间绘制分割线
        painter->setPen(_RGB(230, 235, 241));
        painter->drawHLine(normalCss->paddingLeft(), width() - normalCss->paddingRight(), i * menuData->itemHeight);
    }

    return true;
}
