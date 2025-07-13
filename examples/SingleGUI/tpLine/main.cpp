#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpLine.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	tpLine* hLine = new tpLine(vScreen);
	hLine->setLineType(tpLine::HLine);
	hLine->setColor(_RGB(255, 255, 255));

	hLine->setRect(20, 20, 100, 5);

	tpLine* vLine = new tpLine(vScreen);
	vLine->setLineType(tpLine::VLine);
	vLine->setColor(_RGB(255, 255, 255));

	vLine->setRect(20, 40, 5, 100);

	vScreen->update();

	return app.run();
}
