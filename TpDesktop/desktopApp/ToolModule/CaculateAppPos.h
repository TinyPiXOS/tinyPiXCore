#ifndef __CACUALTE_BAR_POS_H
#define __CACUALTE_BAR_POS_H

/*
    用于拖拽APP过程中计算APP吸附位置及移动其他APP图标
*/

#include "TpVector.h"
#include "TpIconTopButton.h"

class CaculateAppPos
{
public:
    CaculateAppPos();
    virtual ~CaculateAppPos();

    // 设置当前在第几页移动的
    void setCurPage(const int32_t& pageNum, const TpVector<TpIconTopButton *>& curPageAppList);

    // 所有行的Y坐标; 内部会根据页码自动叠加偏移量
    void setRowY(const TpVector<int32_t>& yList);

    // 设置所有列的X; 内部会根据页码自动叠加偏移量
    void setColumnX(const TpVector<int32_t>& xList);

    // 根据拖拽按钮坐标，计算是否偏移所有其他按钮腾出位置
    void moveBtn(const TpPoint& curPoint);

    // 鼠标释放按钮，自动吸附至最近的网格
    void releaseBtn();

private:
    // 当前正在移动的按钮
    TpIconTopButton *curMoveBtn_;

    int32_t curPage_;
    // 当前页的APP列表；从左到右依次放置的
    TpVector<TpIconTopButton *> appList_;
};

#endif