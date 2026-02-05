
#include <assert.h>
#include "Network/dhcpcd/DhcpcdControl.h"

// 工具函数

//去掉头尾空白字符
static char* trim_whitespace(char *str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = 0;
    return str;
}

static int file_exists(const char *path) {
    return access(path, F_OK) == 0;
}

static int is_valid_ipv4(const char *ip) {
    int a, b, c, d;
    return (sscanf(ip, "%d.%d.%d.%d", &a, &b, &c, &d) == 4 &&
            a >= 0 && a <= 255 && b >= 0 && b <= 255 &&
            c >= 0 && c <= 255 && d >= 0 && d <= 255);
}

static pid_t get_dhcpcd_pid(const char *ifname) {
    char pid_path[256];
    pid_t pid = -1;
    
    // 尝试接口特定的PID文件
    snprintf(pid_path, sizeof(pid_path), "/var/run/dhcpcd-%s.pid", ifname);
    FILE *fp = fopen(pid_path, "r");
    if (fp) {
        if (fscanf(fp, "%d", &pid) != 1) {
            pid = -1;
        }
        fclose(fp);
    }
    
    // 如果没找到，尝试通用PID文件
    if (pid <= 0) {
        fp = fopen("/var/run/dhcpcd.pid", "r");
        if (fp) {
            if (fscanf(fp, "%d", &pid) != 1) {
                pid = -1;
            }
            fclose(fp);
        }
    }
    
    // 验证进程是否存在
    if (pid > 0 && kill(pid, 0) != 0) {
        return -1;  // 进程不存在
    }
    
    return pid;
}

static int restart_dhcpcd(const char *ifname) {
    pid_t pid = get_dhcpcd_pid(ifname);
    
    if (pid > 0) {
        // 1. 先停止接口
        char stop_cmd[256];
        snprintf(stop_cmd, sizeof(stop_cmd), "ip link set %s down 2>/dev/null", ifname);
        system(stop_cmd);
        
        // 2. 发送SIGHUP信号重新加载配置
        if (kill(pid, SIGHUP) != 0) {
            return DHCPCD_ERROR;
        }
        
        // 3. 等待配置重新加载
        usleep(500000);  // 等待500ms
        
        // 4. 重新启动接口
        snprintf(stop_cmd, sizeof(stop_cmd), "ip link set %s up 2>/dev/null", ifname);
        system(stop_cmd);
        
        // 5. 等待网络完全就绪
        sleep(2);
        
        return DHCPCD_SUCCESS;
    } else {
        // dhcpcd没有运行，启动它
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "dhcpcd -B -w -4 -t 30 %s 2>/dev/null", ifname);
        int ret = system(cmd);
        return (WIFEXITED(ret) && WEXITSTATUS(ret) == 0) ? DHCPCD_SUCCESS : DHCPCD_ERROR;
    }
}

static int get_dhcpcd_config_path(char *path, size_t size) {
    // 尝试多个可能的配置文件位置
    const char *paths[] = {
        "/etc/dhcpcd.conf",
        "/etc/dhcpcd/dhcpcd.conf",
        "/etc/dhcpcd5/dhcpcd.conf",
        "/etc/dhcpcd/dhcpcd.conf",
        NULL
    };
    
    for (int i = 0; paths[i] != NULL; i++) {
        if (file_exists(paths[i])) {
            strncpy(path, paths[i], size - 1);
            path[size - 1] = '\0';
            return DHCPCD_SUCCESS;
        }
    }
    
    // 没有找到，使用默认路径
    strncpy(path, "/etc/dhcpcd.conf", size - 1);
    path[size - 1] = '\0';
    return DHCPCD_SUCCESS;
}

int dhcpcd_set_dynamic_dhcp(const char *ifname) {
    if (!ifname) return DHCPCD_ERROR;
    
    char config_path[256];
    if (get_dhcpcd_config_path(config_path, sizeof(config_path)) != DHCPCD_SUCCESS) {
        return DHCPCD_ERROR;
    }
    
    // 备份原配置
    char backup_path[260];

    snprintf(backup_path, sizeof(backup_path), "%s.bak", config_path);
    char backup_cmd[534];
    snprintf(backup_cmd, sizeof(backup_cmd), "cp -f %s %s 2>/dev/null", config_path, backup_path);
    system(backup_cmd);
    
    // 读取并修改配置文件
    FILE *in_fp = fopen(config_path, "r");
    if (!in_fp) {
        // 如果文件不存在，创建新配置
        FILE *out_fp = fopen(config_path, "w");
        if (!out_fp) return DHCPCD_ERROR;
        fprintf(out_fp, "interface %s\n", ifname);
        fclose(out_fp);
        
        return restart_dhcpcd(ifname);
    }
    
    FILE *out_fp = fopen("/tmp/dhcpcd.conf.tmp", "w");
    if (!out_fp) {
        fclose(in_fp);
        return DHCPCD_ERROR;
    }
    
    char line[512];
    int in_interface_block = 0;
    int found_interface = 0;
    int has_static_config = 0;
    
    while (fgets(line, sizeof(line), in_fp)) {
        char current_ifname[64];
        
        // 检查是否是接口声明
        if (sscanf(line, "interface %63s", current_ifname) == 1) {
            if (in_interface_block) {
                // 离开前一个接口块
                in_interface_block = 0;
            }
            
            if (strcmp(trim_whitespace(current_ifname), ifname) == 0) {
                in_interface_block = 1;
                found_interface = 1;
            }
        }
        
        if (in_interface_block) {
            // 在目标接口块中，跳过静态配置
            if (strstr(line, "static ip_address") ||
                strstr(line, "static routers") ||
                strstr(line, "static subnet_mask")) {
                has_static_config = 1;
                continue;  // 跳过这行
            }
            
            // 也跳过静态DNS，但保留动态DNS配置
            if (strstr(line, "static domain_name_servers")) {
                has_static_config = 1;
                continue;
            }
        }
        
        fputs(line, out_fp);
    }
    
    // 如果接口不存在，添加它
    if (!found_interface) {
        fprintf(out_fp, "\n# Auto-configured for dynamic DHCP\n");
        fprintf(out_fp, "interface %s\n", ifname);
    }
    
    fclose(in_fp);
    fclose(out_fp);
    
    // 替换原配置文件
    if (rename("/tmp/dhcpcd.conf.tmp", config_path) != 0) {
        return DHCPCD_ERROR;
    }
    
    // 重新加载配置
    return restart_dhcpcd(ifname);
}

int dhcpcd_set_static_ip(const char *ifname, 
                        const char *ip_cidr,
                        int subnet_mask,
                        const char *gateway) 
{
    if (!ifname || !ip_cidr) return DHCPCD_ERROR;
    
    char config_path[256];
    if (get_dhcpcd_config_path(config_path, sizeof(config_path)) != DHCPCD_SUCCESS) {
        return DHCPCD_ERROR;
    }
    
    // 备份原配置
    char backup_cmd[534];
    snprintf(backup_cmd, sizeof(backup_cmd), "cp -f %s %s.bak 2>/dev/null", 
             config_path, config_path);
    system(backup_cmd);
    
    // 读取并修改配置文件
    FILE *in_fp = fopen(config_path, "r");
    if (!in_fp) {
        // 文件不存在，创建新文件
        in_fp = fopen(config_path, "w");
        if (!in_fp) return DHCPCD_ERROR;
        fclose(in_fp);
        in_fp = fopen(config_path, "r");
        if (!in_fp) return DHCPCD_ERROR;
    }
    
    FILE *out_fp = fopen("/tmp/dhcpcd.conf.tmp", "w");
    if (!out_fp) {
        fclose(in_fp);
        return DHCPCD_ERROR;
    }
    
    char line[512];
    int in_interface_block = 0;
    int found_interface = 0;
    int has_written_config = 0;
    
    while (fgets(line, sizeof(line), in_fp)) {
        char current_ifname[64];
		char *original_line = strdup(line);  // 保存原始指针
        char *trimmed_line = trim_whitespace(original_line);
        
        // 检查是否是接口声明
        if (sscanf(trimmed_line, "interface %63s", current_ifname) == 1) {
            if (in_interface_block && !has_written_config) {
                // 离开前一个接口块前写入配置
                fprintf(out_fp, "static ip_address=%s\n", ip_cidr);
                
                // 注意：dhcpcd不需要单独设置subnet_mask，因为它可以从CIDR表示法推断
                // 但为了兼容性，我们保留这个参数
                if (subnet_mask > 0 && subnet_mask <= 32) {
                    fprintf(out_fp, "static subnet_mask=%d\n", subnet_mask);
                }
                
                if (gateway && strlen(gateway) > 0) {
                    fprintf(out_fp, "static routers=%s\n", gateway);
                }
                has_written_config = 1;
            }
            
            in_interface_block = 0;
            if (strcmp(trim_whitespace(current_ifname), ifname) == 0) {
                in_interface_block = 1;
                found_interface = 1;
                has_written_config = 0;
            }
        }
        
        if (in_interface_block) {
            // 跳过旧的IP、网关和子网掩码配置
            if (strstr(line, "static ip_address") ||
                strstr(line, "static routers") ||
                strstr(line, "static subnet_mask")) {
                free(original_line);
                continue;
            }
        }
        
        fputs(line, out_fp);
        free(original_line);
    }
    
    // 处理文件末尾
    if (in_interface_block && !has_written_config) {
        fprintf(out_fp, "static ip_address=%s\n", ip_cidr);
        
        if (subnet_mask > 0 && subnet_mask <= 32) {
            fprintf(out_fp, "static subnet_mask=%d\n", subnet_mask);
        }
        
        if (gateway && strlen(gateway) > 0) {
            fprintf(out_fp, "static routers=%s\n", gateway);
        }
        has_written_config = 1;
    }
    
    // 如果接口配置不存在，添加它
    if (!found_interface) {
        fprintf(out_fp, "\n# Auto-configured static IP for %s\n", ifname);
        fprintf(out_fp, "interface %s\n", ifname);
        fprintf(out_fp, "static ip_address=%s\n", ip_cidr);
        
        if (subnet_mask > 0 && subnet_mask <= 32) {
            fprintf(out_fp, "static subnet_mask=%d\n", subnet_mask);
        }
        
        if (gateway && strlen(gateway) > 0) {
            fprintf(out_fp, "static routers=%s\n", gateway);
        }
    }
    
    fclose(in_fp);
    fclose(out_fp);
    
    // 替换原配置文件
    if (rename("/tmp/dhcpcd.conf.tmp", config_path) != 0) {
        return DHCPCD_ERROR;
    }
    
    // 重新加载配置
    return restart_dhcpcd(ifname);
}


// DNS配置缓存结构
typedef struct {
    char ifname[16];
    int dns_count;
    char dns_servers[5][20];  // 最多保存5个DNS，每个最长20字符
    int is_valid;
} DnsCache;

#define MAX_CACHE_ENTRIES 10
static DnsCache dns_cache[MAX_CACHE_ENTRIES];
static int cache_initialized = 0;

// 初始化缓存
static void init_cache(void) {
    if (!cache_initialized) {
        memset(dns_cache, 0, sizeof(dns_cache));
        cache_initialized = 1;
    }
}

// 保存DNS配置到缓存
static void save_dns_to_cache(const char *ifname, const char **dns_list, int count) {
    init_cache();
    
    if (count <= 0 || count > 5) return;
    
    // 查找或创建缓存条目
    DnsCache *entry = NULL;
    for (int i = 0; i < MAX_CACHE_ENTRIES; i++) {
        if (!dns_cache[i].is_valid) {
            entry = &dns_cache[i];
            break;
        } else if (strcmp(dns_cache[i].ifname, ifname) == 0) {
            entry = &dns_cache[i];
            break;
        }
    }
    
    if (!entry) {
        // 缓存已满，覆盖第一个
        entry = &dns_cache[0];
    }
    
    // 保存配置
    strncpy(entry->ifname, ifname, sizeof(entry->ifname) - 1);
    entry->ifname[sizeof(entry->ifname) - 1] = '\0';
    entry->dns_count = count;
    
    for (int i = 0; i < count; i++) {
        strncpy(entry->dns_servers[i], dns_list[i], sizeof(entry->dns_servers[i]) - 1);
        entry->dns_servers[i][sizeof(entry->dns_servers[i]) - 1] = '\0';
    }
    
    entry->is_valid = 1;
}

// 从缓存读取DNS配置
static int get_dns_from_cache(const char *ifname, const char **dns_list_out) {
    init_cache();
    
    for (int i = 0; i < MAX_CACHE_ENTRIES; i++) {
        if (dns_cache[i].is_valid && strcmp(dns_cache[i].ifname, ifname) == 0) {
            for (int j = 0; j < dns_cache[i].dns_count; j++) {
                dns_list_out[j] = dns_cache[i].dns_servers[j];
            }
            return dns_cache[i].dns_count;
        }
    }
    
    return 0;  // 没有缓存
}

// 从配置文件中读取当前静态DNS配置
static int read_current_static_dns(const char *config_path, const char *ifname, 
                                   char **dns_list, int *dns_count) {
    FILE *fp = fopen(config_path, "r");
    if (!fp) return DHCPCD_ERROR;
    
    char line[512];
    int in_interface_block = 0;
    int found_dns = 0;
    
    *dns_count = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        char current_ifname[64];
        
        if (sscanf(line, "interface %63s", current_ifname) == 1) {
            in_interface_block = (strcmp(trim_whitespace(current_ifname), ifname) == 0);
        }
        
        if (in_interface_block) {
            if (strstr(line, "static domain_name_servers=")) {
                char *dns_str = strchr(line, '=');
                if (dns_str) {
                    dns_str++;  // 跳过'='
                    char *saveptr = NULL;
                    char *token = strtok_r(dns_str, " \t\n", &saveptr);
                    
                    while (token && *dns_count < 5) {
                        char *trimmed = trim_whitespace(token);
                        if (strlen(trimmed) > 0) {
                            dns_list[*dns_count] = strdup(trimmed);
                            (*dns_count)++;
                        }
                        token = strtok_r(NULL, " \t\n", &saveptr);
                    }
                    found_dns = 1;
                }
            }
        }
    }
    
    fclose(fp);
    return found_dns ? DHCPCD_SUCCESS : DHCPCD_ERROR;
}

int dhcpcd_set_static_dns(const char *ifname, 
                         const char **dns_list, 
                         uint32_t count) 
{
    if (!ifname) 
		return DHCPCD_ERROR;
    printf("[Debug]:dhcpcd_set_static_dns\n");
    // 如果dns_list为空，尝试从缓存读取
    const char *use_dns_list[5];
    int use_count = 0;
    
    if (dns_list == NULL || count == 0) {
        // 尝试从缓存读取
        use_count = get_dns_from_cache(ifname, use_dns_list);
        if (use_count == 0) {
            return DHCPCD_ERROR;  // 缓存中没有配置
        }
    } else {
        // 使用传入的配置
        for (uint32_t i = 0; i < count && i < 5; i++) {
            use_dns_list[i] = dns_list[i];
        }
        use_count = (count < 5) ? count : 5;
    }
    
    // 保存到缓存
    save_dns_to_cache(ifname, use_dns_list, use_count);
    
    char config_path[256];
    if (get_dhcpcd_config_path(config_path, sizeof(config_path)) != DHCPCD_SUCCESS) {
        return DHCPCD_ERROR;
    }
    
    FILE *in_fp = fopen(config_path, "r");
    if (!in_fp) {
        // 文件不存在，创建新文件
        in_fp = fopen(config_path, "w");
        if (!in_fp) return DHCPCD_ERROR;
        fclose(in_fp);
        in_fp = fopen(config_path, "r");
        if (!in_fp) return DHCPCD_ERROR;
    }
    
    FILE *out_fp = fopen("/tmp/dhcpcd.conf.tmp", "w");
    if (!out_fp) {
        fclose(in_fp);
        return DHCPCD_ERROR;
    }
    
    char line[512];
    int in_interface_block = 0;
    int found_interface = 0;
    int has_written_dns = 0;
    
    while (fgets(line, sizeof(line), in_fp)) 
    {
        char current_ifname[64];
        
        // 检查是否是接口声明
        if (sscanf(line, "interface %63s", current_ifname) == 1) 
        {
            if (in_interface_block && !has_written_dns) {
                // 离开接口块前写入DNS配置
                fprintf(out_fp, "static domain_name_servers=");
                for (int i = 0; i < use_count; i++) {
                    fprintf(out_fp, "%s%s", use_dns_list[i],
                            (i + 1 < use_count) ? " " : "");
                }
                fprintf(out_fp, "\n");
                has_written_dns = 1;
            }
            
            in_interface_block = 0;
            if (strcmp(trim_whitespace(current_ifname), ifname) == 0) {
                in_interface_block = 1;
                found_interface = 1;
                has_written_dns = 0;
            }
        }
        
        if (in_interface_block) {
            // 跳过旧的DNS配置
            if (strstr(line, "static domain_name_servers")) {
                continue;
            }
        }
        
        fputs(line, out_fp);
    }
    
    // 处理文件末尾
    if (in_interface_block && !has_written_dns) {
        fprintf(out_fp, "static domain_name_servers=");
        for (int i = 0; i < use_count; i++) {
            fprintf(out_fp, "%s%s", use_dns_list[i],
                    (i + 1 < use_count) ? " " : "");
        }
        fprintf(out_fp, "\n");
    }
    
    // 如果接口配置不存在，添加它
    if (!found_interface) {
        fprintf(out_fp, "\n# DNS configuration for %s\n", ifname);
        fprintf(out_fp, "interface %s\n", ifname);
        fprintf(out_fp, "static domain_name_servers=");
        for (int i = 0; i < use_count; i++) {
            fprintf(out_fp, "%s%s", use_dns_list[i],
                    (i + 1 < use_count) ? " " : "");
        }
        fprintf(out_fp, "\n");
    }
    
    fclose(in_fp);
    fclose(out_fp);
    
    // 替换原配置文件
    if (rename("/tmp/dhcpcd.conf.tmp", config_path) != 0) {
        return DHCPCD_ERROR;
    }
    
    // 重新加载配置
    return restart_dhcpcd(ifname);
}

int dhcpcd_set_dynamic_dns(const char *ifname) {
    if (!ifname) 
		return DHCPCD_ERROR;
    printf("[Debug]:dhcpcd_set_dynamic_dns\n");
    char config_path[256];
    if (get_dhcpcd_config_path(config_path, sizeof(config_path)) != DHCPCD_SUCCESS) {
        return DHCPCD_ERROR;
    }
    
    // 在删除之前，先读取当前的静态DNS配置并保存到缓存
    char *current_dns[5] = {NULL};
    int dns_count = 0;
    
    // 确保有文件存在
    if (file_exists(config_path)) {
        int result = read_current_static_dns(config_path, ifname, current_dns, &dns_count);
        
        if (result == DHCPCD_SUCCESS && dns_count > 0) {
            // 保存到缓存
            const char *dns_list[5];
            for (int i = 0; i < dns_count; i++) {
                dns_list[i] = current_dns[i];
            }
            save_dns_to_cache(ifname, dns_list, dns_count);
        }
    }
    
    // 清理内存
    for (int i = 0; i < dns_count; i++) {
        if (current_dns[i]) {
            free(current_dns[i]);
        }
    }
    
    FILE *in_fp = fopen(config_path, "r");
    if (!in_fp) {
        return DHCPCD_ERROR;
    }
    
    FILE *out_fp = fopen("/tmp/dhcpcd.conf.tmp", "w");
    if (!out_fp) {
        fclose(in_fp);
        return DHCPCD_ERROR;
    }
    
    char line[512];
    int in_interface_block = 0;
    
    while (fgets(line, sizeof(line), in_fp)) {
        char current_ifname[64];
        
        // 检查是否是接口声明
        if (sscanf(line, "interface %63s", current_ifname) == 1) {
            in_interface_block = (strcmp(trim_whitespace(current_ifname), ifname) == 0);
        }
        
        if (in_interface_block) {
            // 跳过静态DNS配置
            if (strstr(line, "static domain_name_servers")) {
                continue;
            }
        }
        
        fputs(line, out_fp);
    }
    
    fclose(in_fp);
    fclose(out_fp);
    
    // 替换原配置文件
    if (rename("/tmp/dhcpcd.conf.tmp", config_path) != 0) {
        return DHCPCD_ERROR;
    }
    
    // 重新加载配置
    return restart_dhcpcd(ifname);
}

int dhcpcd_get_dns_status(const char *ifname) {
    if (!ifname) return DNS_STATE_UNKNOWN;
    
    char config_path[256];
    if (get_dhcpcd_config_path(config_path, sizeof(config_path)) != DHCPCD_SUCCESS) {
        return DNS_STATE_UNKNOWN;
    }
    
    FILE *fp = fopen(config_path, "r");
    if (!fp) {
        return DNS_STATE_UNKNOWN;
    }
    
    char line[512];
    int in_interface_block = 0;
    int has_static_dns = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        char current_ifname[64];
        
        if (sscanf(line, "interface %63s", current_ifname) == 1) {
            in_interface_block = (strcmp(trim_whitespace(current_ifname), ifname) == 0);
        }
        
        if (in_interface_block) {
            if (strstr(line, "static domain_name_servers")) {
                has_static_dns = 1;
                break;
            }
        }
    }
    
    fclose(fp);
    
    return has_static_dns ? DNS_STATE_STATIC : DNS_STATE_DYNAMIC;
}
int dhcpcd_get_dns_list(const char *ifname, char ***dns_list_out) {
    if (!ifname || !dns_list_out) return DHCPCD_ERROR;
    *dns_list_out = NULL;
    
    // 先尝试从租约文件读取
    char lease_path[256];
    snprintf(lease_path, sizeof(lease_path), 
             "/var/lib/dhcpcd/dhcpcd-%s.lease", ifname);
    
    char dns_line[512] = "";
    
    if (file_exists(lease_path)) {
        FILE *fp = fopen(lease_path, "r");
        if (fp) {
            char line[512];
            while (fgets(line, sizeof(line), fp)) {
                if (strstr(line, "option domain-name-servers")) {
                    char *start = strchr(line, '=');
                    if (start) {
                        char *end = strchr(start, ';');
                        if (end) *end = '\0';
                        
                        // 复制并去除可能的单引号
                        char *ptr = start + 1;  // 跳过'='
                        int i = 0;
                        while (*ptr && i < sizeof(dns_line) - 1) {
                            if (*ptr != '\'' && *ptr != '"') {  // 跳过单引号和双引号
                                dns_line[i++] = *ptr;
                            }
                            ptr++;
                        }
                        dns_line[i] = '\0';
                        break;
                    }
                }
            }
            fclose(fp);
        }
    }
    
    // 如果租约文件没有，尝试从dhcpcd命令获取
    if (strlen(dns_line) == 0) {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "dhcpcd -U %s 2>/dev/null", ifname);
        
        FILE *fp = popen(cmd, "r");
        if (fp) {
            char output[1024];
            while (fgets(output, sizeof(output), fp)) {
                char *dns_start = strstr(output, "domain_name_servers=");
                if (dns_start) {
                    dns_start += strlen("domain_name_servers=");
                    char *dns_end = strchr(dns_start, '\n');
                    if (dns_end) *dns_end = '\0';
                    
                    // 去除可能的单引号
                    char *ptr = dns_start;
                    int i = 0;
                    while (*ptr && i < sizeof(dns_line) - 1) {
                        if (*ptr != '\'' && *ptr != '"') {
                            dns_line[i++] = *ptr;
                        }
                        ptr++;
                    }
                    dns_line[i] = '\0';
                    break;
                }
            }
            pclose(fp);
        }
    }
    
    if (strlen(dns_line) == 0) {
        return 0;  // 没有DNS信息
    }
    
    // 解析DNS列表（支持逗号或空格分隔）
    char *saveptr = NULL;
    char *token = strtok_r(dns_line, ", ", &saveptr);
    int count = 0;
    char **dns_list = NULL;
    
    while (token) {
        char *trimmed = trim_whitespace(token);
        if (strlen(trimmed) > 0 && is_valid_ipv4(trimmed)) {
            char **new_list = realloc(dns_list, (count + 1) * sizeof(char *));
            if (!new_list) {
                // 内存分配失败，清理
                for (int i = 0; i < count; i++) free(dns_list[i]);
                free(dns_list);
                return DHCPCD_ERROR;
            }
            dns_list = new_list;
            
            dns_list[count] = strdup(trimmed);
            if (!dns_list[count]) {
                for (int i = 0; i < count; i++) free(dns_list[i]);
                free(dns_list);
                return DHCPCD_ERROR;
            }
            count++;
        }
        token = strtok_r(NULL, ", ", &saveptr);
    }
    
    *dns_list_out = dns_list;
    return count;
}


int dhcpcd_get_dhcp_status(const char *ifname) {
    if (!ifname) return DHCPCD_ERROR;
    
    // 检查配置文件
    char config_path[256];
    if (get_dhcpcd_config_path(config_path, sizeof(config_path)) != DHCPCD_SUCCESS) {
        return DHCP_STATE_UNKNOWN;
    }
    
    FILE *fp = fopen(config_path, "r");
    if (!fp) {
        return DHCP_STATE_UNKNOWN;
    }
    
    char line[512];
    int in_interface_block = 0;
    int has_static_ip = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        char current_ifname[64];
        
        // 检查是否是接口声明
        if (sscanf(line, "interface %63s", current_ifname) == 1) {
            in_interface_block = (strcmp(trim_whitespace(current_ifname), ifname) == 0);
        }
        
        if (in_interface_block) {
            if (strstr(line, "static ip_address")) {
                has_static_ip = 1;
                break;
            }
        }
    }
    
    fclose(fp);
    
    if (has_static_ip) {
        return DHCP_STATE_STATIC;
    }
    
    // 检查租约文件
    char lease_path[256];
    snprintf(lease_path, sizeof(lease_path), 
             "/var/lib/dhcpcd/dhcpcd-%s.lease", ifname);
    
    if (!file_exists(lease_path)) {
        return DHCP_STATE_DYNAMIC;  // 配置文件是动态，但可能还没租约
    }
    
    // 检查实际接口状态
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "dhcpcd -U %s 2>/dev/null | grep -q 'bound\\|leased'", ifname);
    int ret = system(cmd);
    
    if (WIFEXITED(ret) && WEXITSTATUS(ret) == 0) {
        return DHCP_STATE_BOUND;  // 已绑定租约
    }
    
    return DHCP_STATE_DYNAMIC;
}


//租约
int dhcpcd_get_lease_info(const char *ifname, dhcp_lease_t *lease) {
    if (!ifname || !lease) return DHCPCD_ERROR;
    
    // 初始化结构体
    memset(lease, 0, sizeof(dhcp_lease_t));
    
    char lease_path[256];
    snprintf(lease_path, sizeof(lease_path), 
             "/var/lib/dhcpcd/dhcpcd-%s.lease", ifname);
    
    if (!file_exists(lease_path)) {
        return DHCPCD_NO_LEASE;
    }
    
    FILE *fp = fopen(lease_path, "r");
    if (!fp) {
        return DHCPCD_ERROR;
    }
    
    char line[512];
    int in_lease_block = 0;
    int block_count = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "lease {")) {
            in_lease_block = 1;
            block_count++;
        } else if (strstr(line, "}") && in_lease_block) {
            in_lease_block = 0;
        }
        
        if (in_lease_block) {
            char *trimmed = trim_whitespace(line);
            
            // 解析固定地址
            if (strstr(trimmed, "fixed-address")) {
                char *start = strchr(trimmed, '=');
                if (start) {
                    char *end = strchr(start, ';');
                    if (end) *end = '\0';
                    char *value = trim_whitespace(start + 1);
                    strncpy(lease->ip_address, value, sizeof(lease->ip_address) - 1);
                }
            }
            
            // 解析子网掩码
            else if (strstr(trimmed, "option subnet-mask")) {
                char *start = strchr(trimmed, '=');
                if (start) {
                    char *end = strchr(start, ';');
                    if (end) *end = '\0';
                    char *value = trim_whitespace(start + 1);
                    strncpy(lease->subnet_mask, value, sizeof(lease->subnet_mask) - 1);
                }
            }
            
            // 解析网关
            else if (strstr(trimmed, "option routers")) {
                char *start = strchr(trimmed, '=');
                if (start) {
                    char *end = strchr(start, ';');
                    if (end) *end = '\0';
                    char *value = trim_whitespace(start + 1);
                    strncpy(lease->gateway, value, sizeof(lease->gateway) - 1);
                }
            }
            
            // 解析DHCP服务器
            else if (strstr(trimmed, "option dhcp-server-identifier")) {
                char *start = strchr(trimmed, '=');
                if (start) {
                    char *end = strchr(start, ';');
                    if (end) *end = '\0';
                    char *value = trim_whitespace(start + 1);
                    strncpy(lease->dhcp_server, value, sizeof(lease->dhcp_server) - 1);
                }
            }
            
            // 解析DNS服务器
            else if (strstr(trimmed, "option domain-name-servers")) {
                char *start = strchr(trimmed, '=');
                if (start) {
                    char *end = strchr(start, ';');
                    if (end) *end = '\0';
                    char *value = trim_whitespace(start + 1);
                    strncpy(lease->dns_servers, value, sizeof(lease->dns_servers) - 1);
                }
            }
            
            // 解析租约时间
            else if (strstr(trimmed, "option dhcp-lease-time")) {
                char *start = strchr(trimmed, '=');
                if (start) {
                    char *end = strchr(start, ';');
                    if (end) *end = '\0';
                    char *value = trim_whitespace(start + 1);
                    lease->lease_time = atoi(value);
                }
            }
        }
    }
    
    fclose(fp);
    
    // 如果租约文件为空
    if (block_count == 0) {
        return DHCPCD_NO_LEASE;
    }
    
    return DHCPCD_SUCCESS;
}

// 释放DNS列表内存
void dhcpcd_free_dns_list(char **dns_list, int count) {
    if (!dns_list) return;
    
    for (int i = 0; i < count; i++) {
        if (dns_list[i]) {
            free(dns_list[i]);
        }
    }
    free(dns_list);
}

// 检查dhcpcd是否在运行
int dhcpcd_is_running(const char *ifname) {
    pid_t pid = get_dhcpcd_pid(ifname);
    return (pid > 0) ? 1 : 0;
}

// 强制释放租约
int dhcpcd_release_lease(const char *ifname) {
    pid_t pid = get_dhcpcd_pid(ifname);
    if (pid > 0) {
        if (kill(pid, SIGUSR2) == 0) {  // 发送SIGUSR2释放租约
            sleep(1);  // 等待租约释放
            return DHCPCD_SUCCESS;
        }
    }
    return DHCPCD_ERROR;
}

// 强制续租
int dhcpcd_renew_lease(const char *ifname) {
    pid_t pid = get_dhcpcd_pid(ifname);
    if (pid > 0) {
        if (kill(pid, SIGUSR1) == 0) {  // 发送SIGUSR1续租
            return DHCPCD_SUCCESS;
        }
    }
    return DHCPCD_ERROR;
}