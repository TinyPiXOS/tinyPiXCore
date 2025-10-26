
#include <TpUtils.h>
#include <iostream>
#include <cstring>
#include "TpVariant.h"

int32_t main(int32_t argc, char *argv[])
{
	std::string testStr = "This Is a String";

	int32_t testInt = 99;

	TpVariant variantTest(testInt);

	TpVariant variantTest2(testStr);

	int32_t resInt = int32_t(variantTest);
	// std::string resString = (const char *)(variantTest);

	std::cout << resInt << std::endl;
	// std::cout << resString << std::endl;

	return 0;
}
