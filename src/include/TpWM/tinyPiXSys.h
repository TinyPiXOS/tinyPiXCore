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
#ifndef __TINY_PIX_SYS_H
#define __TINY_PIX_SYS_H

#include "tinyPiXUtils.h"

#ifdef __cplusplus
extern "C"{
#endif
/**
@brief 系统api的句柄，该句柄不会生成虚拟窗体
@return 返回值如果为空，表示创建失败。否则不为空
*/
extern DECLSPEC IPiSysApiAgent* STDCALL tinyPiX_sys_create();

/**
@brief 释放系统api句柄
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@return 无返回值
*/
extern DECLSPEC void STDCALL tinyPiX_sys_free(IPiSysApiAgent *sysApiagent);

/**
@brief 虚拟窗体的全屏大小（只对fixScreen有效），同时所有窗体都会收到resize事件
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param width（unsigned int）设置的物理屏幕和虚拟窗体的宽度
@param height（unsigned int）设置的物理屏幕和虚拟窗体的高度
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_sys_set_resolution(IPiSysApiAgent *sysApiagent, unsigned int width, unsigned int height);

/**
@brief 根据进程id，直接关闭应用
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param pid（int）PiXApp所在应用的系统id
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_sys_kill_process(IPiSysApiAgent *sysApiagent, int pid);

/**
@brief 根据虚拟窗体id发送renturn事件，对应窗体会被隐藏,会直接引发visible和active回调事件，对desktop和floatScreen无效
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param pid（int）PiXApp所在应用的系统id
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_sys_send_return(IPiSysApiAgent *sysApiagent, int id);

/**
@brief 对全部的虚拟窗体发送home事件，除desktop外，其他fixScreen会被统一设置成visible属性，会直接引发visible和active回调事件
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_sys_send_home(IPiSysApiAgent *sysApiagent);

/**
@brief 锁定屏幕，使屏幕进入黑屏状态
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param locked（int）为TP_FALSE进行解锁，恢复到应用正常显示状态；为TP_TRUE时，屏幕黑屏；PiXWM停止发送任何消息和事件处理
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_sys_set_lock(IPiSysApiAgent *sysApiagent, int locked);

/**
@brief 根据虚拟窗体的id，指定的虚拟窗体设置显隐，除desktop，对其他虚拟窗体都有效果
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param pid（int）进程id
@param visible（int）设置为TP_FALSE时，指定的虚拟窗体会被设置成隐藏；设置为TP_TRUE时，指定窗体会被设置成显示（但未必会出现在显示的顶层，需要active属性）
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_sys_set_visible(IPiSysApiAgent *sysApiagent, int pid, int visible);

/**
@brief 对tinyPiX_sys_set_active重新定义
**/
#define tinyPiX_sys_set_visible_bypid tinyPiX_sys_set_visible

/**
@brief 将一个未被激活的虚拟窗体激活，可以从隐藏状态或未占用屏幕状态切换为顶级显示状态（对floatScreen无效）
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param id（int）虚拟窗体id
@param actived（int）设置为TP_FALSE时，指定的虚拟窗体所在应用将不在顶级显示；设置为TP_TRUE时，指定窗体会被设置成顶级显示）
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_sys_set_active(IPiSysApiAgent *sysApiagent, int pid, int actived);

/**
@brief 对tinyPiX_sys_set_active重新定义
**/
#define tinyPiX_sys_set_active_byid tinyPiX_sys_set_active

/**
@brief 将一个未被激活的虚拟窗体激活，可以从隐藏状态或未占用屏幕状态切换为顶级显示状态（对floatScreen无效）
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param id（int）虚拟窗体id
@param actived（int）设置为TP_FALSE时，指定的虚拟窗体所在应用将不在顶级显示；设置为TP_TRUE时，指定窗体会被设置成顶级显示）
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_sys_set_active_bypid(IPiSysApiAgent *sysApiagent, int pid, int actived);

/**
@brief 通过软件方式设置屏幕亮度
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param lightness（unsigned char）设置屏幕亮度0~255
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_sys_set_lightness(IPiSysApiAgent *sysApiagent, unsigned char lightness);

//add below api
/**
@brief 将虚拟窗体移动到指定位置，（x，y）为虚拟窗体左上角坐标
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param id（int）虚拟窗体id
@param x（int）窗体左上角横坐标
@param y（int）窗体左上角纵坐标
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_sys_float_move(IPiSysApiAgent *sysApiagent, int id, int x, int y);

/**
@brief 根据id关闭指定的虚拟窗体，且虚拟窗体被销毁
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param id（int）虚拟窗体id
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/  
extern DECLSPEC int STDCALL tinyPiX_sys_set_close(IPiSysApiAgent *sysApiagent, int id);

/**
@brief 返回正在显示的顶级窗体id
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@return 成功对应的虚拟窗体id，否则返回-1
*/
extern DECLSPEC int STDCALL tinyPiX_sys_find_top_id(IPiSysApiAgent *sysApiagent);

/**
@brief 返回正在显示的desktop虚拟窗体id
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@return 成功对应的虚拟窗体id，否则返回-1
*/
extern DECLSPEC int STDCALL tinyPiX_sys_find_desk_id(IPiSysApiAgent *sysApiagent);

/**
@brief 根据id向指定的虚拟窗体发送属性数据
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param sid（int）发送app state的虚拟窗体的id（源头id）
@param did（int）接收app state的虚拟窗体的id（目的id）
@param visible（int）源虚拟窗体的显隐属性
@param active（int）源虚拟窗体的激活属性
@param color（int）源虚拟窗体向目标发送请求的颜色
@param alpha（unsigned char）源虚拟窗体向目标发送请求的透明度
@param require（int）与目标协商的自定义内容，接收方可以自定义实现过程
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_sys_send_app_state(IPiSysApiAgent *sysApiagent, 
														int sid, int did, int pid, int visible, int active, int color, unsigned char alpha, int require);
/**
@brief 获取虚拟窗体的id以及所在进程的id
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param bytes（PiShmBytes）窗体id，进程id，窗体类型
@param length（int*）获取窗体的个数
@param range（int）轮询虚拟窗体的类型
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/ 
extern DECLSPEC int STDCALL tinyPiX_sys_find_win_ids(IPiSysApiAgent *sysApiagent, PiShmBytes **bytes, int *length, int range);

/**
@brief 获取虚拟窗体的id以及所在进程的id
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param title（const char*）虚拟窗体的字符串标识（请查阅tinyPiX_wf_set_title函数）
@param length（int*）获取窗体的个数
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/ 
extern DECLSPEC int STDCALL tinyPiX_sys_find_win_ids_bystring(IPiSysApiAgent *sysApiagent, PiShmBytes **bytes, const char *title, int *length);

/**
@brief 获取虚拟窗体的id以及所在进程的id
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param pid（int）进程id
@return 成功返回主窗体id
*/ 
extern DECLSPEC int STDCALL tinyPiX_sys_find_win_main_id_bypid(IPiSysApiAgent *sysApiagent, int pid);

/**
@brief 根据指定的虚拟窗口id，获取对应的pid
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param id（int）虚拟窗口id
@return 成功返回进程id
*/ 
extern DECLSPEC int STDCALL tinyPiX_sys_find_win_pid_byid(IPiSysApiAgent *sysApiagent, int id);

/**
@brief 获取进程应用整体截图
@param sysApiagent(IPisysApiAgent)系统api的句柄
@param pid(int)进程id
@return 成功返回表面指针,失败返回NULL
**/
extern DECLSPEC IPiWFSurface *STDCALL tinyPiX_sys_get_process_surface(IPiSysApiAgent *sysApiagent, int pid);

/**
@brief 获取截图
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param bytes（PiShmBytes）窗体id，进程id，其中窗体类型会被忽略
@return 成功返回表面指针,失败返回NULL
@note 对所有窗体有效,如果采用shared_mode=1方式，获取的截图可能有延迟和滞后
*/
extern DECLSPEC IPiWFSurface *STDCALL tinyPiX_sys_get_obj_surface(IPiSysApiAgent *sysApiagent, int id, int pid);

/**get objDir for whole system**/
extern DECLSPEC const char *STDCALL tinyPiX_sys_get_obj_dir(IPiSysApiAgent *sysApiagent);

/**get dispDir for whole system**/
extern DECLSPEC const char *STDCALL tinyPiX_sys_get_disp_dir(IPiSysApiAgent *sysApiagent);

/**
@brief 屏幕截图
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@return 失败返回NULL
*/
extern DECLSPEC IPiWFSurface *STDCALL tinyPiX_sys_capture_screen(IPiSysApiAgent *sysApiagent);

/**
@brief 向PiXWM发送键盘模拟事件
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param scancode（int）键盘硬扫描码
@param virtualKey（int）虚拟键盘码
@param shortCut（char *）键盘描述字符串，比如ESC键，字符串为“ESC”，如果为空，可以设置为NULL
@param state（int）键的状态，按下TP_TRUE，弹起TP_FALSE
@param keyMod（unsigned short）组合键值，比如（shif+ctrl）
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 详细定义参考keyboard.h
*/
extern DECLSPEC int STDCALL tinyPiX_sys_send_keyboard_event(IPiSysApiAgent *sysApiagent, unsigned int scancode, unsigned int virtualKey, 
														unsigned int symbol, char *shortCut, int state, unsigned short keyMod);
/**
@brief 获取虚拟窗体的id
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param button（unsigned int）鼠标键序号（1-左键，2-中键，3-右键，4-中键向上滚动，5-中键向下滚动， 6-扩展键1， 7-扩展键2，触屏默认1）
@param x（int）设备的横坐标
@param y（int）设备的纵坐标
@param state（int）被按下状态为TP_TRUE，弹起为TP_FALSE
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/ 
extern DECLSPEC int STDCALL tinyPiX_sys_send_mouse_event(IPiSysApiAgent *sysApiagent, unsigned int button, int x, int y, int state);

/**
@brief 获取虚拟窗体的id
@param sysApiagent（IPiSysApiAgent） 系统api的句柄
@param button（unsigned int）鼠标键序号（1-左键，2-中键，3-右键，4-中键向上滚动，5-中键向下滚动， 6-扩展键1， 7-扩展键2，触屏默认1）
@param x（int）设备的横坐标
@param y（int）设备的纵坐标
@param dx（int）表示当前值和上一次值的方向，小于0表示负向，大于0表示正向
@param dy（int）表示当前值和上一次值的方向，小于0表示负向，大于0表示正向
@param state（int）被按下状态为TP_TRUE，弹起为TP_FALSE
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/ 
extern DECLSPEC int STDCALL tinyPiX_sys_send_mouse_motion_event(IPiSysApiAgent *sysApiagent, unsigned int button, int x, int y, int dx, int dy, int state);
#ifdef __cplusplus
}
#endif

#endif
