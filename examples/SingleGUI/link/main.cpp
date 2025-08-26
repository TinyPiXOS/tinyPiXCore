#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpColors.h"
#include "TpFont.h"
#include "TpLink.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true);//vScreen setvisible will be update display weekly
	app.bindVScreen(vScreen);
	vScreen->update();
		
	TpLink *link1 = new TpLink(vScreen);
	link1->setRect(10, 10, 200, 30);
	link1->setText("http:://www.baidu.com");
	link1->setVisible(true);
	link1->update();
	
	TpLink *link2 = new TpLink(vScreen);
	link2->setRect(10, 40, 200, 300);
	link2->setAutoFit(true);
	link2->setText("http:://www.baidu.com");
	link2->setVisible(true);
	link2->update();

	return app.run();
}
