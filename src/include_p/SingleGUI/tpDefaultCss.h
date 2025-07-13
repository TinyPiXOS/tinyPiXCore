/***
 * @Author: hywang
 * @Date: 2024-08-17 10:54:59
 * @LastEditors: hywang
 * @LastEditTime: 2024-08-17 10:55:55
 * @FilePath: /pix-singlegui/PixSingleGUI/include_p/SingleGUI/tpDefaultCss.h
 * @Description: 默认CSS样式
 * @
 * @PiXOS
 */
#ifndef __TP_DEFAULT_CSS_H
#define __TP_DEFAULT_CSS_H

#include <tpString.h>

/// @brief 根据主题类型，获取对应CSS文件全路径
/// @param style
/// @return
static tpString parseThemeFile(const tinyPiX::SystemTheme &style)
{
	switch (style)
	{
	case tinyPiX::Default:
		return "/usr/data/tinyPiX/SystemTheme/purple-light.css";
	case tinyPiX::PurpleLight:
		return "/usr/data/tinyPiX/SystemTheme/purple-light.css";
	default:
		return "/usr/data/tinyPiX/SystemTheme/purple-light.css";
	}

	return "/usr/data/tinyPiX/SystemTheme/purple-light.css";
}

#endif