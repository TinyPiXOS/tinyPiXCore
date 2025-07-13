
#ifndef __TP_CLIP_BOARD_H
#define __TP_CLIP_BOARD_H

#include "tpUtils.h"
#include "tpSurface.h"
#include <string>

TP_DEF_VOID_TYPE_VAR(ItpClipboardData);
/// @brief 剪切板，可以设置、获取剪切板内容，大小限制为2MB
class tpClipboard
{
public:
	static tpClipboard *Inst(); // singleton

private:
	tpClipboard();

public:
	virtual ~tpClipboard();

	/// @brief 设置剪切板文本
	/// @param text 文本字符串
	virtual void setText(const tpString &text);

	/// @brief 获取剪切板当前文本字符串
	/// @return 文本字符串
	virtual tpString text();

	/// @brief 剪切板内是否存在文本
	/// @return 存在返回true，否则返回false
	virtual bool hasText();

	/// @brief 获取剪切板内容大小
	/// @return 大小，单位KB
	virtual uint64_t size();

	/// @brief 清空剪切板
	virtual void clear();

private:
	ItpClipboardData *data_;
};

#endif
