#include "Service/TpSystemApi.h"
#include "Service/TpAppConfigIO.h"
#include "TpGateway.h"
#include "TpFileInfo.h"
#include "TpFile.h"
#include "TpJsonDocument.h"
#include "TpJsonObject.h"
#include "TpNetDataGlobal.h"
#include "TpImage.h"
#include "TpHash.h"
#include "TpDir.h"
#include "TpProcess.h"
#include "TpSurface.h"
#include "tinyPiXUtils.h"
#include "tinyPiXSys.h"
#include "TpApp.h"
#include "TpApp_p.h"
#include <mutex>

const TpString globalAppFilePathStr = "/System/app/";

struct TpSystemApiData
{
    IPiSysApiAgent *globalAgent = tinyPiX_sys_create();

    // TODO 将缓存数据放入Service中，远程IPC调用
    std::mutex readAppMutex;
    // 已启动应用的UUID和pid映射表
    TpHash<TpString, int32_t> appUuidPidMap = TpHash<TpString, int32_t>();
};

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

void TpSystemApi::notifyWidgetsResize(const TpString &widgetUuid, const TpSize &widgetSize)
{
    TpString notifyTopic = widgetUuid + "_WidgetGateway2W";

    TpInitWidgets paintEvent;
    paintEvent.width = widgetSize.width();
    paintEvent.height = widgetSize.height();
    paintEvent.depth = 32;
    paintEvent.Rmask = 0x00ff0000;
    paintEvent.Gmask = 0x0000ff00;
    paintEvent.Bmask = 0x000000ff;
    paintEvent.Amask = 0xff000000;

    PStructPackager sPack;
    paintEvent.StructSerialize(sPack);

    publishGatewayData(notifyTopic.c_str(), sPack.data(), sPack.size());
}

void TpSystemApi::notifyWidgetsPaint(const TpString &widgetUuid)
{
    TpString notifyTopic = widgetUuid + "_WidgetGateway2W";

    TpPaintWidgets paintEvent;
    PStructPackager sPack;
    paintEvent.StructSerialize(sPack);

    publishGatewayData(notifyTopic.c_str(), sPack.data(), sPack.size());
}

TpImage TpSystemApi::appImage(const TpString &uuid)
{
    TpSystemApiData *apiData = static_cast<TpSystemApiData *>(data_);
    if (!apiData)
        return TpImage();

    if (!apiData->appUuidPidMap.contains(uuid))
        return TpImage();

    int32_t appPid = apiData->appUuidPidMap.value(uuid);
    IPiWFSurface *surfacePtr = tinyPiX_sys_get_process_surface(apiData->globalAgent, appPid);
    // std::cout << "appPid " << appPid << " , " << surfacePtr << std::endl;
    if (!surfacePtr)
        return TpImage();

    tpShared<TpSurface> appDisplayImage = tpMakeShared<TpSurface>(surfacePtr);

    // 如果有工具栏，需要裁剪掉工具栏位置
    TpRect imageRect;
    TpAppData *appData = (TpAppData *)TpApp::Inst()->appObjectSet();
    if (appData->deskStatusBarInfo_.statusBarVislble)
    {
        if (appData->deskStatusBarInfo_.statusBarLocation == 0)
        {
            imageRect.setX(0);
            imageRect.setY(appData->deskStatusBarInfo_.statusBarHeight);
            imageRect.setWidth(appDisplayImage->width());
            imageRect.setHeight(appDisplayImage->height() - appData->deskStatusBarInfo_.statusBarHeight);
        }
        else if (appData->deskStatusBarInfo_.statusBarLocation == 1)
        {
            imageRect.setX(0);
            imageRect.setY(0);
            imageRect.setWidth(appDisplayImage->width() - appData->deskStatusBarInfo_.statusBarWidth);
            imageRect.setHeight(appDisplayImage->height());
        }
        else if (appData->deskStatusBarInfo_.statusBarLocation == 2)
        {
            imageRect.setX(0);
            imageRect.setY(0);
            imageRect.setWidth(appDisplayImage->width());
            imageRect.setHeight(appDisplayImage->height() - appData->deskStatusBarInfo_.statusBarHeight);
        }
        else if (appData->deskStatusBarInfo_.statusBarLocation == 3)
        {
            imageRect.setX(appData->deskStatusBarInfo_.statusBarWidth);
            imageRect.setY(0);
            imageRect.setWidth(appDisplayImage->width() - appData->deskStatusBarInfo_.statusBarWidth);
            imageRect.setHeight(appDisplayImage->height());
        }
        else
        {
            imageRect.setX(0);
            imageRect.setY(appData->deskStatusBarInfo_.statusBarHeight);
            imageRect.setWidth(appDisplayImage->width());
            imageRect.setHeight(appDisplayImage->height() - appData->deskStatusBarInfo_.statusBarHeight);
        }
    }
    TpImage resImage;
    resImage.load(appDisplayImage->matrix(), TpSize(appDisplayImage->width(), appDisplayImage->height()), imageRect);

    tinyPiX_surface_free(surfacePtr);

    return resImage;
}

bool TpSystemApi::home()
{
    TpSystemApiData *apiData = static_cast<TpSystemApiData *>(data_);
    if (!apiData)
        return false;
    tinyPiX_sys_send_home(apiData->globalAgent);
    return true;
}

bool TpSystemApi::startApp(const TpString &uuid, const TpVector<TpString> &argList)
{
    if (uuid.empty())
        return false;

    TpString appFileDirPath = globalAppFilePathStr + uuid;
    TpDir appFileDir(appFileDirPath);
    if (!appFileDir.exists())
    {
        std::cout << "UUid: " << uuid << " 应用文件夹不存在" << std::endl;
        return false;
    }

    TpSystemApiData *apiData = static_cast<TpSystemApiData *>(data_);
    if (!apiData)
        return false;

    // 启动对应应用
    if (apiData->appUuidPidMap.contains(uuid))
    {
        int32_t pid = apiData->appUuidPidMap.value(uuid);

        // 根据pid查询winid
        PiShmBytes *appIdList = nullptr;
        int appSize = 0;
        tinyPiX_sys_find_win_ids(apiData->globalAgent, &appIdList, &appSize, Q_FIXS);

        int32_t winId = 0;
        for (int i = 0; i < appSize; ++i)
        {
            PiShmBytes appIdInfo = appIdList[i];
            if (appIdInfo.p_id == pid)
            {
                winId = appIdInfo.s_id;
                break;
            }
        }

        std::cout << "恢复应用 pid: " << pid << std::endl;
        tinyPiX_sys_set_visible(apiData->globalAgent, winId, true);
        tinyPiX_sys_set_active(apiData->globalAgent, winId, true);
    }
    else
    {
        // 解析应用图标、名称信息
        TpAppConfigIO configIO(uuid);

        TpString runnerPath = configIO.runnerPath();
        TpFileInfo runnerFileInfo(runnerPath);
        if (!runnerFileInfo.exists())
        {
            std::cout << "应用 " << configIO.appName() << " 可执行程序不存在!" << std::endl;
            return false;
        }

        TpProcess exeProcess;
        exeProcess.start(runnerPath, argList);
        // exeProcess.start(exePathStr);
        int32_t processPID = exeProcess.launchProcessID();

        // RunAppInfo runAppInfo;
        // runAppInfo.appName = configIO.appName();
        // runAppInfo.appUuid = uuid;
        // runAppInfo.appIconPath = configIO.iconPath();
        // runAppInfo.pid = processPID;

        std::cout << "processPID " << processPID << std::endl;
        std::lock_guard<std::mutex> lockG(apiData->readAppMutex);
        // globalRunAppMap_[processPID] = runAppInfo;
        apiData->appUuidPidMap[uuid] = processPID;
    }

    return true;
}

bool TpSystemApi::killApp(const TpString &uuid)
{
    TpSystemApiData *apiData = static_cast<TpSystemApiData *>(data_);
    if (!apiData)
        return false;

    if (apiData->appUuidPidMap.contains(uuid))
    {
        int32_t pid = apiData->appUuidPidMap.value(uuid);

        std::lock_guard<std::mutex> lockG(apiData->readAppMutex);
        apiData->appUuidPidMap.erase(uuid);
        // globalRunAppMap_.erase(pid);

        std::cout << "结束应用 pid: " << pid << std::endl;
        tinyPiX_sys_kill_process(apiData->globalAgent, pid);
    }

    return true;
}

bool TpSystemApi::killAllApp()
{
    TpSystemApiData *apiData = static_cast<TpSystemApiData *>(data_);
    if (!apiData)
        return false;

    // 获取所有应用列表
    PiShmBytes *appIdList = nullptr;
    int appSize = 0;
    tinyPiX_sys_find_win_ids(apiData->globalAgent, &appIdList, &appSize, 1);

    // 杀掉所有应用
    for (int i = 0; i < appSize; ++i)
    {
        PiShmBytes appIdInfo = appIdList[i];

        tinyPiX_sys_kill_process(apiData->globalAgent, appIdInfo.p_id);
    }

    // 清理缓存的应用运行信息
    {
        std::lock_guard<std::mutex> lockG(apiData->readAppMutex);
        // globalRunAppMap_.clear();
        apiData->appUuidPidMap.clear();
    }

    return false;
}

TpVector<TpSystemApi::RunAppInfo> TpSystemApi::runAppList()
{
    TpVector<RunAppInfo> runAppList;

    TpSystemApiData *apiData = static_cast<TpSystemApiData *>(data_);
    if (!apiData)
        return runAppList;

    // 获取所有应用列表
    PiShmBytes *appIdList = nullptr;
    int appSize = 0;
    tinyPiX_sys_find_win_ids(apiData->globalAgent, &appIdList, &appSize, Q_FIXS);

    // 运行的应用PID列表
    TpList<int32_t> runAppPidList = apiData->appUuidPidMap.values();

    for (int i = 0; i < appSize; ++i)
    {
        PiShmBytes appIdInfo = appIdList[i];

        if (runAppPidList.contains(appIdInfo.p_id))
        {
            TpString curAppUuid = apiData->appUuidPidMap.key(appIdInfo.p_id);

            RunAppInfo appInfo;
            appInfo.appInfo.setAppUuid(curAppUuid);
            appInfo.pid = appIdInfo.p_id;

            runAppList.emplace_back(appInfo);
        }
    }

    return runAppList;
}

TpSystemApi::RunAppInfo TpSystemApi::runAppInfo(const TpString &uuid)
{
    RunAppInfo runAppInfo;

    TpSystemApiData *apiData = static_cast<TpSystemApiData *>(data_);
    if (!apiData)
        return runAppInfo;

    if (!apiData->appUuidPidMap.contains(uuid))
        return runAppInfo;

    int32_t appPid = apiData->appUuidPidMap.value(uuid);

    // 获取所有应用列表
    PiShmBytes *appIdList = nullptr;
    int appSize = 0;
    tinyPiX_sys_find_win_ids(apiData->globalAgent, &appIdList, &appSize, Q_FIXS);

    for (int i = 0; i < appSize; ++i)
    {
        PiShmBytes appIdInfo = appIdList[i];

        if (appPid == appIdInfo.p_id)
        {
            runAppInfo.pid = appPid;
            runAppInfo.appInfo.setAppUuid(uuid);
            break;
        }
    }

    return runAppInfo;
}

TpSystemApi::TpSystemApi()
{
    data_ = new TpSystemApiData();
    initializeGateway();
}

TpSystemApi::~TpSystemApi()
{
    TpSystemApiData *apiData = static_cast<TpSystemApiData *>(data_);
    if (apiData)
    {
        delete apiData;
        apiData = nullptr;
    }
}
