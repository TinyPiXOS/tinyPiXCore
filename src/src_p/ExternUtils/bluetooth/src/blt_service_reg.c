#include "bluetooth_inc.h"  // 包含您提供的ProfileManager头文件
#include <glib.h>
#include "blt_service_reg.h"

// 服务注册上下文
typedef struct {
	guint handle;           // 服务句柄
//    gchar *service_id;      // 服务唯一标识
    gchar *profile_path;    // Profile对象路径
    gchar *uuid;            // 服务UUID
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
    g_free(reg->uuid);
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
guint register_bluetooth_service( 
                                   const gchar *uuid,
                                   const gchar *name,
                                   guint channel,
                                   const gchar *role)
{
    g_return_val_if_fail(uuid != NULL, FALSE);
    g_return_val_if_fail(role != NULL, FALSE);
    
    // 创建ProfileManager实例
    ProfileManager *manager = profile_manager_new();
    if (!manager) {
        g_critical("Failed to create ProfileManager instance");
        return -1;
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
    
//    GVariant *options = g_variant_builder_end(&options_builder);
    printf("register_bluetooth_service 1\n");
    // 创建Profile对象路径
	guint service_handle = next_service_handle++;
    gchar *profile_path = g_strdup_printf("/org/bluez/profile/%u", service_handle);
     printf("register_bluetooth_service 2\n");
    // 注册服务
    GError *error = NULL;
    profile_manager_proxy_register_profile(manager, profile_path, uuid, g_variant_builder_end(&options_builder), &error);
    
    if (error) {
        g_critical("Failed to register service: %s", error->message);
        g_error_free(error);
        g_object_unref(manager);
        g_free(profile_path);
//        g_variant_unref(options);
        return -1;
    }
 printf("register_bluetooth_service 3\n");
    // 保存注册信息
    ServiceRegistration *reg = g_new0(ServiceRegistration, 1);
    reg->handle = service_handle;
    reg->profile_path = profile_path;
    reg->uuid = g_strdup(uuid);
    reg->options = NULL;
	 printf("register_bluetooth_service 4\n");
    g_hash_table_insert(registered_services, GUINT_TO_POINTER(service_handle), reg);
	 printf("register_bluetooth_service 5\n");
    g_object_unref(manager);
	printf("ok\n");
    return service_handle;
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
    ServiceRegistration *reg = g_hash_table_lookup(registered_services, GUINT_TO_POINTER(service_handle));
    if (!reg) {
        g_warning("Service '%d' not found", service_handle);
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
 * @param uuid [out] 服务UUID
 * @param name [out] 服务名称
 * 
 * @return gboolean 是否成功获取
 */
gboolean get_service_info(guint service_handle, gchar **uuid, gchar **name)
{
    ServiceRegistration *reg = g_hash_table_lookup(registered_services, 
                                                 GUINT_TO_POINTER(service_handle));
    if (!reg) {
        return FALSE;
    }
    
    if (uuid) *uuid = g_strdup(reg->uuid);
    if (name) {
        // 需要从BlueZ获取实际的服务名称
        // 这里简化处理，实际应用中需要查询服务属性
        *name = g_strdup("Service Name");
    }
    return TRUE;
}
