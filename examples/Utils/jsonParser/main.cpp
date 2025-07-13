

// #include "tpJsonDocument.h"
// #include "tpJsonObject.h"
// #include "tpFile.h"

// #include <tpString.h>
// #include <tpHash.h>

// int32_t main(int32_t argc, char *argv[])
// {
// #if 0
// 	tpString testJsonStr = "{ \"key1\": 110119, \"key3\": \"qweqweasd\", \"keyJson\": { \"sonKey\": \"zxcvvb\" }, \"keyArray\": [ 1, 2, 3], \"keyArrayStr\": [ \"att1\", \"att2\", \"att3\"] }";

// 	tpJsonDocument testJsonDoc = tpJsonDocument::fromJson(testJsonStr);
// 	tpString resJsonStr = testJsonDoc.toJson();
// 	std::cout << "resJsonStr: " << resJsonStr << std::endl;

// 	tpJsonObject jsonObj = testJsonDoc.object();

// 	int32_t key1Value = jsonObj.value("key1").toInt();
// 	tpString key2Value = jsonObj.value("key2").toString();
// 	tpString key3Value = jsonObj.value("key3").toString();

// 	std::cout << "key1Value: " << key1Value << std::endl;
// 	std::cout << "key2Value: " << key2Value << std::endl;
// 	std::cout << "key3Value: " << key3Value << std::endl;

// 	tpString sonKeyValue = jsonObj.value("keyJson").toObject().value("sonKey").toString();
// 	std::cout << "sonKeyValue: " << sonKeyValue << std::endl;

// 	tpJsonArray jsonArray1 = jsonObj.value("keyArray").toArray();
// 	tpJsonArray jsonArray2 = jsonObj.value("keyArrayStr").toArray();

// 	for (int32_t i = 0; i < jsonArray1.count(); ++i)
// 	{
// 		std::cout << "jsonArray1 index : " << i << " Value: " << jsonArray1.at(i).toInt() << std::endl;
// 	}

// 	for (int32_t i = 0; i < jsonArray2.count(); ++i)
// 	{
// 		std::cout << "jsonArray2 index : " << i << " Value: " << jsonArray2.at(i).toString() << std::endl;
// 	}

// 	jsonObj.insert("key4", true);

// 	tpJsonDocument testJsonDoc2(jsonObj);
// 	tpString resJsonStr2 = testJsonDoc2.toJson();
// 	std::cout << "resJsonStr2: " << resJsonStr2 << std::endl;
// #endif

// 	// 解析已安装APP配置信息文件
// 	// tpFile installAppConfigFile("/System/conf/install.conf");

// 	// installAppConfigFile.open(tpFile::ReadOnly);
// 	// if (!installAppConfigFile.isOpen())
// 	// {
// 	// 	std::cout << "System Install App ConfigFile Not Open!" << std::endl;
// 	// 	return 0;
// 	// }

// 	// tpString installAppConfigJsonStr = installAppConfigFile.readAll();

// 	// installAppConfigFile.close();

// 	// tpJsonObject installAppConfigJson = tpJsonDocument::fromJson(installAppConfigJsonStr).object();
// 	// tpJsonArray installAppArray = installAppConfigJson.value("appInstall").toArray();
// 	// for (int32_t i = 1; i < installAppArray.count(); ++i)
// 	// {
// 	// 	tpJsonObject appInstallInfoJson = installAppArray.at(i).toObject();

// 	// 	tpJsonDocument testDoc(appInstallInfoJson);

// 	// 	std::cout << "OneAppJson : " << testDoc.toJson() << std::endl
// 	// 			  << std::endl;

// 	// 	std::cout << "Hase Uuid Key : " << appInstallInfoJson.contains("uuid") << std::endl;

// 	// 	tpString appUuid = appInstallInfoJson.value("uuid").toString();
// 	// 	uint32_t appPage = appInstallInfoJson.value("page").toUint();
// 	// 	uint32_t appIndex = appInstallInfoJson.value("index").toUint();

// 	// 	std::cout << "UUid Value is string : " << appInstallInfoJson.value("uuid").isString() << std::endl;
		
// 	// 	std::cout << "appUuid: " << appUuid << "  page: " << appPage << "  index : " << appIndex << std::endl;
// 	// }
	
// 	// 添加子json对象测试
// 	tpJsonObject parentJsonObj;

// 	tpJsonObject childJsonObj;
// 	childJsonObj.insert("key1", "123");
// 	childJsonObj.insert("key2", "456");
// 	childJsonObj.insert("key3", "789");

// 	tpJsonObject child2JsonObj;
// 	child2JsonObj.insert("key44444", "asdfg");
// 	childJsonObj.insert("key4", child2JsonObj);

//  	// rapidjson::Document::AllocatorType &allocator = childJsonObj.doc_.GetAllocator();
//     // rapidjson::Value jsonKey;
//     // jsonKey.SetString("key4",tpString("key4").size(), allocator);
//     // childJsonObj.doc_.AddMember(jsonKey, child2JsonObj.doc_, allocator);

// 	parentJsonObj.insert("child", childJsonObj);
//  	// rapidjson::Document::AllocatorType &allocator2 = parentJsonObj.doc_.GetAllocator();
//     // rapidjson::Value jsonKey2;
//     // jsonKey2.SetString("child",tpString("child").size(), allocator2);
//     // parentJsonObj.doc_.AddMember(jsonKey2, childJsonObj.doc_, allocator2);

//     tpJsonObject cssJsonObj;

//     tpHash<tpString, tpHash<tpString, tpHash<tpString, tpString>>> cssOriginDataMap;

// 	cssOriginDataMap["11111111"]["1111111"]["111111"] = "222222";
// 	cssOriginDataMap["11111111"]["1111111"]["222222"] = "aaaaaaa";

// 	cssOriginDataMap["11111111"]["222222"]["111111"] = "bbbbbbbbb";
// 	cssOriginDataMap["11111111"]["222222"]["222222"] = "bbbbbbbbb";

// 	cssOriginDataMap["222222222"]["1111111"]["111111"] = "cccccccc";
// 	cssOriginDataMap["222222222"]["222222"]["111111"] = "cccccccc";

//     for (const auto& firstMapIter : cssOriginDataMap)
//     {
//         tpJsonObject typeObj;
//         for (const auto& secondTypeIter : firstMapIter.second)
//         {
//             tpJsonObject cssValueObj;

//             for (const auto& thirdCssNameIter : secondTypeIter.second)
//             {
//                 cssValueObj.insert(thirdCssNameIter.first, thirdCssNameIter.second);
//             }

//             typeObj.insert(secondTypeIter.first, cssValueObj);
//         }

//         cssJsonObj.insert(firstMapIter.first, typeObj);
//     }

//     tpJsonDocument jsonDoc(cssJsonObj);
//     tpString jsonStr = jsonDoc.toJson();
//     // tpString jsonStr = ret;

// 	std::cout << jsonStr << std::endl;

// 	return 0;
// }

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
