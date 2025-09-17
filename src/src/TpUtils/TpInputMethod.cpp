#include "TpInputMethod.h"
#include "TpPainter.h"
#include "TpChildWidget.h"
#include "TpApp.h"
#include "rime/rime_api.h"
#include <thread>

struct TpInputMethodData
{
    RimeApi *rimeApi;
    RimeSessionId sessionId;

    RimeContext *context;
    std::mutex contextMutex;   // 添加互斥锁
    bool contextValid = false; // 标记上下文有效性

    TpString cachePinyin = "";
};

// 刷新上下文
void refreshContext(TpInputMethodData *inputData)
{
    std::lock_guard<std::mutex> lock(inputData->contextMutex);

    // 安全释放现有上下文
    if (inputData->context)
    {
        inputData->rimeApi->free_context(inputData->context);
    }

    // 准备新上下文
    memset(inputData->context, 0, sizeof(RimeContext));
    inputData->context->data_size = sizeof(RimeContext);

    // 获取最新上下文
    inputData->contextValid = inputData->rimeApi->get_context(
        inputData->sessionId,
        inputData->context);
}

TpInputMethod::TpInputMethod()
{
    TpInputMethodData *inputData = new TpInputMethodData();
    data_ = inputData;

    // 初始化成员变量
    inputData->context = nullptr;
    inputData->contextValid = false;

    // 1. 创建自定义的配置特性
    RimeTraits traits = {0}; // 初始化清零

    // 设置应用基本信息
    traits.app_name = "tinyPiXOS";                      // 应用名称
    traits.shared_data_dir = "/usr/data/tinyPiX/rime/"; // 修改共享数据目录
    traits.user_data_dir = "/usr/data/tinyPiX/rime/";   // 修改用户数据目录
    traits.min_log_level = 1;
    traits.log_dir = "usr/data/tinyPiX/rime/log/";
    traits.data_size = sizeof(RimeTraits);

    // 设置分发信息（可选）
    traits.distribution_name = "MyCustomRime";
    traits.distribution_code_name = "custom_rime";
    traits.distribution_version = "1.0.0";

    // 应用自定义配置进行初始化
    inputData->rimeApi = rime_get_api();
    inputData->rimeApi->setup(&traits);
    inputData->rimeApi->initialize(&traits);

    // 创建会话
    inputData->sessionId = inputData->rimeApi->create_session();

    inputData->context = new RimeContext();
    memset(inputData->context, 0, sizeof(RimeContext));
    inputData->context->data_size = sizeof(RimeContext);
}

TpInputMethod::~TpInputMethod()
{
    TpInputMethodData *inputData = static_cast<TpInputMethodData *>(data_);

    // 加锁保护
    std::lock_guard<std::mutex> lock(inputData->contextMutex);
    if (inputData->context)
    {
        inputData->rimeApi->free_context(inputData->context);
        delete inputData->context;
        inputData->context = nullptr;
    }

    if (inputData->sessionId)
    {
        inputData->rimeApi->destroy_session(inputData->sessionId);
    }

    inputData->rimeApi->finalize();

    delete inputData;
    inputData = nullptr;
    data_ = nullptr;
}

void TpInputMethod::inputKeySequence(const TpString &inputSequence)
{
    TpInputMethodData *inputData = static_cast<TpInputMethodData *>(data_);

    // 加锁确保操作原子性
    {
        std::lock_guard<std::mutex> lock(inputData->contextMutex);

        inputData->cachePinyin += inputSequence;
        // 处理输入
        inputData->rimeApi->simulate_key_sequence(inputData->sessionId, inputSequence.c_str());

        // 等待引擎处理
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    refreshContext(inputData);
}

TpString TpInputMethod::keySequence()
{
    TpInputMethodData *inputData = static_cast<TpInputMethodData *>(data_);
    return inputData->cachePinyin;
}

void TpInputMethod::clear()
{
    TpInputMethodData *inputData = static_cast<TpInputMethodData *>(data_);

    {
        std::lock_guard<std::mutex> lock(inputData->contextMutex);

        inputData->cachePinyin = "";

        // 发送 Escape 键
        inputData->rimeApi->process_key(inputData->sessionId, 0xFF1B, 0); // Escape 键码
    }

    // 可选：刷新上下文
    refreshContext(inputData);
}

TpVector<TpString> TpInputMethod::currentPageCandidates()
{
    TpInputMethodData *inputData = static_cast<TpInputMethodData *>(data_);

    TpVector<TpString> candidatesList;

    {
        // 加锁保护
        std::lock_guard<std::mutex> lock(inputData->contextMutex);

        // 确保上下文有效
        if (!inputData->contextValid)
        {
            return candidatesList;
        }
    }

    // 遍历当前页所有候选词
    for (int i = 0; i < inputData->context->menu.num_candidates; ++i)
    {
        candidatesList.emplace_back(inputData->context->menu.candidates[i].text);
    }

    return candidatesList;
}

TpVector<TpString> TpInputMethod::allCandidates()
{
    TpInputMethodData *inputData = static_cast<TpInputMethodData *>(data_);

    TpVector<TpString> candidatesList;

    // 加锁保护
    // std::lock_guard<std::mutex> lock(inputData->contextMutex);

    // 确保上下文有效
    if (!inputData->contextValid)
    {
        return candidatesList;
    }

    // 遍历当前页所有候选词
    for (int i = 0; i < inputData->context->menu.num_candidates; ++i)
    {
        candidatesList.emplace_back(inputData->context->menu.candidates[i].text);
    }

    // 检查是否可以下翻页
    while ((inputData->context->menu.page_size > 0) && (!inputData->context->menu.is_last_page))
    {
        // 模拟按下 PAGE_DOWN 键翻页
        int keycode = 0xff56; // PAGE_DOWN
        inputData->rimeApi->process_key(inputData->sessionId, keycode, 0);

        // 刷新上下文
        refreshContext(inputData);

        // 遍历当前页所有候选词
        for (int i = 0; i < inputData->context->menu.num_candidates; ++i)
        {
            candidatesList.emplace_back(inputData->context->menu.candidates[i].text);
        }
    }

    // 重置为首页
    TpString cachePinyin = inputData->cachePinyin;
    clear();
    inputKeySequence(cachePinyin);

    return candidatesList;
}

int32_t TpInputMethod::currentPage()
{
    TpInputMethodData *inputData = static_cast<TpInputMethodData *>(data_);

    // 加锁保护
    std::lock_guard<std::mutex> lock(inputData->contextMutex);
    if (!inputData->contextValid)
        return 0;

    return inputData->context->menu.page_no;
}

bool TpInputMethod::previousPage()
{
    TpInputMethodData *inputData = static_cast<TpInputMethodData *>(data_);

    {
        // 加锁保护
        std::unique_lock<std::mutex> lock(inputData->contextMutex);

        if (!inputData->contextValid)
            return false;

        // 检查是否可以上翻页
        if (inputData->context->menu.page_no <= 0)
        {
            return false;
        }
    }

    // 模拟按下 PAGE_UP 键翻页
    int keycode = 0xff55; // PAGE_UP
    inputData->rimeApi->process_key(inputData->sessionId, keycode, 0);

    // 等待翻页完成
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    // 刷新上下文
    refreshContext(inputData);

    return true;
}

bool TpInputMethod::nextPage()
{
    TpInputMethodData *inputData = static_cast<TpInputMethodData *>(data_);

    {
        // 加锁保护
        std::unique_lock<std::mutex> lock(inputData->contextMutex);

        if (!inputData->contextValid)
            return false;

        // 检查是否可以下翻页
        if (inputData->context->menu.is_last_page)
        {
            return false;
        }
    }

    // 模拟按下 PAGE_DOWN 键翻页
    int keycode = 0xff56; // PAGE_DOWN
    inputData->rimeApi->process_key(inputData->sessionId, keycode, 0);

    // 等待翻页完成
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    // 刷新上下文
    refreshContext(inputData);

    return true;
}

bool TpInputMethod::hasNext()
{
    TpInputMethodData *inputData = static_cast<TpInputMethodData *>(data_);

    // 加锁保护
    std::lock_guard<std::mutex> lock(inputData->contextMutex);

    if (!inputData->contextValid)
        return false;

    return !inputData->context->menu.is_last_page;
}
