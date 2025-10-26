#ifndef __TP_HOST_ADDRESS_H
#define __TP_HOST_ADDRESS_H

#include "TpString.h"

class TpHostAddress
{
public:
	enum SpecialAddress{
		Any,
	};
public:
	TpHostAddress(SpecialAddress addr);
	~TpHostAddress();
	
};





#endif
