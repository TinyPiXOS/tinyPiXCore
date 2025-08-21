#include "bluetooth_inc.h"  // 包含您提供的ProfileManager头文件
#include <glib.h>
#include "blt_service_reg.h"

// 服务注册上下文
typedef struct {
    gchar *service_id;      // 服务唯一标识
    gchar *profile_path;    // Profile对象路径
    gchar *uuid;            // 服务UUID
    GVariant *options;      // 服务选项
} ServiceRegistration;

// 全局服务注册表
static GHashTable *registered_services = NULL;

/**
 * @brief 初始化服务注册系统
 */
void service_registry_init()
{
    if (!registered_services) {
        registered_services = g_hash_table_new_full(g_str_hash, g_str_equal, 
                                                    g_free, g_free);
    }
}

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
            unregister_bluetooth_service(reg->service_id);
        }

        g_hash_table_destroy(registered_services);
        registered_services = NULL;
    }
}

void service_registration_free(ServiceRegistration *reg)
{
    if (!reg) return;

    g_free(reg->service_id);
    g_free(reg->profile_path);
    g_free(reg->uuid);
    if (reg->options) g_variant_unref(reg->options);
    g_free(reg);
}


/**
 * @brief 注册新的蓝牙服务
 * 
 * @param service_id 服务唯一标识
 * @param uuid 服务UUID
 * @param name 服务名称
 * @param channel RFCOMM通道号(0表示不使用)
 * @param role "client"或"server"
 * 
 * @return gboolean 注册是否成功
 */
gboolean register_bluetooth_service(const gchar *service_id, 
                                   const gchar *uuid,
                                   const gchar *name,
                                   guint channel,
                                   const gchar *role)
{
    g_return_val_if_fail(service_id != NULL, FALSE);
    g_return_val_if_fail(uuid != NULL, FALSE);
    g_return_val_if_fail(role != NULL, FALSE);
    
    // 创建ProfileManager实例
    ProfileManager *manager = profile_manager_new();
    if (!manager) {
        g_critical("Failed to create ProfileManager instance");
        return FALSE;
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
    
    GVariant *options = g_variant_builder_end(&options_builder);
    
    // 创建Profile对象路径
    gchar *profile_path = g_strdup_printf("/org/bluez/profile/%s", service_id);
    
    // 注册服务
    GError *error = NULL;
    profile_manager_proxy_register_profile(manager, profile_path, uuid, options, &error);
    
    if (error) {
        g_critical("Failed to register service: %s", error->message);
        g_error_free(error);
        g_object_unref(manager);
        g_free(profile_path);
        g_variant_unref(options);
        return FALSE;
    }
    
    // 保存注册信息
    ServiceRegistration *reg = g_new0(ServiceRegistration, 1);
    reg->service_id = g_strdup(service_id);
    reg->profile_path = profile_path;
    reg->uuid = g_strdup(uuid);
    reg->options = g_variant_ref(options);
    
    g_hash_table_insert(registered_services, g_strdup(service_id), reg);
    
    g_object_unref(manager);
    g_variant_unref(options);
    return TRUE;
}

/**
 * @brief 注销蓝牙服务
 * 
 * @param service_id 服务唯一标识
 * 
 * @return gboolean 注销是否成功
 */
gboolean unregister_bluetooth_service(const gchar *service_id)
{
    g_return_val_if_fail(service_id != NULL, FALSE);
    
    // 查找服务注册信息
    ServiceRegistration *reg = g_hash_table_lookup(registered_services, service_id);
    if (!reg) {
        g_warning("Service '%s' not found", service_id);
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
    g_hash_table_remove(registered_services, service_id);
    g_object_unref(manager);
    return TRUE;
}

/**
 * @brief 获取所有已注册服务
 * 
 * @return GList* 服务ID列表(需使用g_list_free_full(list, g_free)释放)
 */
GList *get_registered_services()
{
    return g_hash_table_get_keys(registered_services);
}


