#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpFilePathWidget.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	TpFilePathWidget* testWidget = new TpFilePathWidget(vScreen);

	testWidget->setPath("/home/hawk/Public");

	testWidget->setRect(0, 0, vScreen->width(), 50);

	vScreen->update();
	return app.run();
}
