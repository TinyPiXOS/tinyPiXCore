#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpButton.h"
#include "TpGridLayout.h"
#include "TpVariant.h"
#include "TpFont.h"
#include "TpProcess.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpMainWindow *vScreen = new TpMainWindow();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	TpButton *button1 = new TpButton("吉林省1", vScreen);
	button1->setProperty("type", "ControlPanelPowerButton");
	button1->setSize(305, 64);
	button1->move(150, 150);

	connect(button1, onClicked, [=](bool)
			{ 
	TpProcess testProcess;
				testProcess.start("/home/hawk/Public/tinyPiXOS/tinyPiXApp/fileManagement/bin/fileManagement"); });

	vScreen->update();
	return app.run();
}
