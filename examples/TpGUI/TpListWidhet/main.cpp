#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpListWidget.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpMainWindow *vScreen = new TpMainWindow();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	
	

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
