#ifndef __EVENT_H
#define __EVENT_H

#include "typesDef.h"
#include "keyboard.h"

/**press state**/
#define TP_RELEASED 0
#define TP_PRESSED 1

/**touch or mouse event source**/
#define TP_EVENT_TOUCH ((unsigned int)-1)

TP_DEF_VOID_TYPE_VAR(IPiEventCore);

/// @brief 鼠标事件枚举
typedef enum
{
	BUTTON_INVALIDATE_VALUE = -1,

	BUTTON_LEFT = 1,
	BUTTON_MIDDLE,
	BUTTON_RIGHT,
	BUTTON_WHEELUP,
	BUTTON_WHEELDOWN,
	BUTTON_X1,
	BUTTON_X2
} MouseEventType;



#endif
