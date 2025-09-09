#ifndef __TP_CAROUSE_BUTTON_H
#define __TP_CAROUSE_BUTTON_H

#include "TpChildWidget.h"
#include "TpSignalSlot.h"
#include <TpString.h>

TP_DEF_VOID_TYPE_VAR(ITpCarouselButtonData);
/// @brief 轮播按钮
class TpCarouselButton : public TpChildWidget
{
public:
    /// @brief 显示模式
    enum ShowMode
    {
        /// @brief 横向显示
        Horizon,
        /// @brief 纵向显示
        Vertical
    };

public:
    TpCarouselButton(TpChildWidget *parent = nullptr);

    TpCarouselButton(const uint32_t &count, TpChildWidget *parent = nullptr);

    virtual ~TpCarouselButton();

public:
    /// @brief 设置显示模式
    /// @param mode 显示模式枚举 
    void setMode(const ShowMode& mode);
    /// @brief 获取当前显示模式
    /// @return 显示模式枚举
    ShowMode mode();

    /// @brief 设置选中节点颜色
    /// @param color 颜色RGB值
    void setSelectColor(int32_t color);
    /// @brief 获取选中节点颜色
    /// @return 颜色RGB值
    int32_t selectColor();

    /// @brief 设置默认节点颜色
    /// @param color 颜色RGB
    void setDefaultColor(int32_t color);
    /// @brief 获取默认节点颜色
    /// @return 颜色RGB
    int32_t defaultColor();

    /// @brief 获取轮播按钮计数
    /// @return 计数
    uint32_t count();
    /// @brief 设置轮播按钮计数
    /// @param count 计数值
    void setCount(const uint32_t &count);

    /// @brief 获取当前选中下标
    /// @return 下标值
    uint32_t currentIndex();
    /// @brief 设置当前下标
    /// @param index 下标
    void setCurrentIndex(const uint32_t &index);

    /// @brief 设置节点间间距
    /// @param spacing 间距PX
    void setSpacing(int32_t spacing);
    /// @brief 获取节点间距
    /// @return 节点间距px
    int32_t spacing();

    /// @brief 设置是否响应点击切换，默认为false，设置true后点击节点按钮会自动切换至对应节点；且发送点击信号
    /// @param respond true点击节点后自动切换当前选中；false不处理
    void setRespondClick(bool respond);

public:
    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onPaintEvent(TpPaintEvent *event) override;

protected:
    virtual TpString pluginType() override { return TO_STRING(TpCarouselButton); }

private:
    void Init();

public
signals:
    declare_signal(onClicked, int32_t);

private:
    ITpCarouselButtonData *data_;
};

#endif
