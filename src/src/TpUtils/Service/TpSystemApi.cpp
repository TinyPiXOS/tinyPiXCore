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
#include <erpc_client_setup.h>
#include "c_TpSystemApi_client.h"
#include "TpSystemApi_client.hpp"

const TpString globalAppFilePathStr = "/System/app/";

struct TpSystemApiData
{
    IPiSysApiAgent *globalAgent = tinyPiX_sys_create();

    // RPC 传输层指针
    erpc_transport_t transportPtr = nullptr;

    // TODO 将缓存数据放入Service中，远程IPC调用
    // std::mutex readAppMutex;
    // 已启动应用的UUID和pid映射表
    // TpHash<TpString, int32_t> appUuidPidMap = TpHash<TpString, int32_t>();
};

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

    TpRunApp startAppData;
    startAppData.appUuid = parseAppUuid;
    startAppData.argList = argList;

    PStructPackager structPackage;
    startAppData.StructSerialize(structPackage);

    publishGatewayData(TpRunAppKey, structPackage.data(), structPackage.size());

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

    auto runAppMap = queryRunAppInfo();
    if (!runAppMap.contains(uuid))
        return TpImage();

    int32_t appPid = runAppMap.value(uuid);
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
        std::cout << "UUid: " << uuid << " 未安装!" << std::endl;
        return false;
    }

    /* RPC 调用 */
    list_string_1_t *rpcArgs = new list_string_1_t();
    rpcArgs->elementsCount = argList.size();
    rpcArgs->elements = nullptr;
    if (argList.size() > 0)
    {
        rpcArgs->elements = new char *[rpcArgs->elementsCount];

        for (int i = 0; i < argList.size(); ++i)
        {
            rpcArgs->elements[i] = strdup(argList.at(i).c_str());
        }
    }

    bool startRes = TPR_StartApp(uuid.c_str(), rpcArgs);

    // 释放输入参数
    for (int i = 0; i < rpcArgs->elementsCount; i++)
    {
        free(rpcArgs->elements[i]);
    }
    delete[] rpcArgs->elements;
    delete rpcArgs;

    return startRes;
}

bool TpSystemApi::killApp(const TpString &uuid)
{
    TpSystemApiData *apiData = static_cast<TpSystemApiData *>(data_);
    if (!apiData)
        return false;

    /* RPC 调用 */
    return TPR_KillApp(uuid.c_str());
}

bool TpSystemApi::killAllApp()
{
    TpSystemApiData *apiData = static_cast<TpSystemApiData *>(data_);
    if (!apiData)
        return false;

    /* RPC 调用 */
    return TPR_KillAllApp();
}

TpVector<TpSystemApi::RunAppInfo> TpSystemApi::runAppInfoList()
{
    TpVector<RunAppInfo> runAppList;

    TpSystemApiData *apiData = static_cast<TpSystemApiData *>(data_);
    if (!apiData)
        return runAppList;

    // 运行的应用PID列表
    auto runAppMap = queryRunAppInfo();
    TpList<int32_t> runAppPidList = runAppMap.values();

    // 获取所有应用列表
    PiShmBytes *appIdList = nullptr;
    int appSize = 0;
    tinyPiX_sys_find_win_ids(apiData->globalAgent, &appIdList, &appSize, Q_FIXS);

    for (int i = 0; i < appSize; ++i)
    {
        PiShmBytes appIdInfo = appIdList[i];

        if (runAppPidList.contains(appIdInfo.p_id))
        {
            TpString curAppUuid = runAppMap.key(appIdInfo.p_id);

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

    auto runAppMap = queryRunAppInfo();

    if (!runAppMap.contains(uuid))
        return runAppInfo;

    int32_t appPid = runAppMap.value(uuid);

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
    TpSystemApiData *apiData = new TpSystemApiData();
    data_ = apiData;
    initializeGateway();

    /* 创建client端传输层对象(TCP) */
    apiData->transportPtr = erpc_transport_tcp_init("127.0.0.1", 12581, false);
    auto message_buffer_factory = erpc_mbf_dynamic_init();

    if (!apiData->transportPtr)
    {
        std::cout << "RPC 服务初始化失败!" << std::endl;
        return;
    }

    /* 初始化客户端 */
    auto client = erpc_client_init(apiData->transportPtr, message_buffer_factory);

    // 初始化RPC客户端
    initSystemApiService_client(client);
}

TpSystemApi::~TpSystemApi()
{
    TpSystemApiData *apiData = static_cast<TpSystemApiData *>(data_);
    if (apiData)
    {
        erpc_transport_tcp_deinit(apiData->transportPtr);
        delete apiData;
        apiData = nullptr;
    }
}
