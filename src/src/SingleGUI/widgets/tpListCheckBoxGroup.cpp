#include "tpListCheckBoxGroup.h"
#include "tpVector.h"
#include "tpCanvas.h"
#include "tpEvent.h"

#define SPACER_LINE_WIDTH 1

struct tpListCheckBoxGroupData
{
    tinyPiX::ItemSelectionMode selectMode = tinyPiX::SingleSelection;

    tpVector<tpListCheckBoxGroupItem *> itemList;

    ~tpListCheckBoxGroupData()
    {
        for (auto &item : itemList)
        {
            item->setParent(nullptr);
            delete item;
            item = nullptr;
        }
        itemList.clear();
    }
};

tpListCheckBoxGroup::tpListCheckBoxGroup(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    tpListCheckBoxGroupData *cbxData = new tpListCheckBoxGroupData();

    data_ = cbxData;

    refreshBaseCss();
}

tpListCheckBoxGroup::~tpListCheckBoxGroup()
{
    tpListCheckBoxGroupData *cbxData = static_cast<tpListCheckBoxGroupData *>(data_);
    if (cbxData)
    {
        delete cbxData;
        cbxData = nullptr;
        data_ = nullptr;
    }
}

void tpListCheckBoxGroup::setSelectionMode(const tinyPiX::ItemSelectionMode &selectionMode)
{
    tpListCheckBoxGroupData *cbxData = static_cast<tpListCheckBoxGroupData *>(data_);
    cbxData->selectMode = selectionMode;

    if ((selectionMode != tinyPiX::SingleSelection) && (selectionMode != tinyPiX::MultiSelection))
    {
        cbxData->selectMode = tinyPiX::SingleSelection;
    }

    setSelected(0);
}

tinyPiX::ItemSelectionMode tpListCheckBoxGroup::selectionMode()
{
    tpListCheckBoxGroupData *cbxData = static_cast<tpListCheckBoxGroupData *>(data_);
    return cbxData->selectMode;
}

tpListCheckBoxGroupItem *tpListCheckBoxGroup::addItem(const tpString &text, const tpVariant &itemData)
{
    tpListCheckBoxGroupData *cbxData = static_cast<tpListCheckBoxGroupData *>(data_);

    tpListCheckBoxGroupItem *curItem = new tpListCheckBoxGroupItem(this);
    curItem->setText(text);
    curItem->setData(itemData);

    addItem(curItem);

    return curItem;
}

void tpListCheckBoxGroup::addItem(tpListCheckBoxGroupItem *item)
{
    if (!item)
        return;

    tpListCheckBoxGroupData *cbxData = static_cast<tpListCheckBoxGroupData *>(data_);

    item->setParent(this);

    cbxData->itemList.emplace_back(item);

    if (cbxData->selectMode == tinyPiX::SingleSelection && cbxData->itemList.size() == 1)
    {
        item->setChecked(true);
    }

    // 绑定item点击事件
    connect(item, onStatusChanged, [=](tpListCheckBoxGroupItem *item)
            { 
                if (item->checked())
                {
                    setSelectedItem(item);
                    onStatusChanged.emit(item);
                }
                else
                {
                    // 取消选中，如果是单选模式且是最后一个选中项，不允许取消选中
                    if (cbxData->selectMode == tinyPiX::SingleSelection)
                    {
                        bool isHaveSelected = false;
                        for (const auto& item : cbxData->itemList)
                        {
                            if (item->checked())
                            {
                                isHaveSelected = true;
                                break;
                            }
                        }
                        if (isHaveSelected == false)
                            item->setChecked(true);
                        else
                            onStatusChanged.emit(item);
                    }
                    update();
                } });

    setFixedHeight(cbxData->itemList.size() * item->height() + (cbxData->itemList.size() - 1) * SPACER_LINE_WIDTH);
}

tpVector<uint32_t> tpListCheckBoxGroup::currentIndex()
{
    tpVector<uint32_t> selectIdList;
    tpListCheckBoxGroupData *cbxData = static_cast<tpListCheckBoxGroupData *>(data_);

    for (int i = 0; i < cbxData->itemList.size(); ++i)
    {
        if (cbxData->itemList.at(i)->checked())
        {
            selectIdList.emplace_back(i);
            if (cbxData->selectMode == tinyPiX::SingleSelection)
                break;
        }
    }
    return selectIdList;
}

tpVector<tpListCheckBoxGroupItem *> tpListCheckBoxGroup::currentItem()
{
    tpVector<tpListCheckBoxGroupItem *> selectItemList;
    tpListCheckBoxGroupData *cbxData = static_cast<tpListCheckBoxGroupData *>(data_);

    for (const auto &item : cbxData->itemList)
    {
        if (item->checked())
        {
            selectItemList.emplace_back(item);
            if (cbxData->selectMode == tinyPiX::SingleSelection)
                break;
        }
    }
    return selectItemList;
}

void tpListCheckBoxGroup::setSelected(const uint32_t &index)
{
    tpListCheckBoxGroupData *cbxData = static_cast<tpListCheckBoxGroupData *>(data_);
    for (int i = 0; i < cbxData->itemList.size(); ++i)
    {
        tpListCheckBoxGroupItem *curItem = cbxData->itemList.at(i);

        if (i == index)
        {
            curItem->setChecked(true);
            if (cbxData->selectMode == tinyPiX::MultiSelection)
            {
                break;
            }
        }
        else
        {
            if (cbxData->selectMode == tinyPiX::SingleSelection)
                curItem->setChecked(false);
        }
    }

    update();
}

void tpListCheckBoxGroup::setSelectedItem(const tpListCheckBoxGroupItem *item)
{
    tpListCheckBoxGroupData *cbxData = static_cast<tpListCheckBoxGroupData *>(data_);
    for (int i = 0; i < cbxData->itemList.size(); ++i)
    {
        tpListCheckBoxGroupItem *curItem = cbxData->itemList.at(i);

        if (curItem == item)
        {
            curItem->setChecked(true);
            if (cbxData->selectMode == tinyPiX::MultiSelection)
            {
                break;
            }
        }
        else
        {
            if (cbxData->selectMode == tinyPiX::SingleSelection)
                curItem->setChecked(false);
        }
    }

    update();
}

tpListCheckBoxGroupItem *tpListCheckBoxGroup::takeItem(const uint32_t &index)
{
    tpListCheckBoxGroupData *cbxData = static_cast<tpListCheckBoxGroupData *>(data_);
    for (int i = 0; i < cbxData->itemList.size(); ++i)
    {
        if (i == index)
        {
            tpListCheckBoxGroupItem *curItem = cbxData->itemList.at(i);
            cbxData->itemList.remove(i);
            return curItem;
        }
    }

    return nullptr;
}

void tpListCheckBoxGroup::clear()
{
    tpListCheckBoxGroupData *cbxData = static_cast<tpListCheckBoxGroupData *>(data_);
    for (int i = 0; i < cbxData->itemList.size(); ++i)
    {
        tpListCheckBoxGroupItem *curItem = cbxData->itemList.at(i);
        curItem->deleteLater();
    }
    cbxData->itemList.clear();
}

bool tpListCheckBoxGroup::onPaintEvent(tpObjectPaintEvent *event)
{
    tpChildWidget::onPaintEvent(event);

    tpCanvas *paintCanvas = event->canvas();
    tpListCheckBoxGroupData *cbxData = static_cast<tpListCheckBoxGroupData *>(data_);
    tpShared<tpCssData> normalCss = currentStatusCss();

    for (int i = 1; i < cbxData->itemList.size(); ++i)
    {
        tpListCheckBoxGroupItem *curItem = cbxData->itemList.at(i);
        if (!curItem)
            continue;

        uint32_t y = i * (curItem->height() + SPACER_LINE_WIDTH);
        uint32_t x2 = normalCss->paddingLeft() + (width() - normalCss->paddingRight() - normalCss->paddingLeft());

        paintCanvas->hline(normalCss->paddingLeft(), x2, y, _RGB(230, 235, 241), SPACER_LINE_WIDTH);
    }
    return true;
}

bool tpListCheckBoxGroup::onResizeEvent(tpObjectResizeEvent *event)
{
    tpChildWidget::onResizeEvent(event);

    tpListCheckBoxGroupData *cbxData = static_cast<tpListCheckBoxGroupData *>(data_);

    tpShared<tpCssData> normalCss = currentStatusCss();

    // 依次排列item
    for (int i = 0; i < cbxData->itemList.size(); ++i)
    {
        tpListCheckBoxGroupItem *curItem = cbxData->itemList.at(i);
        if (!curItem)
            continue;

        curItem->setWidth(width() - normalCss->paddingLeft() - normalCss->paddingRight());

        curItem->move(normalCss->paddingLeft(), i * (curItem->height() + SPACER_LINE_WIDTH));
    }

    return true;
}

bool tpListCheckBoxGroup::eventFilter(tpObject *watched, tpEvent *event)
{
    return false;
}
