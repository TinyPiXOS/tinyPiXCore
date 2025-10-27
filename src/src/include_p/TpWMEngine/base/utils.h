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
#ifndef __UTILS_H
#define __UTILS_H

#include "typesDef.h"
#include "surface.h"

#include <sys/mman.h>
#include <stdint.h>

#define pixwm_output_errors(msg)	\
do							\
{							\
	fprintf(stderr, "\033[0m\033[31m%s\033[0m", (msg));\
}while(0);

#define pixwm_output_message(msg)	\
do							\
{							\
	fprintf(stderr, "\033[0m\033[32m%s\033[0m", (msg));\
}while(0);

#define pixwm_output_warning(msg)	\
do							\
{							\
	fprintf(stderr, "\033[0m\033[1;33m%s\033[0m", (msg));\
}while(0);

#define pixwm_output_normal(msg)	\
do							\
{							\
	fprintf(stderr, "%s", (msg));\
}while(0);

#define SURFACE_MIN_SIZE	1
#define SURFACE_MAX_SIZE	65536
#define SURFACE_CHECK_SIZE(size)	(((size) >= SURFACE_MIN_SIZE) && ((size) <= SURFACE_MAX_SIZE))
#define IS_ALIGNED(ptr) (((uintptr_t)(ptr) % (sysconf(_SC_PAGESIZE))) == 0)

#ifdef __cplusplus
extern "C"{
#endif

static inline tpBool is_valid(void *addr) 
{
    tpUInt8 vec;
    return (mincore(addr, sysconf(_SC_PAGESIZE), &vec) == 0);
}

static inline tpBool is_readable(void *surface) 
{
	void *addr = surface_get_matrix(surface);
	
    if (addr == NULL) return TP_FALSE;
    size_t page_size = sysconf(_SC_PAGESIZE);
    void *aligned_addr = (void*)((uintptr_t)addr & ~(page_size - 1));
    
    if (!IS_ALIGNED(aligned_addr)) return TP_FALSE;
    if (!is_valid(aligned_addr)) return TP_FALSE;

    return (!mprotect(aligned_addr, page_size, PROT_READ | PROT_WRITE));
}

static inline tpBool is_width_valid(void *surface)
{
	return SURFACE_CHECK_SIZE(surface_get_width(surface));
}

static inline tpBool is_height_valid(void *surface)
{
	return SURFACE_CHECK_SIZE(surface_get_height(surface));
}

static inline tpBool is_size_valid(void *surface)
{
	return (is_width_valid(surface) && is_height_valid(surface));
}

#define IS_SURFACE_VALID(surface) (is_readable((surface)) && is_size_valid((surface)))

/**time delay, u second**/
extern DECLSPEC void STDCALL timer_delay(tpUInt64 usec);
/**generate proper directory or filename**/
extern DECLSPEC char* STDCALL dir_generate_string(const tpChar *rundir, const tpChar *runwork, const tpChar *subdir, const tpChar *filename);
/**dir create function**/
extern DECLSPEC tpBool STDCALL dir_do_create(const tpChar *dir);
/**set file mode**/
extern DECLSPEC tpBool STDCALL mode_do_set(const tpChar *file, mode_t mode);
/**trim line**/
extern DECLSPEC void STDCALL trim_line(tpChar *line);
/**upper case**/
extern DECLSPEC tpBool STDCALL string_upcase(tpChar *args);
/**mount tmpfs**/
extern DECLSPEC tpBool STDCALL mount_tmpfs(const tpChar *target, tpUInt32 size);

/**umount tmpfs**/
extern DECLSPEC void STDCALL umount_tmpfs(const tpChar *target);
/**set limit**/
extern DECLSPEC void STDCALL set_unlimited_files();
/**check pixwm run**/
extern DECLSPEC tpBool STDCALL check_pixwm_run(const tpChar *runPath);
/**create deamon**/
extern DECLSPEC void STDCALL daemon_create();

/**create hard link file**/
extern DECLSPEC tpBool STDCALL create_obj_hard_file(const tpChar *filename, const tpChar *uuid_map);

#ifdef __cplusplus
}
#endif

#endif
