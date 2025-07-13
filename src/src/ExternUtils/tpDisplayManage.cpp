/*///------------------------------------------------------------------------------------------------------------------------//
		逻辑屏幕显示管理(单个逻辑屏幕的包含多个物理屏幕，暂不考虑多个逻辑屏幕的情况)
说 明 :	在开机后点击头像，在右下角的设置按钮点击，Ubuntu的登陆模式选择Ubuntu on Xorg。
日 期 : 2024.11.19

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <limits.h>
#include "tpDisplayManage.h"
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/XInput2.h>


struct tpDisplayManageData
{
	Display *display;
	XRRScreenResources *resources;				//逻辑屏幕信息
	tpList<tpShared<tpDisplay>>	device_list;	//物理屏幕列表
	Window root;
	int screen_number;
	int logic_resolution_height;
	int logic_resolution_width;
	tpDisplayManageData()
	{
	}
};

static int get_crtc_logic_max_min_xy(XRRCrtcInfo *crtc,int *min_x,int *max_x,int *min_y,int *max_y)
{
	// 取 crtc_info 中的 (x,y) 作为左上角，(x+width, y+height) 为右下角

	if (crtc->x < *min_x) 
		*min_x = crtc->x;
	if (crtc->y < *min_y) 
		*min_y = crtc->y;
	int temp=crtc->x + crtc->width;
	if (temp > *max_x) 
//	if ((crtc->x + crtc->width) > *max_x) 
		*max_x = crtc->x + crtc->width;
	temp=crtc->y + crtc->height;
	if (temp > *max_y) 
//	if ((crtc->y + crtc->height) > *max_y) 
		*max_y = crtc->y + crtc->height;

	return 0;
}

tpDisplayManage::tpDisplayManage()
{
	data_ = new tpDisplayManageData();
	tpDisplayManageData *displayParm = static_cast<tpDisplayManageData *>(data_);

	displayParm->display = XOpenDisplay(NULL);		//打开x11服务
	if (!displayParm->display)
	{
		std::cerr << "Cannot open display\n";
		return;
	}
	Window root = DefaultRootWindow(displayParm->display);		//获取默认的根窗口(暂时只考虑单个逻辑屏幕的情况)
	XRRScreenResources *screen_res= XRRGetScreenResources(displayParm->display, root); // 获取所有物理屏幕(输出设备)的信息
	if (!screen_res)
	{
		std::cerr <<"Failed to get screen resources\n";
		XCloseDisplay(displayParm->display);
		return ;
	}
	displayParm->resources=screen_res;

	int screen_number = DefaultScreen(displayParm->display);		//获取默认的逻辑屏幕编号(适用于只有一个逻辑屏幕，如果有多个则需要使用XScreenOfDisplay(display, i);)依次遍历所有逻辑屏幕
	displayParm->screen_number=screen_number;
	getDeviceList();
}

tpDisplayManage::~tpDisplayManage()
{
	tpDisplayManageData *displayParm = static_cast<tpDisplayManageData *>(data_);
	//for (auto &it : displayParm->device_list)
	//{
		//delete &it;
	//}

	if(displayParm->display)
	{
		XCloseDisplay(displayParm->display);
		displayParm->display=nullptr;
	}
	if(displayParm->resources)
	{
		XRRFreeScreenResources(displayParm->resources);
		displayParm->resources=nullptr;
	}
}

const tpList<tpShared<tpDisplay>> &tpDisplayManage::getDeviceList() const
{
	tpDisplayManageData *displayParm = static_cast<tpDisplayManageData *>(data_);
	int min_x=INT_MAX,max_x=INT_MIN,min_y=INT_MAX,max_y=INT_MIN;
	printf("min_x=%d,max_x=%d,min_y=%d,max_y=%d\n",min_x,max_x,min_y,max_y);
	Window root = DefaultRootWindow(displayParm->display);		//获取默认的根窗口(暂时只考虑单个逻辑屏幕的情况)
	XRRScreenResources *screen_res= displayParm->resources; 
	XRROutputInfo *target_output_info = NULL;
	RROutput target_output = None;
	for (int i = 0; i < displayParm->resources->noutput; i++)
	{
		XRROutputInfo *output_info = XRRGetOutputInfo(displayParm->display, screen_res, screen_res->outputs[i]);
		if(!output_info)
			std::cerr<<"XRRGetOutputInfo err\n";
		if (output_info->connection == RR_Connected) 
		{
			XRRCrtcInfo *crtc = XRRGetCrtcInfo(displayParm->display, screen_res, output_info->crtc);
			if (crtc) {
				// 取 crtc_info 中的 (x,y) 作为左上角，(x+width, y+height) 为右下角
				get_crtc_logic_max_min_xy(crtc,&min_x,&max_x,&min_y,&max_y);
				printf("min_x=%d,max_x=%d,min_y=%d,max_y=%d\n",min_x,max_x,min_y,max_y);
				XRRFreeCrtcInfo(crtc);
			}
			target_output_info = output_info;
			target_output = screen_res->outputs[i];
			//printf("add to list:%s--%dmm*%dmm\n",target_output_info->name,target_output_info->mm_width,target_output_info->mm_height);
			//tpDisplay display(displayParm->display,displayParm->resources ,target_output_info);
			displayParm->device_list.emplace_back(std::make_shared<tpDisplay>(displayParm->display,displayParm->resources ,target_output_info,target_output));
		}
	}

	displayParm->logic_resolution_width=max_x - min_x;
	displayParm->logic_resolution_height=max_y - min_y;

	return displayParm->device_list;
}

tpShared<tpDisplay> tpDisplayManage::getDevice(const tpString &name)
{
	tpDisplayManageData *displayParm = static_cast<tpDisplayManageData *>(data_);
	for(auto it :displayParm->device_list)
	{
		if(it->getName()==name)
			return it;
	}
	std::cerr << "can't get device for name" << std::endl;
	return nullptr;
}

tpShared<tpDisplay> tpDisplayManage::getDevice(tpInt32 num)
{
	tpDisplayManageData *displayParm = static_cast<tpDisplayManageData *>(data_);
	tpInt32 i=0;
	for(auto it :displayParm->device_list)
	{
		if(i==num)
			return it;
		i++;
	}
	std::cerr << "can't get device for num" << std::endl;
	return nullptr;
}

tpInt32 tpDisplayManage::getPhysicsHeight()
{
	tpDisplayManageData *displayParm = static_cast<tpDisplayManageData *>(data_);
	return DisplayHeightMM(displayParm->display,displayParm->screen_number);
}
tpInt32 tpDisplayManage::getPhysicsWidth()
{
	tpDisplayManageData *displayParm = static_cast<tpDisplayManageData *>(data_);
	return DisplayWidthMM(displayParm->display,displayParm->screen_number);
}

tpInt32 tpDisplayManage::getResolutionHeight()
{
	tpDisplayManageData *displayParm = static_cast<tpDisplayManageData *>(data_);
	return displayParm->logic_resolution_height;
}

tpInt32 tpDisplayManage::getResolutionWidth()
{
	tpDisplayManageData *displayParm = static_cast<tpDisplayManageData *>(data_);
	return displayParm->logic_resolution_width;
}

tpInt32 tpDisplayManage::getPiXWMPhysicsHeight()
{
	tpDisplayManageData *displayParm = static_cast<tpDisplayManageData *>(data_);
	tpInt32 pixwmPhysics = tpDisplay::getPiXWMResolutionHeight();
	tpInt32 resolution = getResolutionHeight();
	tpInt32 physics = getPhysicsHeight();

	if(physics<0 || pixwmPhysics<0 || resolution<=0)
		return -1;
	double pixwmResolution = (double)physics * (double)pixwmPhysics / (double)resolution;
	return (tpInt32)pixwmResolution;
}

tpInt32 tpDisplayManage::getPiXWMPhysicsWidth()
{
	tpDisplayManageData *displayParm = static_cast<tpDisplayManageData *>(data_);
	tpInt32 pixwmPhysics = tpDisplay::getPiXWMResolutionWidth();
	tpInt32 resolution = getResolutionWidth();
	tpInt32 physics = getPhysicsWidth();

	if(physics<0 || pixwmPhysics<0 || resolution<=0)
		return -1;
	double pixwmResolution = (double)physics * (double)pixwmPhysics / (double)resolution;
	return (tpInt32)pixwmResolution;
}

int tpDisplayManage::correctMousePosition()
{
	tpDisplayManageData *displayParm = static_cast<tpDisplayManageData *>(data_);

	int x = 0, y = 0;
	Window root = DefaultRootWindow(displayParm->display);
	XWarpPointer(displayParm->display, None, root, 0, 0, 0, 0, x, y);
	XFlush(displayParm->display);
	return 0;
}
