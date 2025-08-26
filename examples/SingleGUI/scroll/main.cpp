#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpRadioButton.h"
#include "TpFont.h"
#include "TpColors.h"
#include "TpScroll.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	app.setDisableEventType(TpApp::TP_DIS_KEYBOARD);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true);//vScreen setvisible will be update display
	app.bindVScreen(vScreen);
	vScreen->update();

	TpScroll *scroll1 = new TpScroll(vScreen);
	
	scroll1->setRect(10, 100, 20, 400);
	scroll1->setVisible(true);
	scroll1->setMaxRange(200);
	scroll1->setLinePerPage(50);
	scroll1->setDirection(TpScroll::TP_SCROLL_VERTICAL);
	scroll1->update();
	
	TpScroll *scroll2 = new TpScroll(vScreen);
	
	scroll2->setRect(40, 100, 400, 20);
	scroll2->setVisible(true);
	scroll2->setMaxRange(200);
	scroll2->setLinePerPage(50);
	scroll2->update();

	return app.run();
}
