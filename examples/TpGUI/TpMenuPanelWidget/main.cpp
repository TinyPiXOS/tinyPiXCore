#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpSwitchButton.h"
#include "TpMenuPanelWidget.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpMainWindow *vScreen = new TpMainWindow();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	 weekly
	

	TpMenuPanelWidget *menuPanelWidget = new TpMenuPanelWidget(vScreen);
	connect(menuPanelWidget, onClicked, [=](TpMenuPanelItem *)
			{ std::cout << "菜单面板被点击" << std::endl; });

	TpMenuPanelItem *panelItem1 = new TpMenuPanelItem();
	panelItem1->setIcon(applicationDirPath() + "/蓝牙.png");
	panelItem1->setTitle("蓝牙");

	TpSwitchButton *onOffBtn1 = new TpSwitchButton();
	onOffBtn1->setFixedSize(45, 24);
	panelItem1->setCustomizeWidget(onOffBtn1);
	menuPanelWidget->addItem(panelItem1);

	TpMenuPanelItem *panelItem2 = new TpMenuPanelItem();
	panelItem2->setIcon(applicationDirPath() + "/热点.png");
	panelItem2->setTitle("热点");

	TpSwitchButton *onOffBtn2 = new TpSwitchButton();
	onOffBtn2->setFixedSize(45, 24);
	panelItem2->setCustomizeWidget(onOffBtn2);
	menuPanelWidget->addItem(panelItem2);

	TpMenuPanelItem *panelItem3 = new TpMenuPanelItem();
	panelItem3->setIcon(applicationDirPath() + "/以太网.png");
	panelItem3->setTitle("以太网");
	menuPanelWidget->addItem(panelItem3);

	menuPanelWidget->setWidth(450);
	menuPanelWidget->move(20, 20);

	vScreen->update();

	return app.run();
}
