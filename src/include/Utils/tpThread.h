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
#ifndef __TP_THREAD_H
#define __TP_THREAD_H

#include "tpUtils.h"

TP_DEF_VOID_TYPE_VAR(IPiThreadSet);

class tpThread
{
public:
	tpThread();
	virtual ~tpThread();

	virtual bool start();
	virtual void terminate();
	virtual void stop();

	virtual void setRunOnce(bool runOnce);
	virtual bool getRunOnce();

	virtual int32_t getThreadID();
	virtual bool isFinished();
	virtual bool isRunning();

	virtual void run();
public:
	static bool setMask(int32_t signalMask, int32_t threadMask);
private:
	IPiThreadSet *threadSet;
};



#endif
