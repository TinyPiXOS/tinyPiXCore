/*///------------------------------------------------------------------------------------------------------------------------//
		单个物理屏幕
说 明 :
	主机上会有多个显示输出设备，其中每个设备有不同的输出模式(分辨率，刷新率等)，
	XrandR为了方便高效的管理主机上所有的输出设备，他把所有的输出设备统一管理，而把所有的输出模式也放在统一的池子里进行管理，
	使用XRRScreenResources来管理所有的输出设备和输出模式，
日 期 : 2024.11.18

/*/
//------------------------------------------------------------------------------------------------------------------------//

#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <drm/drm.h>           // 定义了 DRM_CAP_* 宏
#include <drm/drm_mode.h>      // 定义了 DRM_IOCTL_* 常量
#include <sys/ioctl.h>         // 用于 ioctl 调用
//#include <xf86drm.h>
//#include <xf86drmMode.h>
#include <sys/ioctl.h>
#include <math.h>
#include "tpConfig.h"
#include "tpDisplay.h"

#define DRM_DEVICE_PATH "/dev/dri/card0"
#define TINYPIX_CONF_PATH	"/System/conf/tinyPiX.conf"

/*typedef struct {
	Time timestamp;          // 资源的时间戳，标识资源的更新时间
	Time configTimestamp;    // 配置的时间戳，标识配置的更新时间
	int ncrtc;               // CRTC 的数量
	RRCrtc *crtcs;           // CRTC 的数组
	int noutput;             // 输出设备的数量
	RROutput *outputs;       // 输出设备的数组
	int nmode;               // 模式的数量
	XRRCrtcModeInfo *modes;  // 可用模式的数组
} XRRScreenResources;*/

// XRROutputInfo结构体内容如下：
/*typedef struct {
	int           status;          // 状态
	unsigned char *name;           // 输出的名称（如 "HDMI-1"）
	int           nameLen;         // 名称长度
	RRCrtc        crtc;            // 关联的 CRTC（控制器）
	unsigned int  mm_width;        // 输出的物理宽度（毫米）
	unsigned int  mm_height;       // 输出的物理高度（毫米）
	Connection    connection;      // 连接状态（如 Connected/Disconnected）
	SubpixelOrder subpixel_order;  // 子像素顺序（如 RGB、BGR）
	int           ncrtc;           // 支持的 CRTC 数量
	RRCrtc        *crtcs;          // 支持的 CRTC 列表
	int           nclone;          // 支持的克隆输出数量
	RROutput      *clones;         // 支持克隆的输出列表
	int           nmode;           // 支持的模式数量
	int           npreferred;      // 首选模式的数量
	RRMode        *modes;          // 支持的模式列表
} XRROutputInfo;*/
struct tpDisplayInfoParam
{
/*    int fd;                         // DRM 设备文件描述符
    drmModeRes *resources;          // DRM 资源
    drmModeConnector *connector;    // 当前连接的显示器
    drmModeCrtc *crtc;              // 当前CRTC
    uint32_t connector_id;          // 连接器ID

    tpDisplayInfoParam() : fd(-1), resources(nullptr), 
                          connector(nullptr), crtc(nullptr),
                          connector_id(0) {}  */  
};

// 打开DRM设备
/*
static int open_drm_device() {
    int fd = open(DRM_DEVICE_PATH, O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        perror("Failed to open DRM device");
        return -1;
    }
    
    // 检查是否支持KMS
    uint64_t has_dumb;
    if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
        fprintf(stderr, "DRM device does not support dumb buffers\n");
        close(fd);
        return -1;
    }
    
    return fd;
}

// 获取当前活动的连接器
static drmModeConnector* find_active_connector(int fd, drmModeRes *res) {
    for (int i = 0; i < res->count_connectors; i++) {
        drmModeConnector *conn = drmModeGetConnector(fd, res->connectors[i]);
        if (!conn) continue;
        
        if (conn->connection == DRM_MODE_CONNECTED && 
            conn->count_modes > 0) {
            return conn;
        }
        drmModeFreeConnector(conn);
    }
    return nullptr;
}


// 初始化DRM显示参数
static int init_drm_display(tpDisplayInfoParam *drm) {
    drm->fd = open_drm_device();
    if (drm->fd < 0) return -1;
    
    drm->resources = drmModeGetResources(drm->fd);
    if (!drm->resources) {
        perror("Failed to get DRM resources");
        close(drm->fd);
        return -1;
    }
    
    drm->connector = find_active_connector(drm->fd, drm->resources);
    if (!drm->connector) {
        fprintf(stderr, "No active connector found\n");
        drmModeFreeResources(drm->resources);
        close(drm->fd);
        return -1;
    }
    
    drm->connector_id = drm->connector->connector_id;
    
    // 获取当前活动的CRTC
    if (drm->connector->encoder_id) {
        drmModeEncoder *encoder = drmModeGetEncoder(drm->fd, drm->connector->encoder_id);
        if (encoder) {
            drm->crtc = drmModeGetCrtc(drm->fd, encoder->crtc_id);
            drmModeFreeEncoder(encoder);
        }
    }
    
    if (!drm->crtc) {
        fprintf(stderr, "Warning: Failed to get current CRTC\n");
    }
    
    return 0;
}
// 初始化DRM显示参数
static int init_drm_display(tpDisplayInfoParam *drm) {
    drm->fd = open_drm_device();
    if (drm->fd < 0) return -1;
    
    drm->resources = drmModeGetResources(drm->fd);
    if (!drm->resources) {
        perror("Failed to get DRM resources");
        close(drm->fd);
        return -1;
    }
    
    drm->connector = find_active_connector(drm->fd, drm->resources);
    if (!drm->connector) {
        fprintf(stderr, "No active connector found\n");
        drmModeFreeResources(drm->resources);
        close(drm->fd);
        return -1;
    }
    
    drm->connector_id = drm->connector->connector_id;
    
    // 获取当前活动的CRTC
    if (drm->connector->encoder_id) {
        drmModeEncoder *encoder = drmModeGetEncoder(drm->fd, drm->connector->encoder_id);
        if (encoder) {
            drm->crtc = drmModeGetCrtc(drm->fd, encoder->crtc_id);
            drmModeFreeEncoder(encoder);
        }
    }
    
    if (!drm->crtc) {
        fprintf(stderr, "Warning: Failed to get current CRTC\n");
    }
    
    return 0;
}*/

static double caculateDpi()
{
	// 求DPI
	tpDisplay caculateDisplay;

	tpInt32 pxHeight = caculateDisplay.getResolutionHeight();
	tpInt32 pxWidth = caculateDisplay.getResolutionWidth();

	// 求对角线尺寸 英寸
	tpInt32 physicsHeight = caculateDisplay.getPhysicsHeight();
	tpInt32 physicsWidth = caculateDisplay.getPhysicsWidth();

	// 屏幕对角线物理尺寸 mm，需要转换为英寸
	double physicsDiagonal = std::sqrt(physicsHeight * physicsHeight + physicsWidth * physicsWidth);

	// mm转英寸
	physicsDiagonal /= 24.0;

	double dpi = std::sqrt(pxHeight * pxHeight + pxWidth * pxWidth) / physicsDiagonal;

	return dpi;
}

uint32_t tpDisplay::dp2Px(const uint32_t &_dp, const int32_t &_screenNum)
{
	// 暂时先不转换
	return _dp;

//	uint32_t pxValue = _dp * (caculateDpi() / 160.0) + 0.5f;

//	return pxValue;
}

uint32_t tpDisplay::sp2Px(const uint32_t &_sp, const int32_t &_screenNum)
{
	// 暂时先不转换
	return _sp;

//	uint32_t pxValue = _sp * (caculateDpi() / 160.0) * 1.0f + 0.5f;

//	return pxValue;
}

// 获取屏幕参数列表
tpList<tpDisplay::tpDisplayModeParam> tpDisplay::getDisplayMode() {
    /*tpList<tpDisplayModeParam> mode_list;
    tpDisplayInfoParam *drm = static_cast<tpDisplayInfoParam*>(data_);
    
    if (!drm || !drm->connector) return mode_list;
    
    for (int i = 0; i < drm->connector->count_modes; i++) {
        const drmModeModeInfo *mode = &drm->connector->modes[i];
        double refresh = (double)mode->clock * 1000.0 / (mode->htotal * mode->vtotal);
        tpDisplayModeParam modeParam(mode->hdisplay, mode->vdisplay, refresh);
        mode_list.push_back(modeParam);
    }
    
    return mode_list;*/
}


tpDisplay::tpDisplay(tpString &name)
{
	data_ = new tpDisplayInfoParam();

	std::cerr << "error:不支持1" << std::endl;
}

tpDisplay::tpDisplay()
{
	data_ = new tpDisplayInfoParam();

	std::cerr << "error:不支持3" << std::endl;
}


tpDisplay::tpDisplay(tpInt32 num) {
    /*data_ = new tpDisplayInfoParam();
    if (init_drm_display(static_cast<tpDisplayInfoParam*>(data_)) != 0) {
        fprintf(stderr, "Failed to initialize DRM display\n");
        delete static_cast<tpDisplayInfoParam*>(data_);
        data_ = nullptr;
    }*/
}

tpDisplay::~tpDisplay() {
    /*tpDisplayInfoParam *drm = static_cast<tpDisplayInfoParam*>(data_);
    if (drm) {
        if (drm->crtc) drmModeFreeCrtc(drm->crtc);
        if (drm->connector) drmModeFreeConnector(drm->connector);
        if (drm->resources) drmModeFreeResources(drm->resources);
        if (drm->fd >= 0) close(drm->fd);
        delete drm;
        data_ = nullptr;
    }*/
}

tpInt32 tpDisplay::getPhysicsHeight() {
    /*tpDisplayInfoParam *drm = static_cast<tpDisplayInfoParam*>(data_);
    if (!drm || !drm->connector) return 0;
    return drm->connector->mmHeight;  // 物理高度(mm)*/
}

tpInt32 tpDisplay::getPhysicsWidth() {
    /*tpDisplayInfoParam *drm = static_cast<tpDisplayInfoParam*>(data_);
    if (!drm || !drm->connector) return 0;
    return drm->connector->mmWidth;   // 物理宽度(mm)*/
	return 0;
}

tpInt32 tpDisplay::getResolutionHeight() {
    /*tpDisplayInfoParam *drm = static_cast<tpDisplayInfoParam*>(data_);
    if (drm && drm->crtc) return drm->crtc->height;*/
    return 0;
}

tpInt32 tpDisplay::getResolutionWidth() {
    /*tpDisplayInfoParam *drm = static_cast<tpDisplayInfoParam*>(data_);
    if (drm && drm->crtc) return drm->crtc->width;*/
    return 0;
}

tpInt32 tpDisplay::getPiXWMResolutionWidth()
{
	tpConfig conf;
	conf.readFrom(TINYPIX_CONF_PATH);
	return conf.keyValueAsInteger("display-setting", "width");
}

tpInt32 tpDisplay::getPiXWMResolutionHeight()
{
	tpConfig conf;
	conf.readFrom(TINYPIX_CONF_PATH);
	return conf.keyValueAsInteger("display-setting", "height");
}


tpInt32 tpDisplay::getPiXWMPhysicsHeight()
{
	tpInt32 physics = getPhysicsHeight();
	tpInt32 pixwmPhysics = getPiXWMResolutionHeight();
	tpInt32 resolution = getResolutionHeight();

	if(physics<0 || pixwmPhysics<0 || resolution<=0)
		return -1;
	double pixwmResolution = (double)physics * (double)pixwmPhysics / (double)resolution;
	return (tpInt32)pixwmResolution;
}

tpInt32 tpDisplay::getPiXWMPhysicsWidth()
{
	tpInt32 physics = getPhysicsWidth();
	tpInt32 pixwmPhysics = getPiXWMResolutionWidth();
	tpInt32 resolution = getResolutionWidth();

	if(physics<0 || pixwmPhysics<0 || resolution<=0)
		return -1;
	double pixwmResolution = (double)physics * (double)pixwmPhysics / (double)resolution;
	return (tpInt32)pixwmResolution;
}


// 刷新率和分辨率是绑定的，某个分辨率对应的只有一种刷新率
tpInt32 tpDisplay::setResolution(tpUInt32 width, tpUInt32 height) {
    /*tpDisplayInfoParam *drm = static_cast<tpDisplayInfoParam*>(data_);
    if (!drm || !drm->connector || !drm->crtc) return -1;
    
    // 查找匹配的显示模式
    const drmModeModeInfo *target_mode = nullptr;
    for (int i = 0; i < drm->connector->count_modes; i++) {
        const drmModeModeInfo *mode = &drm->connector->modes[i];
        if (mode->hdisplay == width && mode->vdisplay == height) {
            target_mode = mode;
            break;
        }
    }
    
    if (!target_mode) {
        fprintf(stderr, "Requested mode %dx%d not found\n", width, height);
        return -1;
    }
    
    // 设置新分辨率
    int ret = drmModeSetCrtc(drm->fd, drm->crtc->crtc_id, 
                            drm->crtc->buffer_id, // 保持当前framebuffer
                            0, 0,                 // x,y位置
                            &drm->connector_id, 1, 
                            const_cast<drmModeModeInfo*>(target_mode));
    
    if (ret != 0) {
        fprintf(stderr, "Failed to set mode: %s\n", strerror(-ret));
        return -1;
    }*/
    
    return 0;
}



tpInt32 tpDisplay::setLight(tpUInt8 light)
{
	return 0;
}

tpInt32 tpDisplay::getLight()
{
	tpDisplayInfoParam *displayParm = static_cast<tpDisplayInfoParam *>(data_);
	return 100;
}