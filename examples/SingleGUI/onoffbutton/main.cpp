#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpColors.h"
#include "tpLabel.h"
#include "tpDialog.h"
#include "tpFont.h"
#include "tpOnOffButton.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display weekly
	app.bindVScreen(vScreen);
	vScreen->update();

	tpOnOffButton *onOffBtn1 = new tpOnOffButton(vScreen, tpOnOffButton::TP_HORIZONTAL);
	onOffBtn1->setRect(120, 120, 150, 75);
	onOffBtn1->update();

	tpOnOffButton *onOffBtn2 = new tpOnOffButton(vScreen, tpOnOffButton::TP_VERTICAL);
	onOffBtn2->setRect(280, 120, 75, 150);
	onOffBtn2->update();

	return app.run();
}
