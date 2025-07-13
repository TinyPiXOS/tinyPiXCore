#include <vector>
#include "JsonStructPackage/JsonStructPackageHeader.h"

struct TestStructQQQ
{
	int32_t intTest;
	short shortTest;
	long longTest;
	long long longLongTest;

	float floatTest;
	int32_t unsignedIntTest;

	double doubleTest;

	unsigned long long unsignedLongLongTest;

	std::string tsttr;
};
JSONTRANSLATE(TestStructQQQ, intTest
	, shortTest
	, longTest
	, longLongTest
	, floatTest
	, unsignedIntTest
	, doubleTest
	, unsignedLongLongTest
	, tsttr
	);

struct TestStructA
{
	int32_t ab;
	std::string str;
	double doubleValue;
	float floatValue;

	TestStructQQQ qqq;
	char testChar[64];

	std::vector<TestStructQQQ> testVectorStruct;
	std::vector<int32_t> testVectorInt;
	std::vector<std::string> testVectorStr;

	TestStructA()
	{
		memset(testChar, 0, sizeof(testChar));
	}
};
JSONTRANSLATE(TestStructA, ab
	, str
	, doubleValue
	, floatValue
	, qqq
	, testChar
	, testVectorStruct, testVectorInt, testVectorStr
	);


int32_t main(int32_t argc, char* argv[])
{
	TestStructQQQ testQQQ;

	testQQQ.intTest = 999;
	testQQQ.shortTest = 32765;
	testQQQ.longTest = 65536789;
	testQQQ.longLongTest = 665565345;
	testQQQ.floatTest = 12345.4567;
	testQQQ.unsignedIntTest = 7678;
	testQQQ.doubleTest = 456.67867876;
	testQQQ.unsignedLongLongTest = 1232345678;

	testQQQ.tsttr = "This is TestString!哈哈";

	TestStructA testStruct;
	testStruct.ab = 99;

	testStruct.doubleValue = 123.652432;
	testStruct.floatValue = 99.987;
	testStruct.str = "This is a String";

	testStruct.qqq = testQQQ;

	testStruct.testVectorStruct.emplace_back(testQQQ);
	testStruct.testVectorStruct.emplace_back(testQQQ);

	testStruct.testVectorInt.emplace_back(99);
	testStruct.testVectorInt.emplace_back(55);

	testStruct.testVectorStr.emplace_back("String1");
	testStruct.testVectorStr.emplace_back("String2");

	testStruct.testChar[0] = 'T';
	testStruct.testChar[1] = 'E';
	testStruct.testChar[2] = 'S';
	testStruct.testChar[3] = 'T';

	JsonStructPackager jsonPackage;
	jsonPackage << testStruct;

	std::string resJson = jsonPackage.data();
	std::cout << "resJson : " << resJson << std::endl;
	
	JsonStructUnpackager jsonUnpackage(resJson);
	TestStructA unpackageStruct;
	jsonUnpackage >> unpackageStruct;

	return 0;
}
