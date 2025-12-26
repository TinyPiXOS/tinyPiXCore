#include "AppRunnerManage.h"
#include "AppManage/TpAppConfigIO.h"
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
#include "tinyPiXUtils.h"
#include "tinyPiXSys.h"
#include <mutex>
#include <InteractData/TpWidgetsData.h>
#include <InteractData/TpDesktopData.h>
#include <future>

const TpString globalAppFilePathStr = "/System/app/";

struct AppRunnerManageData
{
    IPiSysApiAgent *globalAgent = tinyPiX_sys_create();

    std::mutex readAppMutex;
    TpHash<TpString, int32_t> appUuidPidMap;
};

AppRunnerManage *AppRunnerManage::Instance()
{
    static AppRunnerManage instance;
    return &instance;
}

TpImage AppRunnerManage::appImage(const TpString &uuid)
{
    AppRunnerManageData *apiData = static_cast<AppRunnerManageData *>(data_);
    if (!apiData)
        return TpImage();

    if (!apiData->appUuidPidMap.contains(uuid))
        return TpImage();

    int32_t appPid = apiData->appUuidPidMap.value(uuid);
    IPiWFSurface *surfacePtr = tinyPiX_sys_get_process_surface(apiData->globalAgent, appPid);
    // std::cout << "appPid " << appPid << " , " << surfacePtr << std::endl;
    if (!surfacePtr)
        return TpImage();

    // 如果有工具栏，需要裁剪掉工具栏位置
    TpRect imageRect;

#if 0
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
#endif

    TpImage resImage;
    resImage.load(surfacePtr, imageRect);

    tinyPiX_surface_free(surfacePtr);

    return resImage;
}

bool AppRunnerManage::startApp(const TpString &uuid, const TpVector<TpString> &argList)
{
    AppRunnerManageData *apiData = static_cast<AppRunnerManageData *>(data_);
    if (!apiData)
        return false;

    if (uuid.empty())
        return false;

    TpString appFileDirPath = globalAppFilePathStr + uuid;
    TpDir appFileDir(appFileDirPath);
    if (!appFileDir.exists())
    {
        std::cout << "uuid: " << uuid << " 未安装!" << std::endl;
        return false;
    }

    // 解析应用信息
    TpAppConfigIO configIO(uuid);
    TpString runnerPath = configIO.runnerPath();

    TpFileInfo runnerFileInfo(runnerPath);
    if (!runnerFileInfo.exists())
    {
        std::cout << "应用 " << configIO.appName() << " 可执行程序不存在!" << std::endl;
        return false;
    }

    if (apiData->appUuidPidMap.contains(uuid))
    {
        // 应用已启动，恢复即可
        int32_t pid = apiData->appUuidPidMap.value(uuid);

        std::cout << "恢复应用 pid: " << pid << std::endl;
        tinyPiX_sys_set_visible(apiData->globalAgent, pid, true);
        tinyPiX_sys_set_active(apiData->globalAgent, pid, true);
    }
    else
    {
        TpProcess exeProcess;
        exeProcess.start(runnerPath, argList);
        int32_t processPID = exeProcess.launchProcessID();

        std::cout << "processPID " << processPID << std::endl;

        apiData->appUuidPidMap[uuid] = processPID;
    }

    return true;
}

bool AppRunnerManage::killApp(const TpString &uuid)
{
    AppRunnerManageData *apiData = static_cast<AppRunnerManageData *>(data_);
    if (!apiData)
        return false;

    if (!apiData->appUuidPidMap.contains(uuid))
        return false;

    int32_t pid = 0;
    {
        std::lock_guard<std::mutex> lockG(apiData->readAppMutex);
        pid = apiData->appUuidPidMap.value(uuid);
        apiData->appUuidPidMap.erase(uuid);
    }

    std::cout << "结束应用 pid: " << pid << std::endl;
    tinyPiX_sys_kill_process(apiData->globalAgent, pid);

    return true;
}

bool AppRunnerManage::killAllApp()
{
    AppRunnerManageData *apiData = static_cast<AppRunnerManageData *>(data_);
    if (!apiData)
        return false;

    std::lock_guard<std::mutex> lockG(apiData->readAppMutex);

    for (const auto &appIdIter : apiData->appUuidPidMap)
    {
        tinyPiX_sys_kill_process(apiData->globalAgent, appIdIter.second);
    }
    // 清理缓存的应用运行信息
    apiData->appUuidPidMap.clear();

    /* RPC 调用 */
    return true;
}

TpVector<AppRunnerManage::RunAppInfo> AppRunnerManage::runAppInfoList()
{
    TpVector<RunAppInfo> runAppList;

    AppRunnerManageData *apiData = static_cast<AppRunnerManageData *>(data_);
    if (!apiData)
        return runAppList;

    // 运行的应用PID列表
    for (const auto &runAppInfoIter : apiData->appUuidPidMap)
    {
        // std::cout << "查询到的进程ID： " << runAppInfoIter.second << std::endl;
        RunAppInfo appInfo;
        appInfo.appInfo.setAppUuid(runAppInfoIter.first);
        appInfo.pid = runAppInfoIter.second;

        runAppList.emplace_back(appInfo);
    }

    return runAppList;
}

AppRunnerManage::RunAppInfo AppRunnerManage::runAppInfo(const TpString &uuid)
{
    RunAppInfo runAppInfo;

    AppRunnerManageData *apiData = static_cast<AppRunnerManageData *>(data_);
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

AppRunnerManage::AppRunnerManage()
{
    AppRunnerManageData *apiData = new AppRunnerManageData();
    data_ = apiData;
    initializeGateway();
}

AppRunnerManage::~AppRunnerManage()
{
    AppRunnerManageData *apiData = static_cast<AppRunnerManageData *>(data_);
    if (apiData)
    {
        delete apiData;
        apiData = nullptr;
    }
}
