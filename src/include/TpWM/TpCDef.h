/*
	兼容C/C++的数据定义；API与utils公用数据结构
*/

#ifndef __TP_C_DEF_H
#define __TP_C_DEF_H

#include "typesDef.h"
#include "keyboard.h"
#include "event.h"

/**define key code*/
typedef struct
{
	tpUInt32 scancode;						  // scancode
	tpUInt32 virtualkey;					  // virtual key
	tpUInt32 symbol;						  // combine mod key to translate key number
	tpUInt8 shortcut[TP_KEYBOARD_STRING_LEN]; // define key string

	// mode key
#ifdef __cplusplus
	KeyModeType keymod;
#else
	enum KeyModeType keymod;
#endif

} ItpKeyCode;

/** Keyboard event structure */
typedef struct
{
	tpInt32 type;  /**< TP_KEYDOWN or TP_KEYUP */
	tpUInt8 which; /**< The keyboard device index */
	tpUInt8 state; /**< TP_PRESSED or TP_RELEASED */
	ItpKeyCode keysym;
} ItpKeyboardEvent;

/** Mouse motion event structure */
typedef struct
{
	tpInt32 type;  /**< TP_MOUSEMOTION */
	tpInt32 which; /**< The mouse device index */
	tpInt32 state; /**< The current button state */
	tpInt32 x;
	tpInt32 y;	  /**< The X/Y coordinates of the mouse */
	tpInt32 xrel; /**< The relative motion in the X direction */
	tpInt32 yrel; /**< The relative motion in the Y direction */
} ItpMouseMotionEvent;

/** Mouse button event structure */
typedef struct
{
	tpInt32 type;	/**< TP_MOUSEBUTTONDOWN or TP_MOUSEBUTTONUP */
	tpInt32 which;	/**< The mouse device index */
	MouseEventType button; /**< The mouse button index */
	tpInt32 state;	/**< TP_PRESSED or TP_RELEASED */
	tpInt32 x;
	tpInt32 y; /**< The X/Y coordinates of the mouse at press time */
} ItpMouseButtonEvent;

typedef struct
{
	tpInt32 type;
	tpInt32 timestamp;
	tpInt64 touchId;
	tpInt64 fingerId;
	tpFloat x;
	tpFloat y;
	tpFloat dx;
	tpFloat dy;
	tpFloat pressure;
} ItpTouchFingerEvent;

typedef struct
{
	tpInt32 type;
	tpInt32 timestamp;
	tpInt64 touchid;
	tpInt64 gestureid;
	tpInt32 numfingers;
	tpFloat error;
	tpFloat x;
	tpFloat y;
} ItpDollarEvent;

typedef struct
{
	tpInt32 type;
	tpInt32 timestamp;
	tpInt64 touchid;
	tpFloat dtheta;
	tpFloat ddist;
	tpFloat x;
	tpFloat y;
	tpUInt16 numfingers;
	tpUInt16 padding;
} ItpMultiGestureEvent;

typedef struct
{
	tpInt32 type;
	tpInt32 user_code;
	tpInt32 user_code0;
	tpInt32 user_code1;
	tpInt32 user_code2;
	tpInt32 user_code3;
	tpInt32 user_code4;
	void *user_data0;
	void *user_data1;
	void *user_data2;
	void *user_data3;
	void *user_data4;
} ItpUserEvent;

/**event union*/
typedef union
{
	tpInt32 type;

	ItpKeyboardEvent keyboardEvent;
	ItpMouseMotionEvent mouseMotionEvent;
	ItpMouseButtonEvent mouseButtonEvent;
	ItpTouchFingerEvent fingerEvent;
	ItpDollarEvent dollarEvent;
	ItpMultiGestureEvent gestrueEvent;
	ItpUserEvent userEvent;
} ItpEvent;

/**
@brief 生成事件句柄
@return 成功返回IPiEventCore事件句柄
*/
extern DECLSPEC IPiEventCore *STDCALL event_init();

/**
@brief 把事件放入队列
@param core（IPiEventCore*）事件句柄
@param event（WrapperPacketEvent*）网络或系统事件
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC tpBool STDCALL event_push(IPiEventCore *core, ItpEvent *event);

/**
<<<<<<< HEAD
=======
@brief 队列当前事件数量
@param core（IPiEventCore*）事件句柄
@return 成功返回当前事件的数量
*/
extern DECLSPEC int STDCALL event_length(IPiEventCore *core);

/**
 *get the queue size
 */
extern DECLSPEC int STDCALL event_size(IPiEventCore *core);

/**
@brief 把事件放入队列,直到成功为止
@param core（IPiEventCore*）事件句柄
@param event（WrapperPacketEvent*）网络或系统事件
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC tpBool STDCALL event_always_push(IPiEventCore *core, ItpEvent *event);

/**
>>>>>>> 3fcd642 (event optimization)
@brief 等待事件直至完成
@param core（IPiEventCore*）事件句柄
@param event（WrapperPacketEvent*）网络或系统事件
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC tpBool STDCALL event_wait(IPiEventCore *core, ItpEvent *event);

/**
@brief 非等待直接从队列中取出事件
@param core（IPiEventCore*）事件句柄
@param event（WrapperPacketEvent*）网络或系统事件
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC tpBool STDCALL event_poll(IPiEventCore *core, ItpEvent *event);

/**
<<<<<<< HEAD
@brief 获取事件队列是否准备完毕
@param core（IPiEventCore*）事件句柄
@return 准备完毕成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC tpBool STDCALL event_is_ready(IPiEventCore *core);

/**
@brief 设置队列的准备状态
@param core（IPiEventCore*）事件句柄
@param ready（tpBool）准备状态
@return 准备完毕成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC tpBool STDCALL event_set_ready(IPiEventCore *core, tpBool ready);

/**
@brief 清空队列
@param core（IPiEventCore*）事件句柄
@return 准备完毕成功返回TP_TRUE，失败返回TP_FALSE
@note	当一个应用结束时，队列内容已经无效，可以清除
*/
extern DECLSPEC void STDCALL event_set_clear(IPiEventCore *core);

/**
=======
@brief 从队列中取出全部事件
@param core（IPiEventCore*）事件句柄
@param event（WrapperPacketEvent*）网络或系统事件
@param n（tpInt32）取出队列的长度
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC tpBool STDCALL event_poll_all(IPiEventCore *core, ItpEvent *event, tpInt32 *n);

/**
@brief 获取事件队列是否准备完毕
@param core（IPiEventCore*）事件句柄
@return 准备完毕成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC tpBool STDCALL event_is_ready(IPiEventCore *core);

/**
@brief 设置队列的准备状态
@param core（IPiEventCore*）事件句柄
@param ready（tpBool）准备状态
@return 准备完毕成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC tpBool STDCALL event_set_ready(IPiEventCore *core, tpBool ready);

/**
@brief 清空队列
@param core（IPiEventCore*）事件句柄
@note	当一个应用结束时，队列内容已经无效，可以清除
*/
extern DECLSPEC void STDCALL event_clear(IPiEventCore *core);

/**
>>>>>>> 3fcd642 (event optimization)
@brief 析构事件队列
@param core（IPiEventCore*）事件句柄
*/
extern DECLSPEC void STDCALL event_deinit(IPiEventCore *core);

typedef enum 
{
	TP_NOEVENT = 0,		/**< Unused (do not remove) */
	TP_KEYDOWN,			/**< Keys pressed */
	TP_KEYUP,			/**< Keys released */
	TP_MOUSEMOTION,		/**< Mouse moved */
	TP_MOUSEBUTTONDOWN, /**< Mouse button pressed */
	TP_MOUSEBUTTONUP,	/**< Mouse button released */
	TP_FINGERDOWN,		/**< Finger pressed>**/
	TP_FINGERUP,		/**< Finger released>**/
	TP_FINGERMOTION,	/**< Finger motion>**/
	TP_DOLLARGESTURE,	/**< Dollar gestrure>**/
	TP_DOLLARRECORD,	/**< Dollar record>**/
	TP_MULTIGESTURE,	/**< Multi gestrure>**/
	TP_QUIT,			/**< system-requested quit */
	TP_THEME_CHANGE,
	TP_USER = 0xff, /**user event**/
}tpEventType;

typedef struct
{
	unsigned char keyshortcut[TP_KEYBOARD_STRING_LEN];
} PiKeyMap;

static PiKeyMap PiKeyboardMap[TP_NUM_SCANCODES] = {
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"0",
	"Return",
	"Escape",
	"Backspace",
	"Tab",
	"Space",
	"Minus",
	"Equals",
	"LeftBracket",
	"RightBracket",
	"BackSlash",
	"NoNushash",
	"Semicolon",
	"Apostrophe",
	"Grave",
	"Comma",
	"Period",
	"Slash",
	"CapsLock",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"PrintScreen",
	"ScrollLock",
	"Pause",
	"Insert",
	"Home",
	"PageUP",
	"Delete",
	"End",
	"PageDown",
	"Right",
	"Left",
	"Down",
	"Up",
	"NumLock",
	"KP_Dvide",
	"KP_Multiply",
	"KP_Minus",
	"KP_Plus",
	"KP_Enter",
	"KP_1",
	"KP_2",
	"KP_3",
	"KP_4",
	"KP_5",
	"KP_6",
	"KP_7",
	"KP_8",
	"KP_9",
	"KP_0",
	"KP_Period",
	"NonusBackSlash",
	"Application",
	"Power",
	"KP_Equals",
	"F13",
	"F14",
	"F15",
	"F16",
	"F17",
	"F18",
	"F19",
	"F20",
	"F21",
	"F22",
	"F23",
	"F24",
	"Execute",
	"Help",
	"Menu",
	"Select",
	"Stop",
	"Again",
	"Undo",
	"Cut",
	"Copy",
	"Paste",
	"Find",
	"Mute",
	"VolumeUP",
	"VolumeDown",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"KP_Comma",
	"KP_EqualSas400",
	"International1",
	"International2",
	"International3",
	"International4",
	"International5",
	"International6",
	"International7",
	"International8",
	"International9",
	"Lang1",
	"Lang2",
	"Lang3",
	"Lang4",
	"Lang5",
	"Lang6",
	"Lang7",
	"Lang8",
	"Lang9",
	"AltErase",
	"SysReq",
	"Cancel",
	"Clear",
	"Prior",
	"Return2",
	"Separator",
	"Out",
	"Oper",
	"ClearAgain",
	"Crsel",
	"ExSel",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"KP_00",
	"KP_000",
	"ThousandsSeparator",
	"DecimalSeparator",
	"CurrencyUnit",
	"CurrencySubUnit",
	"KP_LeftParen",
	"KP_RightParen",
	"KP_LeftBrace",
	"KP_RightBrace",
	"KP_Tab",
	"KP_Backspace",
	"KP_A",
	"KP_B",
	"KP_C",
	"KP_D",
	"KP_E",
	"KP_F",
	"KP_XOR",
	"KP_Power",
	"KP_Percent",
	"KP_Less",
	"KP_Greater",
	"KP_AmpersAND",
	"KP_DBLampersAND",
	"KP_VerticalBar",
	"KP_DBLVerticalBar",
	"KP_Colon",
	"KP_Hash",
	"KP_Space",
	"KP_AT",
	"KP_Exclam",
	"KP_MemStore",
	"KP_MemRecall",
	"KP_MemClear",
	"KP_MemADD",
	"KP_MemSubtract",
	"KP_MemMultiply",
	"KP_MemDivide",
	"KP_PlusMinus",
	"KP_Clear",
	"KP_ClearEntry",
	"KP_Binary",
	"KP_Octal",
	"KP_Decimal",
	"KP_HexADecimal",
	"UNKOWN",
	"UNKOWN",
	"LCTRL",
	"LSHIFT",
	"LALT",
	"LGUI",
	"RCTRL",
	"RSHIFT",
	"RALT",
	"RGUI",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"Mode",
	"AudioNext",
	"AudioPrev",
	"AudioStop",
	"AudioPlay",
	"AudioMute",
	"MediaSelect",
	"WWW",
	"Mail",
	"Caculator",
	"Computer",
	"AC_Search",
	"AC_Home",
	"AC_Back",
	"AC_Forword",
	"AC_Stop",
	"AC_Refresh",
	"AC_BookMarks",
	"BrightnessDown",
	"BrightnessUp",
	"DisplaySwitch",
	"KBDIllumToggle",
	"KBDIllumDown",
	"KBDIllumUp",
	"Eject",
	"Sleep",
	"APP1",
	"APP2",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
	"UNKOWN",
};

#endif
