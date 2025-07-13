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
#ifndef __TP_MESSAGE_H
#define __TP_MESSAGE_H

#include <tpUtils.h>
#include "tpCDef.h"

#define TP_MESSAGE_LENGTH	10240

TP_DEF_VOID_TYPE_VAR(IPitpMsg);

class tpMessage
{
public:
	tpMessage(int32_t length = TP_MESSAGE_LENGTH);
	virtual ~tpMessage();
public:
	virtual bool send(ItpUserEvent *message);//message can be defined
	virtual bool sendWait(ItpUserEvent *message);//until success
	virtual bool recvWait(ItpUserEvent *message);//block mode
	virtual bool recv(ItpUserEvent *message);//none block mode
	virtual void clear();
public:
	IPitpMsg *msgSet;
};


#endif
