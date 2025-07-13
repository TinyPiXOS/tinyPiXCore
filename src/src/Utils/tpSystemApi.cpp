#include "tpSystemApi.h"
#include "tpGateway.h"
#include "tpFileInfo.h"
#include "tpFile.h"
#include "tpJsonDocument.h"
#include "tpJsonObject.h"
#include "tpDesktopGlobal.h"

tpSystemApi *tpSystemApi::Instance()
{
    static tpSystemApi instance;
    return &instance;
}

tpSystemApi::OpenFileError tpSystemApi::openFile(const tpString &filePath, const tpString &appUuid)
{
    tpFileInfo fileInfo(filePath);
    if (!fileInfo.exists())
        return tpSystemApi::FileNotExist;

    tpString parseAppUuid = appUuid;

    if (appUuid.empty())
    {
        tpString fileSuffix = fileInfo.suffix();

        tpFile fileTypeMapperFile("/System/conf/fileTypeMapper.conf");
        if (!fileTypeMapperFile.open(tpFile::ReadOnly))
            return tpSystemApi::SystemFileDamage;

        // 查询配置文件内，该后缀类型文件对应的应用信息
        tpString appJsonStr = fileTypeMapperFile.readAll();
        fileTypeMapperFile.close();

        tpJsonDocument jsonDoc;
        jsonDoc.doc_.Parse(appJsonStr.c_str());

        tpJsonObject appJsonObj = jsonDoc.object();

        tpJsonObject fileSuffixTypeObj = appJsonObj.value("extensionType").toObject();
        if (!fileSuffixTypeObj.contains(fileSuffix))
            return tpSystemApi::NotSupport;

        // 再根据文件后缀对应的类型取出应用UUID
        tpString fileTypeStr = fileSuffixTypeObj.value(fileSuffix).toString();
        if (!appJsonObj.contains(fileTypeStr))
            return tpSystemApi::SystemFileDamage;

        tpJsonObject appInfoObj = appJsonObj.value(fileTypeStr).toObject();
        if (!appInfoObj.contains("uuid"))
            return tpSystemApi::SystemFileDamage;

        // 根据应用UUID发送桌面启动应用
        parseAppUuid = appInfoObj.value("uuid").toString();
    }

    tpVector<std::string> argList;
    argList.emplace_back(filePath);

    RunApp startAppData;
    startAppData.appUuid = parseAppUuid;
    startAppData.argList = argList;

    PStructPackager structPackage;
    startAppData.StructSerialize(structPackage);

    publishGatewayData(RunAppTopic, structPackage.data(), structPackage.size());

    return tpSystemApi::Succsssful;
}

tpSystemApi::tpSystemApi()
{
    initializeGateway();
}

tpSystemApi::~tpSystemApi()
{
}
