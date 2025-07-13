/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙SDP协议相关
说 明 : 包含两部分，1.SDP扫描，发现远程设备支持的服务，信息。2.SDP注册，本机注册SDP用于暴露给其他标准蓝牙设备
日 期 : 2025.3.13

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include "../include/blt_sdp.h"
#include "bluetooth_inc.h"



static char UUID_str[MAX_LEN_UUID_STR];

static void print_service_desc(void *value, void *user)
{
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


static void print_profile_desc(void *value, void *userData)
{
	sdp_profile_desc_t *desc = (sdp_profile_desc_t *)value;
	char str[MAX_LEN_PROFILEDESCRIPTOR_UUID_STR];

	sdp_uuid2strn(&desc->uuid, UUID_str, MAX_LEN_UUID_STR);
	sdp_profile_uuid2strn(&desc->uuid, str, MAX_LEN_PROFILEDESCRIPTOR_UUID_STR);

	printf("  \"%s\" (0x%s)\n", str, UUID_str);
	if (desc->version)
		printf("    Version: 0x%04x\n", desc->version);
}

static void print_lang_attr(void *value, void *user)
{
	sdp_lang_attr_t *lang = (sdp_lang_attr_t *)value;
	printf("  code_ISO639: 0x%02x\n", lang->code_ISO639);
	printf("  encoding:    0x%02x\n", lang->encoding);
	printf("  base_offset: 0x%02x\n", lang->base_offset);
}

static void print_access_protos(void *value, void *userData)
{
	sdp_list_t *protDescSeq = (sdp_list_t *)value;
	sdp_list_foreach(protDescSeq, print_service_desc, 0);
}

static void print_service_class(void *value, void *userData)
{
	char ServiceClassUUID_str[MAX_LEN_SERVICECLASS_UUID_STR];
	uuid_t *uuid = (uuid_t *)value;

	sdp_uuid2strn(uuid, UUID_str, MAX_LEN_UUID_STR);
	sdp_svclass_uuid2strn(uuid, ServiceClassUUID_str, MAX_LEN_SERVICECLASS_UUID_STR);
	if (uuid->type != SDP_UUID128)
		printf("  \"%s\" (0x%s)\n", ServiceClassUUID_str, UUID_str);
	else
		printf("  UUID 128: %s\n", UUID_str);
}


static void print_service_attr(sdp_record_t *rec)
{
	sdp_list_t *list = 0, *proto = 0;
	printf("debug print_service_attr\n");
	sdp_record_print(rec);

	printf("Service RecHandle: 0x%x\n", rec->handle);

	if (sdp_get_service_classes(rec, &list) == 0) {
		printf("Service Class ID List:\n");
		sdp_list_foreach(list, print_service_class, 0);
		sdp_list_free(list, free);
	}
	if (sdp_get_access_protos(rec, &proto) == 0) {
		printf("Protocol Descriptor List:\n");
		sdp_list_foreach(proto, print_access_protos, 0);
		sdp_list_foreach(proto, (sdp_list_func_t)sdp_list_free, 0);
		sdp_list_free(proto, 0);
	}
	if (sdp_get_lang_attr(rec, &list) == 0) {
		printf("Language Base Attr List:\n");
		sdp_list_foreach(list, print_lang_attr, 0);
		sdp_list_free(list, free);
	}
	if (sdp_get_profile_descs(rec, &list) == 0) {
		printf("Profile Descriptor List:\n");
		sdp_list_foreach(list, print_profile_desc, 0);
		sdp_list_free(list, free);
	}
}




//按照服务和属性查询
//bt_addr:要查询的蓝牙的地址
//uuid:要哦查询的蓝牙服务或者协议的UUID,这个UUID必须是sdp.h中宏定义的_UUID或_ID
//attr_data:要查询的属性列表
//attr_size:要查询的属性列表的长度
int bluet_quere_profile_attr(const char *bt_addr, uint16_t uuid, struct SdpAttrValue *attr_data, size_t attr_size) 
{
	bdaddr_t target;
	if (str2ba(bt_addr, &target) != 0) {
		perror("Invalid Bluetooth address");
		return -1;
	}

	sdp_session_t *session = sdp_connect(BDADDR_ANY, &target, SDP_RETRY_IF_BUSY);
	if (!session) {
		perror("SDP connection failed");
		return -1;
	}

	//要查询的服务UUID
	uuid_t sdp_uuid;
	sdp_uuid16_create(&sdp_uuid, uuid);
	sdp_list_t *search_list = sdp_list_append(NULL, &sdp_uuid);
	
	//要查询的服务属性
	sdp_list_t *attrid_list = NULL;
	for (size_t i = 0; i < attr_size; i++) {
		uint32_t *p = malloc(sizeof(uint32_t));
		*p = attr_data[i].attr;
		attrid_list = sdp_list_append(attrid_list, p);
	}

	//查询结果
	sdp_list_t *search_result = NULL;
	if (sdp_service_search_attr_req(session, search_list, SDP_ATTR_REQ_RANGE, attrid_list, &search_result) != 0) {
		perror("SDP query failed");
		sdp_list_free(search_list, free);
		sdp_list_free(attrid_list, free);
		sdp_close(session);
		return -1;
	}

	sdp_record_t *record;
	for (sdp_list_t *r = search_result; r; r = r->next) {
		record = (sdp_record_t *)r->data;
		print_service_attr(record);
		sdp_record_free(record);
	}

	sdp_list_free(search_list, free);
	sdp_list_free(attrid_list, free);
	sdp_list_free(search_result, NULL);
	sdp_close(session);
	return 0;
}


//解析查询结果

#define RFCOMM_UUID 0x0003  // RFCOMM 协议 UUID

int get_obex_channel(const char *bt_addr) {
    bdaddr_t target;
    str2ba(bt_addr, &target);

    // 建立 SDP 会话
    sdp_session_t *session = sdp_connect(BDADDR_ANY, &target, SDP_RETRY_IF_BUSY);
    if (!session) {
        perror("SDP 连接失败");
        return -1;
    }

    // 创建 OBEX Object Push 的 UUID
    uuid_t obex_uuid;
    sdp_uuid16_create(&obex_uuid, OBEX_OBJPUSH_SVCLASS_ID);

    // 创建搜索列表
    sdp_list_t *search_list = sdp_list_append(NULL, &obex_uuid);
    uint32_t attr = SDP_ATTR_PROTO_DESC_LIST;
    sdp_list_t *attrid_list = sdp_list_append(NULL, &attr);
    sdp_list_t *search_result = NULL;

    // 查询服务
    if (sdp_service_search_attr_req(session, search_list, SDP_ATTR_REQ_RANGE, attrid_list, &search_result) != 0) {
        perror("SDP 查询失败");
        sdp_list_free(search_list, NULL);
        sdp_list_free(attrid_list, NULL);
        sdp_close(session);
        return -1;
    }

    int channel = -1;

    // 解析查询结果
    for (sdp_list_t *r = search_result; r; r = r->next) {
        sdp_record_t *record = (sdp_record_t *)r->data;
        sdp_list_t *proto_list;

        if (sdp_get_access_protos(record, &proto_list) == 0) 
		{
			printf("Protocol Descriptor List:\n");
		    for (sdp_list_t *p = proto_list; p; p = p->next) 
			{
                sdp_list_t *pds = (sdp_list_t *)p->data;

                // 遍历协议描述符列表
                for (sdp_list_t *pd = pds; pd; pd = pd->next) {
                    sdp_data_t *data = (sdp_data_t *)pd->data;

                    // 打印出所有的属性类型，帮助调试
                    printf("data->dtd = 0x%x\n", data->dtd);
                    printf("UUID: 0x%x\n", data->val.uuid.value.uuid16);

                    // 检查是否为 RFCOMM 协议（UUID 0x0003）
                    if (data->dtd == SDP_UUID16 && data->val.uuid.value.uuid16 == RFCOMM_UUID) {
                        // 找到 RFCOMM 协议，尝试获取端口号
                        sdp_data_t *sub_data = (sdp_data_t *)pd->next->data;  // RFCOMM 描述符紧随其后

                        // 增加检查数据类型的条件
                        if (sub_data) {
                            if (sub_data->dtd == SDP_UINT8 || sub_data->dtd == SDP_UINT16) {
                                channel = (sub_data->dtd == SDP_UINT8) ? sub_data->val.uint8 : sub_data->val.uint16;
                                printf("Found RFCOMM channel: %d\n", channel);
                                break;
                            }
                        } else {
                            printf("No sub_data found\n");
                        }
                    }
                }
                if (channel > 0) break;
            }
            sdp_list_free(proto_list, NULL);
        }
        if (channel > 0) break;
    }

    // 释放资源
    sdp_list_free(search_list, NULL);
    sdp_list_free(attrid_list, NULL);
    sdp_list_free(search_result, NULL);
    sdp_close(session);

    if (channel < 0) {
        printf("没有找到 OBEX RFCOMM 端口号\n");
    }
    return channel;
}



