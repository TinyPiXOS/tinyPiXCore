#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpRadioButton.h"
#include "tpFont.h"
#include "tpColors.h"
#include "tpScrollBar.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	app.setDisableEventType(tpApp::TP_DIS_KEYBOARD);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true);//vScreen setvisible will be update display
	app.bindVScreen(vScreen);
	vScreen->update();

	tpScrollBar *scroll1 = new tpScrollBar(vScreen, tpScroll::TP_SCROLL_VERTICAL);
	
	scroll1->setRect(10, 100, 20, 400);
	scroll1->setVisible(true);
	scroll1->setMaxRange(200);
	scroll1->setLinePerPage(50);
	scroll1->update();
	
	tpScrollBar *scroll2 = new tpScrollBar(vScreen);
	
	scroll2->setRect(40, 100, 400, 20);
	scroll2->setVisible(true);
	scroll2->setMaxRange(200);
	scroll2->setLinePerPage(50);
	scroll2->update();

	return app.run();
}
