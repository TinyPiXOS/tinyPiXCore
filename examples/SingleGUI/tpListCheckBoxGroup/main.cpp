#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpListWidget.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	TpListWidget* checkBoxGroup = new TpListWidget(vScreen);

	checkBoxGroup->addItem("测试1");
	checkBoxGroup->addItem("测试2");
	checkBoxGroup->addItem("测试3");

	checkBoxGroup->addItem(new TpListWidgetItem("测试4"));
	checkBoxGroup->addItem(new TpListWidgetItem("测试5"));

	checkBoxGroup->move(15, 20);
	checkBoxGroup->update();
	
	vScreen->update();
	return app.run();
}
