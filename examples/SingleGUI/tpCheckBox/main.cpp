#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpCheckBox.h"
#include "tpFont.h"
#include "tpColors.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true);//vScreen setvisible will be update display
	app.bindVScreen(vScreen);
	
	tpCheckBox *checkBox1 = new tpCheckBox(vScreen);
	checkBox1->setText("123");
	checkBox1->setRect(100, 100, 80, 30);
	
	checkBox1->update();
	vScreen->update();

	return app.run();
}
