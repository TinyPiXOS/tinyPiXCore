#include "Service/TpAppConfigIO.h"
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

TpAppConfigIO::TpAppConfigIO(const TpAppConfigIO &others)
{
    TpAppConfigIOData *configData = new TpAppConfigIOData();
    TpAppConfigIOData *othersData = static_cast<TpAppConfigIOData *>(others.data_);

    configData->appUuid = othersData->appUuid;
    configData->appStaticJsonObj = othersData->appStaticJsonObj;

    data_ = configData;
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

    configData->appStaticJsonObj = jsonDoc.object();

    return true;
}

void TpAppConfigIO::refreshCache()
{
    TpAppConfigIOData *configData = static_cast<TpAppConfigIOData *>(data_);
    setAppUuid(configData->appUuid);
}

TpString TpAppConfigIO::appUuid() const
{
    TpAppConfigIOData *configData = static_cast<TpAppConfigIOData *>(data_);
    return configData->appUuid;
}

TpString TpAppConfigIO::iconPath() const
{
    TpAppConfigIOData *configData = static_cast<TpAppConfigIOData *>(data_);
    if (configData->appUuid.empty())
        return "";

    if (configData->appStaticJsonObj.isEmpty())
        return "";

    if (!configData->appStaticJsonObj.contains("icon"))
        return "";

    TpString iconPath = configData->appStaticJsonObj.value("icon").toString();
    iconPath = globalAppRootPath + configData->appUuid + "/" + iconPath;
    return iconPath;
}

TpString TpAppConfigIO::runnerPath() const
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

TpString TpAppConfigIO::appName() const
{
    TpAppConfigIOData *configData = static_cast<TpAppConfigIOData *>(data_);
    if (configData->appStaticJsonObj.isEmpty())
        return "";

    if (!configData->appStaticJsonObj.contains("appName"))
        return "";

    return configData->appStaticJsonObj.value("appName").toString();
}

TpVector<TpAppConfigIO::AppWidgetInfo> TpAppConfigIO::widgetsInfo() const
{
    TpVector<AppWidgetInfo> widgetList;

    TpAppConfigIOData *configData = static_cast<TpAppConfigIOData *>(data_);
    if (configData->appStaticJsonObj.isEmpty())
        return widgetList;

    if (!configData->appStaticJsonObj.contains("widgets"))
        return widgetList;

    TpString appUuid = configData->appStaticJsonObj.value("appID").toString();
    TpJsonArray widgetsArray = configData->appStaticJsonObj.value("widgets").toArray();
    for (int i = 0; i < widgetsArray.count(); ++i)
    {
        TpJsonObject widgetObj = widgetsArray.at(i).toObject();

        AppWidgetInfo widgetInfo;
        widgetInfo.appUuid = appUuid;
        widgetInfo.linkAppUuid = widgetObj.value("linkAppUuid").toString();
        widgetInfo.widgetUuid = widgetObj.value("uuid").toString();
        widgetInfo.name = widgetObj.value("name").toString();
        widgetInfo.path = globalAppRootPath + configData->appUuid + "/" + widgetObj.value("path").toString();

        widgetList.emplace_back(widgetInfo);
    }

    return widgetList;
}

tpShared<TpAppConfigIO::AppWidgetInfo> TpAppConfigIO::widgetInfo(const TpString &widgetUuid) const
{
    tpShared<TpAppConfigIO::AppWidgetInfo> findWidgetPtr = nullptr;

    TpAppConfigIOData *configData = static_cast<TpAppConfigIOData *>(data_);
    if (configData->appStaticJsonObj.isEmpty())
        return findWidgetPtr;

    TpString appUuid = configData->appStaticJsonObj.value("appID").toString();
    TpJsonArray widgetsArray = configData->appStaticJsonObj.value("widgets").toArray();
    for (int i = 0; i < widgetsArray.count(); ++i)
    {
        TpJsonObject widgetObj = widgetsArray.at(i).toObject();
        TpString curWidgetUuid = widgetObj.value("uuid").toString();

        // 找到默认的组件节点
        if (curWidgetUuid.compare(widgetUuid) == 0)
        {
            findWidgetPtr = tpMakeShared<TpAppConfigIO::AppWidgetInfo>();
            findWidgetPtr->appUuid = appUuid;
            findWidgetPtr->linkAppUuid = widgetObj.value("linkAppUuid").toString();
            findWidgetPtr->widgetUuid = widgetObj.value("uuid").toString();
            findWidgetPtr->name = widgetObj.value("name").toString();
            findWidgetPtr->path = globalAppRootPath + configData->appUuid + "/" + widgetObj.value("path").toString();

            return findWidgetPtr;
        }
    }

    return findWidgetPtr;
}

tpShared<TpAppConfigIO::AppWidgetInfo> TpAppConfigIO::defaultWidgetInfo() const
{
    tpShared<TpAppConfigIO::AppWidgetInfo> defaultWidgetPtr = nullptr;

    TpAppConfigIOData *configData = static_cast<TpAppConfigIOData *>(data_);
    if (configData->appStaticJsonObj.isEmpty())
        return defaultWidgetPtr;

    if (!configData->appStaticJsonObj.contains("defaultWidget"))
        return defaultWidgetPtr;

    TpString defaulWidgetUuid = configData->appStaticJsonObj.value("defaultWidget").toString();
    if (defaulWidgetUuid.empty())
        return defaultWidgetPtr;

    TpString appUuid = configData->appStaticJsonObj.value("appID").toString();
    TpJsonArray widgetsArray = configData->appStaticJsonObj.value("widgets").toArray();
    for (int i = 0; i < widgetsArray.count(); ++i)
    {
        TpJsonObject widgetObj = widgetsArray.at(i).toObject();
        TpString curWidgetUuid = widgetObj.value("uuid").toString();

        // 找到默认的组件节点
        if (curWidgetUuid.compare(defaulWidgetUuid) == 0)
        {
            defaultWidgetPtr = tpMakeShared<TpAppConfigIO::AppWidgetInfo>();
            defaultWidgetPtr->appUuid = appUuid;
            defaultWidgetPtr->linkAppUuid = widgetObj.value("linkAppUuid").toString();
            defaultWidgetPtr->widgetUuid = widgetObj.value("uuid").toString();
            defaultWidgetPtr->name = widgetObj.value("name").toString();
            defaultWidgetPtr->path = globalAppRootPath + configData->appUuid + "/" + widgetObj.value("path").toString();

            return defaultWidgetPtr;
        }
    }

    return defaultWidgetPtr;
}

TpAppConfigIO &TpAppConfigIO::operator=(const TpAppConfigIO &others)
{
    if (this == &others)
        return *this;

    TpAppConfigIOData *configData = static_cast<TpAppConfigIOData *>(data_);
    TpAppConfigIOData *othersData = static_cast<TpAppConfigIOData *>(others.data_);

    configData->appUuid = othersData->appUuid;
    configData->appStaticJsonObj = othersData->appStaticJsonObj;

    return *this;
}
