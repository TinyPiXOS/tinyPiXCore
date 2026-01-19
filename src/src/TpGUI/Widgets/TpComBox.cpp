#include "TpComBox.h"
#include "TpLabel.h"
#include "TpImage.h"
#include "TpFont.h"
#include "TpHBoxLayout.h"
#include "TpMenu.h"
#include "TpEvent.h"
#include <mutex>

struct ComBoxItemData
{
    TpString text = "";
    TpVariant data;
};

struct TpComboxData
{
    TpLabel *titleLabel;

    TpLabel *curItemLabel;
    TpLabel *expandIconLabel;

    // 点击后展开item菜单
    TpMenu *contextMenu;

    TpVector<ComBoxItemData> itemList;

    int32_t curShowIndex = 0;

    // 更新索引锁
    std::mutex updateIndexMutex;
};

TpComBox::TpComBox(TpWidget *parent)
    : TpWidget(parent)
{
    tpShared<TpCssData> normalCss = currentStatusCss();

    TpComboxData *cbxData = new TpComboxData();

    cbxData->titleLabel = new TpLabel(this);
    cbxData->titleLabel->font()->setFontSize(normalCss->fontSize());
    cbxData->titleLabel->font()->setFontColor(normalCss->color());
    cbxData->titleLabel->font()->setAlign(Tp::AlignVCenter);
    cbxData->titleLabel->setFixedHeight(normalCss->minimumHeight());

    cbxData->curItemLabel = new TpLabel(this);
    // cbxData->curItemLabel->setText("测试");
    cbxData->curItemLabel->font()->setFontSize(normalCss->fontSize());
    cbxData->curItemLabel->font()->setFontColor(normalCss->color());
    cbxData->curItemLabel->font()->setAlign(Tp::AlignVCenter);
    cbxData->curItemLabel->installEventFilter(this);
    cbxData->curItemLabel->setFixedHeight(normalCss->minimumHeight());

    cbxData->expandIconLabel = new TpLabel(this);
    cbxData->expandIconLabel->setFixedHeight(normalCss->minimumHeight());
    cbxData->expandIconLabel->setFixedSize(normalCss->fontSize() * 1.5, normalCss->fontSize() * 1.5);
    cbxData->expandIconLabel->installEventFilter(this);
    cbxData->expandIconLabel->setBackGroundImage(TpImage("/usr/res/TinyPiX/双箭头上.png"));

    cbxData->contextMenu = new TpMenu();
    connect(cbxData->contextMenu, onClicked, [=](uint32_t index)
            {
                if (index >= cbxData->itemList.size())
                    return;

                ComBoxItemData curItem = cbxData->itemList.at(index);

                cbxData->curItemLabel->setText (curItem.text);

                std::lock_guard<std::mutex> lock_g(cbxData->updateIndexMutex);
                cbxData->curShowIndex = index;

                currentIndexChanged.emit(index);
                currentTextChanged.emit(curItem.text);
                update(); });

    data_ = cbxData;

    TpHBoxLayout *mainLayout = new TpHBoxLayout();
    mainLayout->setSpacing(normalCss->gap());
    mainLayout->setContentsMargins(normalCss->paddingLeft(), 0, normalCss->paddingRight(), 0);

    mainLayout->addWidget(cbxData->titleLabel);
    mainLayout->addSpacer(new TpSpacerItem(20, 20, TpSpacerItem::Expanding, TpSpacerItem::Minimum));
    mainLayout->addWidget(cbxData->curItemLabel);
    mainLayout->addWidget(cbxData->expandIconLabel);

    setLayout(mainLayout);

    refreshBaseCss();
}

TpComBox::~TpComBox()
{
    TpComboxData *cbxData = static_cast<TpComboxData *>(data_);
    if (cbxData)
    {
        cbxData->titleLabel->deleteLater();
        cbxData->curItemLabel->deleteLater();
        cbxData->expandIconLabel->deleteLater();

        cbxData->contextMenu->deleteLater();

        delete cbxData;
        cbxData = nullptr;
        data_ = nullptr;
    }
}

void TpComBox::setTitle(const TpString &text)
{
    TpComboxData *cbxData = static_cast<TpComboxData *>(data_);
    cbxData->titleLabel->setText(text);
    update();
}

void TpComBox::addItem(const TpString &text, const TpVariant &itemData)
{
    TpComboxData *cbxData = static_cast<TpComboxData *>(data_);

    ComBoxItemData curItemData;
    curItemData.text = text;
    curItemData.data = itemData;

    cbxData->itemList.emplace_back(curItemData);

    cbxData->contextMenu->addItem(text);

    if (cbxData->itemList.size() == 1)
    {
        cbxData->curShowIndex = 0;
        cbxData->curItemLabel->setText(text);
        update();
    }
}

uint32_t TpComBox::currentIndex()
{
    TpComboxData *cbxData = static_cast<TpComboxData *>(data_);
    return cbxData->curShowIndex;
}

TpString TpComBox::currentText()
{
    TpComboxData *cbxData = static_cast<TpComboxData *>(data_);
    return cbxData->itemList.at(cbxData->curShowIndex).text;
}

TpVariant TpComBox::currentData()
{
    TpComboxData *cbxData = static_cast<TpComboxData *>(data_);
    return cbxData->itemList.at(cbxData->curShowIndex).data;
}

void TpComBox::clear()
{
    TpComboxData *cbxData = static_cast<TpComboxData *>(data_);

    cbxData->itemList.clear();
    cbxData->curShowIndex = 0;
    cbxData->contextMenu->clear();

    update();
}

void TpComBox::setMinumumHeight(const int32_t &height)
{
    // std::cout << " ****************************height " << height << std::endl;

    TpWidget::setMinumumHeight(height);
}

bool TpComBox::onResizeEvent(TpResizeEvent *event)
{
    // std::cout << "TpListComBox Resize " << width() << "  height  " << height() << std::endl;

    return true;
}

bool TpComBox::eventFilter(TpObject *watched, TpEvent *event)
{
    if (event->eventType() == TpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        TpComboxData *cbxData = static_cast<TpComboxData *>(data_);

        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        if (mouseEvent->button() == BUTTON_LEFT)
        {
            cbxData->contextMenu->exec(mouseEvent->globalPos());
        }
    }
    else if (event->eventType() == TpEvent::EVENT_WHEEL_EVENT)
    {
        TpComboxData *cbxData = static_cast<TpComboxData *>(data_);

        TpWheelEvent *mouseEvent = dynamic_cast<TpWheelEvent *>(event);
        if (mouseEvent->angleDelta() > 0)
        {
            if (cbxData->itemList.size() > 0)
            {
                std::lock_guard<std::mutex> lock_g(cbxData->updateIndexMutex);

                cbxData->curShowIndex--;
                if (cbxData->curShowIndex < 0)
                    cbxData->curShowIndex = 0;

                TpString curItemText = cbxData->itemList.at(cbxData->curShowIndex).text;
                cbxData->curItemLabel->setText(curItemText);
                currentIndexChanged.emit(cbxData->curShowIndex);
                currentTextChanged.emit(curItemText);
            }
        }
        else
        {
            if (cbxData->itemList.size() > 0)
            {
                std::lock_guard<std::mutex> lock_g(cbxData->updateIndexMutex);

                cbxData->curShowIndex++;
                if (cbxData->curShowIndex >= cbxData->itemList.size())
                    cbxData->curShowIndex = cbxData->itemList.size() - 1;

                TpString curItemText = cbxData->itemList.at(cbxData->curShowIndex).text;

                cbxData->curItemLabel->setText(cbxData->itemList.at(cbxData->curShowIndex).text);
                currentIndexChanged.emit(cbxData->curShowIndex);
                currentTextChanged.emit(curItemText);
            }
        }
    }

    return false;
}
