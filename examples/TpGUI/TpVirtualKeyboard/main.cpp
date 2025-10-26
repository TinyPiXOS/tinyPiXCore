#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpSignalSlot.h"
#include "TpVirtualKeyboard.h"
#include "TpLineEdit.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);

	TpMainWindow *vScreen = new TpMainWindow();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display weekly
	app.bindVScreen(vScreen);

	TpLineEdit *testEdit = new TpLineEdit(vScreen);

	testEdit->setRect(30, 30, 300, 60);
	testEdit->setVisible(true);
	testEdit->update();

	// TpVirtualKeyboard *virtualKeyboard = new TpVirtualKeyboard();
	// virtualKeyboard->show();

	// vScreen->update();

	return app.run();
}
