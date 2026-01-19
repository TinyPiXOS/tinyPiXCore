#include "TpJsonDocument.h"
#include "TpJsonObject.h"
#include "TpFile.h"

#include <TpString.h>
#include <TpHash.h>

#if 0
int32_t main(int32_t argc, char *argv[])
{
    // Json 字符串 -> Json对象
    TpFile readJsonFile(applicationDirPath() + "/jsonExample.json");
    if (!readJsonFile.open(TpFile::ReadOnly))
    {
        std::cout << "Json 文件打开失败；请检查文件是否存在!" << std::endl;
        return 0;
    }

    TpJsonDocument testJsonDoc = TpJsonDocument::fromJson(readJsonFile.readAll());
    readJsonFile.close();

    TpJsonObject jsonObj = testJsonDoc.object();

    // 解析数字和字符串
    int32_t key1Value = jsonObj.value("key1").toInt();
    TpString key2Value = jsonObj.value("key2").toString();
    TpString key3Value = jsonObj.value("key3").toString();

    std::cout << "key1Value: " << key1Value << std::endl;
    std::cout << "key2Value: " << key2Value << std::endl;
    std::cout << "key3Value: " << key3Value << std::endl;

    // 解析子Json对象
    TpString sonKeyValue = jsonObj.value("keyJson").toObject().value("sonKey").toString();
    std::cout << "sonKeyValue: " << sonKeyValue << std::endl;

    // 解析子列表
    TpJsonArray jsonArray1 = jsonObj.value("keyArray").toArray();
    for (int32_t i = 0; i < jsonArray1.count(); ++i)
    {
        std::cout << "jsonArray1 index : " << i << " Value: " << jsonArray1.at(i).toInt() << std::endl;
    }

    TpJsonArray jsonArray2 = jsonObj.value("keyArrayStr").toArray();
    for (int32_t i = 0; i < jsonArray2.count(); ++i)
    {
        std::cout << "jsonArray2 index : " << i << " Value: " << jsonArray2.at(i).toString() << std::endl;
    }

    // 解析存放Json对象的列表
    TpJsonArray jsonArray3 = jsonObj.value("keyArrayObj").toArray();
    for (int32_t i = 0; i < jsonArray3.count(); ++i)
    {
        TpJsonObject childObj = jsonArray3.at(i).toObject();
        std::cout << "key1: " << childObj.value("key1").toInt() << std::endl;
        std::cout << "key3: " << childObj.value("key3").toString() << std::endl;
        std::cout << "keyJson: " << childObj.value("keyJson").toObject().value("sonKey").toString() << std::endl;
    }

    // 插入bool
    jsonObj.insert("key4", true);

    // 插入字符串
    jsonObj.insert("key5", "这是一个字符串");

    // 插入存放Json对象的列表
    TpJsonArray insertObjArray;
    for (int32_t i = 0; i < 3; ++i)
    {
        TpJsonObject insertObj;
        insertObj.insert("key" + TpString::number(i), "++++++++++++++");
        insertObj.insert("key" + TpString::number(i + 1), i);
        insertObjArray.append(insertObj);
    }
    jsonObj.insert("key6", insertObjArray);

    // 插入存放列表的列表
    TpJsonArray totalArray;
    for (int i = 0; i < 3; ++i)
    {
        TpJsonArray childArray;
        for (int j = 0; j < 5; ++j)
        {
            childArray.append(j);
        }
        totalArray.append(childArray);
    }
    jsonObj.insert("key7", totalArray);

    // Json对象转为字符串并写入文件
    TpJsonDocument testJsonDoc2(jsonObj);
    TpString resJsonStr2 = testJsonDoc2.toJson();

    TpFile writeJsonFile(applicationDirPath() + "/writeNewJson.json");
    if (!writeJsonFile.open(TpFile::WriteOnly))
    {
        std::cout << "Json 写入文件打开失败" << std::endl;
        return 0;
    }

    writeJsonFile.write(resJsonStr2);
    writeJsonFile.close();

    std::cout << "resJsonStr2: " << resJsonStr2 << std::endl;

    return 0;
}
#endif

void array_test(TpJsonArray &array)
{
    TpString value1("array_value1");
    TpString value2("array_value2");
    array.append(value1);
    array.append(value2);
}

void json_test(TpJsonObject &object)
{
    TpJsonObject obj;
    TpString test("");
    obj.insert("name", "Chingan");
    obj.insert("email", "123456789");
    object.insert("author", obj);
    //	object.insert("test10",test);
    object.insert("description", "this is tinyPiXOS test APP");
    object.insert("icon", "./icon.jpeg");
    TpJsonArray array;
    array_test(array);
    object.insert("lib", array);
    object.insert("start", "./start.sh");
    //	std::cout << "json_doc:"<< TpJsonDocument(object).toFormattedJson() << std::endl;
}

int main()
{
    TpJsonObject object;
    char test2[20] = "test_value\n";
    char test3[11] = "test_value";
    char *test4 = (char *)malloc(20);
    memcpy(test4, test3, 11);
    char test5[12] = "test_value\0";
    //	object.insert(TpString("name"), TpJsonValue(TpString("name_hello")));
    //	object.insert(TpString("test"), TpJsonValue(TpString("test_value")));
    object.insert("appID", "f03c8f8c-dd9b-453f-b2d4-d049c073e252");
    object.insert("appName", "MyApp");
    object.insert(TpString("test1").c_str(), (TpString("test_value").c_str()));
    object.insert("test2", test2);
    object.insert("test3", test3);
    object.insert("test4", test4);
    object.insert("test5", test5);
    object.insert("test6", "hahahhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh");
    object.insert("architecture", "amd64");
    json_test(object);
    object.insert("test9", "hahahhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh");

    TpJsonDocument doc(object);
    std::cout << "json_doc:" << doc.toJson() << std::endl;
    std::cout << "json_doc:" << doc.toFormattedJson() << std::endl;
    std::cout << "name: " << object.value(TpString("appID")).toString() << std::endl;
    std::cout << "test: " << object.value(TpString("test")).toString() << std::endl;
}