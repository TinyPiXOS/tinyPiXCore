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
#ifndef __RS_SHARE_H
#define __RS_SHARE_H

#include "TpWMEngine/dataDefine.h"

//run mode
enum{
	PIXWM_RUN_SIMULATOR,
	PIXWM_RUN_REALARCH,
};

//readType
enum{
	PIXWM_PHYSICAL_DATA,
	PIXWM_MLOGICAL_DATA,
};

typedef struct ItpFloatSize{
	tpFloat w;
	tpFloat h;
}ItpFloatSize;

TP_DEF_VOID_TYPE_VAR(IPiShare);

#ifdef __cplusplus
extern "C"{
#endif

/**create share data cache**/
extern DECLSPEC IPiShare *STDCALL rsshare_do_create(const tpChar *shpath,
													tpInt32 running_mode, 
													tpInt32 physical_x, tpInt32 physical_y, tpUInt32 physical_width, tpUInt32 physical_height, 
													tpUInt32 physical_metrics_x, tpUInt32 physical_metrics_y,
													tpInt32 logical_x, tpInt32 logical_y, tpUInt32 logical_width, tpUInt32 logical_height, 
													tpFloat logical_metrics_x, tpFloat logical_metrics_y,
													tpInt32 focus_id, tpInt32 current_id, tpBool isCreate);
/**read share data cache**/
extern DECLSPEC IPiShare *STDCALL rsshare_do_read(const tpChar *shpath);

/**write share data running mode**/
extern DECLSPEC void STDCALL rsshare_write_isSimulator(IPiShare *share, tpBool isSimulator);

/**write share data position for mouse function**/
extern DECLSPEC tpBool STDCALL rsshare_write_position(IPiShare *share, tpBool islogicalMode, tpInt32 x, tpInt32 y);

/**write share data width and height for display function**/
extern DECLSPEC tpBool STDCALL rsshare_write_display(IPiShare *share, tpBool islogicalMode, tpUInt32 width, tpUInt32 height);

/**write share data metrics for display function**/
extern DECLSPEC tpBool STDCALL rsshare_write_metrics(IPiShare *share, tpBool islogicalMode, tpFloat metricsX, tpFloat metricsY);

/**write focus id for display function**/
extern DECLSPEC tpBool STDCALL rsshare_write_focus(IPiShare *share, tpBool islogicalMode, tpInt32 focusID);

/**write current id for display function**/
extern DECLSPEC tpBool STDCALL rsshare_write_current(IPiShare *share, tpBool islogicalMode, tpInt32 currentID);

/**read share data running mode**/
extern DECLSPEC tpBool STDCALL rsshare_read_isSimulator(IPiShare *share);

/**read position for mouse**/
extern DECLSPEC struct PiPoint STDCALL rsshare_read_position(IPiShare *share, tpBool islogicalMode);

/**read width and height for display**/
extern DECLSPEC struct PiSize STDCALL rsshare_read_display(IPiShare *share, tpBool islogicalMode);

/**read metrics for display**/
extern DECLSPEC struct ItpFloatSize STDCALL rsshare_read_metrics(IPiShare *share, tpBool islogicalMode);

/**read focus id for display function**/
extern DECLSPEC tpInt32 STDCALL rsshare_read_focus(IPiShare *share, tpBool islogicalMode);

/**read current id for display function**/
extern DECLSPEC tpInt32 STDCALL rsshare_read_current(IPiShare *share, tpBool islogicalMode);

/**desotry share data**/
extern DECLSPEC void STDCALL rsshare_do_destroy(IPiShare *share);

#ifdef __cplusplus
}
#endif

#endif
