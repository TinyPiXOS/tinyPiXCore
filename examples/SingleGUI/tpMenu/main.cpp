#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpFont.h"
#include "tpMenu.h"
#include "tpButton.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);

	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	tpMenu *menu = new tpMenu();
	menu->addItem("Item1");
	menu->addItem("Item2");
	menu->addItem("Item3");

	tpButton *showMenuBtn = new tpButton(vScreen);
	showMenuBtn->setText("显示弹出菜单");
	showMenuBtn->setProperty("type", "ControlPanelPowerButton");
	showMenuBtn->setSize(300, 64);
	showMenuBtn->move(20, 20);
	connect(showMenuBtn, onClicked, [&](bool)
			{ menu->exec(330, 20); });

	vScreen->update();

	return app.run();
}
