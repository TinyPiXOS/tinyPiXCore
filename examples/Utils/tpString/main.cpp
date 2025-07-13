#include "tpString.h"
#include <iostream>

int main(int argc, char *argv[])
{
	tpString testStr = "123测试qqq";

	std::cout << " testStr " << testStr << std::endl;
	std::cout << " length " << testStr.length() << std::endl;
	std::cout << " logicalLength " << testStr.logicalLength() << std::endl;
	
	std::cout << " substr " << testStr.mid(0, 4) << std::endl;

	return 0;
}
