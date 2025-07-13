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
#include "tpAutoObject.h"

tpAutoObject *tpAutoObject::objectHandle = nullptr;
int32_t tpAutoObject::counter = 0;
bool tpAutoObject::autoFreeObject = false;
std::mutex tpAutoObject::autoMutex;
std::list<void*> tpAutoObject::objectLife;

tpAutoObject *tpAutoObject::Inst()
{
	if(tpAutoObject::objectHandle == nullptr){
		tpAutoObject::objectHandle = (new tpAutoObject());
	}
	
	return tpAutoObject::objectHandle;
}

tpAutoObject::tpAutoObject(){

}

tpAutoObject::~tpAutoObject(){
	if(tpAutoObject::objectLife.size() > 0){
		if(tpAutoObject::autoFreeObject){
			void *object = nullptr;
			std::list<void*>::iterator iter = tpAutoObject::objectLife.begin();

			for(;iter != tpAutoObject::objectLife.end();){
				object = *iter;

				if(object){
					free(object);
				}

				tpAutoObject::objectLife.erase(iter++);
			}

			tpAutoObject::autoFreeObject = false;
		}
	}

	tpAutoObject::objectHandle = nullptr;
	tpAutoObject::counter = 0;
}

int32_t tpAutoObject::selfCounterIncrease()
{
	return (tpAutoObject::counter++);
}

bool tpAutoObject::isExist(void *ptr)
{
	void *object = nullptr;

	if(ptr == nullptr ||
		objectLife.size() <= 0){
		return false;
	}

	tpAutoObject::autoMutex.lock();

	std::list<void*>::iterator iter = tpAutoObject::objectLife.begin();

	for(;iter != tpAutoObject::objectLife.end(); ++iter){
		object = *iter;

		if(object == ptr){
			tpAutoObject::autoMutex.unlock();
			return true;
		}
	}

	tpAutoObject::autoMutex.unlock();

	return false;
}

bool tpAutoObject::addObjectLife(void *ptr)
{
	if(ptr == nullptr){
		return false;
	}

	tpAutoObject::autoMutex.lock();
	tpAutoObject::objectLife.push_back(ptr);
	tpAutoObject::autoMutex.unlock();

	return true;
}

bool tpAutoObject::removeObjectLife(void *ptr)
{
	if(ptr == nullptr){
		return false;
	}

	tpAutoObject::autoMutex.lock();
	tpAutoObject::objectLife.remove(ptr);
	tpAutoObject::autoMutex.unlock();

	return true;
}
