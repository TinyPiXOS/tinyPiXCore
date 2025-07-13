#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpDialog.h"
#include "tpColors.h"
#include "tpSurface.h"
#include "tpButton.h"
#include "tpVariant.h"
#include "tpLabel.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	tpButton *button1 = new tpButton("删除/创建", vScreen);
	button1->setProperty("type", "ControlPanelPowerButton");
	button1->setSize(305, 64);
	button1->move(150, 150);

	tpButton *button2 = nullptr;
	connect(button1, onClicked, [&](bool)
			{
		if (button2)
		{
			button2->deleteLater();
			button2 = nullptr;
		}
		else
		{
			button2 = new tpButton(vScreen);
			button2->setRect(50, 250, 250, 50);
			button2->setButtonStyle(tpButton::TextOnly);
			button2->setText("吉林省2");
			vScreen->update();
		} });

	vScreen->update();
	return app.run();
}
