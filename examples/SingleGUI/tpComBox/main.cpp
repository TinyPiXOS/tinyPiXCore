#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpDialog.h"
#include "tpComBox.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	tpComBox* listCombox = new tpComBox(vScreen);
	listCombox->setTitle("视图模式");
	listCombox->addItem("测试1");
	listCombox->addItem("测试2");
	listCombox->addItem("测试3");

	listCombox->move(20, 20);

	vScreen->update();
	return app.run();
}
