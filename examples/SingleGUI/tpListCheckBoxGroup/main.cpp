#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpListCheckBoxGroup.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	tpListCheckBoxGroup* checkBoxGroup = new tpListCheckBoxGroup(vScreen);

	checkBoxGroup->addItem("测试1");
	checkBoxGroup->addItem("测试2");
	checkBoxGroup->addItem("测试3");

	checkBoxGroup->addItem(new tpListCheckBoxGroupItem("测试4"));
	checkBoxGroup->addItem(new tpListCheckBoxGroupItem("测试5"));

	checkBoxGroup->move(15, 20);
	checkBoxGroup->update();
	
	vScreen->update();
	return app.run();
}
