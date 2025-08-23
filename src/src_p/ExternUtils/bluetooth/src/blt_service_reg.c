#include "bluetooth_inc.h"  // 包含您提供的ProfileManager头文件
#include <glib.h>
#include "blt_service_reg.h"

// 服务注册上下文
typedef struct {
	guint handle;           // 服务句柄
//    gchar *service_id;      // 服务唯一标识
    gchar *profile_path;    // Profile对象路径
    gchar **uuids;            // 服务UUID
	guint num_uuids;
    GVariant *options;      // 服务选项
} ServiceRegistration;

// 全局服务注册表
static GHashTable *registered_services = NULL;
static guint next_service_handle = 1;  // 下一个可用的服务句柄


/**
 * @brief 清理服务注册系统
 */
void service_registry_cleanup()
{
	if (registered_services) {
        // 遍历并注销所有服务
        GHashTableIter iter;
        gpointer key, value;
        g_hash_table_iter_init(&iter, registered_services);
        while (g_hash_table_iter_next(&iter, &key, &value)) {
            ServiceRegistration *reg = value;
            unregister_bluetooth_service(reg->handle);
        }

        g_hash_table_destroy(registered_services);
        registered_services = NULL;
    }
}

void service_registration_free(gpointer data)
{
	ServiceRegistration *reg = (ServiceRegistration *)data;
    if (!reg) return;

    g_free(reg->profile_path);

    for (guint i = 0; i < reg->num_uuids; i++) {
        g_free(reg->uuids[i]);
    }
    g_free(reg->uuids);
    
    if (reg->options) 
        g_variant_unref(reg->options);
    
    g_free(reg);
}

/**
 * @brief 初始化服务注册系统
 */
void service_registry_init()
{
	if (!registered_services) {
		registered_services = g_hash_table_new_full(g_direct_hash, g_direct_equal, 
													NULL, service_registration_free);
		next_service_handle = 1;
	}
}



/**
 * @brief 注册新的蓝牙服务
 * 
 * @param uuid 服务UUID
 * @param name 服务名称（对应SDP扫描结果中的Service Name）
 * @param channel RFCOMM通道号(0表示不使用)
 * @param role "client"或"server"或"sink"或"source"
 * 
 * @return gboolean 注册是否成功
 */

guint register_multi_uuid_service(const gchar **uuids, 
                                 guint num_uuids,
                                 const gchar *name,
                                 guint channel,
                                 const gchar *role)
{
    g_return_val_if_fail(uuids != NULL, 0);
    g_return_val_if_fail(num_uuids > 0, 0);
    g_return_val_if_fail(role != NULL, 0);
    
    // 创建ProfileManager实例
    ProfileManager *manager = profile_manager_new();
    if (!manager) {
        g_critical("Failed to create ProfileManager instance");
        return 0;
    }
    
    // 构建服务选项
    GVariantBuilder options_builder;
    g_variant_builder_init(&options_builder, G_VARIANT_TYPE_VARDICT);
    
    // 添加服务名称
    if (name) {
        g_variant_builder_add(&options_builder, "{sv}", 
                             "Name", g_variant_new_string(name));
    }
    
    // 添加RFCOMM通道
    if (channel > 0) {
        g_variant_builder_add(&options_builder, "{sv}", 
                             "Channel", g_variant_new_uint16(channel));
    }
    
    // 添加角色
    g_variant_builder_add(&options_builder, "{sv}", 
                         "Role", g_variant_new_string(role));
    
    // 添加服务类ID列表（多UUID）
    GVariantBuilder uuid_list_builder;
    g_variant_builder_init(&uuid_list_builder, G_VARIANT_TYPE("as"));
    
    for (guint i = 0; i < num_uuids; i++) {
        g_variant_builder_add(&uuid_list_builder, "s", uuids[i]);
    }
    
    g_variant_builder_add(&options_builder, "{sv}", 
                         "ServiceClassIDList", 
                         g_variant_builder_end(&uuid_list_builder));
    
    // 生成服务句柄
    guint service_handle = next_service_handle++;
    
    // 创建Profile对象路径
    gchar *profile_path = g_strdup_printf("/org/bluez/profile/%u", service_handle);
    
    // 使用第一个UUID作为主服务类
    const gchar *primary_uuid = uuids[0];
    
    // 注册服务
    GError *error = NULL;
    profile_manager_proxy_register_profile(manager, profile_path, primary_uuid, 
                                         g_variant_builder_end(&options_builder),
                                         &error);
    
    if (error) {
        g_critical("Failed to register service: %s", error->message);
        g_error_free(error);
        g_object_unref(manager);
        g_free(profile_path);
        return 0;
    }
    
    // 保存注册信息
    ServiceRegistration *reg = g_new0(ServiceRegistration, 1);
    reg->handle = service_handle;
    reg->profile_path = profile_path;
    
    // 复制UUID数组
    reg->uuids = g_new(gchar *, num_uuids);
    for (guint i = 0; i < num_uuids; i++) {
        reg->uuids[i] = g_strdup(uuids[i]);
    }
    reg->num_uuids = num_uuids;
    reg->options = NULL; // 不需要保存选项
    
    g_hash_table_insert(registered_services, GUINT_TO_POINTER(service_handle), reg);
    g_object_unref(manager);
    
    return service_handle;
}
/**
 * @brief 注册新的蓝牙服务（单个UUID）
 * 
 * @param uuid 服务UUID
 * @param name 服务名称
 * @param channel RFCOMM通道号(0表示不使用)
 * @param role "client"或"server"或"sink"或"source"
 * 
 * @return guint 服务句柄(0表示失败)
 */
guint register_bluetooth_service(const gchar *uuid,
                                const gchar *name,
                                guint channel,
                                const gchar *role)
{
    // 使用多UUID注册函数，但只传入一个UUID
    const gchar *uuids[] = {uuid};
    return register_multi_uuid_service(uuids, 1, name, channel, role);
}


/**
 * @brief 注销蓝牙服务
 * 
 * @param service_handle 服务唯一标识
 * 
 * @return gboolean 注销是否成功
 */
gboolean unregister_bluetooth_service(guint service_handle)
{   
 // 查找服务注册信息
    ServiceRegistration *reg = g_hash_table_lookup(registered_services, 
                                                 GUINT_TO_POINTER(service_handle));
    if (!reg) {
        g_warning("Service handle %u not found", service_handle);
        return FALSE;
    }
    
    // 创建ProfileManager实例
    ProfileManager *manager = profile_manager_new();
    if (!manager) {
        g_critical("Failed to create ProfileManager instance");
        return FALSE;
    }
    
    // 注销服务
    GError *error = NULL;
    profile_manager_proxy_unregister_profile(manager, reg->profile_path, &error);
    
    if (error) {
        g_critical("Failed to unregister service: %s", error->message);
        g_error_free(error);
        g_object_unref(manager);
        return FALSE;
    }
    
    // 从注册表中移除
    g_hash_table_remove(registered_services, GUINT_TO_POINTER(service_handle));
    g_object_unref(manager);
    return TRUE;
}

/**
 * @brief 获取服务信息
 * 
 * @param service_handle 服务句柄
 * @param uuids [out] UUID数组
 * @param num_uuids [out] UUID数量
 * @param name [out] 服务名称
 * 
 * @return gboolean 是否成功获取
 */
gboolean get_service_info(guint service_handle, 
                         gchar ***uuids, 
                         guint *num_uuids,
                         gchar **name)
{
    ServiceRegistration *reg = g_hash_table_lookup(registered_services, 
                                                 GUINT_TO_POINTER(service_handle));
    if (!reg) {
        return FALSE;
    }
    
    if (uuids && num_uuids) {
        // 复制UUID数组
        *uuids = g_new(gchar *, reg->num_uuids);
        for (guint i = 0; i < reg->num_uuids; i++) {
            (*uuids)[i] = g_strdup(reg->uuids[i]);
        }
        *num_uuids = reg->num_uuids;
    }
    
    if (name) {
        // 需要从BlueZ获取实际的服务名称
        // 这里简化处理，实际应用中需要查询服务属性
        *name = g_strdup("Service Name");
    }
    return TRUE;
}