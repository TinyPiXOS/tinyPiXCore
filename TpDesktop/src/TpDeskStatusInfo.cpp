#include "TpDeskStatusInfo.h"
#include "TpApp.h"

struct TpDeskStatusInfoData
{
    // 桌面信息；无桌面则数据无用
    TpDeskStatusBarInfo deskStatusBarInfo_;
};

TpDeskStatusInfo *TpDeskStatusInfo::Instance()
{
    static TpDeskStatusInfo instance;
    return &instance;
}

const TpDeskStatusBarInfo &TpDeskStatusInfo::statusInfo()
{
    TpDeskStatusInfoData *statusData = static_cast<TpDeskStatusInfoData *>(data_);
    return statusData->deskStatusBarInfo_;
}

void TpDeskStatusInfo::setStatusInfo(const TpDeskStatusBarInfo &info)
{
    TpDeskStatusInfoData *statusData = static_cast<TpDeskStatusInfoData *>(data_);
    statusData->deskStatusBarInfo_ = info;
}

TpDeskStatusInfo::TpDeskStatusInfo()
{
    TpDeskStatusInfoData *statusData = new TpDeskStatusInfoData();
    data_ = statusData;
}

TpDeskStatusInfo::~TpDeskStatusInfo()
{
    TpDeskStatusInfoData *statusData = static_cast<TpDeskStatusInfoData *>(data_);
    if (statusData)
    {
        delete statusData;
        statusData = nullptr;
        data_ = nullptr;
    }
}
