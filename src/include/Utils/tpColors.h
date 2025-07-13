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
#ifndef __TP_COLORS_H
#define __TP_COLORS_H

#include <tpUtils.h>

class tpColors
{
public:
	enum
	{
		Snow = 0xFFFAFAFF,
		GhostWhite = 0xF8F8FFFF,
		WhiteSmoke = 0xF5F5F5FF,
		Gainsboro = 0xDCDCDCFF,
		FloralWhite = 0xFFFAF0FF,
		OldLace = 0xFDF5E6FF,
		Linen = 0xFAF0E6FF,
		AntiqueWhite = 0xFAEBD7FF,
		PapayaWhip = 0xFFEFD5FF,
		BlanchedAlmond = 0xFFEBCDFF,
		Bisque = 0xFFE4C4FF,
		PeachPuff = 0xFFDAB9FF,
		NavajoWhite = 0xFFDEADFF,
		Moccasin = 0xFFE4B5FF,
		Cornsilk = 0xFFF8DCFF,
		Ivory = 0xFFFFF0FF,
		LemonChiffon = 0xFFFACDFF,
		Seashell = 0xFFF5EEFF,
		Honeydew = 0xF0FFF0FF,
		MintCream = 0xF5FFFAFF,
		Azure = 0xF0FFFFFF,
		AliceBlue = 0xF0F8FFFF,
		lavender = 0xE6E6FAFF,
		LavenderBlush = 0xFFF0F5FF,
		MistyRose = 0xFFE4E1FF,
		White = 0xFFFFFFFF,
		Black = 0x000000FF,
		DarkSlateGray = 0x2F4F4FFF,
		DimGrey = 0x696969FF,
		SlateGrey = 0x708090FF,
		LightSlateGray = 0x778899FF,
		Grey = 0xBEBEBEFF,
		LightGray = 0xD3D3D3FF,
		MidnightBlue = 0x191970FF,
		NavyBlue = 0x000080FF,
		CornflowerBlue = 0x6495EDFF,
		DarkSlateBlue = 0x483D8BFF,
		SlateBlue = 0x6A5ACDFF,
		MediumSlateBlue = 0x7B68EEFF,
		LightSlateBlue = 0x8470FFFF,
		MediumBlue = 0x0000CDFF,
		RoyalBlue = 0x4169E1FF,
		Blue = 0x0000FFFF,
		DodgerBlue = 0x1E90FFFF,
		DeepSkyBlue = 0x00BFFFFF,
		SkyBlue = 0x87CEEBFF,
		LightSkyBlue = 0x87CEFAFF,
		SteelBlue = 0x4682B4FF,
		LightSteelBlue = 0xB0C4DEFF,
		LightBlue = 0xADD8E6FF,
		PowderBlue = 0xB0E0E6FF,
		PaleTurquoise = 0xAFEEEEFF,
		DarkTurquoise = 0x00CED1FF,
		MediumTurquoise = 0x48D1CCFF,
		Turquoise = 0x40E0D0FF,
		Cyan = 0x00FFFFFF,
		LightCyan = 0xE0FFFFFF,
		CadetBlue = 0x5F9EA0FF,
		MediumAquamarine = 0x66CDAAFF,
		Aquamarine = 0x7FFFD4FF,
		DarkGreen = 0x006400FF,
		DarkOliveGreen = 0x556B2FFF,
		DarkSeaGreen = 0x8FBC8FFF,
		SeaGreen = 0x2E8B57FF,
		MediumSeaGreen = 0x3CB371FF,
		LightSeaGreen = 0x20B2AAFF,
		PaleGreen = 0x98FB98FF,
		SpringGreen = 0x00FF7FFF,
		LawnGreen = 0x7CFC00FF,
		Green = 0x00FF00FF,
		Chartreuse = 0x7FFF00FF,
		MedSpringGreen = 0x00FA9AFF,
		GreenYellow = 0xADFF2FFF,
		LimeGreen = 0x32CD32FF,
		YellowGreen = 0x9ACD32FF,
		ForestGreen = 0x228B22FF,
		OliveDrab = 0x6B8E23FF,
		DarkKhaki = 0xBDB76BFF,
		PaleGoldenrod = 0xEEE8AAFF,
		LtGoldenrodYello = 0xFAFAD2FF,
		LightYellow = 0xFFFFE0FF,
		Yellow = 0xFFFF00FF,
		Gold = 0xFFD700FF,
		LightGoldenrod = 0xEEDD82FF,
		goldenrod = 0xDAA520FF,
		DarkGoldenrod = 0xB8860BFF,
		RosyBrown = 0xBC8F8FFF,
		IndianRed = 0xCD5C5CFF,
		SaddleBrown = 0x8B4513FF,
		Sienna = 0xA0522DFF,
		Peru = 0xCD853FFF,
		Burlywood = 0xDEB887FF,
		Beige = 0xF5F5DCFF,
		Wheat = 0xF5DEB3FF,
		SandyBrown = 0xF4A460FF,
		Tan = 0xD2B48CFF,
		Chocolate = 0xD2691EFF,
		Firebrick = 0xB22222FF,
		Brown = 0xA52A2AFF,
		DarkSalmon = 0xE9967AFF,
		Salmon = 0xFA8072FF,
		LightSalmon = 0xFFA07AFF,
		Orange = 0xFFA500FF,
		DarkOrange = 0xFF8C00FF,
		Coral = 0xFF7F50FF,
		LightCoral = 0xF08080FF,
		Tomato = 0xFF6347FF,
		OrangeRed = 0xFF4500FF,
		Red = 0xFF0000FF,
		HotPink = 0xFF69B4FF,
		DeepPink = 0xFF1493FF,
		Pink = 0xFFC0CBFF,
		LightPink = 0xFFB6C1FF,
		PaleVioletRed = 0xDB7093FF,
		Maroon = 0xB03060FF,
		MediumVioletRed = 0xC71585FF,
		VioletRed = 0xD02090FF,
		Magenta = 0xFF00FFFF,
		Violet = 0xEE82EEFF,
		Plum = 0xDDA0DDFF,
		Orchid = 0xDA70D6FF,
		MediumOrchid = 0xBA55D3FF,
		DarkOrchid = 0x9932CCFF,
		DarkViolet = 0x9400D3FF,
		BlueViolet = 0x8A2BE2FF,
		Purple = 0xA020F0FF,
		MediumPurple = 0x9370DBFF,
		Thistle = 0xD8BFD8FF,
		Snow1 = 0xFFFAFAFF,
		Snow2 = 0xEEE9E9FF,
		Snow3 = 0xCDC9C9FF,
		Snow4 = 0x8B8989FF,
		Seashell1 = 0xFFF5EEFF,
		Seashell2 = 0xEEE5DEFF,
		Seashell3 = 0xCDC5BFFF,
		Seashell4 = 0x8B8682FF,
		AntiqueWhite1 = 0xFFEFDBFF,
		AntiqueWhite2 = 0xEEDFCCFF,
		AntiqueWhite3 = 0xCDC0B0FF,
		AntiqueWhite4 = 0x8B8378FF,
		Bisque1 = 0xFFE4C4FF,
		Bisque2 = 0xEED5B7FF,
		Bisque3 = 0xCDB79EFF,
		Bisque4 = 0x8B7D6BFF,
		PeachPuff1 = 0xFFDAB9FF,
		PeachPuff2 = 0xEECBADFF,
		PeachPuff3 = 0xCDAF95FF,
		PeachPuff4 = 0x8B7765FF,
		NavajoWhite1 = 0xFFDEADFF,
		NavajoWhite2 = 0xEECFA1FF,
		NavajoWhite3 = 0xCDB38BFF,
		NavajoWhite4 = 0x8B795EFF,
		LemonChiffon1 = 0xFFFACDFF,
		LemonChiffon2 = 0xEEE9BFFF,
		LemonChiffon3 = 0xCDC9A5FF,
		LemonChiffon4 = 0x8B8970FF,
		Cornsilk1 = 0xFFF8DCFF,
		Cornsilk2 = 0xEEE8CDFF,
		Cornsilk3 = 0xCDC8B1FF,
		Cornsilk4 = 0x8B8878FF,
		Ivory1 = 0xFFFFF0FF,
		Ivory2 = 0xEEEEE0FF,
		Ivory3 = 0xCDCDC1FF,
		Ivory4 = 0x8B8B83FF,
		Honeydew1 = 0xF0FFF0FF,
		Honeydew2 = 0xE0EEE0FF,
		Honeydew3 = 0xC1CDC1FF,
		Honeydew4 = 0x838B83FF,
		LavenderBlush1 = 0xFFF0F5FF,
		LavenderBlush2 = 0xEEE0E5FF,
		LavenderBlush3 = 0xCDC1C5FF,
		LavenderBlush4 = 0x8B8386FF,
		MistyRose1 = 0xFFE4E1FF,
		MistyRose2 = 0xEED5D2FF,
		MistyRose3 = 0xCDB7B5FF,
		MistyRose4 = 0x8B7D7BFF,
		Azure1 = 0xF0FFFFFF,
		Azure2 = 0xE0EEEEFF,
		Azure3 = 0xC1CDCDFF,
		Azure4 = 0x838B8BFF,
		SlateBlue1 = 0x836FFFFF,
		SlateBlue2 = 0x7A67EEFF,
		SlateBlue3 = 0x6959CDFF,
		SlateBlue4 = 0x473C8BFF,
		RoyalBlue1 = 0x4876FFFF,
		RoyalBlue2 = 0x436EEEFF,
		RoyalBlue3 = 0x3A5FCDFF,
		RoyalBlue4 = 0x27408BFF,
		Blue1 = 0x0000FFFF,
		Blue2 = 0x0000EEFF,
		Blue3 = 0x0000CDFF,
		Blue4 = 0x00008BFF,
		DodgerBlue1 = 0x1E90FFFF,
		DodgerBlue2 = 0x1C86EEFF,
		DodgerBlue3 = 0x1874CDFF,
		DodgerBlue4 = 0x104E8BFF,
		SteelBlue1 = 0x63B8FFFF,
		SteelBlue2 = 0x5CACEEFF,
		SteelBlue3 = 0x4F94CDFF,
		SteelBlue4 = 0x36648BFF,
		DeepSkyBlue1 = 0x00BFFFFF,
		DeepSkyBlue2 = 0x00B2EEFF,
		DeepSkyBlue3 = 0x009ACDFF,
		DeepSkyBlue4 = 0x00688BFF,
		SkyBlue1 = 0x87CEFFFF,
		SkyBlue2 = 0x7EC0EEFF,
		SkyBlue3 = 0x6CA6CDFF,
		SkyBlue4 = 0x4A708BFF,
		LightSkyBlue1 = 0xB0E2FFFF,
		LightSkyBlue2 = 0xA4D3EEFF,
		LightSkyBlue3 = 0x8DB6CDFF,
		LightSkyBlue4 = 0x607B8BFF,
		SlateGray1 = 0xC6E2FFFF,
		SlateGray2 = 0xB9D3EEFF,
		SlateGray3 = 0x9FB6CDFF,
		SlateGray4 = 0x6C7B8BFF,
		LightSteelBlue1 = 0xCAE1FFFF,
		LightSteelBlue2 = 0xBCD2EEFF,
		LightSteelBlue3 = 0xA2B5CDFF,
		LightSteelBlue4 = 0x6E7B8BFF,
		LightBlue1 = 0xBFEFFFFF,
		LightBlue2 = 0xB2DFEEFF,
		LightBlue3 = 0x9AC0CDFF,
		LightBlue4 = 0x68838BFF,
		LightCyan1 = 0xE0FFFFFF,
		LightCyan2 = 0xD1EEEEFF,
		LightCyan3 = 0xB4CDCDFF,
		LightCyan4 = 0x7A8B8BFF,
		PaleTurquoise1 = 0xBBFFFFFF,
		PaleTurquoise2 = 0xAEEEEEFF,
		PaleTurquoise3 = 0x96CDCDFF,
		PaleTurquoise4 = 0x668B8BFF,
		CadetBlue1 = 0x98F5FFFF,
		CadetBlue2 = 0x8EE5EEFF,
		CadetBlue3 = 0x7AC5CDFF,
		CadetBlue4 = 0x53868BFF,
		Turquoise1 = 0x00F5FFFF,
		Turquoise2 = 0x00E5EEFF,
		Turquoise3 = 0x00C5CDFF,
		Turquoise4 = 0x00868BFF,
		Cyan1 = 0x00FFFFFF,
		Cyan2 = 0x00EEEEFF,
		Cyan3 = 0x00CDCDFF,
		Cyan4 = 0x008B8BFF,
		DarkSlateGray1 = 0x97FFFFFF,
		DarkSlateGray2 = 0x8DEEEEFF,
		DarkSlateGray3 = 0x79CDCDFF,
		DarkSlateGray4 = 0x528B8BFF,
		Aquamarine1 = 0x7FFFD4FF,
		Aquamarine2 = 0x76EEC6FF,
		Aquamarine3 = 0x66CDAAFF,
		Aquamarine4 = 0x458B74FF,
		DarkSeaGreen1 = 0xC1FFC1FF,
		DarkSeaGreen2 = 0xB4EEB4FF,
		DarkSeaGreen3 = 0x9BCD9BFF,
		DarkSeaGreen4 = 0x698B69FF,
		SeaGreen1 = 0x54FF9FFF,
		SeaGreen2 = 0x4EEE94FF,
		SeaGreen3 = 0x43CD80FF,
		SeaGreen4 = 0x2E8B57FF,
		PaleGreen1 = 0x9AFF9AFF,
		PaleGreen2 = 0x90EE90FF,
		PaleGreen3 = 0x7CCD7CFF,
		PaleGreen4 = 0x548B54FF,
		SpringGreen1 = 0x00FF7FFF,
		SpringGreen2 = 0x00EE76FF,
		SpringGreen3 = 0x00CD66FF,
		SpringGreen4 = 0x008B45FF,
		Green1 = 0x00FF00FF,
		Green2 = 0x00EE00FF,
		Green3 = 0x00CD00FF,
		Green4 = 0x008B00FF,
		Chartreuse1 = 0x7FFF00FF,
		Chartreuse2 = 0x76EE00FF,
		Chartreuse3 = 0x66CD00FF,
		Chartreuse4 = 0x458B00FF,
		OliveDrab1 = 0xC0FF3EFF,
		OliveDrab2 = 0xB3EE3AFF,
		OliveDrab3 = 0x9ACD32FF,
		OliveDrab4 = 0x698B22FF,
		DarkOliveGreen1 = 0xCAFF70FF,
		DarkOliveGreen2 = 0xBCEE68FF,
		DarkOliveGreen3 = 0xA2CD5AFF,
		DarkOliveGreen4 = 0x6E8B3DFF,
		Khaki1 = 0xFFF68FFF,
		Khaki2 = 0xEEE685FF,
		Khaki3 = 0xCDC673FF,
		Khaki4 = 0x8B864EFF,
		LightGoldenrod1 = 0xFFEC8BFF,
		LightGoldenrod2 = 0xEEDC82FF,
		LightGoldenrod3 = 0xCDBE70FF,
		LightGoldenrod4 = 0x8B814CFF,
		LightYellow1 = 0xFFFFE0FF,
		LightYellow2 = 0xEEEED1FF,
		LightYellow3 = 0xCDCDB4FF,
		LightYellow4 = 0x8B8B7AFF,
		Yellow1 = 0xFFFF00FF,
		Yellow2 = 0xEEEE00FF,
		Yellow3 = 0xCDCD00FF,
		Yellow4 = 0x8B8B00FF,
		Gold1 = 0xFFD700FF,
		Gold2 = 0xEEC900FF,
		Gold3 = 0xCDAD00FF,
		Gold4 = 0x8B7500FF,
		Goldenrod1 = 0xFFC125FF,
		Goldenrod2 = 0xEEB422FF,
		Goldenrod3 = 0xCD9B1DFF,
		Goldenrod4 = 0x8B6914FF,
		DarkGoldenrod1 = 0xFFB90FFF,
		DarkGoldenrod2 = 0xEEAD0EFF,
		DarkGoldenrod3 = 0xCD950CFF,
		DarkGoldenrod4 = 0x8B658BFF,
		RosyBrown1 = 0xFFC1C1FF,
		RosyBrown2 = 0xEEB4B4FF,
		RosyBrown3 = 0xCD9B9BFF,
		RosyBrown4 = 0x8B6969FF,
		IndianRed1 = 0xFF6A6AFF,
		IndianRed2 = 0xEE6363FF,
		IndianRed3 = 0xCD5555FF,
		IndianRed4 = 0x8B3A3AFF,
		Sienna1 = 0xFF8247FF,
		Sienna2 = 0xEE7942FF,
		Sienna3 = 0xCD6839FF,
		Sienna4 = 0x8B4726FF,
		Burlywood1 = 0xFFD39BFF,
		Burlywood2 = 0xEEC591FF,
		Burlywood3 = 0xCDAA7DFF,
		Burlywood4 = 0x8B7355FF,
		Wheat1 = 0xFFE7BAFF,
		Wheat2 = 0xEED8AEFF,
		Wheat3 = 0xCDBA96FF,
		Wheat4 = 0x8B7E66FF,
		Tan1 = 0xFFA54FFF,
		Tan2 = 0xEE9A49FF,
		Tan3 = 0xCD853FFF,
		Tan4 = 0x8B5A2BFF,
		Chocolate1 = 0xFF7F24FF,
		Chocolate2 = 0xEE7621FF,
		Chocolate3 = 0xCD661DFF,
		Chocolate4 = 0x8B4513FF,
		Firebrick1 = 0xFF3030FF,
		Firebrick2 = 0xEE2C2CFF,
		Firebrick3 = 0xCD2626FF,
		Firebrick4 = 0x8B1A1AFF,
		Brown1 = 0xFF4040FF,
		Brown2 = 0xEE3B3BFF,
		Brown3 = 0xCD3333FF,
		Brown4 = 0x8B2323FF,
		Salmon1 = 0xFF8C69FF,
		Salmon2 = 0xEE8262FF,
		Salmon3 = 0xCD7054FF,
		Salmon4 = 0x8B4C39FF,
		LightSalmon1 = 0xFFA07AFF,
		LightSalmon2 = 0xEE9572FF,
		LightSalmon3 = 0xCD8162FF,
		LightSalmon4 = 0x8B5742FF,
		Orange1 = 0xFFA500FF,
		Orange2 = 0xEE9A00FF,
		Orange3 = 0xCD8500FF,
		Orange4 = 0x8B5A00FF,
		DarkOrange1 = 0xFF7F00FF,
		DarkOrange2 = 0xEE7600FF,
		DarkOrange3 = 0xCD6600FF,
		DarkOrange4 = 0x8B4500FF,
		Coral1 = 0xFF7256FF,
		Coral2 = 0xEE6A50FF,
		Coral3 = 0xCD5B45FF,
		Coral4 = 0x8B3E2FFF,
		Tomato1 = 0xFF6347FF,
		Tomato2 = 0xEE5C42FF,
		Tomato3 = 0xCD4F39FF,
		Tomato4 = 0x8B3626FF,
		OrangeRed1 = 0xFF4500FF,
		OrangeRed2 = 0xEE4000FF,
		OrangeRed3 = 0xCD3700FF,
		OrangeRed4 = 0x8B2500FF,
		Red1 = 0xFF0000FF,
		Red2 = 0xEE0000FF,
		Red3 = 0xCD0000FF,
		Red4 = 0x8B0000FF,
		DeepPink1 = 0xFF1493FF,
		DeepPink2 = 0xEE1289FF,
		DeepPink3 = 0xCD1076FF,
		DeepPink4 = 0x8B0A50FF,
		HotPink1 = 0xFF6EB4FF,
		HotPink2 = 0xEE6AA7FF,
		HotPink3 = 0xCD6090FF,
		HotPink4 = 0x8B3A62FF,
		Pink1 = 0xFFB5C5FF,
		Pink2 = 0xEEA9B8FF,
		Pink3 = 0xCD919EFF,
		Pink4 = 0x8B636CFF,
		LightPink1 = 0xFFAEB9FF,
		LightPink2 = 0xEEA2ADFF,
		LightPink3 = 0xCD8C95FF,
		LightPink4 = 0x8B5F65FF,
		PaleVioletRed1 = 0xFF82ABFF,
		PaleVioletRed2 = 0xEE799FFF,
		PaleVioletRed3 = 0xCD6889FF,
		PaleVioletRed4 = 0x8B475DFF,
		Maroon1 = 0xFF34B3FF,
		Maroon2 = 0xEE30A7FF,
		Maroon3 = 0xCD2990FF,
		Maroon4 = 0x8B1C62FF,
		VioletRed1 = 0xFF3E96FF,
		VioletRed2 = 0xEE3A8CFF,
		VioletRed3 = 0xCD3278FF,
		VioletRed4 = 0x8B2252FF,
		Magenta1 = 0xFF00FFFF,
		Magenta2 = 0xEE00EEFF,
		Magenta3 = 0xCD00CDFF,
		Magenta4 = 0x8B008BFF,
		Orchid1 = 0xFF83FAFF,
		Orchid2 = 0xEE7AE9FF,
		Orchid3 = 0xCD69C9FF,
		Orchid4 = 0x8B4789FF,
		Plum1 = 0xFFBBFFFF,
		Plum2 = 0xEEAEEEFF,
		Plum3 = 0xCD96CDFF,
		Plum4 = 0x8B668BFF,
		MediumOrchid1 = 0xE066FFFF,
		MediumOrchid2 = 0xD15FEEFF,
		MediumOrchid3 = 0xB452CDFF,
		MediumOrchid4 = 0x7A378BFF,
		DarkOrchid1 = 0xBF3EFFFF,
		DarkOrchid2 = 0xB23AEEFF,
		DarkOrchid3 = 0x9A32CDFF,
		DarkOrchid4 = 0x68228BFF,
		Purple1 = 0x9B30FFFF,
		Purple2 = 0x912CEEFF,
		Purple3 = 0x7D26CDFF,
		Purple4 = 0x551A8BFF,
		MediumPurple1 = 0xAB82FFFF,
		MediumPurple2 = 0x9F79EEFF,
		MediumPurple3 = 0x8968CDFF,
		MediumPurple4 = 0x5D478BFF,
		Thistle1 = 0xFFE1FFFF,
		Thistle2 = 0xEED2EEFF,
		Thistle3 = 0xCDB5CDFF,
		Thistle4 = 0x8B7B8BFF,
		grey11 = 0x1C1C1CFF,
		grey21 = 0x363636FF,
		grey31 = 0x4F4F4FFF,
		grey41 = 0x696969FF,
		grey51 = 0x828282FF,
		grey61 = 0x9C9C9CFF,
		grey71 = 0xB5B5B5FF,
		gray81 = 0xCFCFCFFF,
		gray91 = 0xE8E8E8FF,
		DarkGrey = 0xA9A9A9FF,
		DarkBlue = 0x00008BFF,
		DarkCyan = 0x008B8BFF,
		DarkMagenta = 0x8B008BFF,
		DarkRed = 0x8B0000FF,
		LightGreen = 0x90EE90FF,
		Transparent = 0x00000000,
		TinyPiX = 0xF1F1F3FF,
	};

public:
	tpColors() : isNull_(true)
	{
		this->colorSet_ = _RGBA(0, 0, 0, 255);
	}

	tpColors(const tpColors &color) : isNull_(false)
	{
		this->colorSet_ = color.rgba();
	}

	tpColors(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a = 255) : isNull_(false)
	{
		this->colorSet_ = _RGBA(r, g, b, a);
	}

	uint8_t red()
	{
		return _R(this->colorSet_);
	}

	uint8_t green()
	{
		return _G(this->colorSet_);
	}

	uint8_t blue()
	{
		return _B(this->colorSet_);
	}

	/// @brief 设置颜色为空，重置为黑色
	inline void setNull();

	/// @brief 颜色是否为空值
	/// @return 为空返回true,否则返回false
	inline bool isNull();

	/// @brief 指定R G B A 值设置颜色
	/// @param r 红色
	/// @param g 绿色
	/// @param b 蓝色
	/// @param a 透明度
	void setRgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

	/// @brief 获取RGBA值
	/// @return 颜色值
	inline int32_t rgba() const { return this->colorSet_; }

public:
	virtual ~tpColors()
	{
	}

public:
	tpColors operator=(int32_t color)
	{
		isNull_ = false;
		this->colorSet_ = color;
		return *this;
	}

public:
	static int32_t transColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		return _RGBA(r, g, b, a);
	}

	static void transColor(int32_t color, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a)
	{
		if (r)
		{
			*r = _R(color);
		}

		if (g)
		{
			*g = _G(color);
		}

		if (b)
		{
			*b = _B(color);
		}

		if (a)
		{
			*a = _A(color);
		}
	}

public:
	int32_t colorSet_;

private:
	bool isNull_;
};

#endif
