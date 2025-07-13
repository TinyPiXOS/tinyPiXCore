/*///------------------------------------------------------------------------------------------------------------------------//
		sdp注册(bluez接口)
说 明 : 为上层代码提供接口，对bluez中sdp注册相关的封装
日 期 : 2025.4.15

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdlib.h>
#include <stdio.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>


typedef struct {
    const char *name;
    uint16_t uuid;
} proto_map_t;

proto_map_t proto_maps[] = {
    { "l2cap",  L2CAP_UUID },      // 0x0100
    { "rfcomm",  RFCOMM_UUID },     // 0x0003
    { "obex",    0x0008 },          // OBEX UUID，一般用0x0008表示OBEX层
    { "avdtp",   0x0019 },          // AVDTP
    { NULL, 0 }
};

// 根据协议名称查找 UUID
static uint16_t lookup_proto_uuid(const char *proto) {
    for (int i = 0; proto_maps[i].name != NULL; i++) {
        if (strcasecmp(proto, proto_maps[i].name) == 0) {
            return proto_maps[i].uuid;
        }
    }
    return 0;
}

// 构造一个单独的协议描述符列表项（即一层协议），返回一个 sdp_list_t* 对象，
// 若协议是 rfcomm 则附加通道号（channel）；其他协议则只添加 UUID。
sdp_list_t *build_protocol_layer(const char *proto, uint8_t rfcomm_channel) {
    uuid_t *uuid = malloc(sizeof(uuid_t));
    if (!uuid) return NULL;

    uint16_t uuid_val = lookup_proto_uuid(proto);
    if (uuid_val == 0) {
        fprintf(stderr, "Unsupported protocol: %s\n", proto);
        free(uuid);
        return NULL;
    }

    sdp_uuid16_create(uuid, uuid_val);
    sdp_list_t *inner_list = sdp_list_append(NULL, uuid);
    if (!inner_list) {
        free(uuid);
        return NULL;
    }

    if (strcasecmp(proto, "rfcomm") == 0) {
        // 使用堆内存保存通道号，避免使用局部变量地址
        uint8_t *ch_ptr = malloc(sizeof(uint8_t));
        if (!ch_ptr) {
            sdp_list_free(inner_list, free);
            return NULL;
        }
        *ch_ptr = rfcomm_channel;
        sdp_data_t *channel = sdp_data_alloc(SDP_UINT8, ch_ptr);
        if (!channel) {
            free(ch_ptr);
            sdp_list_free(inner_list, free);
            return NULL;
        }
        inner_list = sdp_list_append(inner_list, channel);
    }

    return inner_list;
}

// 根据逗号分隔的协议链构造一个协议描述符链表，返回一个 sdp_list_t* 对象。
// 该对象代表一个完整的 Protocol Descriptor List（单个列表项），后续将作为访问协议设置到记录中。
sdp_list_t *build_protocol_descriptor_list(const char *proto_chain, uint8_t rfcomm_channel) {
    char *chain_copy = strdup(proto_chain);
    if (!chain_copy) {
        perror("strdup");
        return NULL;
    }

    sdp_list_t *proto = NULL;  // 用于保存每一层的 inner_list
    char *token = strtok(chain_copy, ",");
    while (token) {
        // 去除前后空白字符
        while (*token == ' ' || *token == '\t')
            token++;
        char *end = token + strlen(token) - 1;
        while (end > token && (*end == ' ' || *end == '\t')) { 
            *end = '\0'; 
            end--;
        }
        sdp_list_t *layer = build_protocol_layer(token, rfcomm_channel);
        if (layer) {
            proto = sdp_list_append(proto, layer);
        } else {
            fprintf(stderr, "Failed to build protocol layer for %s\n", token);
        }
        token = strtok(NULL, ",");
    }
    free(chain_copy);

    if (!proto) {
        fprintf(stderr, "No valid protocol layers found\n");
        return NULL;
    }

    // 将整个 proto 链表包装到一个新的链表中，作为 Access Protocol Descriptor List
    sdp_list_t *access_proto = sdp_list_append(NULL, proto);
    return access_proto;
}

sdp_list_t *build_protocol_descriptor_list_2(const char *proto_chain, uint8_t rfcomm_channel) {
	sdp_list_t *access_proto=NULL, *proto=NULL;
    // 创建 L2CAP UUID
    uuid_t *l2cap_uuid = malloc(sizeof(uuid_t));
    sdp_uuid16_create(l2cap_uuid, L2CAP_UUID);
    sdp_list_t *l2cap_list = sdp_list_append(NULL, l2cap_uuid);
	proto=sdp_list_append(proto, l2cap_list);

    // 创建 RFCOMM UUID
    uuid_t *rfcomm_uuid = malloc(sizeof(uuid_t));
    sdp_uuid16_create(rfcomm_uuid, RFCOMM_UUID);
    sdp_list_t *rfcomm_list = sdp_list_append(NULL, rfcomm_uuid);

    // 添加通道号
    uint8_t channel;
    channel = rfcomm_channel;
    sdp_data_t *channel_data = sdp_data_alloc(SDP_UINT8, &channel);
    if (!channel_data) {
        sdp_list_free(l2cap_list, free);
        sdp_list_free(rfcomm_list, free);
        return NULL;
    }
    rfcomm_list = sdp_list_append(rfcomm_list, channel_data);
	proto=sdp_list_append(proto, rfcomm_list);

    // 构建 Access Protocols 列表
	access_proto=sdp_list_append(NULL, proto);

    return access_proto;
}


//sdp注册的连接
sdp_session_t *bluet_sdp_connect()
{
	sdp_session_t *session = sdp_connect(BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY);//BDADDR_ANY,BDADDR_LOCAL,SDP_RETRY_IF_BUSY
	if (!session) {
		fprintf(stderr, "Failed to connect to local SDP database\n");
		return NULL;
	}
	return session;
}

//sdp断开连接
void bluet_sdp_close(sdp_session_t *session)
{
	sdp_close(session);
}


void debug_print_access_proto(sdp_list_t *access_proto) {
	printf("Access Proto链表: \n");
	for (sdp_list_t *l = access_proto; l; l = l->next) {
		printf("  协议层: ");
		sdp_list_t *layer = (sdp_list_t *)l->data;
		for (sdp_list_t *p = layer; p; p = p->next) {
			printf("[%p] ", p->data);
		}
		printf("\n");
	}
}


// 注册 SDP 服务记录，支持通用协议链，服务 UUID 使用 16 位表示
// service_name: 服务名称
// svc_uuid_val: 服务 UUID（16 位），如 0x1105 表示 OBEX Object Push
// proto_chain: 协议链，如 "l2cap,rfcomm,obex" 或 "l2cap,avdtp"
// rfcomm_channel: 如果协议链中包含 rfcomm，此参数指定通道号；否则可设为 0
sdp_record_t *bluet_register_sdp_service(sdp_session_t *session,
										const char *service_name, uint16_t svc_uuid_val, const char *prov, const char *desc,
                                    	const char *proto_chain, uint8_t rfcomm_channel) {
	sdp_list_t *access_proto=NULL;
	if(!session)
		return NULL;
    sdp_record_t *record = sdp_record_alloc();
    if (!record) {
        fprintf(stderr, "Failed to allocate SDP record\n");
        return NULL;
    }

    // 设置服务 UUID
    uuid_t svc_uuid;
    sdp_uuid16_create(&svc_uuid, svc_uuid_val);
    sdp_set_service_id(record, svc_uuid);
	access_proto=sdp_list_append(NULL, &svc_uuid);
	sdp_set_service_classes(record, access_proto);
	sdp_list_free(access_proto, NULL);

    // 设置服务信息
    sdp_set_info_attr(record, service_name, NULL, NULL);	//service_name,prov,desc

    // 构造 Access Protocols（协议描述符列表）
	access_proto = build_protocol_descriptor_list(proto_chain, rfcomm_channel);
    if (!access_proto) {
        fprintf(stderr, "Failed to build protocol descriptor list\n");
        sdp_record_free(record);
        return NULL;
    }

	debug_print_access_proto(access_proto);
    sdp_set_access_protos(record, access_proto);

    // 设置 Browse Group（公共浏览组）
    uuid_t root_uuid;
    sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
    sdp_list_t *browse_list = sdp_list_append(NULL, &root_uuid);
    sdp_set_browse_groups(record, browse_list);
   

    int ret = sdp_record_register(session, record, 0);
    if (ret < 0) {
        fprintf(stderr, "Failed to register SDP record: %d\n", ret);
        sdp_record_free(record);
        sdp_list_free(browse_list, 0);
        return NULL;
    }

    printf("SDP service \"%s\" registered with UUID 0x%04X on protocol chain \"%s\"", service_name, svc_uuid_val, proto_chain);
    if (strstr(proto_chain, "rfcomm") != NULL) {
        printf(" (channel %d)", rfcomm_channel);
    }

    // 释放 record 及其内部资源后，SDP 数据库会复制记录；因此可以释放我们的 record 及相关链表
//    sdp_record_free(record);		//如果需要注销就不能释放
    sdp_list_free(browse_list, 0);
    // 注意：access_proto 列表中的内容已经被 sdp_record_register 内部拷贝，可以释放
    sdp_list_free(access_proto, 0);

    return record;
}


void bluet_unregister_sdp_service(sdp_session_t *session,sdp_record_t *record) 
{
	if (session && record) {
		int ret = sdp_record_unregister(session, record);
		if (ret < 0) {
			fprintf(stderr, "Failed to unregister SDP record\n");
		} else {
			printf("SDP service unregistered successfully.\n");
		}
		// 释放 record
		sdp_record_free(record);
		record = NULL;
	}
}


