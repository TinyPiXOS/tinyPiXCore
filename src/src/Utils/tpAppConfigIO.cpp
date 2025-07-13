#include "tpAppConfigIO.h"
#include "tpJsonDocument.h"
#include "tpJsonObject.h"
#include "tpFile.h"
#include "tpDir.h"

static const tpString globalConfRootPath = "/System/conf/app/";
static const tpString globalAppRootPath = "/System/app/";

struct tpAppConfigIOData
{
    tpString appUuid = "";

    // static字段json数据
    tpJsonObject appStaticJsonObj;
};

tpAppConfigIO::tpAppConfigIO()
{
    tpAppConfigIOData *configData = new tpAppConfigIOData();
    data_ = configData;
}

tpAppConfigIO::tpAppConfigIO(const tpString &appUuid)
{
    tpAppConfigIOData *configData = new tpAppConfigIOData();
    data_ = configData;

    setAppUuid(appUuid);
}

tpAppConfigIO::~tpAppConfigIO()
{
    tpAppConfigIOData *configData = static_cast<tpAppConfigIOData *>(data_);
    delete configData;
    configData = nullptr;
    data_ = nullptr;
}

tpVector<tpString> tpAppConfigIO::installAppUuidList()
{
    // 首先解析应用安装目录；获取安装的所有APP的uuid列表
    tpDir appInstallDir(globalConfRootPath);

    // 所有已安装的APP的UUID列表
    tpVector<tpString> installAppUuidList;
    for (const auto &appConfigFile : appInstallDir.entryInfoList(tpDir::Files))
    {
        tpString fileSuffix = appConfigFile.suffix();

        // 过滤非json后缀文件
        if (fileSuffix.compare("json") != 0)
            continue;

        tpString fileName = appConfigFile.baseName();
        installAppUuidList.emplace_back(fileName);
    }

    return installAppUuidList;
}

bool tpAppConfigIO::setAppUuid(const tpString &appUuid)
{
    if (appUuid.empty())
        return false;

    tpAppConfigIOData *configData = static_cast<tpAppConfigIOData *>(data_);
    configData->appUuid = appUuid;

    tpFile appJsonConfigFile(globalConfRootPath + appUuid + ".json");
    appJsonConfigFile.open(tpFile::ReadOnly);
    if (!appJsonConfigFile.isOpen())
    {
        std::cout << "UUid: " << appUuid << " IO打开配置文件失败!" << std::endl;
        return false;
    }

    tpString appJsonStr = appJsonConfigFile.readAll();
    appJsonConfigFile.close();

    tpJsonDocument jsonDoc;
    jsonDoc.doc_.Parse(appJsonStr.c_str());
    tpJsonObject appJsonObj = jsonDoc.object();

    if (!appJsonObj.contains("static"))
        return false;

    configData->appStaticJsonObj = appJsonObj.value("static").toObject();

    return true;
}

void tpAppConfigIO::refreshCache()
{
    tpAppConfigIOData *configData = static_cast<tpAppConfigIOData *>(data_);
    setAppUuid(configData->appUuid);
}

tpString tpAppConfigIO::iconPath()
{
    tpAppConfigIOData *configData = static_cast<tpAppConfigIOData *>(data_);
    if (configData->appStaticJsonObj.isEmpty())
        return "";

    if (!configData->appStaticJsonObj.contains("icon"))
        return "";

    tpString iconPath = configData->appStaticJsonObj.value("icon").toString();
    iconPath = globalAppRootPath + configData->appUuid + "/" + iconPath;
    return iconPath;
}

tpString tpAppConfigIO::runnerPath()
{
    tpAppConfigIOData *configData = static_cast<tpAppConfigIOData *>(data_);
    if (configData->appStaticJsonObj.isEmpty())
        return "";

    if (!configData->appStaticJsonObj.contains("appexecName"))
        return "";

    tpString runnerPath = configData->appStaticJsonObj.value("appexecName").toString();
    runnerPath = globalAppRootPath + configData->appUuid + "/bin/" + runnerPath;
    return runnerPath;
}

tpString tpAppConfigIO::appName()
{
    tpAppConfigIOData *configData = static_cast<tpAppConfigIOData *>(data_);
    if (configData->appStaticJsonObj.isEmpty())
        return "";

    if (!configData->appStaticJsonObj.contains("appName"))
        return "";

    return configData->appStaticJsonObj.value("appName").toString();
}
