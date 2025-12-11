#include "Service/TpAppManager.h"
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
#include <erpc_client_setup.h>
#include "c_TpAppManager_client.h"
#include "TpAppManager_client.hpp"
#include <TpInteractDataDef/TpWidgetsData.h>
#include <erpc/erpc_port.h>
#include <future>

const TpString globalAppFilePathStr = "/System/app/";

struct TpAppManagerData
{
    IPiSysApiAgent *globalAgent = tinyPiX_sys_create();

    // RPC 传输层指针 TODO,修改为RPC维护应用信息
    // erpc_client_t erpcClient = nullptr;
    // erpc_transport_t transportPtr = nullptr;

    std::mutex readAppMutex;
    TpHash<TpString, int32_t> appUuidPidMap;
};

#if 0
TpHash<TpString, int32_t> queryRunAppInfo()
{
    TpHash<TpString, int32_t> queryResMap;

    /* RPC 调用 */
    binary_t *appRunInfoPtr = TPR_RunAppProcessInfo();

    TpRPCRunAppProcessInfo recvData;
    recvData.StructDeserialize(appRunInfoPtr->data, appRunInfoPtr->dataLength);

    for (int i = 0; i < recvData.uuidList.size(); ++i)
    {
        queryResMap[recvData.uuidList.at(i)] = recvData.pidList.at(i);
    }

    delete[] appRunInfoPtr->data;
    delete appRunInfoPtr;

    return queryResMap;
}
#endif

TpAppManager *TpAppManager::Instance()
{
    static TpAppManager instance;
    return &instance;
}

TpAppManager::OpenFileError TpAppManager::openFile(const TpString &filePath, const TpString &appUuid)
{
    TpFileInfo fileInfo(filePath);
    if (!fileInfo.exists())
        return TpAppManager::FileNotExist;

    TpString parseAppUuid = appUuid;

    if (appUuid.empty())
    {
        TpString fileSuffix = fileInfo.suffix();

        TpFile fileTypeMapperFile("/System/conf/fileTypeMapper.conf");
        if (!fileTypeMapperFile.open(TpFile::ReadOnly))
            return TpAppManager::SystemFileDamage;

        // 查询配置文件内，该后缀类型文件对应的应用信息
        TpString appJsonStr = fileTypeMapperFile.readAll();
        fileTypeMapperFile.close();

        TpJsonDocument jsonDoc;
        jsonDoc.doc_.Parse(appJsonStr.c_str());

        TpJsonObject appJsonObj = jsonDoc.object();

        TpJsonObject fileSuffixTypeObj = appJsonObj.value("extensionType").toObject();
        if (!fileSuffixTypeObj.contains(fileSuffix))
            return TpAppManager::NotSupport;

        // 再根据文件后缀对应的类型取出应用UUID
        TpString fileTypeStr = fileSuffixTypeObj.value(fileSuffix).toString();
        if (!appJsonObj.contains(fileTypeStr))
            return TpAppManager::SystemFileDamage;

        TpJsonObject appInfoObj = appJsonObj.value(fileTypeStr).toObject();
        if (!appInfoObj.contains("uuid"))
            return TpAppManager::SystemFileDamage;

        // 根据应用UUID发送桌面启动应用
        parseAppUuid = appInfoObj.value("uuid").toString();
    }

    TpVector<std::string> argList;
    argList.emplace_back(filePath);

    TpRunApp startAppData;
    startAppData.appUuid = parseAppUuid;
    startAppData.argList = argList;

    TpStructPackager structPackage;
    startAppData.StructSerialize(structPackage);

    publishGatewayData(TpRunAppKey, structPackage.data(), structPackage.size());

    return TpAppManager::Succsssful;
}

void TpAppManager::notifyWidgetsResize(const TpString &widgetUuid, const TpSize &widgetSize)
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

    TpStructPackager sPack;
    paintEvent.StructSerialize(sPack);

    publishGatewayData(notifyTopic.c_str(), sPack.data(), sPack.size());
}

void TpAppManager::notifyWidgetsPaint(const TpString &widgetUuid)
{
    TpString notifyTopic = widgetUuid + "_WidgetGateway2W";

    TpPaintWidgets paintEvent;
    TpStructPackager sPack;
    paintEvent.StructSerialize(sPack);

    publishGatewayData(notifyTopic.c_str(), sPack.data(), sPack.size());
}

TpImage TpAppManager::appImage(const TpString &uuid)
{
    TpAppManagerData *apiData = static_cast<TpAppManagerData *>(data_);
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

bool TpAppManager::home()
{
    TpAppManagerData *apiData = static_cast<TpAppManagerData *>(data_);
    if (!apiData)
        return false;
    tinyPiX_sys_send_home(apiData->globalAgent);
    return true;
}

bool TpAppManager::startApp(const TpString &uuid, const TpVector<TpString> &argList)
{
    TpAppManagerData *apiData = static_cast<TpAppManagerData *>(data_);
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

bool TpAppManager::killApp(const TpString &uuid)
{
    TpAppManagerData *apiData = static_cast<TpAppManagerData *>(data_);
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

bool TpAppManager::killAllApp()
{
    TpAppManagerData *apiData = static_cast<TpAppManagerData *>(data_);
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

TpVector<TpAppManager::RunAppInfo> TpAppManager::runAppInfoList()
{
    TpVector<RunAppInfo> runAppList;

    TpAppManagerData *apiData = static_cast<TpAppManagerData *>(data_);
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

TpAppManager::RunAppInfo TpAppManager::runAppInfo(const TpString &uuid)
{
    RunAppInfo runAppInfo;

    TpAppManagerData *apiData = static_cast<TpAppManagerData *>(data_);
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

bool TpAppManager::setStatusBarStyle(int32_t rgba)
{
    TpChangeDeskStatusBarStyle statusBarStyle;
    statusBarStyle.bgRgba = rgba;

    TpStructPackager package;
    statusBarStyle.StructSerialize(package);

    return publishGatewayData(statusBarStyle.dataHead_.type_.c_str(), package.data(), package.size());
}

bool TpAppManager::setStatusBarVisible(bool visible)
{
    TpChangeDeskStatusBarVisible statusBarVisible;
    statusBarVisible.visible = visible;

    TpStructPackager package;
    statusBarVisible.StructSerialize(package);

    return publishGatewayData(statusBarVisible.dataHead_.type_.c_str(), package.data(), package.size());
}

TpAppManager::TpAppManager()
{
    TpAppManagerData *apiData = new TpAppManagerData();
    data_ = apiData;
    initializeGateway();

    /* 创建client端传输层对象(TCP) */
    // apiData->transportPtr = erpc_transport_tcp_init("127.0.0.1", 12581, false);
    // auto message_buffer_factory = erpc_mbf_dynamic_init();

    // if (!apiData->transportPtr)
    // {
    //     std::cout << "RPC 服务初始化失败!" << std::endl;
    //     return;
    // }

    // /* 初始化客户端 */
    // erpc_transport_t transportServer;
    // apiData->erpcClient = erpc_client_init(apiData->transportPtr, message_buffer_factory);

    // // 初始化RPC客户端
    // initSystemApiService_client(apiData->erpcClient);
}

TpAppManager::~TpAppManager()
{
    TpAppManagerData *apiData = static_cast<TpAppManagerData *>(data_);
    if (apiData)
    {
        // erpc_transport_tcp_deinit(apiData->transportPtr);
        delete apiData;
        apiData = nullptr;
    }
}
