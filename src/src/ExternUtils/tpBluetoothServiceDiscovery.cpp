

#include "tpBluetoothServiceDiscovery.h"	
#include "bluetooth/include/blt_sdp.h"

struct tpBluetoothServiceDiscoveryData{
	tpBluetoothAddress addr;
	tpList<tpString> uuids_filter;
	tpBluetoothServiceDiscoveryData(const tpBluetoothAddress &addr_):addr(addr_){

	};
};





static char UUID_str[MAX_LEN_UUID_STR];

static void bluet_service_desc(void *value, void *user)
{
	tpBluetoothService *service=(tpBluetoothService *)user;
	char str[MAX_LEN_PROTOCOL_UUID_STR];
	sdp_data_t *p = (sdp_data_t *)value, *s;
	int i = 0, proto = 0;

	for (; p; p = p->next, i++) 
	{
		switch (p->dtd) {
		case SDP_UUID16:
		case SDP_UUID32:
		case SDP_UUID128:
			sdp_uuid2strn(&p->val.uuid, UUID_str, MAX_LEN_UUID_STR);
			sdp_proto_uuid2strn(&p->val.uuid, str, sizeof(str));
			proto = sdp_uuid_to_proto(&p->val.uuid);
			printf("  \"%s\" (0x%s)\n", str, UUID_str);
			break;
		case SDP_UINT8:
			if (proto == RFCOMM_UUID)
				printf("    Channel: %d\n", p->val.uint8);
			else
				printf("    uint8: 0x%02x\n", p->val.uint8);
			break;
		case SDP_UINT16:
			if (proto == L2CAP_UUID) {
				if (i == 1)
					printf("    PSM: %d\n", p->val.uint16);
				else
					printf("    Version: 0x%04x\n", p->val.uint16);
			} else if (proto == BNEP_UUID)
				if (i == 1)
					printf("    Version: 0x%04x\n", p->val.uint16);
				else
					printf("    uint16: 0x%04x\n", p->val.uint16);
			else
				printf("    uint16: 0x%04x\n", p->val.uint16);
			break;
		case SDP_SEQ16:
			printf("    SEQ16:");
			for (s = p->val.dataseq; s; s = s->next)
				printf(" %x", s->val.uint16);
			printf("\n");
			break;
		case SDP_SEQ8:
			printf("    SEQ8:");
			for (s = p->val.dataseq; s; s = s->next)
				printf(" %x", s->val.uint8);
			printf("\n");
			break;
		default:
			printf("    FIXME: dtd=0%x\n", p->dtd);
			break;
		}
	}
}


static void bluet_profile_desc(void *value, void *userData)
{
	tpList<tpBluetoothService> *list=(tpList<tpBluetoothService> *)userData;

	sdp_profile_desc_t *desc = (sdp_profile_desc_t *)value;
	char str[MAX_LEN_PROFILEDESCRIPTOR_UUID_STR];

	sdp_uuid2strn(&desc->uuid, UUID_str, MAX_LEN_UUID_STR);
	sdp_profile_uuid2strn(&desc->uuid, str, MAX_LEN_PROFILEDESCRIPTOR_UUID_STR);

	printf("  \"%s\" (0x%s)\n", str, UUID_str);
	if (desc->version)
		printf("    Version: 0x%04x\n", desc->version);
}

static void bluet_lang_attr(void *value, void *user)
{
	sdp_lang_attr_t *lang = (sdp_lang_attr_t *)value;
	printf("  code_ISO639: 0x%02x\n", lang->code_ISO639);
	printf("  encoding:    0x%02x\n", lang->encoding);
	printf("  base_offset: 0x%02x\n", lang->base_offset);
}

static void bluet_access_protos(void *value, void *userData)
{
	sdp_list_t *protDescSeq = (sdp_list_t *)value;
	sdp_list_foreach(protDescSeq, bluet_service_desc, userData);
}


//uuidhuoqu
static void bluet_service_class(void *value, void *userData)
{
	tpBluetoothService *service=(tpBluetoothService *)userData;
	char ServiceClassUUID_str[MAX_LEN_SERVICECLASS_UUID_STR];
	uuid_t *uuid = (uuid_t *)value;

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
	tpBluetoothUuid tp_uuid;
	switch(uuid->type)
	{
		case SDP_UUID16:
		{
			tp_uuid=tpBluetoothUuid(uuid->value.uuid16);
			break;
		}
		case SDP_UUID32:
		{
			tp_uuid=tpBluetoothUuid(uuid->value.uuid32);
			break;
		}
		case SDP_UUID128:
		{
			tpUInt8 uuid128[16];
			memcpy(uuid128,uuid->value.uuid128.data,sizeof(uuid->value.uuid128));
			tp_uuid=tpBluetoothUuid(uuid128);
			break;
		}
		default:
			return ;
	}

	if(service->getServiceUuid().isNull())
	{
		service->setServiceUuid(tp_uuid);
		service->setServiceName(tp_uuid.toName());
	}		
	service->addServiceClassUuid(tp_uuid);
}

static void callback_service_discovery(sdp_record_t *rec,void *userdata)
{
	tpList<tpBluetoothService> *list=(tpList<tpBluetoothService> *)userdata;

	tpBluetoothService service;
	sdp_data_t *d = sdp_data_get(rec, SDP_ATTR_SVCNAME_PRIMARY);
	if (d && SDP_IS_TEXT_STR(d->dtd))
	{
		printf("Service Name: %.*s\n", d->unitSize, d->val.str);
		service.setServiceName(tpString(d->val.str));
	}
	d = sdp_data_get(rec, SDP_ATTR_SVCDESC_PRIMARY);
	if (d && SDP_IS_TEXT_STR(d->dtd))
	{
		printf("Service Description: %.*s\n", d->unitSize, d->val.str);
		service.setServiceDescription(tpString(d->val.str));
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
	if (sdp_get_service_classes(rec, &sdp_list) == 0) {
		printf("Service Class ID List:\n");
		sdp_list_foreach(sdp_list, bluet_service_class, &service);
		sdp_list_free(sdp_list, free);
	}
	if (sdp_get_access_protos(rec, &proto) == 0) {
		printf("Protocol Descriptor List:\n");
		sdp_list_foreach(proto, bluet_access_protos, &service);
		sdp_list_foreach(proto, (sdp_list_func_t)sdp_list_free, 0);
		sdp_list_free(proto, 0);
	}
	if (sdp_get_lang_attr(rec, &sdp_list) == 0) {
		printf("Language Base Attr List:\n");
		sdp_list_foreach(sdp_list, bluet_lang_attr, &service);
		sdp_list_free(sdp_list, free);
	}
	if (sdp_get_profile_descs(rec, &sdp_list) == 0) {
		printf("Profile Descriptor List:\n");
		sdp_list_foreach(sdp_list, bluet_profile_desc, &service);
		sdp_list_free(sdp_list, free);
	}

}

	
tpBluetoothServiceDiscovery::tpBluetoothServiceDiscovery(const tpBluetoothAddress &addr)
{
	data_ = new tpBluetoothServiceDiscoveryData(addr);
	tpBluetoothServiceDiscoveryData *data = static_cast<tpBluetoothServiceDiscoveryData *>(data_);
	if(!data)
	{
		fprintf(stderr,"[Error]: tpBluetoothServiceDiscovery\n");
		return ;
	}
	
}

tpBluetoothServiceDiscovery::~tpBluetoothServiceDiscovery()
{
	tpBluetoothServiceDiscoveryData *data = static_cast<tpBluetoothServiceDiscoveryData *>(data_);
	if(!data)
		return ;
	

	delete(data);
}

//开始扫描
int tpBluetoothServiceDiscovery::start()
{
	tpBluetoothServiceDiscoveryData *data = static_cast<tpBluetoothServiceDiscoveryData *>(data_);
	struct SdpAttrValue *attr_data=(struct SdpAttrValue *)malloc(sizeof(struct SdpAttrValue)*16);
	memset(attr_data,0,sizeof(struct SdpAttrValue)*16);
	uint16_t uuid=0x0003;
	const char bt_addr[18]="E4:5F:01:37:58:93";

	printf("开始扫描\n");
//	bluet_quere_profile_attr((const char *)bt_addr, uuid,  attr_data,16);
	printf("开始扫描\n");
//	scan_device_services((const char *)bt_addr);
	scan_device_services(NULL,callback_service_discovery,this);
	/*memset(attr_data,0,sizeof(struct SdpAttrValue)*16);	
	int count = sdp_query_device(bt_addr, uuid, attr_data, 16);
	if (count < 0) {
        printf("查询失败或未找到服务。\n");
    } 
	else {
        printf("查询到 %d 个属性:\n", count);		

    }*/

    free(attr_data);



	return 0;
}

//停止扫描
int tpBluetoothServiceDiscovery::stop()
{
	return 0;
}


int tpBluetoothServiceDiscovery::setRemoteAddress(const tpBluetoothAddress &addr)
{
	return 0;
}

tpBluetoothAddress tpBluetoothServiceDiscovery::getRemoteAddress() const
{
	return tpBluetoothAddress();
}

int tpBluetoothServiceDiscovery::setUuidFilter(const tpString &uuid)
{
	return 0;
}

int tpBluetoothServiceDiscovery::setUuidFilter(const tpList<tpString> &uuid)
{
	return 0;
}

tpString tpBluetoothServiceDiscovery::getUuidFilter() const
{
	return nullptr;
}

tpList<tpBluetoothService> tpBluetoothServiceDiscovery::discoveredServices()
{
	tpList<tpBluetoothService> list;
	return list;
}


