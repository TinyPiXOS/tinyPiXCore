#ifndef __TP_EVENT_SOURCE_H
#define __TP_EVENT_SOURCE_H


#include <cstdint>

class TpEventLoop;

class TpEventSource {
public:
    enum Event : uint32_t {
        None      = 0,
        Read      = 1 << 0,
        Write     = 1 << 1,
        Error     = 1 << 2,
        Hangup    = 1 << 3,
        Timeout   = 1 << 4,
        User      = 1 << 16
    };

	TpEventSource();
    virtual ~TpEventSource();

    // ===== 必须实现 =====
    virtual int fd() const = 0;
    virtual uint32_t events() const = 0;
    virtual void onEvent(uint32_t revents) = 0;

    // ===== 状态控制 =====
    bool isEnabled() const;
    void setEnabled(bool enable);

    /// @brief 绑定事件源到事件循环
    /// @param loop 
	/// @return 失败返回-1，成功返回0
    int attach(TpEventLoop *loop);
	/// @brief 解绑事件源
    void detach();

protected:
	void autoAttachIfNeeded();

private:
    TpEventLoop *loop_{nullptr};
    bool enabled_{true};

    friend class TpEventLoop;
};


#endif