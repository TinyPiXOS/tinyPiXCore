#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpDialog.h"
#include "TpColors.h"
#include "TpButton.h"
#include "TpVariant.h"
#include "TpLabel.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	TpButton *button1 = new TpButton("删除/创建", vScreen);
	button1->setProperty("type", "ControlPanelPowerButton");
	button1->setSize(305, 64);
	button1->move(150, 150);

	TpButton *button2 = nullptr;
	connect(button1, onClicked, [&](bool)
			{
		if (button2)
		{
			button2->deleteLater();
			button2 = nullptr;
		}
		else
		{
			button2 = new TpButton(vScreen);
			button2->setRect(50, 250, 250, 50);
			button2->setButtonStyle(TpButton::TextOnly);
			button2->setText("吉林省2");
			vScreen->update();
		} });

	vScreen->update();
	return app.run();
}
