#ifndef __TP_UTILS_H
#define __TP_UTILS_H

#include <cstdint>
#include <iostream>
#include <atomic>
#include "typesDef.h"
#include "keyboard.h"
#include "event.h"
#include "tpString.h"
#include "tpAlphaTable.h"

#ifndef STDCALL
#define STDCALL
#endif

#define TP_DEF_VOID_TYPE_VAR(v) \
	typedef void v

#define TP_INVALIDATE_VALUE -1

// #ifdef TP_RGB_UNKOWN
// #undef TP_RGB_UNKOWN
// #endif

typedef enum
{
	TP_UNKOWN_OBJECT = -1,
	TP_TOP_OBJECT,			// FixScreen
	TP_FLOAT_OBJECT,
	TP_CHILD_OBJECT,
} ItpObjectType;

typedef enum
{
	TP_WM_NONE = -1,
	TP_WM_DESK,
	TP_WM_WIN,
	TP_WM_USE_FLOAT,
	TP_WM_SYS_FLOAT,
	TP_WM_LAYER_NUMBERS,
} ItpObjectSysLayer;

#ifndef PATH_MAX
#define PATH_MAX 2048
#endif

// 获取exe所在目录
static tpString applicationDirPath()
{
	char exePath[PATH_MAX] = {0};
	size_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1); // 读取符号链接

	// 错误处理
	if (len == -1)
	{
		throw std::runtime_error("Failed to read /proc/self/exe");
	}
	exePath[len] = '\0'; // 确保字符串终止

	// 提取父目录路径
	char *lastSlash = strrchr(exePath, '/');
	if (!lastSlash)
	{
		return tpString(); // 无效路径
	}

	// 处理根目录的特殊情况（如 "/usr" → "/"）
	if (lastSlash == exePath)
	{
		return tpString("/");
	}

	// 截取目录部分
	*lastSlash = '\0'; // 将最后一个 '/' 替换为终止符
	return tpString(exePath);
}

static uint32_t generateTimerId()
{
	static std::atomic<uint32_t> uniqueIdGenerator(0);

	return uniqueIdGenerator.fetch_add(1);
}

static uint8_t mapAlpha(uint8_t color, uint8_t alpha)
{
	if (alpha == 0xff)
		return color;

	return alphaVertTatble[color][alpha];
}

#endif

#ifndef WIDGET_MAX_WIDTH
#define WIDGET_MAX_WIDTH 32767
#endif

#ifndef WIDGET_MAX_HEIGHT
#define WIDGET_MAX_HEIGHT 32767
#endif