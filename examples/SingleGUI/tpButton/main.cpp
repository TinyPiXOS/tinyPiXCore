#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpButton.h"
#include "tpVariant.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	tpButton *button1 = new tpButton("北京市", vScreen);
	button1->setProperty("type", "ControlPanelPowerButton");
	button1->setSize(300, 64);
	button1->move(150, 150);

	tpButton *buttonEnable = new tpButton("河北省", vScreen);
	buttonEnable->setProperty("type", "VirtualKeyboardButton");
	buttonEnable->setSize(305, 64);
	buttonEnable->move(460, 150);

	tpButton *button2 = new tpButton(vScreen);
	button2->setButtonStyle(tpButton::TextBesideIcon);
	button2->setProperty("type", "FunctionButton");
	button2->setIcon(applicationDirPath() + "/icon.png");
	button2->setText("吉林省2");
	button2->setRect(50, 250, 250, 50);

	tpButton *button3 = new tpButton(vScreen);
	button3->setButtonStyle(tpButton::IconOnly);
	button3->setIcon(applicationDirPath() + "/icon2.png");
	button3->setRoundCorners(13);
	button3->setRect(200, 350, 200, 200);

	vScreen->update();
	return app.run();
}
