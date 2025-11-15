#include "TpJsonDocument.h"
#include "TpJsonObject.h"
#include "TpFile.h"

#include <TpString.h>
#include <TpHash.h>

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
