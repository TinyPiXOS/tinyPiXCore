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
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/XInput2.h>
#include <math.h>
#include "tpConfig.h"
#include "tpDisplay.h"

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
	tpInt32 screen; // 屏幕编号
	tpString name;	// 屏幕名称
	tpList<tpDisplay::tpDisplayModeParam> mode;
	Display *display;			   // X11服务器链接(通过该服务器管理输出设备，所有输出设备共用同一个display)
	XRRScreenResources *resources; // 获取X11服务器中的所有输出设备信息
	RROutput output;			   // 对应的输出设备
	XRROutputInfo *output_info;

	tpDisplayInfoParam()
	{
	}
};

// 计算刷新率
static double calculate_refresh_rate(const XRRModeInfo *mode)
{
	if (mode->hTotal > 0 && mode->vTotal > 0)
	{
		return (double)mode->dotClock / (mode->hTotal * mode->vTotal);
	}
	return 0.0;
}

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

	uint32_t pxValue = _dp * (caculateDpi() / 160.0) + 0.5f;

	return pxValue;
}

uint32_t tpDisplay::sp2Px(const uint32_t &_sp, const int32_t &_screenNum)
{
	// 暂时先不转换
	return _sp;

	uint32_t pxValue = _sp * (caculateDpi() / 160.0) * 1.0f + 0.5f;

	return pxValue;
}

// 获取屏幕参数列表
tpList<tpDisplay::tpDisplayModeParam> tpDisplay::getDisplayMode()
{
	tpDisplayInfoParam *displayParm = static_cast<tpDisplayInfoParam *>(data_);
	XRROutputInfo *output_info = displayParm->output_info;
	XRRScreenResources *resources = displayParm->resources;
	tpList<tpDisplay::tpDisplayModeParam> *mode_list = &displayParm->mode;
	for (int i = 0; i < output_info->nmode; i++) // 遍历所有输出模式
	{
		RRMode mode_id = output_info->modes[i]; // 当前输出模式的ID，拿到着莪该ID后去模式的池子里面找
		for (int j = 0; j < resources->nmode; j++)
		{
			if (resources->modes[j].id == mode_id)
			{
				XRRModeInfo mode_info = resources->modes[j];
				tpDisplay::tpDisplayModeParam modeParam(mode_info.width, mode_info.height, (mode_info.dotClock / (float)(mode_info.hTotal * mode_info.vTotal)));
				mode_list->push_back(modeParam);
				// printf("- %dx%d @ %.2f Hz\n",mode_info.width, mode_info.height,(mode_info.dotClock / (float)(mode_info.hTotal * mode_info.vTotal)));
			}
		}
	}
	return displayParm->mode;
}

// 设置输出参数(当前用于设置分辨率)
// 是否设置刷新率：&& (int)(mode.dotClock / (mode.hTotal * mode.vTotal)) == refresh_rate
int set_resolution_output(Display *display, XRRScreenResources *resources, RROutput output, uint16_t width, uint16_t height)
{
	for (int i = 0; i < resources->nmode; i++)
	{
		XRRModeInfo mode = resources->modes[i];
		if (mode.width == width && mode.height == height)
		{
			for (int j = 0; j < resources->ncrtc; j++)
			{
				XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(display, resources, resources->crtcs[j]);
				if (crtc_info)
				{
					XRRSetCrtcConfig(display, resources, resources->crtcs[j], CurrentTime,
									 crtc_info->x, crtc_info->y, mode.id,
									 crtc_info->rotation, crtc_info->outputs, crtc_info->noutput);
					XRRFreeCrtcInfo(crtc_info);
					/*
								Window root = DefaultRootWindow(display);
								int mouse_x, mouse_y;
								unsigned int mask;
								XQueryPointer(display, root, &root, &root, &mouse_x, &mouse_y, &mouse_x, &mouse_y, &mask);

								float x_ratio = static_cast<float>(width) / crtc_info->width;
								float y_ratio = static_cast<float>(height) / crtc_info->height;

								// 计算新的鼠标位置
								int new_x = static_cast<int>(mouse_x * x_ratio);
								int new_y = static_cast<int>(mouse_y * y_ratio);

								// 使用XWarpPointer将鼠标移动到新的位置
								XWarpPointer(display, None, root, 0, 0, 0, 0, new_x, new_y);
								XFlush(display);
								break;*/
					break;
				}
			}
			return 0;
		}
	}
	return -1;
}

tpDisplay::tpDisplay(tpInt32 num)
{
	data_ = new tpDisplayInfoParam();
	tpDisplayInfoParam *displayParm = static_cast<tpDisplayInfoParam *>(data_);

	if (num < 0)
	{
		std::cerr << "error:display number\n";
		num = 0;
	}
	displayParm->screen = num;
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

tpDisplay::tpDisplay(void *display, void *resources, void *output_info, unsigned long output)
{
	data_ = new tpDisplayInfoParam();
	tpDisplayInfoParam *displayParm = static_cast<tpDisplayInfoParam *>(data_);

	displayParm->display = (Display *)display;
	displayParm->resources = (XRRScreenResources *)resources;
	displayParm->output_info = (XRROutputInfo *)output_info;
	displayParm->output = output;
}

tpDisplay::~tpDisplay()
{
	tpDisplayInfoParam *displayParm = static_cast<tpDisplayInfoParam *>(data_);

	if (displayParm)
	{
		// XRRFreeOutputInfo(displayParm->output_info);
		if (displayParm->output_info)
		{
			XRRFreeOutputInfo(displayParm->output_info);
			displayParm->output_info = nullptr;
		}
		delete displayParm;
		displayParm = nullptr;
		data_ = nullptr;
	}
}

tpInt32 tpDisplay::getPhysicsHeight()
{
	tpDisplayInfoParam *displayParm = static_cast<tpDisplayInfoParam *>(data_);
	//getParamNow();
	return displayParm->output_info->mm_height;
}

tpInt32 tpDisplay::getPhysicsWidth()
{
	tpDisplayInfoParam *displayParm = static_cast<tpDisplayInfoParam *>(data_);
	//getParamNow();
	return displayParm->output_info->mm_width;
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


tpInt32 tpDisplay::getParamNow(tpUInt32 *width, tpUInt32 *height, tpUInt16 *rota,double *refresh)
{
	tpDisplayInfoParam *displayParm = static_cast<tpDisplayInfoParam *>(data_);
	XRROutputInfo *outputInfo = displayParm->output_info;

	// 获取当前的 CRTC (显示控制器)
	RRCrtc crtc = outputInfo->crtc;
	if (crtc != 0)
	{
		// printf("get crtc info ...\n");
		//  获取与 CRTC 关联的配置信息
		XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(displayParm->display, displayParm->resources, crtc);
		// printf("get crtc info ok\n");
		if (crtc_info)
		{
			RROutput current_mode = crtc_info->mode;
			*rota = crtc_info->rotation;
			*width = crtc_info->width;
			*height = crtc_info->height;
			for (int j = 0; j < displayParm->resources->nmode; j++)
			{
				XRRModeInfo mode_info = displayParm->resources->modes[j];
				if (mode_info.id == current_mode)
				{
					// 计算刷新率
					*refresh = calculate_refresh_rate(&mode_info);
					// printf("  当前刷新率: %.2f Hz\n", *refresh);
					break;
				}
			}
			XRRFreeCrtcInfo(crtc_info);
		}
		else
		{
			std::cerr << "Failed to get CRTC info for the output device!" << std::endl;
			return -1;
		}
	}
	else
	{
		std::cerr << "No CRTC associated with the output device!" << std::endl;
		return -1;
	}

	return 0;
}

tpInt32 tpDisplay::getResolutionHeight()
{
	tpUInt32 width = 0, height = 0;
	tpUInt16 rota;
	double refresh;
	getParamNow(&width, &height, &rota, &refresh);
	return height;
}

tpInt32 tpDisplay::getResolutionWidth()
{
	tpUInt32 width = 0, height = 0;
	tpUInt16 rota;
	double refresh;
	getParamNow(&width, &height, &rota, &refresh);
	return width;
}

double tpDisplay::getRefreshRate()
{
	tpUInt32 width = 0, height = 0;
	tpUInt16 rota;
	double refresh;
	getParamNow(&width, &height, &rota, &refresh);
	return refresh;
}

tpUInt16 tpDisplay::getRotation()
{
	tpUInt32 width = 0, height = 0;
	tpUInt16 rota;
	double refresh;
	getParamNow(&width, &height, &rota, &refresh);
	return rota;
}

// 刷新率和分辨率是绑定的，某个分辨率对应的只有一种刷新率
tpInt32 tpDisplay::setResolution(tpUInt32 width, tpUInt32 height)
{
	tpDisplayInfoParam *displayParm = static_cast<tpDisplayInfoParam *>(data_);
	Display *display = displayParm->display;
	XRRScreenResources *resources = displayParm->resources;

	XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(display, resources, displayParm->output_info->crtc);
	if (!crtc_info)
	{
		std::cerr << "Failed to get CRTC info for output\n";
		return -1;
	}
	for (int i = 0; i < resources->nmode; i++)
	{
		XRRModeInfo mode = resources->modes[i];

		if (mode.width == width && mode.height == height)
		{
			XRRSetCrtcConfig(display, resources, displayParm->output_info->crtc, CurrentTime,
							 crtc_info->x, crtc_info->y, mode.id,
							 crtc_info->rotation, crtc_info->outputs, crtc_info->noutput);
			XRRFreeCrtcInfo(crtc_info);
			return 0;
		}
	}
	XFlush(displayParm->display);
	return -1;
}

tpInt32 tpDisplay::setRefreshRate(double rate)
{
	tpDisplayInfoParam *displayParm = static_cast<tpDisplayInfoParam *>(data_);
	Display *display = displayParm->display;
	XRRScreenResources *resources = displayParm->resources;
	XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(display, resources, displayParm->output_info->crtc);
	if (!crtc_info)
	{
		std::cerr << "Failed to get CRTC info for output\n";
		return -1;
	}
	for (int i = 0; i < resources->nmode; i++)
	{
		XRRModeInfo mode = resources->modes[i];

		double refresh_rate = calculate_refresh_rate(&mode);
		if (mode.width == crtc_info->width && mode.height == crtc_info->height &&
			(rate == 0 || abs(refresh_rate - rate) < 0.1))
		{
			XRRSetCrtcConfig(display, resources, displayParm->output_info->crtc, CurrentTime,
							 crtc_info->x, crtc_info->y, mode.id,
							 crtc_info->rotation, crtc_info->outputs, crtc_info->noutput);
			XRRFreeCrtcInfo(crtc_info);
			return 0;
		}
	}
	XFlush(displayParm->display);
	return -1;
}

tpInt32 tpDisplay::setRotation(tpRotate rotation)
{
	tpDisplayInfoParam *displayParm = static_cast<tpDisplayInfoParam *>(data_);
	Display *display = displayParm->display;
	XRRScreenResources *resources = displayParm->resources;

	XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(display, resources, displayParm->output_info->crtc);
	if (!crtc_info)
	{
		std::cerr << "Failed to get CRTC info for output\n";
		return -1;
	}
	Rotation x11_rota;
	switch (rotation)
	{
	case tpRotate_0:
		x11_rota = RR_Rotate_0;
		break;
	case tpRotate_90:
		x11_rota = RR_Rotate_90;
		break;
	case tpRotate_180:
		x11_rota = RR_Rotate_180;
		break;
	case tpRotate_270:
		x11_rota = RR_Rotate_270;
		break;
	default:
		x11_rota = RR_Rotate_0;
		break;
	}
	XRRSetCrtcConfig(display, resources, displayParm->output_info->crtc, CurrentTime,
					 crtc_info->x, crtc_info->y, crtc_info->mode,
					 x11_rota, crtc_info->outputs, crtc_info->noutput);
	XRRFreeCrtcInfo(crtc_info);
	return 0;
}

tpString tpDisplay::getName()
{
	tpDisplayInfoParam *displayParm = static_cast<tpDisplayInfoParam *>(data_);

	return tpString(displayParm->output_info->name);
}

tpInt32 tpDisplay::getNumber()
{
	tpDisplayInfoParam *displayParm = static_cast<tpDisplayInfoParam *>(data_);

	return displayParm->screen;
}

void *tpDisplay::getOutPutInfo()
{
	tpDisplayInfoParam *displayParm = static_cast<tpDisplayInfoParam *>(data_);
	return displayParm->output_info;
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