#ifndef __TP_INPUTMETHOD_H
#define __TP_INPUTMETHOD_H

#include "tpSignalSlot.h"
#include "tpVector.h"
#include "tpString.h"

/*
    输入法　0.9
*/
TP_DEF_VOID_TYPE_VAR(ItpInputMethodData);
class tpInputMethod
{
public:
    tpInputMethod();
    virtual ~tpInputMethod();

    /// @brief 输入拼音字符;每次调用会追加字符；ex: 第一次输入z；第二次输入h；处理结果zh
    void inputKeySequence(const tpString& inputSequence);

    /// @brief 获取当前输入的拼音字符
    /// @return 拼音字符串
    tpString keySequence();

    /// @brief 重置输入字符
    void clear();

    /// @brief 获取当前页候选词列表
    /// @return 候选词列表
    tpVector<tpString> currentPageCandidates();

    /// @brief 获取当前输入拼音的所有候选词
    /// @return 候选词列表
    tpVector<tpString> allCandidates();

    /// @brief 获取当前页索引
    /// @return 候选词列表
    int32_t currentPage();

    /// @brief 上一页
    /// @return 执行结果
    bool previousPage();

    /// @brief 下一页
    /// @return 执行结果
    bool nextPage();
    
    /// @brief 是否还有下一页
    /// @return 还有下一页返回true否则返回false
    bool hasNext();

private:
    ItpInputMethodData *data_;
};

#endif