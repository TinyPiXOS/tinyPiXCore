#ifndef __TP_MESSAGE_H
#define __TP_MESSAGE_H

#include <TpUtils.h>
#include "TpCDef.h"

/**
 * @def TP_MESSAGE_LENGTH
 * @brief 消息队列的默认长度，值为10240
 * @note 当构造函数参数length小于等于0时，将使用此默认值
 */
#define TP_MESSAGE_LENGTH 10240

TP_DEF_VOID_TYPE_VAR(ITpMessageData);
/**
 * @class TpMessage
 * @brief 线程安全的消息队列类，实现基于循环队列的消息存储和传递
 * @details
 * 该类提供了一个线程安全的消息队列，支持阻塞和非阻塞的消息发送与接收。
 * 使用互斥锁保证多线程环境下的数据安全，采用循环队列结构提高内存使用效率。
 * @note 该类非拷贝构造，不可复制
 * @warning 在多线程环境下使用时应确保正确的生命周期管理
 */
class TpMessage
{
public:
    /**
     * @brief 构造函数，初始化消息队列
     * @param[in] length 消息队列长度，默认为TP_MESSAGE_LENGTH
     * @note 如果length参数小于等于0，将使用TP_MESSAGE_LENGTH作为默认值
     */
    TpMessage(int32_t length = TP_MESSAGE_LENGTH);
    
    /**
     * @brief 析构函数，释放消息队列资源
     */
    virtual ~TpMessage();

public:
    /**
     * @brief 发送消息（非阻塞模式）
     * @param[in] message 要发送的消息指针
     * @return bool 发送成功返回true，失败返回false
     * @retval true 消息成功加入队列
     * @retval false 队列已满或消息指针为空
     * @note 此函数立即返回，不会等待队列有空闲位置
     */
    virtual bool send(ItpUserEvent *message);
    
    /**
     * @brief 发送消息（阻塞模式，直到成功）
     * @param[in] message 要发送的消息指针
     * @return bool 总是返回true（因为会一直重试直到成功）
     * @warning 如果队列始终满，此函数将无限期阻塞
     */
    virtual bool sendWait(ItpUserEvent *message);
    
    /**
     * @brief 接收消息（阻塞模式）
     * @param[out] message 接收消息的缓冲区指针
     * @return bool 接收成功返回true
     * @note 如果队列为空，此函数将阻塞等待直到有消息到达
     */
    virtual bool recvWait(ItpUserEvent *message);
    
    /**
     * @brief 接收消息（非阻塞模式）
     * @param[out] message 接收消息的缓冲区指针
     * @return bool 接收成功返回true，失败返回false
     * @retval true 成功接收到消息
     * @retval false 队列为空或消息指针为空
     */
    virtual bool recv(ItpUserEvent *message);
    
    /**
     * @brief 清空消息队列
     * @note 此操作会删除队列中的所有消息
     */
    virtual void clear();

public:
    ITpMessageData *data_;
};

#endif
