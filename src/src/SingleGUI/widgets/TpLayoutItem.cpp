#include "TpLayoutItem.h"
#include "TpLayout.h"

struct TpLayoutItemData
{
	TpChildWidget *widget = nullptr;
	TpLayout *layout = nullptr;

    TpLayoutItem::ItemType type;
};

TpLayoutItem::TpLayoutItem(const ItemType& type)
{
    TpLayoutItemData* itemData = new TpLayoutItemData();

    itemData->type = type;

    data_ = itemData;
}

TpChildWidget *TpLayoutItem::widget()
{
    return nullptr;
}

TpLayoutItem::~TpLayoutItem()
{

}