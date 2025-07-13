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
#ifndef __TP_MD5_H
#define __TP_MD5_H

#include "tpUtils.h"
#include <string>

TP_DEF_VOID_TYPE_VAR(IPitpMD5);

class tpMD5
{
public:
	typedef enum{
		GEN_32_BYTES,
		GEN_64_BYTES,
		GEN_128_BYTES,
		GEN_256_BYTES,
		GEN_512_BYTES,
	}ItpMD5Type;
public:
	tpMD5();
public:
	~tpMD5();
public:
	virtual const char *create(std::string &srcString, ItpMD5Type type = GEN_32_BYTES);
	virtual const char *create(const char *srcString, ItpMD5Type type = GEN_32_BYTES);
public:
	virtual const char *MD5String();
	virtual int32_t MD5Length();
public:
	virtual void release();
public:
	static const char *getnerateMD5(const char *input, int32_t length);//default 32BYTES
private:
	IPitpMD5 *md5Set;
};



#endif
