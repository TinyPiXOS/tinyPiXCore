#ifndef __TP_CLIP_RECT_OPTIMIZER_H
#define __TP_CLIP_RECT_OPTIMIZER_H

#include "TpWidget.h"
#include "TpWidget_p.h"
#include "thorVG/thorvg.h"
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <queue>

// 组件刷新限制区域 - 优化版本
class ClipRectOptimizer
{
private:
    // 脏标记管理器
    struct DirtyManager
    {
        // static inline std::unordered_set<TpWidget *> dirtyWidgets;
        static std::unordered_set<TpWidget *> &getDirtyWidgets()
        {
            static std::unordered_set<TpWidget *> instance;
            return instance;
        }

        static void markDirty(TpWidget *widget)
        {
            if (widget)
                getDirtyWidgets().insert(widget);
        }

        static void clearDirty(TpWidget *widget)
        {
            getDirtyWidgets().erase(widget);
        }

        static bool isDirty(TpWidget *widget)
        {
            return getDirtyWidgets().find(widget) != getDirtyWidgets().end();
        }
    };

public:
    // 批量刷新入口函数 - 替代原来的单个组件刷新
    static void batchRefreshSceneClipRects()
    {
        // 收集所有脏组件并进行批量处理
        std::vector<TpWidget *> toProcess(DirtyManager::getDirtyWidgets().begin(),
                                          DirtyManager::getDirtyWidgets().end());

        for (auto *widget : toProcess)
        {
            if (widget && DirtyManager::isDirty(widget))
            {
                TpWidgetData *widgetData = static_cast<TpWidgetData *>(widget->objectSets());
                if (widgetData)
                {
                    refreshSceneClipRect(widget, widgetData);
                    DirtyManager::clearDirty(widget);
                }
            }
        }
    }

    // 标记组件需要刷新裁剪区域
    static void markWidgetForRefresh(TpWidget *widget)
    {
        return;

        if (!widget)
            return;

        DirtyManager::markDirty(widget);

        // 标记所有子组件，但不立即刷新（延迟批量处理）
        TpWidgetData *widgetData = static_cast<TpWidgetData *>(widget->objectSets());
        if (widgetData)
        {
            for (const auto &childObj : widgetData->objectList)
            {
                TpWidget *childWidget = dynamic_cast<TpWidget *>(childObj);
                if (childWidget)
                {
                    DirtyManager::markDirty(childWidget);
                }
            }
        }
    }

private:
    // 计算精确的裁剪区域（修正逻辑错误）
    static TpRect calculateAccurateClipRect(TpWidget *widget)
    {
        if (!widget)
            return TpRect(0, 0, 0, 0);

        const TpRect widgetRect = widget->toScreen();
        TpRect clipRect = widgetRect;

        // 遍历父组件链，计算交集[2,5](@ref)
        TpWidget *parent = dynamic_cast<TpWidget *>(widget->parent());
        while (parent)
        {
            TpRect parentRect = parent->toScreen();

            // 计算两个矩形的交集[2](@ref)
            int32_t x1 = TP_MAX(clipRect.x(), parentRect.x());
            int32_t y1 = TP_MAX(clipRect.y(), parentRect.y());
            int32_t x2 = TP_MIN(clipRect.x() + clipRect.width(),
                                parentRect.x() + parentRect.width());
            int32_t y2 = TP_MIN(clipRect.y() + clipRect.height(),
                                parentRect.y() + parentRect.height());

            // 检查是否有有效交集[2](@ref)
            if (x2 <= x1 || y2 <= y1)
            {
                return TpRect(0, 0, 0, 0); // 无交集，完全被裁剪
            }

            clipRect.setX(x1);
            clipRect.setY(y1);
            clipRect.setWidth(x2 - x1);
            clipRect.setHeight(y2 - y1);

            parent = dynamic_cast<TpWidget *>(parent->parent());
        }

        // 处理浮动窗口和主窗口的特殊偏移[3](@ref)
        TpObject *top = widget->topObject();
        if (top && (top != widget) &&
            (top->objectType() == Tp::TP_FLOAT_OBJECT ||
             top->objectType() == Tp::TP_MAIN_WINDOW_OBJECT))
        {
            // 只调整一次偏移，避免重复计算[3](@ref)
            clipRect.setX(clipRect.x() - widget->offsetX());
            clipRect.setY(clipRect.y() - widget->offsetY());
        }

        return clipRect;
    }

    // 应用裁剪区域到图形层（带缓存检查）
    static void applyClipRectToScene(TpWidget *widget, TpWidgetData *widgetData, const TpRect &newClipRect)
    {
        if (!widgetData->tvgScene)
            return;

        // 检查裁剪区域是否实际发生变化
        if (widgetData->m_lastClipRect == newClipRect && !widgetData->m_clipRectDirty)
        {
            return; // 无变化，跳过更新
        }

        // 创建新的裁剪形状
        auto clipper = tvg::Shape::gen();
        clipper->appendRect(newClipRect.x(), newClipRect.y(),
                            newClipRect.width(), newClipRect.height());

        // 应用裁剪器
        if (auto oldClipper = widgetData->tvgScene->clip())
        {
            tvg::Paint::rel(oldClipper);
        }

        auto result = widgetData->tvgScene->clip(clipper);
        if (result == tvg::Result::Success)
        {
            // clipper.release();
        }
        else
        {
            tvg::Paint::rel(clipper);
        }

        // 更新缓存
        widgetData->m_lastClipRect = newClipRect;
        widgetData->m_clipRectDirty = false;
    }

    // 优化的场景裁剪区域刷新
    static void refreshSceneClipRect(TpWidget *widget, TpWidgetData *widgetData)
    {
        if (!widget || !widgetData->tvgScene)
            return;

        // 计算精确的裁剪区域
        TpRect newClipRect = calculateAccurateClipRect(widget);

        // 应用裁剪区域
        applyClipRectToScene(widget, widgetData, newClipRect);

        // 使用迭代方式处理子组件，避免深层递归
        refreshChildSceneClipRectIterative(widget, widgetData);
    }

    // 迭代方式刷新子组件裁剪区域（替代递归）
    static void refreshChildSceneClipRectIterative(TpWidget *rootWidget, TpWidgetData *rootData)
    {
        if (!rootWidget || !rootData)
            return;

        std::queue<std::pair<TpWidget *, TpWidgetData *>> processingQueue;

        // 将直接子组件加入队列[6](@ref)
        for (const auto &childObj : rootData->objectList)
        {
            TpWidget *childWidget = dynamic_cast<TpWidget *>(childObj);
            if (childWidget)
            {
                TpWidgetData *childData = static_cast<TpWidgetData *>(childWidget->objectSets());
                if (childData)
                {
                    processingQueue.push({childWidget, childData});
                }
            }
        }

        // 迭代处理队列
        while (!processingQueue.empty())
        {
            auto currentWidget = processingQueue.front().first;
            auto currentData = processingQueue.front().second;
            processingQueue.pop();

            // 只刷新标记为脏的组件
            if (DirtyManager::isDirty(currentWidget))
            {
                TpRect childClipRect = calculateAccurateClipRect(currentWidget);
                applyClipRectToScene(currentWidget, currentData, childClipRect);
                DirtyManager::clearDirty(currentWidget);
            }

            // 将子组件加入队列继续处理
            for (const auto &childObj : currentData->objectList)
            {
                TpWidget *childWidget = dynamic_cast<TpWidget *>(childObj);
                if (childWidget)
                {
                    TpWidgetData *childData = static_cast<TpWidgetData *>(childWidget->objectSets());
                    if (childData)
                    {
                        processingQueue.push({childWidget, childData});
                    }
                }
            }
        }
    }

public:
    // 旧的递归函数保留为兼容接口（不推荐使用）
    __attribute__((deprecated("Use markWidgetForRefresh and batchRefreshSceneClipRects instead"))) static void refreshChildSceneClipRect(TpWidget *widget, TpWidgetData *widgetData)
    {
        markWidgetForRefresh(widget);
    }
};

// 需要在 TpWidgetData 结构中添加的字段
// struct TpWidgetData
// {
//     tvg::unique_ptr<tvg::Scene> tvgScene;
//     // 新增：裁剪区域缓存和脏标记
//     TpRect m_lastClipRect;
//     bool m_clipRectDirty{true};

//     // 其他现有字段...
// };

// 使用示例
// class TpWidget
// {
// public:
//     // 在移动或调整大小时调用这个而不是直接调用refreshSceneClipRect
//     void move(int x, int y)
//     {
//         // 原有的移动逻辑...

//         // 标记需要刷新裁剪区域
//         ClipRectOptimizer::markWidgetForRefresh(this);
//     }

//     void resize(int width, int height)
//     {
//         // 原有的调整大小逻辑...

//         // 标记需要刷新裁剪区域
//         ClipRectOptimizer::markWidgetForRefresh(this);
//     }

//     // 其他现有方法...
// };

// // 在主循环或渲染前调用批量刷新
// void onBeforeRender()
// {
//     ClipRectOptimizer::batchRefreshSceneClipRects();
// }

#endif