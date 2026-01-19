#ifndef __TP_ANIMATION_H
#define __TP_ANIMATION_H

#include "TpWidget.h"
#include "TpString.h"
#include "TpSignalSlot.h"

#include <functional>

TP_DEF_VOID_TYPE_VAR(ITpAnimationData);
/// @brief 动画类；使用指针对象，不要使用对象变量
class TpAnimation : public TpObject
{
public:
    enum AnimationType
    {
        /// @brief 几何属性；位置和大小 TpRect
        Geometry,
        /// @brief 位置​​ TpPoint
        Pos,
        /// @brief 大小​ TpSize
        Size,
        /// @brief 窗体高度 int32_t
        Height,
        /// @brief 窗体宽度 int32_t
        Width,
        /// @brief 整体不透明度​​（0透明，1不透明 float
        WindowOpacity,
        /// @brief 背景颜色 int32_t
        BackgroundColor,
        /// @brief 自定义动画;支持属性函数参数类型：整形、浮点型、TpRect、TpPoint、TpSize
        CustomAnimation
    };
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
    TpAnimation(TpWidget *target, const AnimationType &propertyType);

    /// @brief 创建自定义动画；需目标对象注册对应属性；参考 TpProperty
    /// @param target 动画绑定目标
    /// @param propertyName 动画关联属性名称
    TpAnimation(TpWidget *target, const TpString &propertyName);

    virtual ~TpAnimation();

    /// @brief 设置动画绑定窗体
    /// @param target 绑定窗体指针
    void setTargetWidget(TpWidget *target);

    /// @brief 获取当前动画绑定窗体
    /// @return 窗体指针
    TpWidget *targetWidget();

    /// @brief 设置属性名称；设置后动画类型为自定义动画；需目标对象注册对应属性；参考 TpProperty
    /// @param propertyName 动画关联属性名称
    void setPropertyName(const TpString &propertyName);

    /// @brief 获取当前设置的属性名称
    /// @return 
    TpString propertyName();

    /// @brief 设置动画循环次数，默认为1次
    /// @param count -1为无限循环，需要手动终止动画
    void setLoopCount(const int32_t count);

    /// @brief 获取动画循环次数
    /// @return 循环次数
    int32_t loopCount();

    /// @brief 设置动画持续时间
    /// @param timeMs 时间，单位ms
    void setDuration(const uint32_t &timeMs);
    /// @brief 获取动画持续时间
    /// @return 时间，单位ms
    uint32_t duration();

    /// @brief 设置动画初始值
    /// @param value 初始值，需按照动画对应的数据类型设置
    void setStartValue(const TpVariant &value);
    /// @brief 获取动画初始值
    /// @return 初始值
    TpVariant startValue();

    /// @brief 设置动画终止值
    /// @param value 需按照动画对应的数据类型设置
    void setEndValue(const TpVariant &value);
    /// @brief 获取动画终止值
    /// @return 终止值
    TpVariant endValue();

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
    void setKeyValueAt(const float &percent, const TpVariant &value);

public
signals:
    /// @brief 动画完成信号
    declare_signal(finished);

private:
    void AnimationRun();

private:
    ITpAnimationData *data_;
};

#endif
