#ifndef __PIXWM_H
#define __PIXWM_H

#include "pixdef.h"
#include "engine.h"
#include "config.h"
#include "service.h"
#include "zorder.h"
#include "object.h"
#include "server.h"
#include "wmlist.h"
#include "parseropts.h"
#include "ids.h"
#include "uuid.h"
#include "utils.h"
#include "loadso.h"
#include "rswrlock.h"

#define DEFAULT_SURFACE_WIDTH	480
#define DEFAULT_SURFACE_HEIGHT	640
#define DEFAULT_SURFACE_FORMAT	32
#define DEFAULT_SURFACE_RMASK	0x00ff0000
#define DEFAULT_SURFACE_GMASK	0x0000ff00
#define DEFAULT_SURFACE_BMASK	0x000000ff
#define DEFAULT_SURFACE_AMASK	0xff000000

#define TOKEN_DEFAULT_LENGTH	1024
#define DOMAIN_SOCK_FILE		"tinyPiX.sock"

struct __PIXWMConfigs;

typedef struct{
	PiVideoSurfaceInfo info;
	void *data;
}PiXScreenParameters;

typedef struct __PIXWMConfigs{
	//init flag
	tpBool inited;
	//record blit object is busy
	tpInt32 busyCount;
	//current object numbers
	tpUInt32 objectCount;
	//lock screen
	tpBool lock;
	
	//alone run
	tpBool deamon;
	//screen brightness
	tpUInt8 lightness;
	//is hardware acceleration
	tpBool accelerated;
	//surface stored in disk or mem
	tpBool sharemem;
	//fix surface share one mem
	tpBool shareone;
	//wait time to kill all proccess
	tpInt32 quitwait;
	
	//use debug
	tpBool simulator;
	
	struct{
		tpChar token[TOKEN_DEFAULT_LENGTH];//share display info flags as filename
		tpChar mainDir[TOKEN_DEFAULT_LENGTH];//main directory
		tpChar objDir[TOKEN_DEFAULT_LENGTH];//all obj information sored directory
		tpChar displayDir[TOKEN_DEFAULT_LENGTH];//all display information stored directory
		tpChar snapDir[TOKEN_DEFAULT_LENGTH];//if share one mode, snap the fix screen to save
		tpChar uuidDir[TOKEN_DEFAULT_LENGTH];//uuid dir for obj and display
		tpChar shareOne[TOKEN_DEFAULT_LENGTH];//share one for share_one mode
	};

	//remain tmp setting
	struct{
		//read from config file, and it's true value decided by running
		tpUInt32 width;
		tpUInt32 height;
		
		ITpFormat format;
		
		tpUInt32 Rmask;
		tpUInt32 Gmask;
		tpUInt32 Bmask;
		tpUInt32 Amask;
	};
	
	struct{
		//zorder and wmlist
		IPiZorderLayer *zorder;
		//service, dispatch event and deal command
		IPiService *service;
		//server, listen client requirement
		IPiServer *server;
		//object, respond to every client window
	};

	struct{
		tpInt32 beUsedID;
		tpInt32 beFocusID;
	};

	//dirver
	PiDriver *driver;
	//rwlock
	IPiWRLock *doWRLock;
	//socket
	IPiTCPSocket *sock;
	//share file
	IPiShare *share;
	//share map, only be used by shareone = TP_TRUE
	IPiWRSurface *share_map;
	
	//driver function
	struct{
		videoDoBlit Blit;
		videoCopyStrenchBlit CpStrenchBlit;
		videoSurfaceInfo SurfaceInfo;
		videoDoUpdate Update;
		videoDoUpdates Updates;
		videoDoFill	Fill;
		videoDoReInit ReInit;
		videoDoSave SaveBMP;
		videoDoCapture Capture;
		videoSetCursor SetCursor;
		videoSetMousePosition SetMousePosition;
		videoSetLogicalSize SetLogicalSize;
		videoGetLogicalSize GetLogicalSize;
		videoSwitch Switch;
		videoGetMetrics GetMetrics;
		videoShowWait ShowWait;
		videoHardware isHardware;
	};
	
	//start options
	struct{
		tpChar startDir[TOKEN_DEFAULT_LENGTH];
		tpChar startApp[TOKEN_DEFAULT_LENGTH];
	};
	
	//capture screen
	tpBool doCapture;
}PiXWMConfigs;

#ifdef __cplusplus
extern "C"{
#endif

/**init pixwm includes parameter and all componets**/
extern DECLSPEC tpBool STDCALL pixwm_init(tpInt32 argc, tpChar *argv[]);

/**start pixwm**/
extern DECLSPEC tpInt32 STDCALL pixwm_start(PiXWMConfigs *args);

/**obtain global PiXWMConfig Object**/
extern DECLSPEC PiXWMConfigs* STDCALL pixwm_configs();



#ifdef __cplusplus
}
#endif

#endif
