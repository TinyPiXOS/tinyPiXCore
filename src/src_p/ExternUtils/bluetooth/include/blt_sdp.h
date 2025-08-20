#ifndef _BLT_SDP_H_
#define _BLT_SDP_H_


#ifdef	__cplusplus
extern "C" {
#endif


#include <unistd.h>
#include <bluetooth/sdp.h>

//主要对常用的几个协议的UUID的二次封装
typedef enum SdpCreatUUID_{
	BLUET_OBEX_UUID	= OBEX_FILETRANS_SVCLASS_ID,
	
}SdpCreatUUID;



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

void print_service_attr(sdp_record_t *rec);
int bluet_quere_profile_attr(const char *bt_addr,uint16_t uuid,struct SdpAttrValue *attr_data,size_t attr_size);
int sdp_query_device(const char *bt_addr, uint16_t uuid, struct SdpAttrValue *attr_data, size_t attr_size);
void bluet_free_attr_array(struct SdpAttrValue *attr_data, size_t count);
void bluet_free_last_search(void);

int get_obex_channel(const char *bt_addr);

int scan_device_services(const char *bt_addr)  ;

#ifdef	__cplusplus
}
#endif

#endif