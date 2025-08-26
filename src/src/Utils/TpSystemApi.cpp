#include "TpSystemApi.h"
#include "TpGateway.h"
#include "TpFileInfo.h"
#include "TpFile.h"
#include "TpJsonDocument.h"
#include "TpJsonObject.h"
#include "TpNetDataGlobal.h"

TpSystemApi *TpSystemApi::Instance()
{
    static TpSystemApi instance;
    return &instance;
}

TpSystemApi::OpenFileError TpSystemApi::openFile(const TpString &filePath, const TpString &appUuid)
{
    TpFileInfo fileInfo(filePath);
    if (!fileInfo.exists())
        return TpSystemApi::FileNotExist;

    TpString parseAppUuid = appUuid;

    if (appUuid.empty())
    {
        TpString fileSuffix = fileInfo.suffix();

        TpFile fileTypeMapperFile("/System/conf/fileTypeMapper.conf");
        if (!fileTypeMapperFile.open(TpFile::ReadOnly))
            return TpSystemApi::SystemFileDamage;

        // 查询配置文件内，该后缀类型文件对应的应用信息
        TpString appJsonStr = fileTypeMapperFile.readAll();
        fileTypeMapperFile.close();

        TpJsonDocument jsonDoc;
        jsonDoc.doc_.Parse(appJsonStr.c_str());

        TpJsonObject appJsonObj = jsonDoc.object();

        TpJsonObject fileSuffixTypeObj = appJsonObj.value("extensionType").toObject();
        if (!fileSuffixTypeObj.contains(fileSuffix))
            return TpSystemApi::NotSupport;

        // 再根据文件后缀对应的类型取出应用UUID
        TpString fileTypeStr = fileSuffixTypeObj.value(fileSuffix).toString();
        if (!appJsonObj.contains(fileTypeStr))
            return TpSystemApi::SystemFileDamage;

        TpJsonObject appInfoObj = appJsonObj.value(fileTypeStr).toObject();
        if (!appInfoObj.contains("uuid"))
            return TpSystemApi::SystemFileDamage;

        // 根据应用UUID发送桌面启动应用
        parseAppUuid = appInfoObj.value("uuid").toString();
    }

    TpVector<std::string> argList;
    argList.emplace_back(filePath);

    RunApp startAppData;
    startAppData.appUuid = parseAppUuid;
    startAppData.argList = argList;

    PStructPackager structPackage;
    startAppData.StructSerialize(structPackage);

    publishGatewayData(RunAppTopic, structPackage.data(), structPackage.size());

    return TpSystemApi::Succsssful;
}

TpSystemApi::TpSystemApi()
{
    initializeGateway();
}

TpSystemApi::~TpSystemApi()
{
}
