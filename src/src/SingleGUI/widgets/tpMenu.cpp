#include "tpMenu.h"
#include "tpEvent.h"
#include "tpCanvas.h"
#include "tpDef.h"
#include "tpSurface.h"
#include "tpFont.h"

struct MenuItemData
{
	uint32_t id;
	tpString text;
	tpShared<tpSurface> iconSurface = nullptr;
};

struct tpMenuData
{
	tpVector<MenuItemData> itemList;

	tpFont *font = new tpFont();

	// ID索引，使用后+1
	uint32_t idIndex = 0;

	uint32_t itemHeight = 0;
	uint32_t itemWidth = 0;

	bool isHover;
	uint32_t hoverIndex = 0;

	int32_t itemHoverColor = _RGB(247, 237, 255);

	tpMenuData()
	{
	}

	~tpMenuData()
	{
		delete font;
		font = nullptr;
	}
};

tpMenu::tpMenu() : tpDialog(), data_(nullptr)
{
	tpMenuData *menuData = new tpMenuData();
	data_ = menuData;

	refreshBaseCss();

	tpShared<tpCssData> normalCss = currentStatusCss();

	menuData->font->setText("tpMenu");
	menuData->font->setFontSize(normalCss->fontSize());
	menuData->font->setFontColor(normalCss->color(), normalCss->color());

	menuData->itemHeight = menuData->font->pixelHeight() + normalCss->paddingTop() + normalCss->paddingBottom();

	setVisible(false);

	// setFixedSize(150, 150);
	// setBackGroundColor(_RGB(255, 0, 0));
}

tpMenu::~tpMenu()
{
	tpMenuData *menuData = static_cast<tpMenuData *>(data_);

	if (menuData)
	{
		delete menuData;
		menuData = nullptr;
		data_ = nullptr;
	}
}

void tpMenu::exec(const ItpPoint &globalPos)
{
	exec(globalPos.x, globalPos.y);
}

void tpMenu::exec(const int32_t &globalPosX, const int32_t &globalPosY)
{
	tpMenuData *menuData = static_cast<tpMenuData *>(data_);
	if (!menuData)
		return;

	int32_t width = tpScreen::screenWidth();
	int32_t height = tpScreen::screenHeight();

	int32_t posX = globalPosX;
	int32_t posY = globalPosY;

	if ((posX + this->width()) > width)
	{
		posX = posX - this->width();
	}

	if ((posY + this->height()) > height)
	{
		posY = posY - this->height();
	}

	uint32_t menuHeight = menuData->itemList.size() * (menuData->itemHeight);
	setSize(menuData->itemWidth, menuHeight);

	this->move(posX, posY);
	this->setVisible(true);
	this->update();
}

uint32_t tpMenu::addItem(const tpString &text)
{
	tpMenuData *menuData = static_cast<tpMenuData *>(data_);

	MenuItemData itemData;
	itemData.id = menuData->idIndex++;
	itemData.text = text;

	menuData->itemList.emplace_back(itemData);

	// 计算item的最大宽度
	tpShared<tpCssData> normalCss = currentStatusCss();
	tpFont tmpFont;
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

uint32_t tpMenu::addItem(const tpString &text, const tpString &iconPath)
{
	tpMenuData *menuData = static_cast<tpMenuData *>(data_);

	MenuItemData itemData;
	itemData.id = menuData->idIndex++;
	itemData.text = text;

	itemData.iconSurface = tpMakeShared<tpSurface>();
	itemData.iconSurface->fromFile(iconPath);

	menuData->itemList.emplace_back(itemData);

	// 计算item的最大宽度
	tpShared<tpCssData> normalCss = currentStatusCss();
	tpFont tmpFont;
	tmpFont.setText(text);
	tmpFont.setFontSize(normalCss->fontSize());

	// 要叠加icon的大小和间距
	uint32_t curItemWidth = normalCss->fontSize() + normalCss->gap() + tmpFont.pixelWidth() + normalCss->paddingLeft() + normalCss->paddingRight();
	if (curItemWidth > menuData->itemWidth)
		menuData->itemWidth = curItemWidth;

	update();

	return itemData.id;
}

void tpMenu::addMenu(const tpString &menuName, tpMenu *menu)
{
}

tpMenu *tpMenu::addMenu(const tpString &menuName)
{
	return nullptr;
}

uint32_t tpMenu::itemHeight()
{
	tpMenuData *menuData = static_cast<tpMenuData *>(data_);

	return menuData->itemHeight;
}

void tpMenu::setItemHeight(const uint32_t &height)
{
	tpMenuData *menuData = static_cast<tpMenuData *>(data_);
	menuData->itemHeight = height;

	uint32_t menuHeight = menuData->itemList.size() * (menuData->itemHeight);
	setHeight(menuHeight);
	update();
}

void tpMenu::setItemHoverColor(const int32_t &color)
{
	tpMenuData *menuData = static_cast<tpMenuData *>(data_);
	menuData->itemHoverColor = color;
}

void tpMenu::clear()
{
	tpMenuData *menuData = static_cast<tpMenuData *>(data_);
	menuData->itemList.clear();

	// ID索引，使用后+1
	menuData->idIndex = 0;
	menuData->itemWidth = 0;
}

tpString tpMenu::itemText(const uint32_t &index)
{
	tpMenuData *menuData = static_cast<tpMenuData *>(data_);

	if (index >= menuData->itemList.size())
		return "";

	return menuData->itemList.at(index).text;
}

bool tpMenu::onFocusEvent(tpObjectFocusEvent *event)
{
	std::cout << "tpMenu::onFocusEvent " << std::endl;
	// if (!event->focused())
	// {
	// 	close();
	// }

	return true;
}

bool tpMenu::onMousePressEvent(tpMouseEvent *event)
{
	if (event->button() != BUTTON_LEFT)
		return true;

	return true;
}

bool tpMenu::onMouseRleaseEvent(tpMouseEvent *event)
{
	if (event->button() == BUTTON_LEFT)
		return true;

	tpMenuData *menuData = static_cast<tpMenuData *>(data_);

	int32_t clickY = event->pos().y;
	uint32_t clickIndex = clickY / menuData->itemHeight;

	onClicked.emit(clickIndex);

	setVisible(false);

	return true;
}

bool tpMenu::onMouseMoveEvent(tpMouseEvent *event)
{
	tpMenuData *menuData = static_cast<tpMenuData *>(data_);

	int32_t motionY = event->pos().y;

	menuData->hoverIndex = motionY / menuData->itemHeight;
	update();

	return true;
}

bool tpMenu::onLeaveEvent(tpObjectLeaveEvent *event)
{
	tpMenuData *menuData = static_cast<tpMenuData *>(data_);
	menuData->isHover = event->leave();

	// update();

	return true;
}

bool tpMenu::onPaintEvent(tpObjectPaintEvent *event)
{
	tpChildWidget::onPaintEvent(event);

	tpCanvas *paintCanvas = event->canvas();

	tpMenuData *menuData = static_cast<tpMenuData *>(data_);
	if (!menuData)
		return true;

	tpShared<tpCssData> normalCss = currentStatusCss();
	menuData->font->setFontSize(normalCss->fontSize());
	menuData->font->setFontColor(normalCss->color(), normalCss->color());

	for (int i = 0; i < menuData->itemList.size(); ++i)
	{
		MenuItemData curItem = menuData->itemList.at(i);

		menuData->font->setText(curItem.text);

		uint32_t curItemX = (width() - menuData->font->pixelWidth()) / 2.0;
		if (curItem.iconSurface)
		{
			curItemX = (width() - menuData->font->fontSize() - normalCss->gap() - menuData->font->pixelWidth()) / 2.0;
		}

		// 悬停item
		if (menuData->hoverIndex == i)
		{
			paintCanvas->roundedBox(0, i * menuData->itemHeight, width(), i * menuData->itemHeight + menuData->itemHeight, roundCorners(), menuData->itemHoverColor);
		}

		uint32_t curItemY = (i * menuData->itemHeight) + ((menuData->itemHeight - menuData->font->pixelHeight()) / 2.0);
		paintCanvas->renderText(*menuData->font, curItemX, curItemY);

		// item之间绘制分割线
		paintCanvas->hline(normalCss->paddingLeft(), width() - normalCss->paddingRight(), i * menuData->itemHeight, _RGB(230, 235, 241));
	}

	return true;
}
