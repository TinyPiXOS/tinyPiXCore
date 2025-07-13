#ifndef __TP_CAROUSE_BUTTON_H
#define __TP_CAROUSE_BUTTON_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include <tpString.h>

TP_DEF_VOID_TYPE_VAR(ItpCarouselButtonData);

/// @brief 轮播按钮,无需外部设置尺寸，setCount后会自动设置尺寸
class tpCarouselButton : public tpChildWidget
{
public:
    tpCarouselButton(tpChildWidget *parent);

    tpCarouselButton(const uint32_t &count, tpChildWidget *parent);

    virtual ~tpCarouselButton();

public:
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

public:
    virtual void setParent(tpObject *parent) override;

    virtual bool onMousePressEvent(tpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;
    virtual void onThemeChangeEvent(tpThemeChangeEvent *event) override;

protected:
    virtual tpString pluginType() override { return TO_STRING(tpCarouselButton); }

private:
    void Init();

public
signals:
    declare_signal(onClicked, uint32_t);

private:
    ItpCarouselButtonData *data_;
};

#endif
