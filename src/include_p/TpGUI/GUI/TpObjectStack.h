#ifndef __TP_VOBJECT_STACK_H
#define __TP_VOBJECT_STACK_H

#include <TpGUI.h>
#include "TpDef.h"

TP_DEF_VOID_TYPE_VAR(IPiVObjectStack);

class TpScreen;
class TpObjectStack
{
public:
	TpObjectStack();
	virtual ~TpObjectStack();

public:
	virtual bool push(TpScreen *hook);
	virtual bool pop();
	virtual bool remove(TpScreen *hook);
	virtual void clear();

public:
	virtual void exit();

public:
	virtual TpScreen *top();
	virtual bool dispatch(ITpEvent *event);

private:
	IPiVObjectStack *objectStackSet;
};

#endif
