#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpColors.h"
#include "tpFont.h"
#include "tpLink.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true);//vScreen setvisible will be update display weekly
	app.bindVScreen(vScreen);
	vScreen->update();
		
	tpLink *link1 = new tpLink(vScreen);
	link1->setRect(10, 10, 200, 30);
	link1->setText("http:://www.baidu.com");
	link1->setVisible(true);
	link1->update();
	
	tpLink *link2 = new tpLink(vScreen);
	link2->setRect(10, 40, 200, 300);
	link2->setAutoFit(true);
	link2->setText("http:://www.baidu.com");
	link2->setVisible(true);
	link2->update();

	return app.run();
}
