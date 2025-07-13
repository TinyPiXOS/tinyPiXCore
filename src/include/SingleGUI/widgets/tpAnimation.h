#ifndef __TP_ANIMATION_H
#define __TP_ANIMATION_H

#include "tpChildWidget.h"
#include "tpString.h"
#include "tpSignalSlot.h"

#include <functional>

TP_DEF_VOID_TYPE_VAR(ItpAnimationData);
/// @brief 动画类；使用指针对象，不要使用对象变量
class tpAnimation : public tpObject
{
public:
    enum AnimationType
    {
        /// @brief 几何属性；位置和大小 ItpRect
        Geometry,
        /// @brief 位置​​   ItpPoint
        Pos,
        /// @brief 大小​​   ItpSize
        Size,
        /// @brief 窗体高度 int32_t
        Height,
        /// @brief 窗体宽度 int32_t
        Width,

        /// @brief 整体透明度​​（0透明，255不透明 int32_t
        WindowOpacity,

        /// @brief 背景颜色 int32_t
        BackgroundColor,

        /// @brief 自定义动画 ，数值自定义； 暂未启用
        CustomAnimation
    };
    typedef std::function<void(const tpVariant &)> CustomAnimationFunc;

    enum DeletionPolicy
    {
        /// @brief 动画停止时不释放指针
        KeepWhenStopped = 0,
        /// @brief 动画停止时释放指针
        DeleteWhenStopped
    };

public:
    /// @brief 动画类构造函数
    /// @param target 动画绑定目标
    /// @param propertyType 动画类型
    tpAnimation(tpChildWidget *target, const AnimationType &propertyType);

    virtual ~tpAnimation();

    /// @brief 设置动画绑定窗体
    /// @param target 绑定窗体指针
    void setTargetWidget(tpChildWidget *target);

    /// @brief 获取当前动画绑定窗体
    /// @return 窗体指针
    tpChildWidget *targetWidget();

    /// @brief 设置动画循环次数，默认为1次
    /// @param count -1为无限循环，需要手动终止动画
    void setLoopCount(const int32_t count);

    /// @brief 获取动画循环次数
    /// @return 循环次数
    int32_t loopCount();

    /// @brief 设置自定义动画处理函数,暂未启用
    /// @param func 自定义动画值修改函数
    void setCustomAnimationFunc(CustomAnimationFunc func);

    /// @brief 设置动画持续时间
    /// @param timeMs 时间，单位ms
    void setDuration(const uint32_t &timeMs);
    /// @brief 获取动画持续时间
    /// @return 时间，单位ms
    uint32_t duration();

    /// @brief 设置动画初始值
    /// @param value 初始值，需按照动画对应的数据类型设置
    void setStartValue(const tpVariant &value);
    /// @brief 获取动画初始值
    /// @return 初始值
    tpVariant startValue();

    /// @brief 设置动画终止值
    /// @param value 需按照动画对应的数据类型设置
    void setEndValue(const tpVariant &value);
    /// @brief 获取动画终止值
    /// @return 终止值
    tpVariant endValue();

    /// @brief 启动动画
    /// @param runMode 启动模式，默认为动画计算完毕释放指针
    void start(const DeletionPolicy &runMode = DeleteWhenStopped);

    /// @brief 暂停动画
    void pause();

    /// @brief 停止动画
    void stop();

    /// @brief 设置在给定步骤使用给定值创建关键帧。给定的步长必须在0到1的范围内
    /// @param percent 指定步骤 (0, 1), 为0则会修改StartValue，为1则会修改endValue
    /// @param value 步骤对应值
    void setKeyValueAt(const float& percent, const tpVariant& value);

public
signals:
    /// @brief 按钮点击信号槽，鼠标释放时触发
    /// @param bool 按钮选中状态
    declare_signal(finished);

private:
    void AnimationRun();

private:
    ItpAnimationData *data_;
};

#endif
