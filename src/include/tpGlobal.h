/***
 * @Author: hywang
 * @Date: 2024-05-23 16:06:15
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-06 09:45:10
 * @FilePath: /pix-singlegui/PixSingleGUI/include/tpGlobal.h
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
// inline void tpSwap(T &value1, T &value2)
//     noexcept(noexcept(SwapExceptionTester::checkSwap(value1)))
// {
//     using std::swap;
//     swap(value1, value2);
// }

#define tpShared std::shared_ptr
#define tpWeakPtr std::weak_ptr
#define tpMakeShared std::make_shared

namespace tinyPiX
{
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
        // AlignJustify = 0x0008,
        // AlignAbsolute = 0x0010,

        AlignTop = 0x0020,
        AlignBottom = 0x0040,
        AlignVCenter = 0x0080,
        // AlignBaseline = 0x0100,

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
        Default,
        PurpleLight
    };

    /// @brief 虚拟键盘输入类型
    enum VirtualKeyboardInputType
    {
        Pinyin,  // 中文输入的拼音
        Chinese, // 输入的汉字
        Delete,  // 删除
        Symbol   // 输入的字符和字母
    };
};

#endif
