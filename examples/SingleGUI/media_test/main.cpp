
#include <iostream>
#include <string>
#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpDialog.h"
#include "tpVideoInterface.h"


auto callback_display = [](uint8_t** data, int* linesize, uint32_t format, void* userdata) -> int
{
	char *user=(char *)userdata;

	uint8_t *r=data[0];
	uint8_t *g=data[1];
	uint8_t *b=data[2];
	printf("userdata:%s\n",user);
	for(int i=0;i<20;i++)
	{
		printf("%02x ",data[0][i]);
	}
	printf("\n");
};

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true);//vScreen setvisible will be update display
	app.bindVScreen(vScreen);
	
	tpDialog *dia = new tpDialog();
	dia->setBackGroundColor(tpColors::Red);//or use tpColors
	dia->setRect(0, 0, 300, 300);
	dia->setAlpha(128);
	dia->setVisible(true);
	dia->setBeMoved(true);

	tpString device("hw:0,1 USB Audio");
	tpVideoInterface video(device);
	video.addFile("/home/pix/Media/sintel_trailer-480p.mkv");
	char *data="Test User Data";
	std::function<int(uint8_t**, int*, uint32_t, void*)> func = callback_display;
	video.openDevice();
	/*video.setWindowCoordinates(0,0);
	video.setWindowSize(1024,768);
	video.setScalingMode(tpVideoInterface::TP_VIDEO_SCALING_FIT);		//推荐格式
	video.playStart();
	video.setSpeed(1.0);*/
	dia->update();
	vScreen->update();
	app.run();
	printf("结束\n");
	video.closeDevice();
	printf("视频结束\n");
	return 0;
}
