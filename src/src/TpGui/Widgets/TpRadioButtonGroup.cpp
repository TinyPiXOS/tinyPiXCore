#include "TpRadioButtonGroup.h"
#include "TpMap.h"
#include "TpRadioButton.h"

struct TpRadioBtnInfo
{
    TpRadioButton *radioBtn;
    LambdaConnectionManager::ConnectionID slotFuncId;
};

struct TpRadioButtonGroupData
{
    TpMap<int32_t, TpRadioBtnInfo> btnMapper;
    int32_t curBtnId = 0;

    void slotButtonClicked(TpRadioButton *clickBtn)
    {
        for (const auto &radioBtnInfo : btnMapper.values())
        {
            radioBtnInfo.radioBtn->setChecked(clickBtn == radioBtnInfo.radioBtn);
        }
    }
};

TpRadioButtonGroup::TpRadioButtonGroup()
{
    TpRadioButtonGroupData *groupData = new TpRadioButtonGroupData();

    data_ = groupData;
}

TpRadioButtonGroup::~TpRadioButtonGroup()
{
    TpRadioButtonGroupData *groupData = static_cast<TpRadioButtonGroupData *>(data_);
    if (groupData)
    {
        // 解绑信号槽
        for (const auto &curBtnInfo : groupData->btnMapper.values())
        {
            disconnect(curBtnInfo.radioBtn, onClicked, curBtnInfo.slotFuncId);
        }
        groupData->btnMapper.clear();

        delete groupData;
        groupData = nullptr;
        data_ = nullptr;
    }
}

int32_t TpRadioButtonGroup::addButton(TpRadioButton *radioBtn, const int32_t &id)
{
    if (radioBtn == nullptr)
        return -1;

    TpRadioButtonGroupData *groupData = static_cast<TpRadioButtonGroupData *>(data_);

    int32_t curBtnId = id;
    if (curBtnId == -1)
        curBtnId = groupData->curBtnId++;

    radioBtn->setChecked(groupData->btnMapper.size() == 0);

        TpRadioBtnInfo radioBtnInfo;
    radioBtnInfo.radioBtn = radioBtn;

    radioBtnInfo.slotFuncId = connect(radioBtn, onClicked, [=](bool)
                                      { groupData->slotButtonClicked(radioBtn); });
    groupData->btnMapper[curBtnId] = radioBtnInfo;

    return curBtnId;
}

TpList<TpRadioButton *> TpRadioButtonGroup::buttons()
{
    TpList<TpRadioButton *> btnList;

    TpRadioButtonGroupData *groupData = static_cast<TpRadioButtonGroupData *>(data_);
    for (const auto &btnInfo : groupData->btnMapper.values())
    {
        btnList.emplace_back(btnInfo.radioBtn);
    }

    return btnList;
}

TpRadioButton *TpRadioButtonGroup::button(const int32_t &id)
{
    TpRadioButtonGroupData *groupData = static_cast<TpRadioButtonGroupData *>(data_);
    if (!groupData->btnMapper.contains(id))
        return nullptr;

    return groupData->btnMapper.value(id).radioBtn;
}

int32_t TpRadioButtonGroup::id(TpRadioButton *button) const
{
    TpRadioButtonGroupData *groupData = static_cast<TpRadioButtonGroupData *>(data_);

    for (const auto &btnIter : groupData->btnMapper)
    {
        if (btnIter.second.radioBtn == button)
            return btnIter.first;
    }

    return -1;
}

TpRadioButton *TpRadioButtonGroup::checkedButton() const
{
    TpRadioButtonGroupData *groupData = static_cast<TpRadioButtonGroupData *>(data_);
    for (const auto &curBtnInfo : groupData->btnMapper.values())
    {
        if (curBtnInfo.radioBtn->checked())
            return curBtnInfo.radioBtn;
    }
    return nullptr;
}

int32_t TpRadioButtonGroup::checkedId() const
{
    TpRadioButtonGroupData *groupData = static_cast<TpRadioButtonGroupData *>(data_);
    for (const auto &btnIter : groupData->btnMapper)
    {
        if (btnIter.second.radioBtn->checked())
            return btnIter.first;
    }
    return -1;
}
