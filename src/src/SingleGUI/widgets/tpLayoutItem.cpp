#include "tpLayoutItem.h"
#include "tpLayout.h"

struct tpLayoutItemData
{
	tpChildWidget *widget = nullptr;
	tpLayout *layout = nullptr;

    tpLayoutItem::ItemType type;
};

tpLayoutItem::tpLayoutItem(const ItemType& type)
{
    tpLayoutItemData* itemData = new tpLayoutItemData();

    itemData->type = type;

    data_ = itemData;
}

tpChildWidget *tpLayoutItem::widget()
{
    return nullptr;
}

tpLayoutItem::~tpLayoutItem()
{

}