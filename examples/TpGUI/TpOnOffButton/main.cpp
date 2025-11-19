#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpColors.h"
#include "TpLabel.h"
#include "TpDialog.h"
#include "TpFont.h"
#include "TpOnOffButton.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);

	TpMainWindow *vScreen = new TpMainWindow();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	

	TpOnOffButton *onOffBtn1 = new TpOnOffButton(vScreen, TpOnOffButton::TP_HORIZONTAL);
	onOffBtn1->setRect(120, 120, 150, 75);
	onOffBtn1->update();

	TpOnOffButton *onOffBtn2 = new TpOnOffButton(vScreen, TpOnOffButton::TP_VERTICAL);
	onOffBtn2->setRect(280, 120, 75, 150);
	onOffBtn2->update();

	return app.run();
}
