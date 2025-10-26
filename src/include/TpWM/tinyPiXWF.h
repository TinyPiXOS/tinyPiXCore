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
#ifndef __TINY_PIX_WF_H
#define __TINY_PIX_WF_H

#include "tinyPiXUtils.h"

#ifdef __cplusplus
extern "C"{
#endif
/**
@brief 设置虚拟窗体的焦点
@param focused（int）TP_FALSE表示失去焦点，TP_TRUE表示窗体获取焦点;当设置为focus是TP_FALSE，若agent对应虚拟窗体获取焦点，则无虚拟窗体被PiXWM聚焦
@note  只能对当前主活动主窗体内可见visible的窗体（包括主窗体）进行焦点设置,对其他主窗体无效
*/
extern DECLSPEC void STDCALL tinyPiX_wf_set_global_focus(int focused);

/**
@brief 获取虚拟窗体的焦点
@return 成功返回focus的窗体id, 无效窗体为-1
@note  如果使用该函数的主窗体不可见或没有获得actived主动权，则返回值永远为-1
*/
extern DECLSPEC int STDCALL tinyPiX_wf_get_global_focus();

/**
@brief 获取屏幕鼠标的位置
@param x（int*）鼠标横坐标位置
@param y（int*）鼠标纵坐标位置
@return 成功返回1, 无效0
*/
extern DECLSPEC int STDCALL tinyPiX_wf_get_global_mouse_position(int *x, int *y);

/**
@brief 获取屏幕大小
@param display_width（unsigned int*）屏幕宽度
@param display_height（unsigned int*）屏幕高度
@return 成功返回1, 无效0
*/
extern DECLSPEC int STDCALL tinyPiX_wf_get_global_display_size(unsigned int *display_width, unsigned int *display_height);

/**
@brief 物理尺寸（cm：厘米）
@param display_width_metrics（float *）屏幕宽度
@param display_height（float*）屏幕高度
@return 成功返回1, 无效0
*/
extern DECLSPEC int STDCALL tinyPiX_wf_get_global_display_metrics(float *display_width_metrics, float *display_height_metrics);

/**
@brief 设置系统鼠标光标
@param surface（IPiSurface *）光标的surface数据
@param hot_x（int）鼠标的响应热点x坐标
@param hot_y（int）鼠标的响应热点y坐标
*/
extern DECLSPEC void STDCALL tinyPiX_wf_set_global_display_cursor(IPiWFSurface *surface, int hot_x, int hot_y);

/**虚拟窗体标准api**/

/**
@brief 创建虚拟窗体
@param type（const char*）输入PiXWM可识别的虚拟窗体字符串
@param x（int）虚拟窗体左上角横坐标
@param y（int）虚拟窗体左上角纵坐标
@param width（unsigned int）虚拟窗体宽度
@param height（unsigned int）虚拟窗体高度
@return 成功返回非空句柄，失败返回NULL
@note 仅对floatScreen属性有效，对fixScreen无效（始终与物理屏幕大小一致）
*/
extern DECLSPEC IPiWFApiAgent* STDCALL tinyPiX_wf_create(const char *type, int x, int y, unsigned int width, unsigned int height);

/**
@brief 创建虚拟窗体
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param back_color（unsigned int）初始虚拟窗体的背景颜色，默认_RGBA(0, 0, 0, 255)
@note 在下次active被激活时生效
*/
extern DECLSPEC void STDCALL tinyPiX_wf_back_color(IPiWFApiAgent *agent, unsigned int back_color);

/**
@brief 析构虚拟窗体
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC void STDCALL tinyPiX_wf_free(IPiWFApiAgent *agent);

/**
@brief 为虚拟窗体赋字符串标识（多用于自定义使用）
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_wf_set_title(IPiWFApiAgent *agent, const char *title);

/**
@brief 获取虚拟窗体字符串标识
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@return 虚拟窗体字符串标识，如果未定义标识，则返回NULL
*/
extern DECLSPEC const char *STDCALL tinyPiX_wf_get_title(IPiWFApiAgent *agent);

/**
@brief 设置虚拟窗体的显隐属性
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param visible（int）TP_FALSE表示隐藏，TP_TRUE表示显示
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_wf_set_visible(IPiWFApiAgent *agent, int visible);

/**
@brief 获取虚拟窗体的显隐属性
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@return 成功返回visible属性
*/
extern DECLSPEC int STDCALL tinyPiX_wf_get_visible(IPiWFApiAgent *agent);

/**
@brief 设置虚拟窗体大小
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param x（int）虚拟窗体左上角横坐标
@param y（int）虚拟窗体左上角纵坐标
@param width（unsigned int）虚拟窗体宽度
@param height（unsigned int）虚拟窗体高度
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 仅对floatScreen属性有效，对fixScreen无效（始终与物理屏幕大小一致）
*/
extern DECLSPEC int STDCALL tinyPiX_wf_set_rect(IPiWFApiAgent *agent, int x, int y, unsigned int width, unsigned int height);

/**
@brief 获取设置虚拟窗体大小
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param x（int*）获取虚拟窗体左上角横坐标
@param y（int*）获取虚拟窗体左上角纵坐标
@param width（unsigned int*）获取虚拟窗体宽度
@param height（unsigned int*）获取虚拟窗体高度
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 仅对floatScreen属性有效，对fixScreen无效（始终与物理屏幕大小一致）
*/
extern DECLSPEC int STDCALL tinyPiX_wf_get_rect(IPiWFApiAgent *agent, int *x, int *y, unsigned int *width, unsigned int *height);

/**
@brief 设置虚拟窗体位置
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param x（int）虚拟窗体左上角横坐标
@param y（int）虚拟窗体左上角纵坐标
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 仅对floatScreen属性有效，对fixScreen无效（始终与物理屏幕左上角坐标一致）
*/
extern DECLSPEC int STDCALL tinyPiX_wf_set_position(IPiWFApiAgent *agent, int x, int y);

/**
@brief 获取虚拟窗体位置
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param x（int*）获取虚拟窗体左上角横坐标
@param y（int*）获取虚拟窗体左上角纵坐标
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 仅对floatScreen属性有效，对fixScreen无效（始终与物理屏幕左上角坐标一致,始终为（0,0））
*/
extern DECLSPEC int STDCALL tinyPiX_wf_get_position(IPiWFApiAgent *agent, int *x, int *y);

/**
@brief 设置鼠标或触屏的拖动属性,仅对floatScreen有效
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param canBeMoved（int）悬浮虚拟窗体（floatScreen）是否能够被拖动
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_wf_set_bemoved(IPiWFApiAgent *agent, int canBeMoved);

/**
@brief 获取鼠标或触屏的拖动属性,仅对floatScreen属性有效
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@return 成功返回拖动属性的值
*/
extern DECLSPEC int STDCALL tinyPiX_wf_get_bemoved(IPiWFApiAgent *agent);

/**
@brief 设置虚拟窗体的alpha属性，,仅对floatScreen属性有效
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param alpha（unsigned char）值范围（0~255）
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_wf_set_alpha(IPiWFApiAgent *agent, unsigned char alpha);

/**
@brief 获取虚拟窗体的透明度
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@return 成功返回alpha属性
*/
extern DECLSPEC unsigned char STDCALL tinyPiX_wf_get_alpha(IPiWFApiAgent *agent);//only for floatWin, return 0--255

/**
@brief 使用共享内存表面锁的锁住功能
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 与tinyPiX_wf_unlock_mutex配合成对使用
*/
extern DECLSPEC int STDCALL tinyPiX_wf_lock_mutex(IPiWFApiAgent *agent);

/**
@brief 使用共享内存表面锁的解锁功能
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 与tinyPiX_wf_lock_mutex配合成对使用
*/
extern DECLSPEC int STDCALL tinyPiX_wf_unlock_mutex(IPiWFApiAgent *agent);

/**
@brief 将窗体提升至最顶层
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 仅对floatScreen属性有效
*/
extern DECLSPEC int STDCALL tinyPiX_wf_bring_to_top(IPiWFApiAgent *agent);

/**
@brief 将窗体提升至最底层
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 仅对floatScreen属性有效
*/
extern DECLSPEC int STDCALL tinyPiX_wf_bring_to_bottom(IPiWFApiAgent *agent);

/**
@brief 获取虚拟窗体的是否被激活
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_wf_get_active(IPiWFApiAgent *agent);

/**
@brief 获取虚拟窗体的id
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@return 成功返回非零的值，无法获取返回-1
*/
extern DECLSPEC int STDCALL tinyPiX_wf_get_id(IPiWFApiAgent *agent);

/**
@brief 获取虚拟窗体的所在进程的id
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@return 成功返回非零的值，无法获取返回-1
*/
extern DECLSPEC int STDCALL tinyPiX_wf_get_pid(IPiWFApiAgent *agent);

/**
@brief 获取虚拟窗体的所在进程的id
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@return 成功返回非零的值，无法获取返回-1
*/
extern DECLSPEC IPiWFSurface* STDCALL tinyPiX_wf_get_surface(IPiWFApiAgent *agent);

/**
@brief 获取虚拟窗体的所在的层
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@return 成功返回非零的值(0-special fixScreen 1-common fixScreen 2-user floatScreen 3-sys floatScreen)，无法获取返回-1
*/
extern DECLSPEC int STDCALL tinyPiX_wf_get_layer(IPiWFApiAgent *agent);

/**
@brief 获取物理屏幕的宽和高,单位：厘米
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param displayWidth（unsigned int *）获取返回的宽度
@param displayHeight（unsigned int *）获取返回的宽度
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_wf_get_display_metrics(IPiWFApiAgent *agent, unsigned int *displayWidth, unsigned int *displayHeight);

/**
@brief 获取物理屏幕像素的宽和高
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param displayWidth（unsigned int *）获取返回的宽度
@param displayHeight（unsigned int *）获取返回的宽度
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_wf_get_display_size(IPiWFApiAgent *agent, unsigned int *displayWidth, unsigned int *displayHeight);


/**
@brief 根据id向指定的虚拟窗体发送属性数据
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param did（int）接收app state的虚拟窗体的id（目的id）
@param pid（int）源虚拟窗体的进程id
@param visible（int）源虚拟窗体的显隐属性
@param active（int）源虚拟窗体的激活属性
@param color（int）源虚拟窗体向目标发送请求的颜色
@param alpha（unsigned char）源虚拟窗体向目标发送请求的透明度
@param require（int）与目标协商的自定义内容，接收方可以自定义实现过程
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_wf_send_app_state(IPiWFApiAgent *agents,int did, int visible, int active, int color, unsigned char alpha, int require);

/**
@brief 设置接收motion事件的属性
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param hold（int）1:允许接收motion事件；0：停止接受motion事件
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC void STDCALL tinyPiX_wf_hold_motion(IPiWFApiAgent *agents, int hold);

/**
@brief 刷新虚拟窗体
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param x（int）虚拟窗体左上角横坐标
@param y（int）虚拟窗体左上角纵坐标
@param width（unsigned int）虚拟窗体宽度
@param height（unsigned int）虚拟窗体高度
@param clip（int）是否需要和屏幕矩形进行裁剪
@param sync（int）同步刷新机制，当为TP_TRUE时，虚拟窗体状态立刻更新至PiXWM，否则将缓存在客户端，直到遇到sync为TP_TRUE时，将缓存一并提交（该功能暂未实现）
@return 成功返回TP_TRUE，失败返回TP_FALSE
*/
extern DECLSPEC int STDCALL tinyPiX_wf_update(IPiWFApiAgent *agent, int x, int y, unsigned int width, unsigned int height, int clip, int sync);

/**设置回调函数和参数,参考tinyPiXUtils.h有关回调函数的定义**/

/**
@brief 设置设备事件回调
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param assignEventCallBack（AssignEvent）鼠标、键盘、触屏等事件的回调函数
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 如果设置为NULL，虚拟窗体将丢失本事件
*/
extern DECLSPEC int STDCALL tinyPiX_wf_event_assign(IPiWFApiAgent *agent, AssignEvent assignEventCallBack);

/**
@brief 设置焦点事件回调
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param assignFocusCallBack（AssignFocus）焦点获取或失去的回调函数
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 如果设置为NULL，虚拟窗体将丢失本事件
*/
extern DECLSPEC int STDCALL tinyPiX_wf_focus_assign(IPiWFApiAgent *agent, AssignFocus assignFocusCallBack);

/**
@brief 设置虚拟窗体移入和移出回调事件
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param assignLeaveCallBack（AssignLeave）移入和移出回调函数
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 如果设置为NULL，虚拟窗体将丢失本事件
*/
extern DECLSPEC int STDCALL tinyPiX_wf_leave_assign(IPiWFApiAgent *agent, AssignLeave assignLeaveCallBack);

/**
@brief 虚拟窗口尺寸变化事件回调
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param assignFocusCallBack（AssignFocus）虚拟窗口尺寸变化事回调函数
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 如果设置为NULL，虚拟窗体将丢失本事件
*/
extern DECLSPEC int STDCALL tinyPiX_wf_resize_assign(IPiWFApiAgent *agent, AssignResize assignResizeCallBack);
 
/**
@brief 设置虚拟窗体显隐回调
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param assignFocusCallBack（AssignFocus）窗体显隐回调函数
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 如果设置为NULL，虚拟窗体将丢失本事件
*/
extern DECLSPEC int STDCALL tinyPiX_wf_visible_assign(IPiWFApiAgent *agent, AssignVisible assignVisibleCallBack);

/**
@brief 设置移动事件回调
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param assignFocusCallBack（AssignFocus）移动事件回调函数
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 如果设置为NULL，虚拟窗体将丢失本事件
*/
extern DECLSPEC int STDCALL tinyPiX_wf_moved_assign(IPiWFApiAgent *agent, AssignMoved assignMovedCallBack);

/**
@brief 设置激活事件回调
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param assignFocusCallBack（AssignFocus）激活事件回调函数
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 如果设置为NULL，虚拟窗体将丢失本事件
*/
extern DECLSPEC int STDCALL tinyPiX_wf_actived_assign(IPiWFApiAgent *agent, AssignActived assignActivedCallBack);

/**
@brief 设置设置退出事件回调
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param assignFocusCallBack（AssignFocus）退出事件回调函数
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 如果设置为NULL，虚拟窗体将丢失本事件
*/
extern DECLSPEC int STDCALL tinyPiX_wf_quit_assign(IPiWFApiAgent *agent, AssignQuit assignQuitCallBack);

/**
@brief 设置返回事件回调
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param assignFocusCallBack（AssignFocus）返回事件回调函数
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 如果设置为NULL，虚拟窗体将丢失本事件
*/
extern DECLSPEC int STDCALL tinyPiX_wf_return_assign(IPiWFApiAgent *agent, AssignReturn assignReturnCallBack);

/**
@brief 设置应用传递数据事件回调
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param assignFocusCallBack（AssignFocus）应用传递数据事件回调函数
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 如果设置为NULL，虚拟窗体将丢失本事件
*/
extern DECLSPEC int STDCALL tinyPiX_wf_app_assign(IPiWFApiAgent *agent, AssignAppChange assignAppCallBack);//only for wm win

/**
@brief 设置回调数据
@param agent（IPiWFApiAgent*）虚拟窗体句柄
@param assignFocusCallBack（AssignFocus）回调数据
@return 成功返回TP_TRUE，失败返回TP_FALSE
@note 如果设置为NULL，虚拟窗体将丢失本事件
*/
extern DECLSPEC int STDCALL tinyPiX_wf_args_assign(IPiWFApiAgent *agent, void *args);//this will be passed by about callback

/**-------------------------------------------------------------------------------------------------------------------------------------**/
#ifdef __cplusplus
}
#endif

#endif
