#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpDialog.h"
#include "tpColors.h"
#include "tpPanel.h"
#include "tpSurface.h"
#include "tpButton.h"
#include "tpGridLayout.h"
#include "tpHelper.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(tpColors::White);
	
	tpSurface surface;
	surface.fromFile("exam1.bmp");
	vScreen->setBackGroundImage(surface);
	vScreen->setVisible(true);//vScreen setvisible will be update display
	app.bindVScreen(vScreen);
	vScreen->update();

	return app.run();
}
