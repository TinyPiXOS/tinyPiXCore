#ifndef _BLT_SDP_H_
#define _BLT_SDP_H_


#ifdef	__cplusplus
extern "C" {
#endif


#include <unistd.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

//主要对常用的几个协议的UUID的二次封装
typedef enum SdpCreatUUID_{
	BLUET_OBEX_UUID	= OBEX_FILETRANS_SVCLASS_ID,
	
}SdpCreatUUID;


typedef void (*ServiceDiscoveryCallback)(sdp_record_t *rec,void *userdata);
struct SdpAttrValue{
	uint32_t attr;
	union {
		int8_t    int8;
		int16_t   int16;
		int32_t   int32;
		int64_t   int64;
		uint128_t int128;
		uint8_t   uint8;
		uint16_t  uint16;
		uint32_t  uint32;
		uint64_t  uint64;
		uint128_t uint128;
		uuid_t    uuid;
		char     *str;
		sdp_data_t *dataseq;
	} val;
};

typedef struct {
    sdp_record_t *record;
    sdp_list_t   *attr_list;
} SdpRecordWrapper;

//服务属性
struct SdpServerInfo{
	char *name;		//服务名称
	char *prov;		//厂商
	char *desc;		//描述
};

int bluet_quere_profile_attr(const char *bt_addr,uint16_t uuid,struct SdpAttrValue *attr_data,size_t attr_size);

int get_obex_channel(const char *bt_addr);

int scan_device_services(const char *bt_addr,ServiceDiscoveryCallback callback,void *userdata);

const char *bluet_uuid_to_name(uint16_t uuid);
uint16_t bluet_name_to_uuid(const char *name);

int bluet_uuid128_to_uuid32(const uint8_t uuid128[16], uint32_t *uuid32);
int bluet_uuid128_to_uuid16(const uint8_t uuid128[16], uint16_t *uuid16);
void bluet_uuid16_to_uuid128(uint16_t uuid16, uint8_t uuid128[16]);
void bluet_uuid32_to_uuid128(uint32_t uuid32, uint8_t uuid128[16]);
char *bluet_uuid128_to_uuidstr(const uint8_t uuid128[16]);
int bluet_uuidstr_to_uuid128(const char *uuidstr, uint8_t uuid128[16]);
char *bluet_name_to_uuidstr(const char *name);
const char *bluet_uuidstr_to_name(const char *uuidstr);


#ifdef	__cplusplus
}
#endif

#endif