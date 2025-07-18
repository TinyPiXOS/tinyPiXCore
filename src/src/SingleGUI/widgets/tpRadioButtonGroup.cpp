#include "tpRadioButtonGroup.h"
#include "tpMap.h"

struct tpRadioButtonGroupData
{
    tpMap<int32_t, tpRadioButton *> btnMapper;
    int32_t curBtnId = 0;
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

    groupData->btnMapper[curBtnId] = radioBtn;

    return curBtnId;
}

tpList<tpRadioButton *> tpRadioButtonGroup::buttons()
{
    tpRadioButtonGroupData *groupData = static_cast<tpRadioButtonGroupData *>(data_);
    return groupData->btnMapper.values();
}

tpRadioButton *tpRadioButtonGroup::button(const int32_t &id)
{
    tpRadioButtonGroupData *groupData = static_cast<tpRadioButtonGroupData *>(data_);
    if (!groupData->btnMapper.contains(id))
        return nullptr;

    return groupData->btnMapper.value(id);
}

int32_t tpRadioButtonGroup::id(tpRadioButton *button) const
{
    tpRadioButtonGroupData *groupData = static_cast<tpRadioButtonGroupData *>(data_);
    return groupData->btnMapper.key(button);
}

tpRadioButton *tpRadioButtonGroup::checkedButton() const
{
    return nullptr;
}

int32_t tpRadioButtonGroup::checkedId() const
{
    return 0;
}
