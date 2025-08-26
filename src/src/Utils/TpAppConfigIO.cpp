#include "TpAppConfigIO.h"
#include "TpJsonDocument.h"
#include "TpJsonObject.h"
#include "TpFile.h"
#include "TpDir.h"

static const TpString globalConfRootPath = "/System/conf/app/";
static const TpString globalAppRootPath = "/System/app/";

struct TpAppConfigIOData
{
    TpString appUuid = "";

    // static字段json数据
    TpJsonObject appStaticJsonObj;
};

TpAppConfigIO::TpAppConfigIO()
{
    TpAppConfigIOData *configData = new TpAppConfigIOData();
    data_ = configData;
}

TpAppConfigIO::TpAppConfigIO(const TpString &appUuid)
{
    TpAppConfigIOData *configData = new TpAppConfigIOData();
    data_ = configData;

    setAppUuid(appUuid);
}

TpAppConfigIO::~TpAppConfigIO()
{
    TpAppConfigIOData *configData = static_cast<TpAppConfigIOData *>(data_);
    delete configData;
    configData = nullptr;
    data_ = nullptr;
}

TpVector<TpString> TpAppConfigIO::installAppUuidList()
{
    // 首先解析应用安装目录；获取安装的所有APP的uuid列表
    TpDir appInstallDir(globalConfRootPath);

    // 所有已安装的APP的UUID列表
    TpVector<TpString> installAppUuidList;
    for (const auto &appConfigFile : appInstallDir.entryInfoList(TpDir::Files))
    {
        TpString fileSuffix = appConfigFile.suffix();

        // 过滤非json后缀文件
        if (fileSuffix.compare("json") != 0)
            continue;

        TpString fileName = appConfigFile.baseName();
        installAppUuidList.emplace_back(fileName);
    }

    return installAppUuidList;
}

bool TpAppConfigIO::setAppUuid(const TpString &appUuid)
{
    if (appUuid.empty())
        return false;

    TpAppConfigIOData *configData = static_cast<TpAppConfigIOData *>(data_);
    configData->appUuid = appUuid;

    TpFile appJsonConfigFile(globalConfRootPath + appUuid + ".json");
    appJsonConfigFile.open(TpFile::ReadOnly);
    if (!appJsonConfigFile.isOpen())
    {
        std::cout << "UUid: " << appUuid << " IO打开配置文件失败!" << std::endl;
        return false;
    }

    TpString appJsonStr = appJsonConfigFile.readAll();
    appJsonConfigFile.close();

    TpJsonDocument jsonDoc;
    jsonDoc.doc_.Parse(appJsonStr.c_str());
    TpJsonObject appJsonObj = jsonDoc.object();

    if (!appJsonObj.contains("static"))
        return false;

    configData->appStaticJsonObj = appJsonObj.value("static").toObject();

    return true;
}

void TpAppConfigIO::refreshCache()
{
    TpAppConfigIOData *configData = static_cast<TpAppConfigIOData *>(data_);
    setAppUuid(configData->appUuid);
}

TpString TpAppConfigIO::iconPath()
{
    TpAppConfigIOData *configData = static_cast<TpAppConfigIOData *>(data_);
    if (configData->appStaticJsonObj.isEmpty())
        return "";

    if (!configData->appStaticJsonObj.contains("icon"))
        return "";

    TpString iconPath = configData->appStaticJsonObj.value("icon").toString();
    iconPath = globalAppRootPath + configData->appUuid + "/" + iconPath;
    return iconPath;
}

TpString TpAppConfigIO::runnerPath()
{
    TpAppConfigIOData *configData = static_cast<TpAppConfigIOData *>(data_);
    if (configData->appStaticJsonObj.isEmpty())
        return "";

    if (!configData->appStaticJsonObj.contains("appexecName"))
        return "";

    TpString runnerPath = configData->appStaticJsonObj.value("appexecName").toString();
    runnerPath = globalAppRootPath + configData->appUuid + "/bin/" + runnerPath;
    return runnerPath;
}

TpString TpAppConfigIO::appName()
{
    TpAppConfigIOData *configData = static_cast<TpAppConfigIOData *>(data_);
    if (configData->appStaticJsonObj.isEmpty())
        return "";

    if (!configData->appStaticJsonObj.contains("appName"))
        return "";

    return configData->appStaticJsonObj.value("appName").toString();
}
