/***
 * @Author: hywang
 * @Date: 2024-05-23 16:06:15
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-06 09:45:10
 * @FilePath: /pix-singlegui/PixSingleGUI/include/TpGlobal.h
 * @Description: 全局头文件
 * @
 * @PiXOS
 */

#ifndef TP_ASSERT
#define TP_ASSERT(cond, content)                              \
    do                                                        \
    {                                                         \
        if (!(cond))                                          \
        {                                                     \
            std::cerr << "Assertion failed: " << content      \
                      << ", file: " << __FILE__               \
                      << ", line: " << __LINE__ << std::endl; \
            std::abort();                                     \
        }                                                     \
    } while (0)
#endif

#ifndef TO_STRING
#define TO_STRING(X) #X
#endif

#ifndef __TP_TPGLOBAL_H
#define __TP_TPGLOBAL_H

#include <iostream>
#include <memory>

// namespace SwapExceptionTester
// {
//     using std::swap;
//     template <typename T>
//     void checkSwap(T &t)
//         noexcept(noexcept(swap(t, t)));
// }

// template <typename T>
// inline void TpSwap(T &value1, T &value2)
//     noexcept(noexcept(SwapExceptionTester::checkSwap(value1)))
// {
//     using std::swap;
//     swap(value1, value2);
// }

#define tpShared std::shared_ptr
#define tpWeakPtr std::weak_ptr
#define tpUniquePtr std::unique_ptr
#define tpMakeShared std::make_shared

namespace Tp
{
    enum TpObjectType
    {
        TP_UNKOWN_OBJECT = -1,
        TP_FIXSCREEN_OBJECT,
        TP_MAIN_WINDOW_OBJECT,
        TP_FLOAT_OBJECT,
        TP_CHILD_OBJECT,
    };

    enum TpObjectSysLayer
    {
        TP_WM_NONE = -1,
        TP_WM_DESK,
        TP_WM_WIN,
        TP_WM_USE_FLOAT,
        TP_WM_SYS_FLOAT,
        TP_WM_LAYER_NUMBERS,
    };

    enum ConnectionType
    {
        /// @brief 自动连接，如果信号在主线程则为 DirectConnection，否则为 QueuedConnection
        AutoConnection,
        /// @brief 直接连接，槽函数会在信号发送线程被执行
        DirectConnection,
        /// @brief 队列连接，槽函数会在主线程执行
        QueuedConnection
    };

    enum SortOrder
    {
        AscendingOrder, // 升序
        DescendingOrder // 降序
    };

    enum MatchFlags
    {
        MatchExactly = 0,
        MatchContains = 1,
        MatchStartsWith = 2,
        MatchEndsWith = 3,
        MatchRegExp = 4,
        MatchWildcard = 5,
        MatchFixedString = 8,
        MatchCaseSensitive = 16,
        MatchWrap = 32,
        MatchRecursive = 64
    };

    enum AlignmentFlag
    {
        AlignLeft = 0x0001,
        AlignRight = 0x0002,
        AlignHCenter = 0x0004,

        AlignTop = 0x0020,
        AlignBottom = 0x0040,
        AlignVCenter = 0x0080,

        AlignCenter = AlignVCenter | AlignHCenter
    };

    enum ItemSelectionMode
    {
        SingleSelection,  // 单选
        MultiSelection,   // 多选
        ExtendedSelection // 按下ctrl、shift多选,否则单选
    };

    enum CheckState
    {
        Unchecked,
        PartiallyChecked,
        Checked
    };

    /// @brief 系统主题
    enum SystemTheme
    {
        /// @brief 默认GUI风格
        Default,
        /// @brief 智能设备GUI风格
        SmartDeviceGUIStyle
    };

    /// @brief 虚拟键盘输入类型
    enum VirtualKeyboardInputType
    {
        Pinyin,  // 中文输入的拼音
        Chinese, // 输入的汉字
        Delete,  // 删除
        Symbol   // 输入的字符和字母
    };

    /// @brief 画笔样式
    enum PenStyle
    {
        /// @brief 实线
        SolidLine,
        /// @brief 虚线
        DashLine,
        /// @brief 点虚线
        DotLine,
        /// @brief 线点虚线
        DashDotLine,
        /// @brief 线点点虚线
        DashDotDotLine,
    };

    /// @brief 笔触样式
    enum PenCapStyle
    {
        /// @brief 笔画在子路径的两个端点处精确结束。对于长度为零的子路径，不绘制笔画
        ButtCap = 0,
        /// @brief 笔画在子路径的两个端点处各延伸半个圆，半径等于笔画宽度的一半。对于长度为零的子路径，则绘制一个完整的圆
        RoundCap,
        /// @brief 在子路径的两个端点，笔划被矩形延伸，矩形的宽度等于笔划宽度，长度等于笔划宽度的一半。对于长度为零的子路径，则以笔划宽度的大小绘制一个正方形
        SquareCap
    };

    /// @brief 用于确定连接描边路径段角落处所使用样式的枚举
    enum PenJoinStyle
    {
        /// @brief 连接路径段的外角呈尖角状。尖角是通过将笔划外边缘的连接点延伸至相交处而形成的。如果延伸超出限制，则连接样式将转换为斜面样式
        MiterJoin = 0,
        /// @brief 连接路径段的外角是圆形的。圆形区域以连接点为中心
        RoundJoin,
        /// @brief 连接的路径段的外角在连接点处被倒角。角的三角形区域由每条笔划外角之间的直线包围
        BevelJoin
    };

    /// @brief 画刷样式
    enum BrushStyle
    {
        /// @brief 无填充
        NoBrush,
        /// @brief 纯色填充
        SolidPattern,
        /// @brief 密集点填充 暂未实现
        DensePattern,
        /// @brief 水平线填充 暂未实现
        HorPattern,
        /// @brief 垂直线填充 暂未实现
        VerPattern,
        /// @brief 水平垂直线交叉填充 暂未实现
        CrossPattern,
        /// @brief 后斜线填充 暂未实现
        BDiagPattern,
        /// @brief 前斜线填充 暂未实现
        FDiagPattern,
        /// @brief 斜线交叉填充 暂未实现
        DiagCrossPattern,
        /// @brief 线性渐变填充
        LinearGradientPattern,
        /// @brief 径向渐变填充
        RadialGradientPattern,
    };
};

#endif
