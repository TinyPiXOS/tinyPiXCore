
#include <xf86drmMode.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("/dev/dri/card0", O_RDWR);
    if (fd < 0) {
        perror("无法打开 /dev/dri/card0");
        return 1;
    }

    drmModeRes *resources = drmModeGetResources(fd);
    if (!resources) {
        perror("无法获取 DRM 资源");
        close(fd);
        return 1;
    }

    printf("设备的连接器数量: %d\n", resources->count_connectors);

    for (int i = 0; i < resources->count_connectors; i++) {
        drmModeConnector *connector = drmModeGetConnector(fd, resources->connectors[i]);
        if (connector) {
            printf("连接器 %d: 类型 %d, 状态 %d\n", resources->connectors[i], connector->connector_type, connector->connection);
            printf("连接器 %d: 物理尺寸: %dmm x %dmm\n",
                   resources->connectors[i], connector->mmWidth, connector->mmHeight);
            drmModeFreeConnector(connector);
        } else {
            printf("连接器 %d: 获取失败\n", resources->connectors[i]);
        }
    }

    drmModeFreeResources(resources);
    close(fd);
    return 0;
}
