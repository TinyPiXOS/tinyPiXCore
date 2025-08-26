#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpLine.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
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
