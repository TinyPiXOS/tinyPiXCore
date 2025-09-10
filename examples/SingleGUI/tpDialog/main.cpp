#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpDialog.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true);//vScreen setvisible will be update display
	app.bindVScreen(vScreen);
	
	TpDialog *dia = new TpDialog();
	dia->setBackGroundColor(_RGB(243, 243, 243));//or use TpColors
	dia->setRect(0, 0, 300, 300);
	dia->setAlpha(128);
    dia->setRoundCorners(50);
	dia->setVisible(true);
	dia->setBeMoved(true);
	
	dia->update();
	vScreen->update();

	return app.run();
}
