#include "tpComBox.h"
#include "tpLabel.h"
#include "tpSurface.h"
#include "tpFont.h"
#include "tpHBoxLayout.h"
#include "tpMenu.h"
#include "tpEvent.h"
#include <mutex>

struct ComBoxItemData
{
    tpString text = "";
    tpVariant data;
};

struct tpComboxData
{
    tpLabel *titleLabel;

    tpLabel *curItemLabel;
    tpLabel *expandIconLabel;

    // 点击后展开item菜单
    tpMenu *contextMenu;

    tpVector<ComBoxItemData> itemList;

    int32_t curShowIndex = 0;

    // 更新索引锁
    std::mutex updateIndexMutex;
};

tpComBox::tpComBox(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    tpShared<tpCssData> normalCss = currentStatusCss();

    tpComboxData *cbxData = new tpComboxData();

    cbxData->titleLabel = new tpLabel(this);
    cbxData->titleLabel->font()->setFontSize(normalCss->fontSize());
    cbxData->titleLabel->font()->setFontColor(normalCss->color(), normalCss->color());
    // cbxData->titleLabel->font()->setFontStyle(TINY_FONT_BOLD);
    cbxData->titleLabel->setFixedHeight(normalCss->minimumHeight());

    cbxData->curItemLabel = new tpLabel(this);
    // cbxData->curItemLabel->setText("测试");
    cbxData->curItemLabel->font()->setFontSize(normalCss->fontSize());
    cbxData->curItemLabel->font()->setFontColor(normalCss->color(), normalCss->color());
    cbxData->curItemLabel->installEventFilter(this);
    cbxData->titleLabel->setFixedHeight(normalCss->minimumHeight());

    cbxData->expandIconLabel = new tpLabel(this);
    cbxData->expandIconLabel->setFixedHeight(normalCss->minimumHeight());
    cbxData->expandIconLabel->setFixedSize(cbxData->titleLabel->font()->pixelHeight() * 0.7, cbxData->titleLabel->font()->pixelHeight());
    cbxData->expandIconLabel->installEventFilter(this);

    tpShared<tpSurface> expandIconSurface = tpMakeShared<tpSurface>();
    expandIconSurface->fromFile("/usr/res/tinyPiX/双箭头上.png");
    cbxData->expandIconLabel->setBackGroundImage(expandIconSurface);

    cbxData->contextMenu = new tpMenu();
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

    tpHBoxLayout *mainLayout = new tpHBoxLayout();
    mainLayout->setSpacing(normalCss->gap());
    mainLayout->setContentsMargins(normalCss->paddingLeft(), 0, normalCss->paddingRight(), 0);

    mainLayout->addWidget(cbxData->titleLabel);
    mainLayout->addSpacer(new tpSpacerItem(20, 20, tpSpacerItem::Expanding, tpSpacerItem::Minimum));
    mainLayout->addWidget(cbxData->curItemLabel);
    mainLayout->addWidget(cbxData->expandIconLabel);

    setLayout(mainLayout);

    refreshBaseCss();
}

tpComBox::~tpComBox()
{
    tpComboxData *cbxData = static_cast<tpComboxData *>(data_);
    if (cbxData)
    {
        cbxData->titleLabel->setParent(nullptr);
        cbxData->curItemLabel->setParent(nullptr);
        cbxData->expandIconLabel->setParent(nullptr);

        delete cbxData->titleLabel;
        cbxData->titleLabel = nullptr;

        delete cbxData->curItemLabel;
        cbxData->curItemLabel = nullptr;

        delete cbxData->expandIconLabel;
        cbxData->expandIconLabel = nullptr;

        delete cbxData->contextMenu;
        cbxData->contextMenu = nullptr;

        delete cbxData;
        cbxData = nullptr;
        data_ = nullptr;
    }
}

void tpComBox::setTitle(const tpString &text)
{
    tpComboxData *cbxData = static_cast<tpComboxData *>(data_);
    cbxData->titleLabel->setText(text);
    update();
}

void tpComBox::addItem(const tpString &text, const tpVariant &itemData)
{
    tpComboxData *cbxData = static_cast<tpComboxData *>(data_);

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

uint32_t tpComBox::currentIndex()
{
    tpComboxData *cbxData = static_cast<tpComboxData *>(data_);
    return cbxData->curShowIndex;
}

tpString tpComBox::currentText()
{
    tpComboxData *cbxData = static_cast<tpComboxData *>(data_);
    return cbxData->itemList.at(cbxData->curShowIndex).text;
}

tpVariant tpComBox::currentData()
{
    tpComboxData *cbxData = static_cast<tpComboxData *>(data_);
    return cbxData->itemList.at(cbxData->curShowIndex).data;
}

void tpComBox::clear()
{
    tpComboxData *cbxData = static_cast<tpComboxData *>(data_);

    cbxData->itemList.clear();
    cbxData->curShowIndex = 0;
    cbxData->contextMenu->clear();

    update();
}

void tpComBox::setMinumumHeight(const uint32_t &height)
{
    // std::cout << " ****************************height " << height << std::endl;

    tpChildWidget::setMinumumHeight(height);
}

bool tpComBox::onResizeEvent(tpObjectResizeEvent *event)
{
    // std::cout << "tpListComBox Resize " << width() << "  height  " << height() << std::endl;

    return true;
}

bool tpComBox::eventFilter(tpObject *watched, tpEvent *event)
{
    if (event->eventType() == tpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        tpComboxData *cbxData = static_cast<tpComboxData *>(data_);

        tpMouseEvent *mouseEvent = dynamic_cast<tpMouseEvent *>(event);
        if (mouseEvent->button() == BUTTON_LEFT)
        {
            cbxData->contextMenu->exec(mouseEvent->globalPos());
        }
    }
    else if (event->eventType() == tpEvent::EVENT_WHEEL_EVENT)
    {
        tpComboxData *cbxData = static_cast<tpComboxData *>(data_);

        tpWheelEvent *mouseEvent = dynamic_cast<tpWheelEvent *>(event);
        if (mouseEvent->angleDelta() > 0)
        {
            if (cbxData->itemList.size() > 0)
            {
                std::lock_guard<std::mutex> lock_g(cbxData->updateIndexMutex);

                cbxData->curShowIndex--;
                if (cbxData->curShowIndex < 0)
                    cbxData->curShowIndex = 0;

                tpString curItemText = cbxData->itemList.at(cbxData->curShowIndex).text;
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

                tpString curItemText = cbxData->itemList.at(cbxData->curShowIndex).text;

                cbxData->curItemLabel->setText(cbxData->itemList.at(cbxData->curShowIndex).text);
                currentIndexChanged.emit(cbxData->curShowIndex);
                currentTextChanged.emit(curItemText);
            }
        }
    }

    return false;
}
