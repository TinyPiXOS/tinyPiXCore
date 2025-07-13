#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpSignalSlot.h"
#include "tpVirtualKeyboard.h"
#include "tpLineEdit.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);

	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display weekly
	app.bindVScreen(vScreen);

	tpLineEdit *testEdit = new tpLineEdit(vScreen);

	testEdit->setRect(30, 30, 300, 60);
	testEdit->setVisible(true);
	testEdit->update();

	// tpVirtualKeyboard *virtualKeyboard = new tpVirtualKeyboard();
	// virtualKeyboard->show();

	// vScreen->update();

	return app.run();
}
