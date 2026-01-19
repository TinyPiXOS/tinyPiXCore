#ifndef __TP_BOX_LAYOUT_H
#define __TP_BOX_LAYOUT_H

#include "TpLayout.h"

TP_DEF_VOID_TYPE_VAR(ITpBoxLayoutData);
class TpBoxLayout
    : public TpLayout
{
public:
    enum Direction
    {
        LeftToRight, // 水平从左到右
        RightToLeft, // 水平从右到左
        TopToBottom, // 垂直从上到下
        BottomToTop  // 垂直从下到上
    };

    explicit TpBoxLayout(TpWidget *parent, Direction direction);
    virtual ~TpBoxLayout();

    // 设置布局方向
    void setDirection(Direction direction);
    Direction direction() const;

    // 动态插入和添加
    void addStretch(int stretch = 1);

	/// @brief 获取布局当前行数
	/// @return 布局行数
	virtual uint32_t rowCount() override;

	/// @brief 获取布局当前列数
	/// @return 布局列数
	virtual uint32_t columnCount() override;

private:
    ITpBoxLayoutData *data_;
};

#endif
