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
#include "TpAutoObject.h"

TpAutoObject *TpAutoObject::objectHandle = nullptr;
int32_t TpAutoObject::counter = 0;
bool TpAutoObject::autoFreeObject = false;
std::mutex TpAutoObject::autoMutex;
std::list<void*> TpAutoObject::objectLife;

TpAutoObject *TpAutoObject::Inst()
{
	if(TpAutoObject::objectHandle == nullptr){
		TpAutoObject::objectHandle = (new TpAutoObject());
	}
	
	return TpAutoObject::objectHandle;
}

TpAutoObject::TpAutoObject(){

}

TpAutoObject::~TpAutoObject(){
	if(TpAutoObject::objectLife.size() > 0){
		if(TpAutoObject::autoFreeObject){
			void *object = nullptr;
			std::list<void*>::iterator iter = TpAutoObject::objectLife.begin();

			for(;iter != TpAutoObject::objectLife.end();){
				object = *iter;

				if(object){
					free(object);
				}

				TpAutoObject::objectLife.erase(iter++);
			}

			TpAutoObject::autoFreeObject = false;
		}
	}

	TpAutoObject::objectHandle = nullptr;
	TpAutoObject::counter = 0;
}

int32_t TpAutoObject::selfCounterIncrease()
{
	return (TpAutoObject::counter++);
}

bool TpAutoObject::isExist(void *ptr)
{
	void *object = nullptr;

	if(ptr == nullptr ||
		objectLife.size() <= 0){
		return false;
	}

	TpAutoObject::autoMutex.lock();

	std::list<void*>::iterator iter = TpAutoObject::objectLife.begin();

	for(;iter != TpAutoObject::objectLife.end(); ++iter){
		object = *iter;

		if(object == ptr){
			TpAutoObject::autoMutex.unlock();
			return true;
		}
	}

	TpAutoObject::autoMutex.unlock();

	return false;
}

bool TpAutoObject::addObjectLife(void *ptr)
{
	if(ptr == nullptr){
		return false;
	}

	TpAutoObject::autoMutex.lock();
	TpAutoObject::objectLife.push_back(ptr);
	TpAutoObject::autoMutex.unlock();

	return true;
}

bool TpAutoObject::removeObjectLife(void *ptr)
{
	if(ptr == nullptr){
		return false;
	}

	TpAutoObject::autoMutex.lock();
	TpAutoObject::objectLife.remove(ptr);
	TpAutoObject::autoMutex.unlock();

	return true;
}
