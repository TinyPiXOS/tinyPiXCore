#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpCheckBox.h"
#include "tpFont.h"
#include "tpColors.h"
#include "tpProgressBar.h"
#include "tpButton.h"

class testProgress : public tpProgressBar
{
public:
	testProgress(tpChildWidget* parent) : tpProgressBar(parent){
	
	};
	
	virtual ~testProgress(){
	
	}
public slots:
	void inCrease(bool clicked){
		double percent = this->percent();
		this->setPercent(percent + 0.01);
	}
	
	void deCrease(bool clicked){
		double percent = this->percent();
		this->setPercent(percent - 0.01);	
	}
};

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true);//vScreen setvisible will be update display1234567498
	
	app.bindVScreen(vScreen);
	vScreen->update();
	
	tpButton *button1 = new tpButton(vScreen);
	button1->setRect(100, 150, 30, 30);
	button1->setText("+");
	button1->setVisible(true);
	button1->update();
	
	tpButton *button2 = new tpButton(vScreen);
	button2->setRect(470, 150, 30, 30);
	button2->setText("-");
	button2->setVisible(true);
	button2->update();
	
	testProgress *progress = new testProgress(vScreen);
	progress->setRect(100, 100, 400, 20);
	progress->setVisible(true);
	progress->update();
	
	connect(button1, SIGNALS(tpButton, onClicked, bool), progress, SLOTS(testProgress, inCrease, bool));
	connect(button2, SIGNALS(tpButton, onClicked, bool), progress, SLOTS(testProgress, deCrease, bool));

	return app.run();
}
