#ifndef __TP_RADIO_BUTTON_GROUP_H
#define __TP_RADIO_BUTTON_GROUP_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpList.h"

TP_DEF_VOID_TYPE_VAR(ItpRadioButtonGroupData);

class tpRadioButton;

/// @brief 单选按钮组；组内单选按钮只能被选中一个
class tpRadioButtonGroup
{
public:
    tpRadioButtonGroup();
    virtual ~tpRadioButtonGroup();

    /// @brief 添加一个单选按钮
    /// @param radioBtn 按钮指针
    /// @param id 按钮ID，不给入则自动分配；给入ID若重复则会重新分配
    /// @return 按钮ID
    int32_t addButton(tpRadioButton *radioBtn, const int32_t &id = -1);

    /// @brief 获取组内所有按钮信息
    /// @return 按钮列表
    tpList<tpRadioButton *> buttons();

    /// @brief 指定按钮ID获取按钮指针
    /// @param id 按钮ID
    /// @return 按钮指针；ID不存在则返回空指针
    tpRadioButton *button(const int32_t &id);
    /// @brief 指定按钮指针获取按钮ID
    /// @param button 按钮指针
    /// @return 按钮ID
    int32_t id(tpRadioButton *button) const;

    /// @brief 获取当前选中的单选按钮
    /// @return 按钮指针，若组内没有按钮，则返回空指针
    tpRadioButton *checkedButton() const;
    /// @brief 获取当前选中的按钮ID
    /// @return 按钮ID；若组内没有按钮，会返回-1
    int32_t checkedId() const;

public
signals:
    /// @brief 按钮点击事件，鼠标释放触发
    /// @param tpRadioButton 按钮指针
    declare_signal(buttonClicked, tpRadioButton *);

private:
    ItpRadioButtonGroupData *data_;
};

#endif
