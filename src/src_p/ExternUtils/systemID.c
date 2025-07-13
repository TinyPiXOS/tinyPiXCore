/*///------------------------------------------------------------------------------------------------------------------------//
		机器唯一ID获取
说 明 : 
日 期 : 2025.

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdint.h>
#include "systemID.h"

#define BUFLEN 128

/* 读取文件到 buf 中，去掉末尾换行 */
static int read_file_trim(const char *path, char *buf, size_t len) {
    FILE *fp = fopen(path, "r");
    if (!fp) return -1;
    if (!fgets(buf, len, fp)) { fclose(fp); return -1; }
    buf[strcspn(buf, "\r\n")] = 0;
    fclose(fp);
    return 0;
}

/* 尝试 /etc/machine-id */
static int get_machine_id(char *out, size_t len) {
    if (read_file_trim("/etc/machine-id", out, len) == 0) return 0;
    return read_file_trim("/var/lib/dbus/machine-id", out, len);
}

/* 尝试 DMI UUID */
static int get_dmi_uuid(char *out, size_t len) {
    return read_file_trim("/sys/class/dmi/id/product_uuid", out, len);
}

/* x86: 尝试 CPUID 序列号 */
#if defined(__i386__) || defined(__x86_64__)
static int get_x86_cpuid(char *out, size_t len) {
    uint32_t eax, ebx, ecx, edx;
    __asm__ volatile("cpuid"
                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                     : "a"(0x03));
    if (eax||edx) {
        snprintf(out, len, "%08X%08X", edx, eax);
        return 0;
    }
    return -1;
}
#endif

/* ARM: 解析 /proc/cpuinfo Serial 字段 */
#if defined(__arm__) || defined(__aarch64__)
static int get_arm_serial(char *out, size_t len) {
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (!fp) return -1;
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "Serial", 6) == 0) {
            char *p = strchr(line, ':');
            if (p) {
                p++;
                while (*p==' '||*p=='\t') p++;
                strncpy(out, p, len);
                out[strcspn(out, "\r\n")] = 0;
                fclose(fp);
                return 0;
            }
        }
    }
    fclose(fp);
    return -1;
}
#endif

/* 拿第一块非回环网卡的 MAC */
static int get_first_mac(char *out, size_t len) {
    struct dirent *d;
    DIR *dir = opendir("/sys/class/net");
    if (!dir) return -1;
    while ((d = readdir(dir)) != NULL) {
        if (d->d_name[0]=='.') continue;
        if (strcmp(d->d_name, "lo")==0) continue;
        char path[256], mac[32];
        snprintf(path, sizeof(path),
                 "/sys/class/net/%s/address", d->d_name);
        if (read_file_trim(path, mac, sizeof(mac))==0) {
            strncpy(out, mac, len);
            closedir(dir);
            return 0;
        }
    }
    closedir(dir);
    return -1;
}

/* 主调用：依次尝试多种方式 */
int get_system_id(char *out, size_t len) {
    if (get_machine_id(out, len) == 0) return 0;
    if (get_dmi_uuid(out, len)   == 0) return 0;

#if defined(__i386__) || defined(__x86_64__)
    if (get_x86_cpuid(out, len) == 0) return 0;
#endif

#if defined(__arm__) || defined(__aarch64__)
    if (get_arm_serial(out, len) == 0) return 0;
#endif

    if (get_first_mac(out, len) == 0) return 0;
    return -1;
}
/*
int main(void) {
    char id[BUFLEN] = {0};
    if (get_unique_id(id, BUFLEN) == 0) {
        printf("唯一设备 ID: %s\n", id);
        return 0;
    } else {
        fprintf(stderr, "无法获取唯一 ID\n");
        return 1;
    }
}*/
