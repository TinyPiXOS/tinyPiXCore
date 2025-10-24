#include "TpListWidget.h"
#include "TpVector.h"
#include "TpPainter.h"
#include "TpEvent.h"

#define SPACER_LINE_WIDTH 1

struct TpListCheckBoxGroupData
{
    Tp::ItemSelectionMode selectMode = Tp::SingleSelection;

    TpVector<TpListWidgetItem *> itemList;

    ~TpListCheckBoxGroupData()
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

TpListWidget::TpListWidget(TpWidget *parent)
    : TpWidget(parent)
{
    TpListCheckBoxGroupData *cbxData = new TpListCheckBoxGroupData();

    data_ = cbxData;

    refreshBaseCss();
}

TpListWidget::~TpListWidget()
{
    TpListCheckBoxGroupData *cbxData = static_cast<TpListCheckBoxGroupData *>(data_);
    if (cbxData)
    {
        delete cbxData;
        cbxData = nullptr;
        data_ = nullptr;
    }
}

void TpListWidget::setSelectionMode(const Tp::ItemSelectionMode &selectionMode)
{
    TpListCheckBoxGroupData *cbxData = static_cast<TpListCheckBoxGroupData *>(data_);
    cbxData->selectMode = selectionMode;

    if ((selectionMode != Tp::SingleSelection) && (selectionMode != Tp::MultiSelection))
    {
        cbxData->selectMode = Tp::SingleSelection;
    }

    setSelected(0);
}

Tp::ItemSelectionMode TpListWidget::selectionMode()
{
    TpListCheckBoxGroupData *cbxData = static_cast<TpListCheckBoxGroupData *>(data_);
    return cbxData->selectMode;
}

TpListWidgetItem *TpListWidget::addItem(const TpString &text, const TpVariant &itemData)
{
    TpListCheckBoxGroupData *cbxData = static_cast<TpListCheckBoxGroupData *>(data_);

    TpListWidgetItem *curItem = new TpListWidgetItem(this);
    curItem->setText(text);
    curItem->setData(itemData);

    addItem(curItem);

    return curItem;
}

void TpListWidget::addItem(TpListWidgetItem *item)
{
    if (!item)
        return;

    TpListCheckBoxGroupData *cbxData = static_cast<TpListCheckBoxGroupData *>(data_);

    item->setParent(this);

    cbxData->itemList.emplace_back(item);

    if (cbxData->selectMode == Tp::SingleSelection && cbxData->itemList.size() == 1)
    {
        item->setChecked(true);
    }

    // 绑定item点击事件
    connect(item, onStatusChanged, [=](TpListWidgetItem *item)
            { 
                if (item->checked())
                {
                    setSelectedItem(item);
                    onStatusChanged.emit(item);
                }
                else
                {
                    // 取消选中，如果是单选模式且是最后一个选中项，不允许取消选中
                    if (cbxData->selectMode == Tp::SingleSelection)
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

TpVector<uint32_t> TpListWidget::currentIndex()
{
    TpVector<uint32_t> selectIdList;
    TpListCheckBoxGroupData *cbxData = static_cast<TpListCheckBoxGroupData *>(data_);

    for (int i = 0; i < cbxData->itemList.size(); ++i)
    {
        if (cbxData->itemList.at(i)->checked())
        {
            selectIdList.emplace_back(i);
            if (cbxData->selectMode == Tp::SingleSelection)
                break;
        }
    }
    return selectIdList;
}

TpVector<TpListWidgetItem *> TpListWidget::currentItem()
{
    TpVector<TpListWidgetItem *> selectItemList;
    TpListCheckBoxGroupData *cbxData = static_cast<TpListCheckBoxGroupData *>(data_);

    for (const auto &item : cbxData->itemList)
    {
        if (item->checked())
        {
            selectItemList.emplace_back(item);
            if (cbxData->selectMode == Tp::SingleSelection)
                break;
        }
    }
    return selectItemList;
}

void TpListWidget::setSelected(const uint32_t &index)
{
    TpListCheckBoxGroupData *cbxData = static_cast<TpListCheckBoxGroupData *>(data_);
    for (int i = 0; i < cbxData->itemList.size(); ++i)
    {
        TpListWidgetItem *curItem = cbxData->itemList.at(i);

        if (i == index)
        {
            curItem->setChecked(true);
            if (cbxData->selectMode == Tp::MultiSelection)
            {
                break;
            }
        }
        else
        {
            if (cbxData->selectMode == Tp::SingleSelection)
                curItem->setChecked(false);
        }
    }

    update();
}

void TpListWidget::setSelectedItem(const TpListWidgetItem *item)
{
    TpListCheckBoxGroupData *cbxData = static_cast<TpListCheckBoxGroupData *>(data_);
    for (int i = 0; i < cbxData->itemList.size(); ++i)
    {
        TpListWidgetItem *curItem = cbxData->itemList.at(i);

        if (curItem == item)
        {
            curItem->setChecked(true);
            if (cbxData->selectMode == Tp::MultiSelection)
            {
                break;
            }
        }
        else
        {
            if (cbxData->selectMode == Tp::SingleSelection)
                curItem->setChecked(false);
        }
    }

    update();
}

TpListWidgetItem *TpListWidget::takeItem(const uint32_t &index)
{
    TpListCheckBoxGroupData *cbxData = static_cast<TpListCheckBoxGroupData *>(data_);
    for (int i = 0; i < cbxData->itemList.size(); ++i)
    {
        if (i == index)
        {
            TpListWidgetItem *curItem = cbxData->itemList.at(i);
            cbxData->itemList.remove(i);
            return curItem;
        }
    }

    return nullptr;
}

void TpListWidget::clear()
{
    TpListCheckBoxGroupData *cbxData = static_cast<TpListCheckBoxGroupData *>(data_);
    for (int i = 0; i < cbxData->itemList.size(); ++i)
    {
        TpListWidgetItem *curItem = cbxData->itemList.at(i);
        curItem->deleteLater();
    }
    cbxData->itemList.clear();
}

bool TpListWidget::onPaintEvent(TpPaintEvent *event)
{
    TpWidget::onPaintEvent(event);

    TpPainter *paintCanvas = event->painter();
    TpListCheckBoxGroupData *cbxData = static_cast<TpListCheckBoxGroupData *>(data_);
    tpShared<TpCssData> normalCss = currentStatusCss();

    paintCanvas->pen().setColor(_RGB(230, 235, 241));
    paintCanvas->pen().setWidth(SPACER_LINE_WIDTH);

    for (int i = 1; i < cbxData->itemList.size(); ++i)
    {
        TpListWidgetItem *curItem = cbxData->itemList.at(i);
        if (!curItem)
            continue;

        uint32_t y = i * (curItem->height() + SPACER_LINE_WIDTH);
        uint32_t x2 = normalCss->paddingLeft() + (width() - normalCss->paddingRight() - normalCss->paddingLeft());

        paintCanvas->drawHLine(normalCss->paddingLeft(), x2, y);
    }
    return true;
}

bool TpListWidget::onResizeEvent(TpResizeEvent *event)
{
    TpWidget::onResizeEvent(event);

    TpListCheckBoxGroupData *cbxData = static_cast<TpListCheckBoxGroupData *>(data_);

    tpShared<TpCssData> normalCss = currentStatusCss();

    // 依次排列item
    for (int i = 0; i < cbxData->itemList.size(); ++i)
    {
        TpListWidgetItem *curItem = cbxData->itemList.at(i);
        if (!curItem)
            continue;

        curItem->setWidth(width() - normalCss->paddingLeft() - normalCss->paddingRight());

        curItem->move(normalCss->paddingLeft(), i * (curItem->height() + SPACER_LINE_WIDTH));
    }

    return true;
}

bool TpListWidget::eventFilter(TpObject *watched, TpEvent *event)
{
    return false;
}
