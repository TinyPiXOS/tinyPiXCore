#include "CaculateAppPos.h"

CaculateAppPos::CaculateAppPos()
    : curPage_(0)
{
}

CaculateAppPos::~CaculateAppPos()
{
}

void CaculateAppPos::setCurPage(const int32_t &pageNum, const TpVector<TpIconTopButton *> &curPageAppList)
{
    curPage_ = pageNum;
    appList_ = curPageAppList;
}

void CaculateAppPos::setRowY(const TpVector<int32_t> &yList)
{
}

void CaculateAppPos::setColumnX(const TpVector<int32_t> &xList)
{
}

void CaculateAppPos::moveBtn(const TpPoint &curPoint)
{
}

void CaculateAppPos::releaseBtn()
{
}
