#include <iostream>
#include "SystemInfo/TpDisplay.h"
#include "TpDisplayManage.h"

int main()
{
	TpDisplayManage devices;
	std::cout<<"physics :"<<devices.getPhysicsWidth()<<"mm *" <<devices.getPhysicsHeight()<<"mm"<<std::endl;

	std::cout<<"pixwm width"<<devices.getPiXWMPhysicsWidth()<<"mm"<<std::endl;
	std::cout<<"pixwm height"<<devices.getPiXWMPhysicsHeight()<<"mm"<<std::endl;
	
	tpShared<TpDisplay> device=devices.getDevice(0);
	std::cout<<"deviceinfo:"<<std::endl;

	TpList<TpDisplay::TpDisplayModeParam> modes=device->getDisplayMode();
	for(auto &mode: modes) {
		std::cout << "Width: " << mode.width << ", Height: " << mode.height << ", Refresh Rate: " << mode.refresh << std::endl;
	}
	std::cout<<"physics :"<<device->getPhysicsWidth()<<"mm *" <<device->getPhysicsHeight()<<std::endl;
	std::cout<<"width:"<<device->getResolutionWidth();
	std::cout<<" height:"<<device->getResolutionHeight();
	std::cout<<" Ref:"<<device->getRefreshRate();
	std::cout<<std::endl;

	std::cout<<"pixwm width"<<device->getPiXWMPhysicsWidth()<<"mm"<<std::endl;
	std::cout<<"pixwm height"<<device->getPiXWMPhysicsHeight()<<"mm"<<std::endl;

	//注意：修改分辨率后鼠标的对应坐标系和位置都需要重新计算！！！
	/*std::cout<<"set resolution"<<std::endl;
	device->setResolution(1440, 900);
	device->setRotation(TpDisplay::TpRotate_90);
	sleep(2);
	device->setRotation(TpDisplay::TpRotate_0);
	printf("ok\n");*/
	return 0;
}