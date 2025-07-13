#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpOnOffButton.h"
#include "tpMenuPanelWidget.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display weekly
	app.bindVScreen(vScreen);

	tpMenuPanelWidget *menuPanelWidget = new tpMenuPanelWidget(vScreen);
	connect(menuPanelWidget, onClicked, [=](tpMenuPanelItem *)
			{ std::cout << "菜单面板被点击" << std::endl; });

	tpMenuPanelItem *panelItem1 = new tpMenuPanelItem();
	panelItem1->setIcon(applicationDirPath() + "/蓝牙.png");
	panelItem1->setTitle("蓝牙");

	tpOnOffButton *onOffBtn1 = new tpOnOffButton();
	onOffBtn1->setFixedSize(45, 24);
	panelItem1->setCustomizeWidget(onOffBtn1);
	menuPanelWidget->addItem(panelItem1);

	tpMenuPanelItem *panelItem2 = new tpMenuPanelItem();
	panelItem2->setIcon(applicationDirPath() + "/热点.png");
	panelItem2->setTitle("热点");

	tpOnOffButton *onOffBtn2 = new tpOnOffButton();
	onOffBtn2->setFixedSize(45, 24);
	panelItem2->setCustomizeWidget(onOffBtn2);
	menuPanelWidget->addItem(panelItem2);

	tpMenuPanelItem *panelItem3 = new tpMenuPanelItem();
	panelItem3->setIcon(applicationDirPath() + "/以太网.png");
	panelItem3->setTitle("以太网");
	menuPanelWidget->addItem(panelItem3);

	menuPanelWidget->setWidth(450);
	menuPanelWidget->move(20, 20);

	vScreen->update();

	return app.run();
}
