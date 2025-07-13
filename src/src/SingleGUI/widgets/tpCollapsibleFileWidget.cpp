#include "tpCollapsibleFileWidget.h"
#include "tpLabel.h"
#include "tpSurface.h"
#include "tpVector.h"
#include "tpHBoxLayout.h"
#include "tpVBoxLayout.h"
#include "tpGridLayout.h"
#include "tpDisplay.h"
#include "tpFont.h"
#include "tpLine.h"
#include "tpFlexLayout.h"
#include "tpTimer.h"
#include <thread>
#include "tpCheckBox.h"

struct fileInfo
{
    tpString fileName; // 文件名称
    float fileSizeKb;  // 文件大小
    tpString tagName;  // 标签

    fileInfo() : fileName(""), fileSizeKb(0), tagName("")
    {
    }
};

struct tpCollapsibleFileWidgetData
{
    // 主标题和副标题
    tpLabel *titleLabel;
    tpLabel *subTitleLabel;

    // 总计数
    tpLabel *countLabel;

    // 是否展开图标
    tpLabel *expandLabel;
    tpShared<tpSurface> expandIcon;
    tpShared<tpSurface> noExpandIcon;

    tpCheckBox *selectAllItem;

    // 显示所有文件列表的窗口
    tpChildWidget *fileListWidget;

    // 主布局
    tpVBoxLayout *mainLayout;

    tpFlexLayout *filItemLayout;

    // title和item的分割线
    tpLine *mainLine;

    tpVector<tpCollapsibleFileItem *> fileInfoItemList;

    tpCollapsibleFileWidget::SelectMode selectMode = tpCollapsibleFileWidget::Normal;
};

tpCollapsibleFileWidget::tpCollapsibleFileWidget(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    setCheckable(true);
    refreshBaseCss();

    tpCollapsibleFileWidgetData *widgetData = new tpCollapsibleFileWidgetData();

    widgetData->titleLabel = new tpLabel(this);
    widgetData->titleLabel->installEventFilter(this);
    widgetData->titleLabel->setFixedHeight(tpDisplay::dp2Px(30));

    widgetData->subTitleLabel = new tpLabel(this);
    // widgetData->subTitleLabel->setBackGroundColor(_RGB(255, 0, 0));
    widgetData->subTitleLabel->setFixedHeight(tpDisplay::dp2Px(30));
    widgetData->subTitleLabel->font()->setFontColor(_RGB(140, 140, 140), _RGB(140, 140, 140));
    widgetData->subTitleLabel->installEventFilter(this);

    widgetData->countLabel = new tpLabel(this);
    widgetData->countLabel->installEventFilter(this);
    widgetData->countLabel->setFixedHeight(tpDisplay::dp2Px(30));

    tpLine *subLine = new tpLine();
    subLine->setLineType(tpLine::VLine);
    subLine->setLineLength(18);
    subLine->setColor(_RGB(190, 196, 202));
    subLine->setAlign(tinyPiX::AlignCenter);
    subLine->setLineWidth(2);
    subLine->setFixedHeight(tpDisplay::dp2Px(30));

    widgetData->expandLabel = new tpLabel(this);
    widgetData->expandLabel->installEventFilter(this);
    widgetData->expandLabel->setFixedSize(tpDisplay::dp2Px(30), tpDisplay::dp2Px(30));
    // widgetData->expandLabel->setBackGroundColor(_RGB(255, 0, 0));

    widgetData->expandIcon = tpMakeShared<tpSurface>();
    widgetData->expandIcon->fromFile("/usr/res/tinyPiX/箭头-已展开.png");
    widgetData->noExpandIcon = tpMakeShared<tpSurface>();
    widgetData->noExpandIcon->fromFile("/usr/res/tinyPiX/箭头-未展开.png");

    widgetData->expandLabel->setBackGroundImage(widgetData->noExpandIcon);

    widgetData->selectAllItem = new tpCheckBox();
    widgetData->selectAllItem->setVisible(false);
    connect(widgetData->selectAllItem, onClicked, [=](bool checked)
            {
        for (const auto& item : widgetData->fileInfoItemList)
        {
            item->setSelected(checked);
        } });

    // item的响应式布局
    widgetData->filItemLayout = new tpFlexLayout();
    widgetData->filItemLayout->setContentsMargins(0, 0, 0, 0);
    widgetData->filItemLayout->setFlexDirection(tpFlexLayout::Row);
    widgetData->filItemLayout->setJustifyContent(tpFlexLayout::MainFlexStart); // 默认, 从主轴 开始位置 开始
    widgetData->filItemLayout->setAlignItems(tpFlexLayout::CrossFlexStart);    // 顶部对齐
    widgetData->filItemLayout->installEventFilter(this);

    tpShared<tpCssData> curCssData = currentStatusCss();

    widgetData->mainLayout = new tpVBoxLayout();
    widgetData->mainLayout->setContentsMargins(curCssData->paddingLeft(), curCssData->paddingTop(), curCssData->paddingRight(), curCssData->paddingBottom());
    widgetData->mainLayout->setSpacing(0);

    tpHBoxLayout *titleLayout = new tpHBoxLayout();
    titleLayout->setSpacing(15);
    titleLayout->setContentsMargins(0, 0, 0, 0);

    titleLayout->addWidget(widgetData->titleLabel);
    titleLayout->addWidget(subLine);
    titleLayout->addWidget(widgetData->subTitleLabel);
    titleLayout->addSpacer(new tpSpacerItem(40, 20, tpSpacerItem::Expanding));
    titleLayout->addWidget(widgetData->countLabel);
    titleLayout->addWidget(widgetData->expandLabel);
    titleLayout->addWidget(widgetData->selectAllItem);

    widgetData->mainLine = new tpLine();
    // widgetData->mainLine->setBackGroundColor(_RGB(255, 0, 0));
    widgetData->mainLine->setLineType(tpLine::HLine);
    widgetData->mainLine->setColor(_RGB(190, 196, 202));
    widgetData->mainLine->setMinumumHeight(20);
    widgetData->mainLine->setLineWidth(2);

    widgetData->mainLayout->addLayout(titleLayout);
    // widgetData->mainLayout->addWidget(widgetData->mainLine, 1);
    // widgetData->mainLayout->addLayout(widgetData->filItemLayout, 5);
    widgetData->mainLayout->addSpacer(new tpSpacerItem(20, 40, tpSpacerItem::Minimum, tpSpacerItem::Expanding));

    setLayout(widgetData->mainLayout);

    data_ = widgetData;
}

tpCollapsibleFileWidget::~tpCollapsibleFileWidget()
{
    tpCollapsibleFileWidgetData *widgetData = static_cast<tpCollapsibleFileWidgetData *>(data_);
    if (widgetData)
    {
        delete widgetData;
        widgetData = nullptr;
        data_ = nullptr;
    }
}

void tpCollapsibleFileWidget::setSelectMode(const SelectMode &mode)
{
    tpCollapsibleFileWidgetData *widgetData = static_cast<tpCollapsibleFileWidgetData *>(data_);
    widgetData->selectMode = mode;

    if (mode == tpCollapsibleFileWidget::Normal)
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

void tpCollapsibleFileWidget::setTitle(const tpString &title)
{
    tpCollapsibleFileWidgetData *widgetData = static_cast<tpCollapsibleFileWidgetData *>(data_);
    if (!widgetData)
        return;
    widgetData->titleLabel->setText(title);
}

void tpCollapsibleFileWidget::setSubTitle(const tpString &subTitle)
{
    tpCollapsibleFileWidgetData *widgetData = static_cast<tpCollapsibleFileWidgetData *>(data_);
    if (!widgetData)
        return;
    widgetData->subTitleLabel->setText(subTitle);
}

void tpCollapsibleFileWidget::addFileItem(tpCollapsibleFileItem *item)
{
    tpCollapsibleFileWidgetData *widgetData = static_cast<tpCollapsibleFileWidgetData *>(data_);
    if (!widgetData)
        return;

    if (!item)
        return;

    // item->setParent(this);
    widgetData->filItemLayout->addWidget(item);
    widgetData->fileInfoItemList.emplace_back(item);

    layout()->update();

    uint32_t count = widgetData->fileInfoItemList.size();
    widgetData->countLabel->setText(tpString::number(count) + "项");

    // uint32_t fileItemRowCount = widgetData->filItemLayout->rowCount();
    // setMinumumHeight(fileItemRowCount * item->minumumHeight() + tpDisplay::dp2Px(50));

    update();
}

bool tpCollapsibleFileWidget::onResizeEvent(tpObjectResizeEvent *event)
{
    // std::cout << "tpCollapsibleFileWidget::onResizeEvent " << width() << "  "<< height() <<std::endl;
    return true;
}

bool tpCollapsibleFileWidget::onMousePressEvent(tpMouseEvent *event)
{
    tpChildWidget::onMousePressEvent(event);

    if (event->button() != BUTTON_LEFT)
        return true;

    return true;
}

bool tpCollapsibleFileWidget::onMouseRleaseEvent(tpMouseEvent *event)
{
    tpChildWidget::onMouseRleaseEvent(event);

    if (event->button() != BUTTON_LEFT)
        return true;

    tpCollapsibleFileWidgetData *widgetData = static_cast<tpCollapsibleFileWidgetData *>(data_);
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

bool tpCollapsibleFileWidget::onMouseLongPressEvent(tpMouseEvent *event)
{
    onLongPress.emit();

    return true;
}

bool tpCollapsibleFileWidget::onLeaveEvent(tpObjectLeaveEvent *event)
{
    tpCollapsibleFileWidgetData *widgetData = static_cast<tpCollapsibleFileWidgetData *>(data_);
    if (!widgetData)
        return true;

    return true;
}

bool tpCollapsibleFileWidget::onPaintEvent(tpObjectPaintEvent *event)
{
    // std::cout << "tpCollapsibleFileWidget::onPaintEvent " << std::endl;

    tpChildWidget::onPaintEvent(event);

    tpCollapsibleFileWidgetData *widgetData = static_cast<tpCollapsibleFileWidgetData *>(data_);
    if (!widgetData)
        return true;

    tpShared<tpCssData> curCssData = currentStatusCss();

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

bool tpCollapsibleFileWidget::eventFilter(tpObject *watched, tpEvent *event)
{
    if (event->eventType() == tpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        tpMouseEvent *mouseEvent = dynamic_cast<tpMouseEvent *>(event);
        onMousePressEvent(mouseEvent);
    }
    else if (event->eventType() == tpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        tpMouseEvent *mouseEvent = dynamic_cast<tpMouseEvent *>(event);
        onMouseRleaseEvent(mouseEvent);
    }
    else if (event->eventType() == tpEvent::EVENT_MOUSE_LONG_PRESS_TYPE)
    {
        tpMouseEvent *mouseEvent = dynamic_cast<tpMouseEvent *>(event);
        onMouseLongPressEvent(mouseEvent);
    }
    else
    {
    }

    return true;
}
