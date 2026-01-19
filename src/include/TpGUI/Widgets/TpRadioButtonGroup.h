#ifndef __TP_RADIO_BUTTON_GROUP_H
#define __TP_RADIO_BUTTON_GROUP_H

#include "TpWidget.h"
#include "TpSignalSlot.h"
#include "TpList.h"

TP_DEF_VOID_TYPE_VAR(ITpRadioButtonGroupData);

class TpRadioButton;

/// @brief 单选按钮组；组内单选按钮只能被选中一个
class TpRadioButtonGroup
{
public:
    TpRadioButtonGroup();
    virtual ~TpRadioButtonGroup();

    /// @brief 添加一个单选按钮
    /// @param radioBtn 按钮指针
    /// @param id 按钮ID，不给入则自动分配；给入ID若重复则会重新分配
    /// @return 按钮ID
    int32_t addButton(TpRadioButton *radioBtn, const int32_t &id = -1);

    /// @brief 获取组内所有按钮信息
    /// @return 按钮列表
    TpList<TpRadioButton *> buttons();

    /// @brief 指定按钮ID获取按钮指针
    /// @param id 按钮ID
    /// @return 按钮指针；ID不存在则返回空指针
    TpRadioButton *button(const int32_t &id);
    /// @brief 指定按钮指针获取按钮ID
    /// @param button 按钮指针
    /// @return 按钮ID
    int32_t id(TpRadioButton *button) const;

    /// @brief 获取当前选中的单选按钮
    /// @return 按钮指针，若组内没有按钮，则返回空指针
    TpRadioButton *checkedButton() const;
    /// @brief 获取当前选中的按钮ID
    /// @return 按钮ID；若组内没有按钮，会返回-1
    int32_t checkedId() const;

public
signals:
    /// @brief 按钮点击事件，鼠标释放触发
    /// @param TpRadioButton 按钮指针
    declare_signal(buttonClicked, TpRadioButton *);

private:
    ITpRadioButtonGroupData *data_;
};

#endif
