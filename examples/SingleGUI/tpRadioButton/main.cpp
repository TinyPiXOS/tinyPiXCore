#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpRadioButton.h"
#include "tpFont.h"
#include "tpColors.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	app.setDisableEventType(tpApp::TP_DIS_KEYBOARD);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true);//vScreen setvisible will be update display
	app.bindVScreen(vScreen);
	vScreen->update();
	
	tpRadioButton *RadioButton1 = new tpRadioButton(vScreen);
	RadioButton1->setText("选我不会错的");
	RadioButton1->setRect(0, 0, 200, 50);
	RadioButton1->setAutoFit(true);
	RadioButton1->setVisible(true);
	RadioButton1->update();
	//when use child, parent must call update
	tpRadioButton *RadioButton2 = new tpRadioButton(vScreen);
	RadioButton2->font()->setFontSize(50);
	RadioButton2->setAutoFit(true);
	RadioButton2->setText("选我不会错的");
	RadioButton2->setRect(0, 50, 200, 50);
	RadioButton2->setVisible(true);
	RadioButton2->update();
	
	tpRadioButton *RadioButton3 = new tpRadioButton(vScreen);
	RadioButton3->setRect(0, 150, 0, 0);
	RadioButton3->setAutoFit(true);
	RadioButton3->setText("raidoGroup1");
	RadioButton3->setVisible(true);
	RadioButton3->update();
	
	tpRadioButton *RadioButton4 = new tpRadioButton(vScreen);
	RadioButton4->setRect(120, 150, 0, 0);
	RadioButton4->setAutoFit(true);
	RadioButton4->setText("raidoGroup2");
	RadioButton4->setVisible(true);
	RadioButton4->update();	
	
	tpRadioButton *RadioButton5 = new tpRadioButton(vScreen);
	RadioButton5->setRect(240, 150, 0, 0);
	RadioButton5->setAutoFit(true);
	RadioButton5->setText("raidoGroup3");
	RadioButton5->setVisible(true);
	RadioButton5->update();	
	
	tpRadioButton *RadioButton6 = new tpRadioButton(vScreen);
	RadioButton6->setRect(360, 150, 0, 0);
	RadioButton6->setAutoFit(true);
	RadioButton6->setText("raidoGroup4");
	RadioButton6->setVisible(true);
	RadioButton6->update();	
	
	RadioButton4->addToGroup(RadioButton3);
	RadioButton5->addToGroup(RadioButton3);
	RadioButton6->addToGroup(RadioButton3);
	
	RadioButton3->delFromGroup(RadioButton6);

	return app.run();
}
