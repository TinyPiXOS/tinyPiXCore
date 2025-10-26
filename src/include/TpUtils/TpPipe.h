/**
 * @file TpPipe.h
 * @brief 定义用于进程间通信的命名管道（FIFO）类 TpPipe
 * @note 该类提供了基于主题的消息发送和接收功能，简化了命名管道的使用
 */

#ifndef __TP_PIPE_H
#define __TP_PIPE_H

#include "TpString.h"
#include "TpVector.h"

TP_DEF_VOID_TYPE_VAR(ITpPipeData);
/**
 * @class TpPipe
 * @brief 命名管道封装类，用于进程间通信（IPC）
 * 此类封装了Unix/Linux命名管道（FIFO）的创建、打开、读写和关闭操作。
 * 它支持以阻塞模式进行读写，并提供了简单的消息序列化格式（长度+数据）。
 */
class TpPipe
{
public:
    /// @brief 管道打开模式枚举
    enum Mode
    {
        /// @brief 以只读模式打开管道，用于接收数据
        Read,
        /// @brief 以只写模式打开管道，用于发送数据
        Write
    };

    /// 从管道接收到的数据结构
    struct PipeData
    {
        /// @brief 消息主题
        TpString topic;
        /// @brief 消息负载数据
        TpVector<char> data;
    };

    /**
     * @brief 构造函数，创建或打开一个命名管道
     * @param pipePath 管道的文件系统路径
     * @param mode 管道模式（读或写）
     * @param isBlock 是否为阻塞模式；以阻塞模式打开管道。读取端打开时会阻塞直到写入端也被打开，反之亦然。
     * @throw std::system_error 当管道创建或打开失败时抛出
     */
    TpPipe(const TpString &pipePath, Mode mode, bool isBlock = false);

    /// 析构函数，自动关闭管道文件描述符
    ~TpPipe();

    /**
     * @brief 向管道发送一条消息
     * @param topic 消息主题
     * @param data 消息数据指针
     * @param dataLength 消息数据长度
     * @throw std::system_error 当写入操作失败时抛出
     * @note 消息格式：主题长度(uint32_t) -> 主题数据 -> 数据长度(uint32_t) -> 负载数据
     */
    void send(const TpString &topic, const char *data, uint32_t dataLength);

    /**
     * @brief 从管道接收一条消息
     * @return PipeData 包含主题和数据的结构体
     * @note 这是一个阻塞调用。如果没有数据可用，调用线程将阻塞直到数据到达或管道写入端关闭。
     *       如果读取过程中发生错误（如管道破裂）或遇到文件结束（写入端关闭），返回的PipeData对象可能包含部分数据或为空。
     */
    PipeData recv();

private:
    ITpPipeData *data_;
};
#endif // TP_PIPE_H