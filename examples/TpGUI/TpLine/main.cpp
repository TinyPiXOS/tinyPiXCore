#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpLine.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpMainWindow *vScreen = new TpMainWindow();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	TpLine* hLine = new TpLine(vScreen);
	hLine->setLineType(TpLine::HLine);
	hLine->setColor(_RGB(255, 255, 255));

	hLine->setRect(20, 20, 100, 5);

	TpLine* vLine = new TpLine(vScreen);
	vLine->setLineType(TpLine::VLine);
	vLine->setColor(_RGB(255, 255, 255));

	vLine->setRect(20, 40, 5, 100);

	vScreen->update();

	return app.run();
}
