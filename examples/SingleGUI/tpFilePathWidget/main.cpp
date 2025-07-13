#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpFilePathWidget.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	tpFilePathWidget* testWidget = new tpFilePathWidget(vScreen);

	testWidget->setPath("/home/hawk/Public");

	testWidget->setRect(0, 0, vScreen->width(), 50);

	vScreen->update();
	return app.run();
}
