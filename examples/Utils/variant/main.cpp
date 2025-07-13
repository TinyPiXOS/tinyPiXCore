
#include <tpUtils.h>
#include <iostream>
#include <cstring>
#include "tpVariant.h"

int32_t main(int32_t argc, char *argv[])
{
	std::string testStr = "This Is a String";

	int32_t testInt = 99;

	tpVariant variantTest(testInt);

	tpVariant variantTest2(testStr);

	int32_t resInt = int32_t(variantTest);
	// std::string resString = (const char *)(variantTest);

	std::cout << resInt << std::endl;
	// std::cout << resString << std::endl;

	return 0;
}
