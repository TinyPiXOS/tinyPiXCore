/* liucy has been here，but nothing to see and nothing left ^_^!*/

/*
** Copyright (c) 2007-2021 By Alexander.King.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/
#include "TpMessage.h"
#include <mutex>

#define TP_MESSAGE_WAIT 10000

struct MsgData
{
    int32_t id;
    ItpUserEvent data;
};

struct TpMessageData
{
    int32_t front;
    int32_t rear;
    int32_t length;
    MsgData *msg;
    std::mutex gMutex;
};

static int32_t msgObject = 0;

static inline void timerdelay(unsigned long long usec)
{
    struct timeval tv;
    tv.tv_sec = usec / 1000000;
    tv.tv_usec = usec % 1000000;
    int32_t err;
    do
    {
        err = select(0, NULL, NULL, NULL, &tv);
    } while (err < 0 && errno == EINTR);
}

TpMessage::TpMessage(int32_t length)
{
    TpMessageData *set = new TpMessageData();

    if (set)
    {
        set->front = 0;
        set->rear = 0;

        set->length = length;

        if (set->length <= 0)
        {
            set->length = TP_MESSAGE_LENGTH;
        }

        set->msg = new MsgData[set->length];

        this->data_ = set;
    }
}

TpMessage::~TpMessage()
{
    TpMessageData *set = (TpMessageData *)this->data_;

    if (set)
    {
        set->gMutex.lock();
        delete[] set->msg;
        delete set;
        set->gMutex.unlock();
    }
}

static inline bool isFull(TpMessageData *set)
{
    return (set->front == (set->rear + 1) % set->length);
}

static inline bool isEmpty(TpMessageData *set)
{
    return (set->front == set->rear);
}

bool TpMessage::send(ItpUserEvent *message)
{
    TpMessageData *set = (TpMessageData *)this->data_;

    bool ret = isFull(set);

    if (ret ||
        message == nullptr)
    {
        return false;
    }

    set->gMutex.lock();

    set->msg[set->rear].id = msgObject++;
    set->msg[set->rear].data = *message;
    set->rear = (set->rear + 1) % set->length;

    set->gMutex.unlock();

    return true;
}

bool TpMessage::sendWait(ItpUserEvent *message)
{
    bool ret = false;

    do
    {
        ret = this->send(message);
    } while (ret == false);

    return ret;
}

bool TpMessage::recvWait(ItpUserEvent *message)
{
    bool ret = false;

    while (ret == false)
    {
        ret = this->recv(message);

        if (ret == false)
        {
            timerdelay(TP_MESSAGE_WAIT);
        }
    }

    return ret;
}

bool TpMessage::recv(ItpUserEvent *message)
{
    TpMessageData *set = (TpMessageData *)this->data_;

    if (set == nullptr ||
        isEmpty(set) ||
        message == nullptr)
    {
        return false;
    }

    set->gMutex.lock();

    *message = set->msg[set->front].data;
    set->front = (set->front + 1) % set->length;

    set->gMutex.unlock();

    return true;
}

void TpMessage::clear()
{
    TpMessageData *set = (TpMessageData *)this->data_;

    set->gMutex.lock();
    set->front = set->rear;
    set->gMutex.unlock();
}
