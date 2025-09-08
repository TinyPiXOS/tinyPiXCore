/***
 * @Author: hywang
 * @Date: 2024-08-17 10:54:59
 * @LastEditors: hywang
 * @LastEditTime: 2024-08-17 10:55:55
 * @FilePath: /pix-singlegui/PixSingleGUI/include_p/SingleGUI/TpDefaultCss.h
 * @Description: 默认CSS样式
 * @
 * @PiXOS
 */
#ifndef __TP_DEFAULT_CSS_H
#define __TP_DEFAULT_CSS_H

#include <TpString.h>

/// @brief 根据主题类型，获取对应CSS文件全路径
/// @param style
/// @return
static TpString parseThemeFile(const tinyPiX::SystemTheme &style)
{
	switch (style)
	{
	case tinyPiX::Default:
		return "/usr/data/tinyPiX/SystemTheme/DefaultStyle.css";
	case tinyPiX::SmartDeviceGUIStyle:
		return "/usr/data/tinyPiX/SystemTheme/SmartDeviceGUI.css";
	default:
		return "/usr/data/tinyPiX/SystemTheme/DefaultStyle.css";
	}

	return "/usr/data/tinyPiX/SystemTheme/DefaultStyle.css";
}

#endif