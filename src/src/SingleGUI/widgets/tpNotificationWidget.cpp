#include "tpNotificationWidget.h"
#include "tpCanvas.h"
#include <math.h>
#include "tpScrollPanel.h"

// 样式参数（直接内联）
const int32_t BG_COLOR = _RGBA(58, 107, 160, 128);     // 背景色
const int32_t TITLE_COLOR = _RGBA(255, 255, 255, 255); // 标题色

const int ITEM_ROUNDING = 8;    // 圆角半径
const int ITEM_SPACING = 5;     // 项间距
const int COLLAPSE_HEIGHT = 60; // 折叠高度
const int EXPAND_HEIGHT = 200;  // 展开高度
const int CLOSE_BTN_SIZE = 16;  // 关闭按钮尺寸
const int TITLE_PADDING = 5;

struct tpNotificationWidgetData
{
    tpScrollPanel *mainScrollPanel;

    tpList<tpNotificationItem *> itemList; // 所有通知项

    // <APPid, app下的所有item>
    tpList<tpList<tpNotificationItem *>> appMessageItemMap_;

    // <appid, 展开状态>
    tpHash<tpString, bool> groupExpand;

    int m_scroll_offset = 0; // 当前滚动偏移量
    int m_total_height = 0;  // 所有项总高度

    // 预定义字体
    tpFont m_title_font;   // 标题字体
    tpFont m_time_font;    // 时间字体
    tpFont m_content_font; // 内容字体
};

tpNotificationWidget::tpNotificationWidget(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    tpNotificationWidgetData *widgetData = new tpNotificationWidgetData();
    data_ = widgetData;

    widgetData->mainScrollPanel = new tpScrollPanel(this);
    widgetData->mainScrollPanel->installEventFilter(this);

    // 初始化字体
    widgetData->m_title_font.setFontSize(14);
    widgetData->m_title_font.setFontStyle(TINY_FONT_BOLD);
    widgetData->m_title_font.setFontForeColor(TITLE_COLOR);

    widgetData->m_time_font.setFontSize(12);
    widgetData->m_time_font.setFontForeColor(_RGBA(208, 180, 255, 255));

    widgetData->m_content_font.setFontSize(14);
}

tpNotificationWidget::~tpNotificationWidget()
{
}

void tpNotificationWidget::addItem(tpNotificationItem *item)
{
    tpNotificationItem::NotificationItemData curItemData = item->itemData();
    if (curItemData.appId.empty() || curItemData.messageId.empty())
        return;

    // 如果当前消息已经被添加过，不再重复添加
    tpNotificationWidgetData *widgetData = static_cast<tpNotificationWidgetData *>(data_);
    if (widgetData->itemList.contains(item))
        return;

    widgetData->itemList.emplace_back(item);

    bool isFindGroup = false;

    // 被更新的APP的list，被更新后，放在最前边
    tpList<tpNotificationItem *> updateList;

    for (auto &appItemList : widgetData->appMessageItemMap_)
    {
        if (appItemList.size() == 0)
            continue;

        // 如果当前通知栏已经有新消息的app分组，添加进分组
        if (appItemList.front()->itemData().appId.compare(curItemData.appId) == 0)
        {
            isFindGroup = true;
            appItemList.emplace_front(item);
            updateList = appItemList;
            widgetData->appMessageItemMap_.remove(appItemList);
            break;
        }
    }

    // 如果已有分组内没有当前消息的APP分组，添加新分组
    if (isFindGroup == false)
    {
        widgetData->appMessageItemMap_.emplace_front(tpList<tpNotificationItem *>{item});
    }
}

bool tpNotificationWidget::onPaintEvent(tpObjectPaintEvent *event)
{
    tpNotificationWidgetData *widgetData = static_cast<tpNotificationWidgetData *>(data_);

    tpCanvas *canvas = event->canvas();

    ItpRect widgetRect = this->rect();

    // 初始化绘制参数
    int yPos = -widgetData->m_scroll_offset; // 起始位置考虑滚动偏移
    const int itemWidth = widgetRect.w - 10; // 左右留边5px

    // canvas->save();
    // canvas->setClipRect(&widgetRect);
    // canvas->fillRect(widgetRect, BG_COLOR);

    for (const auto &appItemList : widgetData->appMessageItemMap_)
    {
        if (appItemList.size() == 0)
            continue;

        tpNotificationItem *firstItem = appItemList.front();
        if (!firstItem)
            continue;

        tpNotificationItem::NotificationItemData firstItemData = firstItem->itemData();

        // 绘制分组名
    }

    // 遍历所有通知项
    // for (int i = 0; i < widgetData->itemList.size(); ++i)
    // {
    //     tpNotificationItem *item = widgetData->itemList.at(i);
    //     const bool isExpanded = item->isExpanded();
    //     const int itemHeight = isExpanded ? EXPAND_HEIGHT : COLLAPSE_HEIGHT;

    //     // 计算项的实际显示区域
    //     ItpRect itemRect(5, yPos, itemWidth, itemHeight);

    //     // 只绘制可见项
    //     if (itemRect.bottom() > 0 && itemRect.top() < widgetRect.h)
    //     {
    //         // 绘制项背景
    //         canvas->roundedRectangle(itemRect.left(), itemRect.top(), itemRect.right(), itemRect.bottom(), ITEM_ROUNDING, _RGBA(255, 255, 255, 30));

    //         // 应用名称（左对齐）
    //         const int textY = itemRect.y + TITLE_PADDING;
    //         canvas->renderText(
    //             item->itemData().appName.c_str(),
    //             itemRect.x() + TITLE_PADDING,
    //             textY,
    //             widgetData->m_title_font);

    //         // 时间（右对齐）
    //         const int timeX = itemRect.right() - TITLE_PADDING - widgetData->m_time_font.textWidth(item->itemData().timeStr.c_str());
    //         canvas->drawText(
    //             item->itemData().timeStr.c_str(),
    //             timeX,
    //             textY,
    //             widgetData->m_time_font);

    //         // 标题文本（最多2行）
    //         int contentY = textY + widgetData->m_title_font.fontSize() + 8;
    //         canvas->drawTextMultiLine(
    //             item->itemData().titleText.c_str(),
    //             itemRect.x() + TITLE_PADDING,
    //             contentY,
    //             itemWidth - TITLE_PADDING * 2,
    //             2,
    //             widgetData->m_content_font);

    //         // 展开时绘制完整内容
    //         if (isExpanded)
    //         {
    //             contentY += (widgetData->m_content_font.fontSize() + 2) * 2 + 10;
    //             canvas->drawTextMultiLine(
    //                 item->itemData().contentText.c_str(),
    //                 itemRect.x() + TITLE_PADDING,
    //                 contentY,
    //                 itemWidth - TITLE_PADDING * 2,
    //                 5, // 最多5行
    //                 widgetData->m_content_font);
    //         }

    //         // 更新关闭按钮点击区域（相对控件坐标）
    //         const ItpRect closeArea(
    //             itemRect.right() - CLOSE_BTN_SIZE - TITLE_PADDING,
    //             itemRect.y() + TITLE_PADDING,
    //             CLOSE_BTN_SIZE,
    //             CLOSE_BTN_SIZE);
    //         item->setCloseHitArea(closeArea - ItpPoint(0, widgetData->m_scroll_offset)); // 转换到控件坐标系

    //         // 绘制关闭按钮
    //         canvas->drawLine(
    //             closeArea.left(), closeArea.top(),
    //             closeArea.right(), closeArea.bottom(),
    //             _RGBA(255, 100, 100, 200));
    //         canvas->drawLine(
    //             closeArea.right(), closeArea.top(),
    //             closeArea.left(), closeArea.bottom(),
    //             _RGBA(255, 100, 100, 200));
    //     }

    //     yPos += itemHeight + ITEM_SPACING;
    // }

    return true;
}

bool tpNotificationWidget::onMousePressEvent(tpMouseEvent *event)
{
    tpNotificationWidgetData *widgetData = static_cast<tpNotificationWidgetData *>(data_);

    // 遍历检测点击
    for (int i = 0; i < widgetData->itemList.size(); ++i)
    {
        tpNotificationItem *item = widgetData->itemList.at(i);

        // if (item->closeHitArea().contains(event->pos()))
        // {
        //     // 处理关闭操作
        //     widgetData->itemList.remove(i);
        //     update();
        //     return true;
        // }

        // 处理展开点击...
    }

    return true;
}

bool tpNotificationWidget::onMouseRleaseEvent(tpMouseEvent *event)
{
    return true;
}

bool tpNotificationWidget::onResizeEvent(tpObjectResizeEvent *event)
{
    tpNotificationWidgetData *widgetData = new tpNotificationWidgetData();
    widgetData->mainScrollPanel->setRect(0, 0, width(), height());

    return true;
}
