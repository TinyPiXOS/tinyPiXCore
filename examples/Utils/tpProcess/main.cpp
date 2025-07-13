#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpButton.h"
#include "tpGridLayout.h"
#include "tpVariant.h"
#include "tpFont.h"
#include "tpProcess.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	tpButton *button1 = new tpButton("吉林省1", vScreen);
	button1->setProperty("type", "ControlPanelPowerButton");
	button1->setSize(305, 64);
	button1->move(150, 150);

	connect(button1, onClicked, [=](bool)
			{ 
	tpProcess testProcess;
				testProcess.start("/home/hawk/Public/tinyPiXOS/tinyPiXApp/fileManagement/bin/fileManagement"); });

	vScreen->update();
	return app.run();
}
