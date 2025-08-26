#ifndef __TP_LIST_CHECKBOX_GROUP_H
#define __TP_LIST_CHECKBOX_GROUP_H

#include "TpChildWidget.h"
#include "TpSignalSlot.h"
#include "TpString.h"
#include "TpListCheckBoxGroupItem.h"

TP_DEF_VOID_TYPE_VAR(ItpListCheckBoxGroupData);
/// @brief 列表模式下的单选复选按钮组
class TpListCheckBoxGroup : public TpChildWidget
{
public:
    TpListCheckBoxGroup(TpChildWidget *parent = nullptr);
    virtual ~TpListCheckBoxGroup();

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
    virtual TpListCheckBoxGroupItem *addItem(const TpString &text, const TpVariant &itemData = TpVariant());

    /// @brief 添加一个item
    /// @param item 指针对象
    virtual void addItem(TpListCheckBoxGroupItem *item);

    /// @brief 获取当前索引
    /// @return 索引值,单选模式下只会返回一个，多选模式下可能返回空
    TpVector<uint32_t> currentIndex();

    /// @brief 获取当前选中的item
    /// @return 选中的item指针,单选模式下只会返回一个，多选模式下可能返回空
    TpVector<TpListCheckBoxGroupItem *> currentItem();

    /// @brief 指定索引选中item
    /// @param index item索引，从0开始
    void setSelected(const uint32_t &index);

    /// @brief 指定item选中
    /// @param item 选中item指针
    void setSelectedItem(const TpListCheckBoxGroupItem *item);

    /// @brief 指定索引取出指定item；取出后窗体内无该item；外部需释放指针
    /// @param index 索引值
    /// @return 索引对应的item;索引值无效则返回nullptr
    TpListCheckBoxGroupItem *takeItem(const uint32_t &index);

    /// @brief 清理所有Item
    void clear();

public
signals:
    /// @brief 选中项切换事件
    /// @param TpListCheckBoxGroupItem* 当前状态切换item
    declare_signal(onStatusChanged, TpListCheckBoxGroupItem *);

    /// @brief 选中项切换时间
    /// @param const TpVariant& 状态切换项数据
    /// @param bool 当前item状态
    // declare_signal(onStatusChanged, const TpVariant &, bool);

protected:
    virtual bool onPaintEvent(TpObjectPaintEvent *event) override;
    virtual bool onResizeEvent(TpObjectResizeEvent *event) override;
    virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

protected:
    virtual TpString pluginType() override { return TO_STRING(TpListCheckBoxGroup); }

private:
    ItpListCheckBoxGroupData *data_;
};

#endif
