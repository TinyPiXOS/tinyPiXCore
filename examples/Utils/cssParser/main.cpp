/***
 * @Author: hywang
 * @Date: 2024-08-07 10:47:34
 * @LastEditors: hywang
 * @LastEditTime: 2024-08-08 15:57:10
 * @FilePath: /pix-singlegui/tinyPiX-V7.0.2/demo/ExternUtils/cssParser/main.cpp
 * @Description: CSS解析器测试
 * @
 * @PiXOS
 */

#include "tpCssParser.h"
#include <tpString.h>
#include <tpHash.h>

int32_t main(int32_t argc, char *argv[])
{
	tpCssParser cssParser(tpString("./test.css"));

	tpVector<tpCssParser::BlockCssDataSPtr> cssList = cssParser.findUiCss("tpComBox");
	for (const auto& cssData : cssList)
	{
		std::cout << cssData->attrName << " 	cssData->attrName" << std::endl;
		std::cout << cssData->attrValue << " 	cssData->attrValue" << std::endl;
		// std::cout << cssData->color.red() << " 		cssData->red" << std::endl;
		// std::cout << cssData->color.green() << " 	cssData->green" << std::endl;
		std::cout << cssData->color.rgba() << " 	cssData->blue" << std::endl;
		std::cout << cssData->borderColor.rgba() << " 	cssData->borderColor" << std::endl;

		std::cout << "_RGBA( ) : " << _RGB(255, 170, 255) << std::endl;;
	}

	// std::unordered_map<std::string, tpString> testHash;
	// tpHash<std::string, tpString> testHash;
	// tpHash<tpString, tpString> testHash;

	// testHash["123"] = tpString("123");

	// tpString findKey("123");
	// auto findIter = testHash.find(findKey);
	// if (findIter == testHash.end())
	// {
	// 	int32_t a = 0;
	// }
	// else
	// {
	// 	int32_t a = 0;
	// }

	// if (testHash.contains(findKey))
	// {
	// 	int32_t a = 0;
	// }
	// else
	// {
	// 	int32_t a = 0;
	// }

	// tpString testString = "   \t \n TestString     \t\n Is QQQ \t\n   ";
	// testString = testString.simplified();
	// std::cout << testString;

	return 0;
}
