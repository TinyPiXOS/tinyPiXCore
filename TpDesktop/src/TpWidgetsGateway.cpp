#include "TpWidgetsGateway.h"
#include "TpGateway.h"
#include "TpShareTrans.h"
#include "TpSize.h"
#include "InteractData/TpWidgetsData.h"

struct TpWidgetsGatewayData
{
    TpString subTopic = "";
    TpString pubTopic = "";

    // 当前画布尺寸；画布尺寸变化，才会重新创建surface
    TpSize curSurfaceSize;
    IPiWFSurface *wfSurface = nullptr;

    tpUInt64 shmid = 0;
};

TpWidgetsGateway::TpWidgetsGateway(const TpString &widgetUuid)
{
    TpWidgetsGatewayData *widgetsData = new TpWidgetsGatewayData();
    data_ = widgetsData;

    initializeGateway();
    widgetsData->subTopic = widgetUuid + "_WidgetGateway2W";
    widgetsData->pubTopic = widgetUuid + "_WidgetGateway2D";
    subscribeGatewayData(widgetsData->subTopic.c_str(), this);
}

TpWidgetsGateway::~TpWidgetsGateway()
{
    TpWidgetsGatewayData *widgetsData = static_cast<TpWidgetsGatewayData *>(data_);
    if (widgetsData)
    {
        delete widgetsData;
        widgetsData = nullptr;
        data_ = nullptr;
    }
}

void TpWidgetsGateway::update()
{
    TpWidgetsGatewayData *widgetsData = static_cast<TpWidgetsGatewayData *>(data_);

    TpUpdateWidgets updateData;
    updateData.sharedId = widgetsData->shmid;
    TpStructPackager sPack;
    updateData.StructSerialize(sPack);

    publishGatewayData(widgetsData->pubTopic.c_str(), sPack.data(), sPack.size());
}

void TpWidgetsGateway::recvData(const char *topic, const void *data, const uint32_t &size)
{
    TpString recvTopic(topic);

    TpWidgetsGatewayData *widgetsData = static_cast<TpWidgetsGatewayData *>(data_);
    if (recvTopic.compare(widgetsData->subTopic) != 0)
        return;

    RecvTopicDataImpl recvTopicData;
    recvTopicData.StructDeserialize(data, size);
    if (recvTopicData.dataHead_.type_.compare(TpInitWidgetsKey) == 0)
    {
        // 初始化指令
        TpInitWidgets initWidget;
        initWidget.StructDeserialize(data, size);

        // 如果收到的宽高有一个为0，则画布无效，不再处理
        if (widgetsData->curSurfaceSize.width() == 0 || widgetsData->curSurfaceSize.height() == 0)
            return;

        // 创建新的Surface对象;如果尺寸变化；释放旧指针，创建新画布指针
        if (widgetsData->curSurfaceSize.width() == initWidget.width &&
            widgetsData->curSurfaceSize.height() == initWidget.height)
            return;

        if (widgetsData->wfSurface)
        {
            tinyPiX_surface_free(widgetsData->wfSurface);
        }

        widgetsData->curSurfaceSize.setWidth(initWidget.width);
        widgetsData->curSurfaceSize.setHeight(initWidget.height);

        widgetsData->wfSurface = tinyPiX_surface_create(nullptr, initWidget.width, initWidget.height, initWidget.depth, initWidget.Rmask, initWidget.Gmask, initWidget.Bmask, initWidget.Amask);

        widgetsData->shmid = TpShareTrans::createShareTrans(widgetsData->wfSurface);
    }
    else if (recvTopicData.dataHead_.type_.compare(TpPaintWidgetsKey) == 0)
    {
        // TpPaintWidgets paintEvent;
        // 通知重绘
        onPaintEvent();
    }
    else
    {
    }
}
