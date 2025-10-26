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
#ifndef __TINY_PIX_UTILS_H
#define __TINY_PIX_UTILS_H

#define TEXT_TAG_LENGTH	128

#ifndef DECLSPEC
#define DECLSPEC __attribute__ ((visibility("default")))
#endif

#ifndef STDCALL
#define STDCALL
#endif

/**
 * @brief 虚拟窗体的句柄
 * @details 四种虚拟窗体的抽象类型
 *          和PiXWM交互的核心要素
 * @note 返回值如果为空，表示创建失败。否则不为空
 */
typedef void IPiWFApiAgent;

/**
 * @brief PiXWM为PiXApp创建的虚拟窗体的共享内存句柄
 * @details 该虚拟窗体内存句柄，可以指向具体文件、内存或物理显卡内存，具备抽象性。支持256色、16、24和32真彩
 * @note 返回值如果为空，表示创建失败。否则不为空
 */
typedef void IPiWFSurface;

/**
 * @brief 获取PiXWM和PiXApp的锁，一般指的是PiXApp一端的写锁
 * @details 该锁是基于进程之间的，用于PiXApp写入数据时使用，也可以直接调用提供的锁函数
 * @note 返回值如果为空，表示创建失败。否则不为空
 */
typedef void IPiWFMutex;

/**
 * @brief 获取的是一个不基于窗体的共享内存句柄
 * @details 该句柄仅是对PiXWM进行通信和连接，获取PiXWM的信任，并能够支持多个连接，用于发送一些系统级的命令和请求
 *          特指作为系统Api接口的句柄
 * @note 返回值如果为空，表示创建失败。否则不为空
 */
typedef void IPiSysApiAgent;

/**
@enum 使用tinyPiX_sys_find_win_ids时的轮询窗体类型
@brief 可以通过|运算符号组合使用，查看tinyPiX_sys_find_win_ids的参数range
*/
enum{
	/**选择固定桌面**/
	Q_DESK = 0x01,
	/**普通固定窗体**/
	Q_FIXS = 0x02,
	/**普通虚拟悬浮窗体**/
	Q_FLTS = 0x04,
	/**顶级系统悬浮窗体**/
	Q_SYSS = 0x08,
	/**全部类型**/
	Q_ALLS = 0x0f,
};

/**
@struct 结构体名
@brief  获取PiXApp中的fixScreen窗体所在的进程id和窗体id
@var    s_id为窗体的id；p_id为对应进程id
*/
typedef struct{
	/**虚拟窗体id**/
	int s_id;
	/**虚拟窗体所在应用进程id**/
	int p_id;
	/**窗体类型:0-桌面，1-普通固定窗体，2-普通悬浮窗体，3-系统悬浮窗体**/
	int type;
	/**是否可见**/
	int visible;
}PiShmBytes;

/**
@enum 响应应答标记
@brief 用于对PiXApp的事件传输或反馈，详见回调函数中的question参数
*/
enum{
	/**不使用的标记**/
	Q_NORMAL = -1,
	/**标记是由PiXWM产生的**/
	Q_PIXWM,
	/**标记是由PiXApp自己产生或用户自定义产生**/
	Q_SELF,
};

/**
@brief 事件的回调函数，包括鼠标、键盘、触摸屏以及移动等派生事件
@param id（int） 返回给PiXApp的虚拟窗体id
@param event（void*）返回给PIXApp的事件地址
@param args（void*）将PiXApp传递给PiXWM的数据回传给PiXApp
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
typedef int (*AssignEvent)(int id, void *event, void *args);

/**
@brief 虚拟窗体焦点变化回调函数
@param id（int） 返回给PiXApp的虚拟窗体id
@param focused（int）对应窗体id获取焦点时focused为TP_TRUE，失去焦点时为TP_TRUE
@param args（void*）将PiXApp传递给PiXWM的数据回传给PiXApp
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
typedef int (*AssignFocus)(int id, int focused, void *args);

/**
@brief 虚拟窗体移入和移出回调函数
@param id（int） 返回给PiXApp的虚拟窗体id
@param leaved（int）对应窗体id鼠标或触屏移出时为TP_TRUE，移入时为TP_TRUE
@param mouseX（int）鼠标的X轴坐标
@param mouseY（int）鼠标的Y轴坐标
@param args（void*）将PiXApp传递给PiXWM的数据回传给PiXApp
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
typedef int (*AssignLeave)(int id, int leaved, int mouseX, int mouseY, void *args);

/**
@brief 窗体大小变化的回调事件
@param id（int） 返回给PiXApp的虚拟窗体id
@param new_width（int）窗体的变化后的宽度
@param new_height（int）窗体的变化后的高度
@param question（int）当值为Q_PIXWM时表示物理屏幕的分辨率发生了变化；如果为Q_PIXAPP时表示窗体由用户设置；当接收到该回调函数时，虚拟窗体变化已经完成
@param args（void*）将PiXApp传递给PiXWM的数据回传给PiXApp
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
typedef int (*AssignResize)(int id, unsigned int new_width, unsigned int new_height, int question, void *args);

/**
@brief 虚拟窗体显示或隐藏变化的回调函数
@param id（int） 返回给PiXApp的虚拟窗体id
@param visible（int）窗体变化后的状态，显示为TP_TRUE，隐藏为TP_FALSE
@param question（int）当值为Q_PIXWM时表示物理屏幕的分辨率发生了变化；如果为Q_PIXAPP时表示窗体由用户设置；当接收到该回调函数时，虚拟窗体变化已经完成
@param （void*）将PiXApp传递给PiXWM的数据回传给PiXApp
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
typedef int (*AssignVisible)(int id, int visible, void *args);

/**
@brief 虚拟窗体移动事件的回调函数（仅针对floatScreen窗体，对fixScreen无效）
@param id（int） 返回给PiXApp的虚拟窗体id
@param new_x（int）窗体变化后的x横坐标
@param new_y（int）窗体变化后的y纵坐标
@param question（int）当值为Q_PIXWM时表示物理屏幕的分辨率发生了变化；如果为Q_PIXAPP时表示窗体由用户设置；当接收到该回调函数时，虚拟窗体变化已经完成
@param args（void*）将PiXApp传递给PiXWM的数据回传给PiXApp
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
typedef int (*AssignMoved)(int id, int new_x, int new_y, int question, void *args);

/**
@brief 虚拟窗体被激活或被取消激活时的回调函数（仅针对fixScreen窗体，对floatScreen无效），仅是系统事件;
@param id（int） 返回给PiXApp的虚拟窗体id
@param actived（int）当收到值为TP_TRUE时，当前的fixScreen窗体置于显示的最顶层；当为TP_FALSE上，fixScreen将被其他被激活的窗体取代
@param args（void*）将PiXApp传递给PiXWM的数据回传给PiXApp
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
typedef int (*AssignActived)(int id, int actived, void *args);

/**
@brief quit事件的回调函数，当收到该信息时，PiXWM已经与PiXApp断开
@param id（int） 返回给PiXApp的虚拟窗体id
@param question（int）当值为Q_PIXWM时表示物理屏幕的分辨率发生了变化；如果为Q_PIXAPP时表示窗体由用户设置；当接收到该回调函数时，虚拟窗体变化已经完成
@param args（void*）将PiXApp传递给PiXWM的数据回传给PiXApp
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
typedef int (*AssignQuit)(int id, int question, void *args);

/**
@brief return事件的回调函数
@param id（int） 返回给PiXApp的虚拟窗体id
@param args（void*）将PiXApp传递给PiXWM的数据回传给PiXApp
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
typedef int (*AssignReturn)(int id, void *args);

/**
@brief 由普通窗体向fixScreen窗体中的desktop发送属性信息，以便desktop做出变化
@param id（int） 返回给PiXApp的虚拟窗体id
@param pid（int）普通窗体的进程id
@param visible（int）普通窗体的显隐属性
@param active（int）普通窗体是否被激活，即是否在显示的最顶层
@param color（int）普通窗体传递的color请求，一般由desktop用于全局工具栏
@param alpha（unsigned char）普通窗体的alpha属性请求，一般由desktop用于全局工具栏
@param require（int）普通窗体自定义请求，一般由desktop自定义解释和实现
@param args（void*）将PiXApp传递给PiXWM的数据回传给PiXApp
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
typedef int (*AssignAppChange)(int id, int pid, int visible, int active, int color, unsigned char alpha, int require, void *args);

#ifdef __cplusplus
extern "C"{
#endif

/**surface tools, could be used to expand to SDL, carrio, pixman and so on**/
/**
function name: create surface
@bits		: the memory to store pixels, if NULL, will create new, otherwise use other space
@width		: the width for surface
@height		: the height for surface
@depth		: the bitsperpixel, such as 16, 24 and 32
@r,g,b,amask: the mask for surface
return value: if failed, return zero. otherwise return the fact bytes per line
*/

extern DECLSPEC IPiWFSurface *STDCALL tinyPiX_surface_create(void *bits, unsigned int width, unsigned int height, int depth, 
															unsigned int rmask, unsigned int gmask, unsigned int bmask, unsigned int amask);

/**
function name: free surface
@surface	: the exist surface pointer
**/
extern DECLSPEC void STDCALL tinyPiX_surface_free(IPiWFSurface *surface);

/**
function name: caculate the stride which denotes the bytes per line
@width		: the width for surface
@depth		: the bitsperpixel, such as 16, 24 and 32

return value: if failed, return zero. otherwise return the fact bytes per line
*/
extern DECLSPEC unsigned int STDCALL tinyPiX_surface_cal_stride(unsigned int width, int format);

/**
function name: set_alpha, aim to set alpha for surface
@surface	: the exist surface pointer
@alpha		: the alpha value ranges from 0-255

return value: if failed, return TP_FALSE. otherwise return TP_TRUE
*/
extern DECLSPEC int STDCALL tinyPiX_surface_set_alpha(IPiWFSurface *surface, unsigned char alpha);

/**
function name: set alpha enable for surface
@surf		: the exist surface pointer
@enable		: if TP_TRUE, enable alpha mode. otherwise not
*/
extern DECLSPEC void STDCALL tinyPiX_surface_set_alpha_enable(IPiWFSurface *surface, int enable);

/**
function name: tinyPiX_surface_get_alpha, aim to get alpha for surface
@surface		: the exist surface pointer

return value: if failed, return 255. otherwise return true alpha value
*/
extern DECLSPEC unsigned char STDCALL tinyPiX_surface_get_alpha(IPiWFSurface *surface);

/**
function name: get alpha enable state for surface
@surface		: the exist surface pointer

return value:  return alpha enable true state, if surface is NULL, return TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_surface_get_alpha_enable(IPiWFSurface *surface);

/**
function name:set the color key (transparent pixel)
@surface	: the exist surface pointer
@colorkey	: the colorkey value for R-G-B-A or A-R-G-B

return value: if failed, return TP_FALSE. otherwise return TP_TRUE
*/
extern DECLSPEC int STDCALL tinyPiX_surface_set_colorkey(IPiWFSurface *surface, unsigned int colorkey);

/**
function name: set colorkey for surface
@surface	: the exist surface pointer
@enable		: if TP_TRUE, enable alpha mode. otherwise not
*/
extern DECLSPEC void STDCALL tinyPiX_surface_set_colorkey_enable(IPiWFSurface *surface, int enable);

/**
function name: tinyPiX_surface_get_colorkey, aim to get colorkey for surface
@surf		: the exist surface pointer

return value: if failed, return default 0. otherwise return true colorkey value
*/
extern DECLSPEC unsigned int STDCALL tinyPiX_surface_get_colorkey(IPiWFSurface *surface);

/**
function name: tinyPiX_surface_get_colorkey, aim to get colorkey enable state for surface
@surf		: the exist surface pointer

return value:  return colorkey enable true state, if surf is NULL, return TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_surface_get_colorkey_enable(IPiWFSurface *surface);

/**
function name: tinyPiX_surface_set_cliprect, aim to set cliprect for surface
@surf		: the exist surface pointer

return value:  if surf is NULL or w = 0 || h = 0 for rect, will return TP_FALSE, otherwise return TP_TRUE
*/
extern DECLSPEC int STDCALL tinyPiX_surface_set_cliprect(IPiWFSurface *surface, int x, int y, unsigned int width, unsigned int height);

/**
function name: tinyPiX_surface_get_cliprect, aim to get cliprect for surface
@surf		: the exist surface pointer

return value:  if surf is NULL , will return TP_FALSE, otherwise return true cliprect
*/
extern DECLSPEC int STDCALL tinyPiX_surface_get_cliprect(IPiWFSurface *surface, int *x, int *y, unsigned int *width, unsigned int *height);

/**
function name: tinyPiX_surface_get_width, aim to get width for surface
@surf		: the exist surface pointer

return value:  if surf is NULL , will return 0
*/
extern DECLSPEC unsigned int STDCALL tinyPiX_surface_get_width(IPiWFSurface *surface);

/**
function name: tinyPiX_surface_get_height, aim to get height for surface
@surf		: the exist surface pointer

return value:  if surf is NULL , will return 0
*/
extern DECLSPEC unsigned int STDCALL tinyPiX_surface_get_height(IPiWFSurface *surface);

/**
function name: tinyPiX_surface_get_stride, aim to get stride for surface
@surf		: the exist surface pointer

return value:  if surf is NULL , will return 0
*/
extern DECLSPEC unsigned int STDCALL tinyPiX_surface_get_stride(IPiWFSurface *surface);

/**
function name: tinyPiX_surface_get_format, aim to get image format for surface
@surf		: the exist surface pointer

return value:  if surf is NULL , will return TP_RGB_UNKOWN
*/
extern DECLSPEC int STDCALL tinyPiX_surface_get_format(IPiWFSurface *surface);

/**
function name: tinyPiX_surface_get_size, aim to get image size for surface
@surf		: the exist surface pointer

return value:  if surf is NULL , will return 0
*/
extern DECLSPEC unsigned int STDCALL tinyPiX_surface_get_size(IPiWFSurface *surface);

/**
function name: tinyPiX_surface_get_bits, aim to get image buffer for surface
@surf		: the exist surface pointer

return value:  if surf is NULL , will return NULL. also if width = 0 || height = 0 for surf, will return NULL.
*/
extern DECLSPEC void* STDCALL tinyPiX_surface_get_matrix(IPiWFSurface *surface);

/**surface fill**/
extern DECLSPEC void STDCALL tinyPiX_surface_clear(IPiWFSurface *surface, unsigned int color, int use_alpha);

/**
function name: tinyPiX_surface_map_rgba, aim to map r, g, b, a to surface color
**/
extern DECLSPEC unsigned int STDCALL tinyPiX_surface_map_rgba(IPiWFSurface *surface, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

/**get Rmask, Gmask, Bmask, Amask;
*/
extern DECLSPEC unsigned int STDCALL tinyPiX_surface_get_rmask(IPiWFSurface *surface);
extern DECLSPEC unsigned int STDCALL tinyPiX_surface_get_gmask(IPiWFSurface *surface);
extern DECLSPEC unsigned int STDCALL tinyPiX_surface_get_bmask(IPiWFSurface *surface);
extern DECLSPEC unsigned int STDCALL tinyPiX_surface_get_amask(IPiWFSurface *surface);
extern DECLSPEC int STDCALL tinyPiX_surface_get_rgba_mask(IPiWFSurface *surface, unsigned int *Rmask, unsigned int *Gmask, unsigned int *Bmask, unsigned int *Amask);

#ifdef __cplusplus
}
#endif

#endif
