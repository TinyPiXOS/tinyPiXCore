#ifndef __TP_LIST_WIDGET_H
#define __TP_LIST_WIDGET_H

#include "TpChildWidget.h"
#include "TpSignalSlot.h"
#include "TpString.h"
#include "TpListWidgetItem.h"

TP_DEF_VOID_TYPE_VAR(ITpListWidgetData);
/// @brief 列表窗口
class TpListWidget : public TpChildWidget
{
public:
    TpListWidget(TpChildWidget *parent = nullptr);
    virtual ~TpListWidget();

public:
    /// @brief 设置选择模式
    /// @param selectionMode 选择模式枚举, 只有单选和多选生效
    void setSelectionMode(const tinyPiX::ItemSelectionMode &selectionMode);

    /// @brief 获取当前widget设置的选中模式
    /// @return 选中模式枚举
    tinyPiX::ItemSelectionMode selectionMode();

    /// @brief 添加一个item
    /// @param text item文本
    /// @param itemData item绑定数据，可以不给入
    /// @return 添加的item指针
    virtual TpListWidgetItem *addItem(const TpString &text, const TpVariant &itemData = TpVariant());

    /// @brief 添加一个item
    /// @param item 指针对象
    virtual void addItem(TpListWidgetItem *item);

    /// @brief 获取当前索引
    /// @return 索引值,单选模式下只会返回一个，多选模式下可能返回空
    TpVector<uint32_t> currentIndex();

    /// @brief 获取当前选中的item
    /// @return 选中的item指针,单选模式下只会返回一个，多选模式下可能返回空
    TpVector<TpListWidgetItem *> currentItem();

    /// @brief 指定索引选中item
    /// @param index item索引，从0开始
    void setSelected(const uint32_t &index);

    /// @brief 指定item选中
    /// @param item 选中item指针
    void setSelectedItem(const TpListWidgetItem *item);

    /// @brief 指定索引取出指定item；取出后窗体内无该item；外部需释放指针
    /// @param index 索引值
    /// @return 索引对应的item;索引值无效则返回nullptr
    TpListWidgetItem *takeItem(const uint32_t &index);

    /// @brief 清理所有Item
    void clear();

public
signals:
    /// @brief 选中项切换事件
    /// @param TpListWidgetItem* 当前状态切换item
    declare_signal(onStatusChanged, TpListWidgetItem *);

    /// @brief 选中项切换时间
    /// @param const TpVariant& 状态切换项数据
    /// @param bool 当前item状态
    // declare_signal(onStatusChanged, const TpVariant &, bool);

protected:
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool onResizeEvent(TpResizeEvent *event) override;
    virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

protected:
    virtual TpString pluginType() override { return TO_STRING(TpListWidget); }

private:
    ITpListWidgetData *data_;
};

#endif
