#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpRadioButton.h"
#include "tpFont.h"
#include "tpColors.h"
#include "tpRadioButtonGroup.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	app.setDisableEventType(tpApp::TP_DIS_KEYBOARD);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true);
	app.bindVScreen(vScreen);

	tpRadioButton *RadioButton1 = new tpRadioButton(vScreen);
	RadioButton1->font()->setFontSize(50);
	RadioButton1->setAutoFit(true);
	RadioButton1->setText("选我不会错的");
	RadioButton1->setRect(20, 30, 200, 50);

	tpRadioButton *RadioButton2 = new tpRadioButton(vScreen);
	RadioButton2->font()->setFontSize(20);
	RadioButton2->setRect(20, 150, 0, 0);
	RadioButton2->setAutoFit(true);
	RadioButton2->setText("raidoGroup1");

	tpRadioButton *RadioButton3 = new tpRadioButton(vScreen);
	RadioButton3->font()->setFontSize(20);
	RadioButton3->setRect(220, 150, 0, 0);
	RadioButton3->setAutoFit(true);
	RadioButton3->setText("raidoGroup2");

	tpRadioButton *RadioButton4 = new tpRadioButton(vScreen);
	RadioButton4->font()->setFontSize(20);
	RadioButton4->setRect(420, 150, 0, 0);
	RadioButton4->setAutoFit(true);
	RadioButton4->setText("raidoGroup3");

	tpRadioButton *RadioButton5 = new tpRadioButton(vScreen);
	RadioButton5->font()->setFontSize(20);
	RadioButton5->setRect(620, 150, 0, 0);
	RadioButton5->setAutoFit(true);
	RadioButton5->setText("raidoGroup4");

	tpRadioButtonGroup btnGroup;
	btnGroup.addButton(RadioButton2);
	btnGroup.addButton(RadioButton3);
	btnGroup.addButton(RadioButton4);
	btnGroup.addButton(RadioButton5);

	vScreen->update();

	return app.run();
}
