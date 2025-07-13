#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpDialog.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true);//vScreen setvisible will be update display
	app.bindVScreen(vScreen);
	
	tpDialog *dia = new tpDialog();
	dia->setBackGroundColor(tpColors::Red);//or use tpColors
	dia->setRect(0, 0, 300, 300);
	dia->setAlpha(128);
	dia->setVisible(true);
	dia->setBeMoved(true);
	
	dia->update();
	vScreen->update();

	return app.run();
}
