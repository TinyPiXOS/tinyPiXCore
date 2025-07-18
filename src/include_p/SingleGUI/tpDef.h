/*
	内部 全局数据定义，禁止include目录下的文件包含此文件
*/

#ifndef __TP_DEF_H
#define __TP_DEF_H

#include "tpUtils.h"
#include "tpList.h"
#include "tpEvent.h"
#include "tpCDef.h"
#include "keyboard.h"
#include <tinyPiXWF.h>
#include <mutex>
#include <list>
#include "tpHash.h"
#include "tpVariant.h"
#include "tpChildWidget.h"

#define OBJECT_MAX_TEXT_LENGTH 1024

class tpLayout;
class tpObjectStack;
class tpEvent;

struct ItpTempDef
{
	tpChildWidget *curfocus = nullptr;
	tpChildWidget *lstfocus = nullptr;
	tpChildWidget *curmotion = nullptr;
	tpChildWidget *lstmotion = nullptr;
	tpChildWidget *curObject = nullptr;
	// tpChildWidget *dragObject = nullptr;

	// 鼠标移动前的上一个坐标
	ItpPoint lastPoint;

	// 新增悬停链状态
	// tpVector<tpChildWidget *> prevHoverChain; // 上一帧悬停链
	// tpVector<tpChildWidget *> currHoverChain; // 当前悬停链

	/// @brief 移除对象时，如果缓存了obj，需要同步置空
	/// @param delObj
	void deleteObject(tpObject *delObj)
	{
		tpChildWidget *needDelObj = dynamic_cast<tpChildWidget *>(delObj);
		if (!needDelObj)
			return;

		if (curfocus == needDelObj)
			curfocus = nullptr;

		if (lstfocus == needDelObj)
			lstfocus = nullptr;

		if (curmotion == needDelObj)
			curmotion = nullptr;

		if (lstmotion == needDelObj)
			lstmotion = nullptr;

		if (curObject == needDelObj)
			curObject = nullptr;

		// if (dragObject == needDelObj)
		// dragObject = nullptr;
	}
};

struct ItpObjectSet
{
	// 鼠标按下的对象，用于判断拖拽、等事件
	// 记录鼠标按下时的对象，最后鼠标无论在哪释放，都触发按下对象的release
	tpChildWidget *mousePressObject = nullptr;

	tpList<tpObject *> objectList;

	tpObject *filterObject = nullptr;

	tpObject *parent = nullptr;
	tpObject *top = nullptr;

	bool visible = true;
	bool enable = true;

	char text[OBJECT_MAX_TEXT_LENGTH];

	std::mutex layoutMutex;
	tpLayout *layout = nullptr;

	int32_t offsetX;
	int32_t offsetY;

	/// @brief 绝对坐标
	ItpRect absoluteRect;
	/// @brief 逻辑坐标
	ItpRect logicalRect;

	// 窗口最小宽高
	uint32_t minimumWidth = 0;
	uint32_t minimumHeight = 0;

	// 窗口最大宽高
	uint32_t maximumWidth = WIDGET_MAX_WIDTH;
	uint32_t maximumHeight = WIDGET_MAX_HEIGHT;

	int32_t objectID;

	// 背景图片
	bool enableImage;
	tpShared<tpSurface> reserveImage = nullptr;
	tpShared<tpSurface> cacheImage = nullptr;

	// 背景颜色
	bool enableColor = true;
	uint32_t backColor;

	// 边框颜色
	bool enableBorderColor = false;
	uint32_t borderColor;

	// 是否可选中；选中状态
	bool checkable = false;
	bool isChecked = false;

	// 是否启用背景模糊，模糊半径 px
	bool enableBlur = false;
	uint32_t blurRadius = 8;

	bool isHover = false;
	bool isPress = false;

	// 圆角值，单位px
	uint32_t round = 0;

	uint32_t varSize;
	void *varShape = nullptr;

	uint8_t alpha;
	uint32_t colorKey;
	bool enableColorKey;

	IPiWFApiAgent *agent = nullptr;
	std::mutex gMutex;
	// tpObjectStack *objectStack = nullptr;
	ItpTempDef tmp;

	tpHash<tpString, tpVariant> objPropertyMap;

	ItpObjectSet()
	{
	}
};

struct ItpMouseSet
{
	tpEvent::ItpEventType type;
	uint32_t which;
	MouseEventType button;
	bool state;

	ItpPoint pos;
	ItpPoint globalPos;

	ItpMouseSet() : type(tpEvent::EVENT_NONE_TYPE), which(0), button(BUTTON_INVALIDATE_VALUE), state(false)
	{
	}
};

/// @brief 键盘事件数据
typedef struct
{
	/// @brief 事件类型
	tpEvent::ItpEventType type;
	struct
	{
		/// @brief 测试注释
		uint8_t which;
		bool state;
		uint32_t scancode;
		uint32_t virtualKey;
		uint32_t symbol;
		char shortCut[KEYBOARD_STRING_LEN];
		KeyModeType keyMod;
	};
} ItpKeyboardSet;

typedef struct
{
	tpEvent::ItpEventType type;
	struct
	{
		int32_t touchFingerType;
		uint32_t timestamp;
		int64_t fingerID;
		int64_t touchID;
		int32_t x;
		int32_t y;
		int32_t dx;
		int32_t dy;
		float pressure;
	};
} ItpFingerSet;

typedef struct
{
	tpEvent::ItpEventType type;
	struct
	{
		int32_t dollarType;
		uint32_t timestamp;
		int64_t touchID;
		int64_t GestureID;
		uint32_t numFingers;
		int32_t x;
		int32_t y;
	};
} ItpDollarSet;

typedef struct
{
	tpEvent::ItpEventType type;
	struct
	{
		uint32_t timestamp;
		int64_t touchID;
		float dtheta;
		float ddist;
		int32_t x, y;
		uint16_t numfingers;
		uint16_t padding;
	};
} ItpMultiGestureSet;

typedef struct
{
	tpEvent::ItpEventType type;
	struct
	{
		tpObject *object;
		struct
		{
			int32_t nx;
			int32_t ny;
		};
	};
} ItpObjectMoveSet;

typedef struct
{
	tpEvent::ItpEventType type;
	struct
	{
		tpObject *object;
		struct
		{
			int32_t nw;
			int32_t nh;
		};
		int32_t question;
	};
} ItpObjectResizeSet;

typedef struct
{
	tpEvent::ItpEventType type;
	struct
	{
		tpObject *object;
		struct
		{
			bool focused;
		};
	};
} ItpObjectFocusSet;

typedef struct
{
	tpEvent::ItpEventType type;
	tpObject *object;
	bool leaved;
} ItpObjectLeaveSet;

typedef struct
{
	tpEvent::ItpEventType type;
	struct
	{
		tpObject *object;
		struct
		{
			bool visible;
		};
	};
} ItpObjectVisibleSet;

typedef struct
{
	tpEvent::ItpEventType type;
	struct
	{
		tpObject *object;
		struct
		{
			ItpRotateType rotate;
		};
	};
} ItpObjectRotateSet;

typedef struct
{
	tpObject *object;
	tpSurface *surface;
	ItpRect updateRect;
} ItpObjectPaintInput;

typedef struct
{
	tpEvent::ItpEventType type;

	tpObject *object;

	tpCanvas *canvas;
	tpSurface *surface;
	ItpSufaceData *itpSurface;

	int32_t offsetX;
	int32_t offsetY;

	ItpRect updateRect;
	ItpRect rect;

	bool canDraw;
} ItpObjectPaintSet;

typedef struct
{
	tpEvent::ItpEventType type;
	struct
	{
		tpObject *object;
		struct
		{
			bool actived;
		};
	};
} ItpObjectActiveSet;

#endif

// (触发事件的对象指针，事件数据，事件函数名，对象是否是可用态（外部传入，这样可以控制某些事件在disbled状态下也可触发事件，例如resize）)
#ifndef IssueObjEvent
#define IssueObjEvent(Obj, keyEvent, eventFuncName, enabled)             \
	do                                                                   \
	{                                                                    \
		if (!enabled)                                                    \
			break;                                                       \
		tpObject *_current = (Obj)->eventFilterObject();                 \
		tpObject *_filterStack[16] = {0};                                \
		int _depth = 0;                                                  \
		while (_current && _depth < 16)                                  \
		{                                                                \
			_filterStack[_depth++] = _current;                           \
			_current = _current->eventFilterObject();                    \
		}                                                                \
		bool _handled = false;                                           \
		for (int _i = _depth - 1; _i >= 0; --_i)                         \
		{                                                                \
			tpObject *lastObj = (_i > 0) ? _filterStack[_i - 1] : (Obj); \
			if (_filterStack[_i]->eventFilter((lastObj), &keyEvent))     \
			{                                                            \
				_handled = true;                                         \
				break;                                                   \
			}                                                            \
		}                                                                \
		if (!_handled)                                                   \
		{                                                                \
			(Obj)->eventFuncName(&keyEvent);                             \
			_handled = true;                                             \
		}                                                                \
	} while (0);
#endif