#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpCheckBox.h"
#include "TpFont.h"
#include "TpColors.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpMainWindow *vScreen = new TpMainWindow();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true);//vScreen setvisible will be update display
	app.bindVScreen(vScreen);
	
	TpCheckBox *checkBox1 = new TpCheckBox(vScreen);
	checkBox1->setText("CheckBox1");
	checkBox1->setRect(100, 100, 80, 30);
	
	TpCheckBox *checkBox2 = new TpCheckBox(vScreen);
	checkBox2->setText("CheckBox2");
	checkBox2->setRect(100, 150, 80, 30);

	vScreen->update();

	return app.run();
}
