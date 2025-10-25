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
#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include "event.h"
#include "TpCDef.h"
#include "TpWMEngine/dataDefine.h"

#define TOKEN_RING_LENGTH 128
#define TP_SYSTEM_HIGH_PRIVILLEGE 0xc00fe014
#define TINY_PIX_RUN_DIR "/System/run"
#define TINY_PIX_RUN_WORK "/.tinyPiX/"
#define DOMAIN_SOCK_FILE "tinyPiX.sock"

//share one -- tinyPiX.map; share info -- tinyPiX.share
#define TINY_PIX_SHARE	"tinyPiX"

#define TIME_WAIT_SYNC	20000
#define TIME_WAIT_LITE	200
#define TIME_WAIT_MIDS	2000

enum{
	PIX_CMD_CREATE = 0x000a0001,
	PIX_CMD_QUIT,
	PIX_CMD_EVENT,
	PIX_CMD_FOCUS,
	PIX_CMD_LEAVE,
	PIX_CMD_MOVED,
	PIX_CMD_VISIBLE,
	PIX_CMD_RESLOUTION,
	PIX_CMD_ACTIVE,
	PIX_CMD_UPDATE,
	//
	PIX_CMD_INIT,
	PIX_CMD_CHANGE,
	PIX_CMD_CAPTURE,
	PIX_CMD_BRING_TO_TOP,
	PIX_CMD_CURSOR,
	//
	PIX_SET_MOVED,
	PIX_SET_VISIBLE,
	PIX_SET_RESLOUTION,
	//
	PIX_SET_KILLED,
	PIX_SET_RETURN,
	PIX_SET_HOME,
	PIX_SET_LOCK,
	PIX_SET_ACTIVE,
	PIX_SET_LIGHTNESS,
	PIX_SET_APPSTATE,
	PIX_FIND_TOP_ID,
	PIX_FIND_DESK_ID,
	PIX_FIND_WIN_ID,
	PIX_FIND_WIN_ID_BYSTRING,
	PIX_FIND_WIN_ID_BYPID,
	PIX_FIND_PID_BYID,
	PIX_ALPHA_CHANGE,
	PIX_KEYBOARD_EVENT,
	PIX_MOUSE_KEY_EVENT,
	PIX_MOUSE_MOTION_EVENT
};

#define TP_UPD_MESSAGE	(TP_USER + 1)
#define TP_RES_MESSAGE	(TP_USER + 2)

typedef struct
{
	tpInt32 id; // must be TP_INVALIDATE_VALUE
	tpUInt32 display_width;
	tpUInt32 display_height;

	tpChar token[TOKEN_RING_LENGTH];		// share display info flags as filename
	tpChar mainDir[TOKEN_RING_LENGTH];	// main directory
	tpChar objDir[TOKEN_RING_LENGTH];		// all obj information stored directory
	tpChar displayDir[TOKEN_RING_LENGTH]; // all display information stored directory
	tpChar snapDir[TOKEN_RING_LENGTH];//save image for objects, if shareone enable
	tpChar uuidIPC[TOKEN_RING_LENGTH];

	ItpFormat display_format;
	tpUInt32 Rmask, Gmask, Bmask, Amask;
	
	tpBool shareone;
} PiXInit;

typedef struct
{
	tpInt32 id;
	tpInt32 x, y;
	tpUInt32 width, height;
	tpBool visible;

	struct
	{
		tpInt32 pid;
		tpInt32 layer;
	};
} PiXCreate;

typedef struct
{
	tpInt32 id;
	tpInt32 question;
} PiXQuit;

typedef struct
{
	tpInt32 id;
	ItpEvent event;
} PiXEvents;

typedef struct
{
	tpInt32 id;
	tpInt32 extra_id;
	tpBool focused;
} PiXFocus;

typedef struct
{
	tpInt32 id;
	tpBool leaved;

	// 触发leave事件时鼠标坐标
	tpInt32 x;
	tpInt32 y;	  
} PiXLeave;

typedef struct
{
	tpInt32 id;
	tpInt32 x, y;
} PiXMove;

typedef struct
{
	tpInt32 id;
	tpBool visible;
} PiXVisible;

typedef struct
{
	tpInt32 id;
	tpUInt32 width, height;
} PiXResloution;

typedef struct
{
	tpInt32 id;
	tpInt32 hid; // only sys api used
	tpBool byid;
	tpBool actived;
} PiXActive;

typedef struct
{
	tpInt32 id;
	PiRect rect;
	tpInt32 index;
} PiXUpdate;

typedef struct
{
	tpInt32 id;
	tpInt32 pid;
} PiXKill;

typedef struct
{
	tpInt32 id;
} PiXReturn;

typedef struct
{
	tpInt32 id;
} PiXHome;

typedef struct
{
	tpInt32 id;
	tpUInt8 lightness;
} PiXLight;

typedef struct
{
	tpInt32 id;
	tpBool locked;
} PiXLock;

typedef struct
{
	tpInt32 id;
	tpUInt64 shmid;
} PiXIdInfo;

typedef struct
{
	tpInt32 id;
	tpInt32 state;
	tpInt32 index;
} PiXState;

typedef struct
{
	tpUInt64 shmid;
} PiXAppState;

typedef struct
{
	tpInt32 id;
	tpInt32 direct;
}PiXToTop;

typedef struct
{
	tpUInt64 shmid;
	tpInt32 width;
	tpInt32 height;
	tpInt32 stride;
	tpInt32 format;
	tpUInt32 Rmask;
	tpUInt32 Gmask;
	tpUInt32 Bmask;
	tpUInt32 Amask;
} PiXCapture;

typedef struct{
	tpInt32 hot_x;
	tpInt32 hot_y;
	PiXCapture shape;
}PiXCursor;

/**only for float win**/
typedef struct
{
	tpInt32 id;
	tpInt32 cmdType;

	union
	{
		struct
		{
			struct
			{
				tpUInt32 scancode;			  //
				tpUInt32 virtualKey;		  //
				tpUInt32 symbol;			  //
				tpChar shortCut[TOKEN_RING_LENGTH]; //
				tpUInt16 keyMod;			  //
			};

			// mouse down and up
			struct
			{
				tpUInt32 button;
				tpInt32 x;
				tpInt32 y;
				tpInt32 dx;
				tpInt32 dy;
			};

			tpInt32 state;
		};

		struct
		{
			// app state
			tpUInt8 alpha;
			tpUInt8 lightness;
			tpInt32 sid;
			tpInt32 did;
			tpInt32 pid;
			tpInt32 visible;
			tpInt32 active;
			tpInt32 color;
			tpInt32 require;
		};
		// app state
		struct
		{
			tpUInt64 shmid;
			tpInt32 range;
			tpChar text[TOKEN_RING_LENGTH];
		};
	};
} PiXChange;

typedef struct
{
	tpInt32 cmdType;

	union
	{
		tpInt32 id;
		PiXInit init;
		PiXCreate create;
		PiXQuit quit; // for pid, and all about id will be destoryed
		PiXEvents events;
		PiXFocus focus;
		PiXLeave leave;
		PiXMove move;
		PiXVisible visible;
		PiXResloution resloution;
		PiXActive active;
		PiXUpdate update;
		PiXKill kill;
		PiXReturn returns;
		PiXHome home;
		PiXLight light;
		PiXLock lock;
		PiXChange change; // only for folat win
		PiXIdInfo idinfo;
		PiXState state;
		PiXAppState app;
		PiXCapture capture;
		PiXCursor cursor;
		PiXToTop totop;
	};
} PiXPacketCmd;

#endif
