
#include <thread>
#include "include/blt_sdp.h"
#include "TpBluetoothServiceDiscovery.h"

struct TpBluetoothServiceDiscoveryData
{
    TpBluetoothAddress addr;              // 远端主机地址
    TpList<TpBluetoothUuid> uuids_filter; // uuid过滤器列表,用于只扫描指定服务
    TpList<TpBluetoothService> list;      // 扫描结果
    std::thread thread_t;                 // 扫描的线程
    tpBool is_discovering;                // 是否扫描中
    TpBluetoothServiceDiscoveryData(const TpBluetoothAddress &addr_) : addr(addr_) {

                                                                       };
};

static TpBluetoothUuid uuidToBluetoothUuid(const uuid_t *uuid)
{
    switch (uuid->type)
    {
    case SDP_UUID16:
        return TpBluetoothUuid(uuid->value.uuid16);
    case SDP_UUID32:
        return TpBluetoothUuid(uuid->value.uuid32);
    case SDP_UUID128:
        return TpBluetoothUuid(uuid->value.uuid128.data);
    default:
        return TpBluetoothUuid(); // 返回空 UUID
    }
}

static char UUID_str[MAX_LEN_UUID_STR];

static void bluet_service_desc(void *value, void *user)
{

    TpBluetoothService::Sequence *protocolListSeq = (TpBluetoothService::Sequence *)user;

    char str[MAX_LEN_PROTOCOL_UUID_STR];
    sdp_data_t *p = (sdp_data_t *)value, *s;
    int i = 0, proto = 0;

    for (; p; p = p->next, i++)
    {
        TpBluetoothService::Sequence protocolSeq;
        switch (p->dtd)
        {
        case SDP_UUID16:
        case SDP_UUID32:
        case SDP_UUID128:
            // 调试打印开始
            sdp_uuid2strn(&p->val.uuid, UUID_str, MAX_LEN_UUID_STR);
            sdp_proto_uuid2strn(&p->val.uuid, str, sizeof(str));
            proto = sdp_uuid_to_proto(&p->val.uuid);
            printf("  \"%s\" (0x%s)\n", str, UUID_str);
            // 调试打印结束
            protocolSeq << uuidToBluetoothUuid(&p->val.uuid);
            break;
        case SDP_UINT8:
            if (proto == RFCOMM_UUID)
                printf("    Channel: %d\n", p->val.uint8);
            else
                printf("    uint8: 0x%02x\n", p->val.uint8);
            protocolSeq << static_cast<uint8_t>(p->val.uint8);
            break;
        case SDP_UINT16:
            if (proto == L2CAP_UUID)
            {
                if (i == 1)
                    printf("    PSM: %d\n", p->val.uint16);
                else
                    printf("    Version: 0x%04x\n", p->val.uint16);
            }
            else if (proto == BNEP_UUID)
                if (i == 1)
                    printf("    Version: 0x%04x\n", p->val.uint16);
                else
                    printf("    uint16: 0x%04x\n", p->val.uint16);
            else
                printf("    uint16: 0x%04x\n", p->val.uint16);
            protocolSeq << static_cast<uint16_t>(p->val.uint16);
            break;
        case SDP_SEQ16:
        case SDP_SEQ8:
        {
            TpBluetoothService::Sequence nestedSeq;
            for (sdp_data_t *s = p->val.dataseq; s; s = s->next)
            {
                // 递归处理嵌套序列
                if (s->dtd == SDP_SEQ8)
                {
                    nestedSeq << static_cast<uint8_t>(s->val.uint8);
                }
                else if (s->dtd == SDP_SEQ16)
                {
                    nestedSeq << static_cast<uint16_t>(s->val.uint16);
                }
                else if (s->dtd == SDP_SEQ32)
                {
                    nestedSeq << TpBluetoothUuid(s->val.uuid.value.uuid16);
                }
            }
            protocolSeq << nestedSeq;
            break;
        }
        default:
            printf("    FIXME: dtd=0%x\n", p->dtd);
            break;
        }
        *protocolListSeq << protocolSeq;
    }
}

static void bluet_profile_desc(void *value, void *userData)
{
    TpBluetoothService::Sequence *profileSeq = (TpBluetoothService::Sequence *)userData;
    sdp_profile_desc_t *desc = (sdp_profile_desc_t *)value;

    TpBluetoothService::Sequence profileDescSeq;

    char str[MAX_LEN_PROFILEDESCRIPTOR_UUID_STR];

    sdp_uuid2strn(&desc->uuid, UUID_str, MAX_LEN_UUID_STR);
    sdp_profile_uuid2strn(&desc->uuid, str, MAX_LEN_PROFILEDESCRIPTOR_UUID_STR);

    printf("  \"%s\" (0x%s)\n", str, UUID_str);
    if (desc->version)
        printf("    Version: 0x%04x\n", desc->version);

    profileDescSeq << uuidToBluetoothUuid(&desc->uuid);
    if (desc->version)
        profileDescSeq << static_cast<uint16_t>(desc->version);

    *profileSeq << profileDescSeq;
}

static void bluet_lang_attr(void *value, void *user)
{
    sdp_lang_attr_t *lang = (sdp_lang_attr_t *)value;
    TpBluetoothService::Sequence *langSeq = (TpBluetoothService::Sequence *)user;
    TpBluetoothService::Sequence langAttrSeq;
    printf("  code_ISO639: 0x%02x\n", lang->code_ISO639);
    printf("  encoding:    0x%02x\n", lang->encoding);
    printf("  base_offset: 0x%02x\n", lang->base_offset);

    langAttrSeq << static_cast<uint16_t>(lang->code_ISO639);
    langAttrSeq << static_cast<uint16_t>(lang->encoding);
    langAttrSeq << static_cast<uint16_t>(lang->base_offset);

    *langSeq << langAttrSeq;
}

static void bluet_access_protos(void *value, void *userData)
{
    sdp_list_t *protDescSeq = (sdp_list_t *)value;
    sdp_list_foreach(protDescSeq, bluet_service_desc, userData);
}

// uuidhuoqu
static void bluet_service_class(void *value, void *userData)
{
    TpBluetoothService *service = (TpBluetoothService *)userData;
    TpList<TpBluetoothUuid> *classIdList = (TpList<TpBluetoothUuid> *)classIdList;

    TpBluetoothService::Sequence *classUuidSeq = (TpBluetoothService::Sequence *)userData;

    char ServiceClassUUID_str[MAX_LEN_SERVICECLASS_UUID_STR];
    uuid_t *uuid = (uuid_t *)value;

    // 以下为调试打印
    sdp_uuid2strn(uuid, UUID_str, MAX_LEN_UUID_STR);
    sdp_svclass_uuid2strn(uuid, ServiceClassUUID_str, MAX_LEN_SERVICECLASS_UUID_STR);
    if (uuid->type != SDP_UUID128)
    {
        printf("  \"%s\" (0x%s)\n", ServiceClassUUID_str, UUID_str);
    }
    else
    {
        printf("  UUID 128: %s\n", UUID_str);
    }
    // 以上为调试打印

    classIdList->emplace_back(uuidToBluetoothUuid(uuid));
}

static void callback_service_discovery(sdp_record_t *rec, void *userdata)
{
    TpList<TpBluetoothService> *list = (TpList<TpBluetoothService> *)userdata;
    printf("callback_service_discovery\n");
    TpBluetoothService service;
    sdp_data_t *d = sdp_data_get(rec, SDP_ATTR_SVCNAME_PRIMARY);
    if (d && SDP_IS_TEXT_STR(d->dtd))
    {
        printf("Service Name: %.*s\n", d->unitSize, d->val.str);
        service.setServiceName(TpString(d->val.str));
    }
    d = sdp_data_get(rec, SDP_ATTR_SVCDESC_PRIMARY);
    if (d && SDP_IS_TEXT_STR(d->dtd))
    {
        printf("Service Description: %.*s\n", d->unitSize, d->val.str);
        service.setServiceDescription(TpString(d->val.str));
    }
    d = sdp_data_get(rec, SDP_ATTR_PROVNAME_PRIMARY);
    if (d && SDP_IS_TEXT_STR(d->dtd))
    {
        printf("Service Provider: %.*s\n", d->unitSize, d->val.str);
        service.setServiceProvider(d->val.str);
    }

    printf("Service RecHandle: 0x%x\n", rec->handle);
    service.setServiceRecHandle(rec->handle);

    sdp_list_t *sdp_list = 0, *proto = 0;
    if (sdp_get_service_classes(rec, &sdp_list) == 0)
    {
        printf("Service Class ID List:\n");
        TpList<TpBluetoothUuid> classIdList;
        sdp_list_foreach(sdp_list, bluet_service_class, &classIdList);
        service.setServiceClassUuids(classIdList);
        sdp_list_free(sdp_list, free);
    }
    if (sdp_get_access_protos(rec, &proto) == 0)
    {
        printf("Protocol Descriptor List:\n");
        TpBluetoothService::Sequence protocolListSeq;
        sdp_list_foreach(proto, bluet_access_protos, &protocolListSeq);
        service.setProtocolDescriptorList(protocolListSeq);
        sdp_list_foreach(proto, (sdp_list_func_t)sdp_list_free, 0);
        sdp_list_free(proto, 0);
    }
    if (sdp_get_lang_attr(rec, &sdp_list) == 0)
    {
        printf("Language Base Attr List:\n");
        TpBluetoothService::Sequence langSeq;
        sdp_list_foreach(sdp_list, bluet_lang_attr, &langSeq);
        service.setAttribute(TpBluetoothService::LanguageBaseAttributeIdList, langSeq);
        sdp_list_free(sdp_list, free);
    }
    if (sdp_get_profile_descs(rec, &sdp_list) == 0)
    {
        printf("Profile Descriptor List:\n");
        TpBluetoothService::Sequence profileSeq;
        sdp_list_foreach(sdp_list, bluet_profile_desc, &profileSeq);
        service.setProtocolDescriptorList(profileSeq);
        sdp_list_free(sdp_list, free);
    }
    list->emplace_back(service);
}

TpBluetoothServiceDiscovery::TpBluetoothServiceDiscovery(const TpBluetoothAddress &addr)
{
    data_ = new TpBluetoothServiceDiscoveryData(addr);
    TpBluetoothServiceDiscoveryData *data = static_cast<TpBluetoothServiceDiscoveryData *>(data_);
    if (!data)
    {
        fprintf(stderr, "[Error]: TpBluetoothServiceDiscovery\n");
        return;
    }
}

TpBluetoothServiceDiscovery::~TpBluetoothServiceDiscovery()
{
    TpBluetoothServiceDiscoveryData *data = static_cast<TpBluetoothServiceDiscoveryData *>(data_);
    if (!data)
        return;

    data->list.clear();

    if (data->thread_t.joinable())
        data->thread_t.join(); // 等待线程完成
    delete (data);
}

void TpBluetoothServiceDiscovery::discoveryOnce()
{
    TpBluetoothServiceDiscoveryData *data = static_cast<TpBluetoothServiceDiscoveryData *>(data_);

    scan_device_services(data->addr.toString().c_str(), callback_service_discovery, &data->list);

    data->is_discovering = TP_FALSE;
    finished.emit(data->list);
}

// 开始扫描
int TpBluetoothServiceDiscovery::discoveryServices()
{
    TpBluetoothServiceDiscoveryData *data = static_cast<TpBluetoothServiceDiscoveryData *>(data_);

    if (data->addr.isNull())
    {
        fprintf(stderr, "[Error]: 未设置目标地址\n");
        return -1;
    }

    data->is_discovering = TP_TRUE;
    data->thread_t = std::thread(&TpBluetoothServiceDiscovery::discoveryOnce, this);

    return 0;
}

int TpBluetoothServiceDiscovery::setRemoteAddress(const TpBluetoothAddress &addr)
{
    TpBluetoothServiceDiscoveryData *data = static_cast<TpBluetoothServiceDiscoveryData *>(data_);
    data->addr = addr;
    return 0;
}

TpBluetoothAddress TpBluetoothServiceDiscovery::getRemoteAddress() const
{
    TpBluetoothServiceDiscoveryData *data = static_cast<TpBluetoothServiceDiscoveryData *>(data_);
    return data->addr;
}

int TpBluetoothServiceDiscovery::setUuidFilter(const TpBluetoothUuid &uuid)
{

    return 0;
}

int TpBluetoothServiceDiscovery::setUuidFilter(const TpList<TpBluetoothUuid> &uuid)
{

    return 0;
}

TpList<TpBluetoothUuid> TpBluetoothServiceDiscovery::getUuidFilter() const
{
    TpBluetoothServiceDiscoveryData *data = static_cast<TpBluetoothServiceDiscoveryData *>(data_);
    return data->uuids_filter;
}

tpBool TpBluetoothServiceDiscovery::isDiscovering() const
{
    TpBluetoothServiceDiscoveryData *data = static_cast<TpBluetoothServiceDiscoveryData *>(data_);
    return data->is_discovering;
}

TpList<TpBluetoothService> TpBluetoothServiceDiscovery::getDiscoveredServices() const
{
    TpBluetoothServiceDiscoveryData *data = static_cast<TpBluetoothServiceDiscoveryData *>(data_);
    if (isDiscovering())
        return TpList<TpBluetoothService>(); // 扫描过程中不允许获取
    return data->list;
}
