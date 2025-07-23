#include "tpRadioButtonGroup.h"
#include "tpMap.h"
#include "tpRadioButton.h"

struct tpRadioBtnInfo
{
    tpRadioButton *radioBtn;
    LambdaConnectionManager::ConnectionID slotFuncId;
};

struct tpRadioButtonGroupData
{
    tpMap<int32_t, tpRadioBtnInfo> btnMapper;
    int32_t curBtnId = 0;

    void slotButtonClicked(tpRadioButton *clickBtn)
    {
        for (const auto &radioBtnInfo : btnMapper.values())
        {
            radioBtnInfo.radioBtn->setChecked(clickBtn == radioBtnInfo.radioBtn);
        }
    }
};

tpRadioButtonGroup::tpRadioButtonGroup()
{
    tpRadioButtonGroupData *groupData = new tpRadioButtonGroupData();

    data_ = groupData;
}

tpRadioButtonGroup::~tpRadioButtonGroup()
{
    tpRadioButtonGroupData *groupData = static_cast<tpRadioButtonGroupData *>(data_);
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

int32_t tpRadioButtonGroup::addButton(tpRadioButton *radioBtn, const int32_t &id)
{
    if (radioBtn == nullptr)
        return -1;

    tpRadioButtonGroupData *groupData = static_cast<tpRadioButtonGroupData *>(data_);

    int32_t curBtnId = id;
    if (curBtnId == -1)
        curBtnId = groupData->curBtnId++;

    radioBtn->setChecked(groupData->btnMapper.size() == 0);

        tpRadioBtnInfo radioBtnInfo;
    radioBtnInfo.radioBtn = radioBtn;

    radioBtnInfo.slotFuncId = connect(radioBtn, onClicked, [=](bool)
                                      { groupData->slotButtonClicked(radioBtn); });
    groupData->btnMapper[curBtnId] = radioBtnInfo;

    return curBtnId;
}

tpList<tpRadioButton *> tpRadioButtonGroup::buttons()
{
    tpList<tpRadioButton *> btnList;

    tpRadioButtonGroupData *groupData = static_cast<tpRadioButtonGroupData *>(data_);
    for (const auto &btnInfo : groupData->btnMapper.values())
    {
        btnList.emplace_back(btnInfo.radioBtn);
    }

    return btnList;
}

tpRadioButton *tpRadioButtonGroup::button(const int32_t &id)
{
    tpRadioButtonGroupData *groupData = static_cast<tpRadioButtonGroupData *>(data_);
    if (!groupData->btnMapper.contains(id))
        return nullptr;

    return groupData->btnMapper.value(id).radioBtn;
}

int32_t tpRadioButtonGroup::id(tpRadioButton *button) const
{
    tpRadioButtonGroupData *groupData = static_cast<tpRadioButtonGroupData *>(data_);

    for (const auto &btnIter : groupData->btnMapper)
    {
        if (btnIter.second.radioBtn == button)
            return btnIter.first;
    }

    return -1;
}

tpRadioButton *tpRadioButtonGroup::checkedButton() const
{
    tpRadioButtonGroupData *groupData = static_cast<tpRadioButtonGroupData *>(data_);
    for (const auto &curBtnInfo : groupData->btnMapper.values())
    {
        if (curBtnInfo.radioBtn->checked())
            return curBtnInfo.radioBtn;
    }
    return nullptr;
}

int32_t tpRadioButtonGroup::checkedId() const
{
    tpRadioButtonGroupData *groupData = static_cast<tpRadioButtonGroupData *>(data_);
    for (const auto &btnIter : groupData->btnMapper)
    {
        if (btnIter.second.radioBtn->checked())
            return btnIter.first;
    }
    return -1;
}
