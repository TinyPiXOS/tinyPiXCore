#include "Service/TpDesktopAPI.h"
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
#include "TpSurface.h"
#include "tinyPiXUtils.h"
#include "tinyPiXSys.h"
#include "TpApp.h"
#include "TpApp_def.h"
#include <mutex>
#include <erpc_client_setup.h>
#include <InteractData/TpWidgetsData.h>
#include <InteractData/TpDesktopData.h>
#include <erpc/erpc_port.h>
#include <future>

const TpString globalAppFilePathStr = "/System/app/";

struct TpDesktopAPIData
{
    IPiSysApiAgent *globalAgent = tinyPiX_sys_create();

    // RPC 传输层指针 TODO,修改为RPC维护应用信息
    erpc_client_t erpcClient = nullptr;
    erpc_transport_t transportPtr = nullptr;

    std::mutex readAppMutex;
    TpHash<TpString, int32_t> appUuidPidMap;
};

TpDesktopAPI *TpDesktopAPI::Instance()
{
    static TpDesktopAPI instance;
    return &instance;
}

TpDesktopAPI::OpenFileError TpDesktopAPI::openFile(const TpString &filePath, const TpString &appUuid)
{
    TpFileInfo fileInfo(filePath);
    if (!fileInfo.exists())
        return TpDesktopAPI::FileNotExist;

    TpString parseAppUuid = appUuid;

    if (appUuid.empty())
    {
        TpString fileSuffix = fileInfo.suffix();

        TpFile fileTypeMapperFile("/System/conf/fileTypeMapper.conf");
        if (!fileTypeMapperFile.open(TpFile::ReadOnly))
            return TpDesktopAPI::SystemFileDamage;

        // 查询配置文件内，该后缀类型文件对应的应用信息
        TpString appJsonStr = fileTypeMapperFile.readAll();
        fileTypeMapperFile.close();

        TpJsonDocument jsonDoc;
        jsonDoc.doc_.Parse(appJsonStr.c_str());

        TpJsonObject appJsonObj = jsonDoc.object();

        TpJsonObject fileSuffixTypeObj = appJsonObj.value("extensionType").toObject();
        if (!fileSuffixTypeObj.contains(fileSuffix))
            return TpDesktopAPI::NotSupport;

        // 再根据文件后缀对应的类型取出应用UUID
        TpString fileTypeStr = fileSuffixTypeObj.value(fileSuffix).toString();
        if (!appJsonObj.contains(fileTypeStr))
            return TpDesktopAPI::SystemFileDamage;

        TpJsonObject appInfoObj = appJsonObj.value(fileTypeStr).toObject();
        if (!appInfoObj.contains("uuid"))
            return TpDesktopAPI::SystemFileDamage;

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

    return TpDesktopAPI::Succsssful;
}

#if 0
void TpDesktopAPI::notifyWidgetsResize(const TpString &widgetUuid, const TpSize &widgetSize)
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

void TpDesktopAPI::notifyWidgetsPaint(const TpString &widgetUuid)
{
    TpString notifyTopic = widgetUuid + "_WidgetGateway2W";

    TpPaintWidgets paintEvent;
    TpStructPackager sPack;
    paintEvent.StructSerialize(sPack);

    publishGatewayData(notifyTopic.c_str(), sPack.data(), sPack.size());
}
#endif

bool TpDesktopAPI::home()
{
    TpDesktopAPIData *apiData = static_cast<TpDesktopAPIData *>(data_);
    if (!apiData)
        return false;
    tinyPiX_sys_send_home(apiData->globalAgent);
    return true;
}

bool TpDesktopAPI::setStatusBarStyle(int32_t rgba)
{
    TpChangeDeskStatusBarStyle statusBarStyle;
    statusBarStyle.bgRgba = rgba;

    TpStructPackager package;
    statusBarStyle.StructSerialize(package);

    return publishGatewayData(statusBarStyle.dataHead_.type_.c_str(), package.data(), package.size());
}

bool TpDesktopAPI::setStatusBarVisible(bool visible)
{
    TpChangeDeskStatusBarVisible statusBarVisible;
    statusBarVisible.visible = visible;

    TpStructPackager package;
    statusBarVisible.StructSerialize(package);

    return publishGatewayData(statusBarVisible.dataHead_.type_.c_str(), package.data(), package.size());
}

TpDesktopAPI::TpDesktopAPI()
{
    TpDesktopAPIData *apiData = new TpDesktopAPIData();
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
    // erpc_transport_t g_arbitrator;
    // erpc_transport_t transportServer;
    // apiData->erpcClient = erpc_client_init(apiData->transportPtr, message_buffer_factory);

    // // 初始化RPC客户端
    // initSystemApiService_client(apiData->erpcClient);
}

TpDesktopAPI::~TpDesktopAPI()
{
    TpDesktopAPIData *apiData = static_cast<TpDesktopAPIData *>(data_);
    if (apiData)
    {
        erpc_transport_tcp_deinit(apiData->transportPtr);
        delete apiData;
        apiData = nullptr;
    }
}
