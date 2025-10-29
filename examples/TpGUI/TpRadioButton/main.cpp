#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpRadioButton.h"
#include "TpFont.h"
#include "TpColors.h"
#include "TpRadioButtonGroup.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	app.setDisableEventType(TpApp::TP_DIS_KEYBOARD);
	TpMainWindow *vScreen = new TpMainWindow();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true);
	

	TpRadioButton *RadioButton1 = new TpRadioButton(vScreen);
	RadioButton1->font()->setFontSize(50);
	RadioButton1->setAutoFit(true);
	RadioButton1->setText("选我不会错的");
	RadioButton1->setRect(20, 30, 200, 50);

	TpRadioButton *RadioButton2 = new TpRadioButton(vScreen);
	RadioButton2->font()->setFontSize(20);
	RadioButton2->setRect(20, 150, 0, 0);
	RadioButton2->setAutoFit(true);
	RadioButton2->setText("raidoGroup1");

	TpRadioButton *RadioButton3 = new TpRadioButton(vScreen);
	RadioButton3->font()->setFontSize(20);
	RadioButton3->setRect(220, 150, 0, 0);
	RadioButton3->setAutoFit(true);
	RadioButton3->setText("raidoGroup2");

	TpRadioButton *RadioButton4 = new TpRadioButton(vScreen);
	RadioButton4->font()->setFontSize(20);
	RadioButton4->setRect(420, 150, 0, 0);
	RadioButton4->setAutoFit(true);
	RadioButton4->setText("raidoGroup3");

	TpRadioButton *RadioButton5 = new TpRadioButton(vScreen);
	RadioButton5->font()->setFontSize(20);
	RadioButton5->setRect(620, 150, 0, 0);
	RadioButton5->setAutoFit(true);
	RadioButton5->setText("raidoGroup4");

	TpRadioButtonGroup btnGroup;
	btnGroup.addButton(RadioButton2);
	btnGroup.addButton(RadioButton3);
	btnGroup.addButton(RadioButton4);
	btnGroup.addButton(RadioButton5);

	vScreen->update();

	return app.run();
}
