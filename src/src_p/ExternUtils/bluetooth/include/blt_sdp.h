#ifndef _BLT_SDP_H_
#define _BLT_SDP_H_

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


//服务属性
struct SdpServerInfo{
	char *name;		//服务名称
	char *prov;		//厂商
	char *desc;		//描述
};



int bluet_quere_profile_attr(const char *bt_addr,uint16_t uuid,struct SdpAttrValue *attr_data,size_t attr_size);

int get_obex_channel(const char *bt_addr);
int sdp_dbus_test(const char *addr) ;
#endif