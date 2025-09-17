#include "TpCollapsibleFileWidget.h"
#include "TpLabel.h"
#include "TpImage.h"
#include "TpVector.h"
#include "TpHBoxLayout.h"
#include "TpVBoxLayout.h"
#include "TpGridLayout.h"
#include "TpDisplay.h"
#include "TpFont.h"
#include "TpLine.h"
#include "TpFlexLayout.h"
#include "TpTimer.h"
#include <thread>
#include "TpCheckBox.h"

struct fileInfo
{
    TpString fileName; // 文件名称
    float fileSizeKb;  // 文件大小
    TpString tagName;  // 标签

    fileInfo() : fileName(""), fileSizeKb(0), tagName("")
    {
    }
};

struct TpCollapsibleFileWidgetData
{
    // 主标题和副标题
    TpLabel *titleLabel;
    TpLabel *subTitleLabel;

    // 总计数
    TpLabel *countLabel;

    // 是否展开图标
    TpLabel *expandLabel;
    TpImage expandIcon;
    TpImage noExpandIcon;

    TpCheckBox *selectAllItem;

    // 显示所有文件列表的窗口
    TpChildWidget *fileListWidget;

    // 主布局
    TpVBoxLayout *mainLayout;

    TpFlexLayout *filItemLayout;

    // title和item的分割线
    TpLine *mainLine;

    TpVector<TpCollapsibleFileItem *> fileInfoItemList;

    TpCollapsibleFileWidget::SelectMode selectMode = TpCollapsibleFileWidget::Normal;
};

TpCollapsibleFileWidget::TpCollapsibleFileWidget(TpChildWidget *parent)
    : TpChildWidget(parent)
{
    setCheckable(true);
    refreshBaseCss();

    TpCollapsibleFileWidgetData *widgetData = new TpCollapsibleFileWidgetData();

    widgetData->titleLabel = new TpLabel(this);
    widgetData->titleLabel->installEventFilter(this);
    widgetData->titleLabel->setFixedHeight(TpDisplay::dp2Px(30));

    widgetData->subTitleLabel = new TpLabel(this);
    // widgetData->subTitleLabel->setBackGroundColor(_RGB(255, 0, 0));
    widgetData->subTitleLabel->setFixedHeight(TpDisplay::dp2Px(30));
    widgetData->subTitleLabel->font()->setFontColor(_RGB(140, 140, 140), _RGB(140, 140, 140));
    widgetData->subTitleLabel->installEventFilter(this);

    widgetData->countLabel = new TpLabel(this);
    widgetData->countLabel->installEventFilter(this);
    widgetData->countLabel->setFixedHeight(TpDisplay::dp2Px(30));

    TpLine *subLine = new TpLine();
    subLine->setLineType(TpLine::VLine);
    subLine->setLineLength(18);
    subLine->setColor(_RGB(190, 196, 202));
    subLine->setAlign(tinyPiX::AlignCenter);
    subLine->setLineWidth(2);
    subLine->setFixedHeight(TpDisplay::dp2Px(30));

    widgetData->expandLabel = new TpLabel(this);
    widgetData->expandLabel->installEventFilter(this);
    widgetData->expandLabel->setFixedSize(TpDisplay::dp2Px(30), TpDisplay::dp2Px(30));
    // widgetData->expandLabel->setBackGroundColor(_RGB(255, 0, 0));

    widgetData->expandIcon.load("/usr/res/tinyPiX/箭头-已展开.png");
    widgetData->noExpandIcon.load("/usr/res/tinyPiX/箭头-未展开.png");

    widgetData->expandLabel->setBackGroundImage(widgetData->noExpandIcon);

    widgetData->selectAllItem = new TpCheckBox();
    widgetData->selectAllItem->setVisible(false);
    connect(widgetData->selectAllItem, onClicked, [=](bool checked)
            {
        for (const auto& item : widgetData->fileInfoItemList)
        {
            item->setSelected(checked);
        } });

    // item的响应式布局
    widgetData->filItemLayout = new TpFlexLayout();
    widgetData->filItemLayout->setContentsMargins(0, 0, 0, 0);
    widgetData->filItemLayout->setFlexDirection(TpFlexLayout::Row);
    widgetData->filItemLayout->setJustifyContent(TpFlexLayout::MainFlexStart); // 默认, 从主轴 开始位置 开始
    widgetData->filItemLayout->setAlignItems(TpFlexLayout::CrossFlexStart);    // 顶部对齐
    widgetData->filItemLayout->installEventFilter(this);

    tpShared<TpCssData> curCssData = currentStatusCss();

    widgetData->mainLayout = new TpVBoxLayout();
    widgetData->mainLayout->setContentsMargins(curCssData->paddingLeft(), curCssData->paddingTop(), curCssData->paddingRight(), curCssData->paddingBottom());
    widgetData->mainLayout->setSpacing(0);

    TpHBoxLayout *titleLayout = new TpHBoxLayout();
    titleLayout->setSpacing(15);
    titleLayout->setContentsMargins(0, 0, 0, 0);

    titleLayout->addWidget(widgetData->titleLabel);
    titleLayout->addWidget(subLine);
    titleLayout->addWidget(widgetData->subTitleLabel);
    titleLayout->addSpacer(new TpSpacerItem(40, 20, TpSpacerItem::Expanding));
    titleLayout->addWidget(widgetData->countLabel);
    titleLayout->addWidget(widgetData->expandLabel);
    titleLayout->addWidget(widgetData->selectAllItem);

    widgetData->mainLine = new TpLine();
    // widgetData->mainLine->setBackGroundColor(_RGB(255, 0, 0));
    widgetData->mainLine->setLineType(TpLine::HLine);
    widgetData->mainLine->setColor(_RGB(190, 196, 202));
    widgetData->mainLine->setMinumumHeight(20);
    widgetData->mainLine->setLineWidth(2);

    widgetData->mainLayout->addLayout(titleLayout);
    // widgetData->mainLayout->addWidget(widgetData->mainLine, 1);
    // widgetData->mainLayout->addLayout(widgetData->filItemLayout, 5);
    widgetData->mainLayout->addSpacer(new TpSpacerItem(20, 40, TpSpacerItem::Minimum, TpSpacerItem::Expanding));

    setLayout(widgetData->mainLayout);

    data_ = widgetData;
}

TpCollapsibleFileWidget::~TpCollapsibleFileWidget()
{
    TpCollapsibleFileWidgetData *widgetData = static_cast<TpCollapsibleFileWidgetData *>(data_);
    if (widgetData)
    {
        delete widgetData;
        widgetData = nullptr;
        data_ = nullptr;
    }
}

void TpCollapsibleFileWidget::setSelectMode(const SelectMode &mode)
{
    TpCollapsibleFileWidgetData *widgetData = static_cast<TpCollapsibleFileWidgetData *>(data_);
    widgetData->selectMode = mode;

    if (mode == TpCollapsibleFileWidget::Normal)
    {
        // 窗体取消显示勾选框
        widgetData->countLabel->setVisible(true);
        widgetData->expandLabel->setVisible(true);
        widgetData->selectAllItem->setVisible(false);

        // 所有item隐藏勾选框
        for (const auto &item : widgetData->fileInfoItemList)
        {
            item->setSelectEable(false);
        }
    }
    else
    {
        // 窗体添加勾选框
        widgetData->countLabel->setVisible(false);
        widgetData->expandLabel->setVisible(false);
        widgetData->selectAllItem->setVisible(true);

        // 所有item显示勾选框
        for (const auto &item : widgetData->fileInfoItemList)
        {
            item->setSelectEable(true);
        }
    }

    update();
}

void TpCollapsibleFileWidget::setTitle(const TpString &title)
{
    TpCollapsibleFileWidgetData *widgetData = static_cast<TpCollapsibleFileWidgetData *>(data_);
    if (!widgetData)
        return;
    widgetData->titleLabel->setText(title);
}

void TpCollapsibleFileWidget::setSubTitle(const TpString &subTitle)
{
    TpCollapsibleFileWidgetData *widgetData = static_cast<TpCollapsibleFileWidgetData *>(data_);
    if (!widgetData)
        return;
    widgetData->subTitleLabel->setText(subTitle);
}

void TpCollapsibleFileWidget::addFileItem(TpCollapsibleFileItem *item)
{
    TpCollapsibleFileWidgetData *widgetData = static_cast<TpCollapsibleFileWidgetData *>(data_);
    if (!widgetData)
        return;

    if (!item)
        return;

    // item->setParent(this);
    widgetData->filItemLayout->addWidget(item);
    widgetData->fileInfoItemList.emplace_back(item);

    layout()->update();

    uint32_t count = widgetData->fileInfoItemList.size();
    widgetData->countLabel->setText(TpString::number(count) + "项");

    // uint32_t fileItemRowCount = widgetData->filItemLayout->rowCount();
    // setMinumumHeight(fileItemRowCount * item->minumumHeight() + TpDisplay::dp2Px(50));

    update();
}

bool TpCollapsibleFileWidget::onResizeEvent(TpResizeEvent *event)
{
    // std::cout << "TpCollapsibleFileWidget::onResizeEvent " << width() << "  "<< height() <<std::endl;
    return true;
}

bool TpCollapsibleFileWidget::onMousePressEvent(TpMouseEvent *event)
{
    TpChildWidget::onMousePressEvent(event);

    if (event->button() != BUTTON_LEFT)
        return true;

    return true;
}

bool TpCollapsibleFileWidget::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpChildWidget::onMouseRleaseEvent(event);

    if (event->button() != BUTTON_LEFT)
        return true;

    TpCollapsibleFileWidgetData *widgetData = static_cast<TpCollapsibleFileWidgetData *>(data_);
    if (!widgetData)
        return true;

    if (checked())
    {
        widgetData->expandLabel->setBackGroundImage(widgetData->expandIcon);

        // 展开详细列表
        widgetData->mainLayout->insertWidget(1, widgetData->mainLine, 1);
        widgetData->mainLayout->insertLayout(2, widgetData->filItemLayout, 5);
    }
    else
    {
        widgetData->expandLabel->setBackGroundImage(widgetData->noExpandIcon);

        // 收起详细文件列表
        widgetData->mainLayout->removeWidget(widgetData->mainLine);
        widgetData->mainLayout->removeLayout(widgetData->filItemLayout);
    }

    widgetData->mainLayout->update();

    // setHeight(widgetData->mainLayout->minumumSize().h);
    update();

    return true;
}

bool TpCollapsibleFileWidget::onMouseLongPressEvent(TpMouseEvent *event)
{
    onLongPress.emit();

    return true;
}

bool TpCollapsibleFileWidget::onLeaveEvent(TpLeaveEvent *event)
{
    TpCollapsibleFileWidgetData *widgetData = static_cast<TpCollapsibleFileWidgetData *>(data_);
    if (!widgetData)
        return true;

    return true;
}

bool TpCollapsibleFileWidget::onPaintEvent(TpPaintEvent *event)
{
    // std::cout << "TpCollapsibleFileWidget::onPaintEvent " << std::endl;

    TpChildWidget::onPaintEvent(event);

    TpCollapsibleFileWidgetData *widgetData = static_cast<TpCollapsibleFileWidgetData *>(data_);
    if (!widgetData)
        return true;

    tpShared<TpCssData> curCssData = currentStatusCss();

    if (widgetData->titleLabel)
    {
        widgetData->titleLabel->font()->setFontColor(curCssData->color(), curCssData->color());
        widgetData->titleLabel->font()->setFontSize(curCssData->fontSize());
    }

    if (widgetData->subTitleLabel)
    {
        widgetData->subTitleLabel->font()->setFontSize(curCssData->fontSize());
    }

    if (widgetData->countLabel)
    {
        widgetData->countLabel->font()->setFontColor(curCssData->color(), curCssData->color());
        widgetData->countLabel->font()->setFontSize(curCssData->fontSize());
    }

    return true;
}

bool TpCollapsibleFileWidget::eventFilter(TpObject *watched, TpEvent *event)
{
    if (event->eventType() == TpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        onMousePressEvent(mouseEvent);
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        onMouseRleaseEvent(mouseEvent);
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_LONG_PRESS_TYPE)
    {
        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        onMouseLongPressEvent(mouseEvent);
    }
    else
    {
    }

    return true;
}
