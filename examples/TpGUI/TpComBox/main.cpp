#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpDialog.h"
#include "TpComBox.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	TpComBox* listCombox = new TpComBox(vScreen);
	listCombox->setTitle("视图模式");
	listCombox->addItem("测试1");
	listCombox->addItem("测试2");
	listCombox->addItem("测试3");

	listCombox->move(20, 20);

	vScreen->update();
	return app.run();
}
