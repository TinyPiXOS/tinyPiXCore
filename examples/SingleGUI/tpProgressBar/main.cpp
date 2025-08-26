#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpCheckBox.h"
#include "TpFont.h"
#include "TpColors.h"
#include "TpProgressBar.h"
#include "TpButton.h"

class testProgress : public TpProgressBar
{
public:
	testProgress(TpChildWidget* parent) : TpProgressBar(parent){
	
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
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true);//vScreen setvisible will be update display1234567498
	
	app.bindVScreen(vScreen);
	vScreen->update();
	
	TpButton *button1 = new TpButton(vScreen);
	button1->setRect(100, 150, 30, 30);
	button1->setText("+");
	button1->setVisible(true);
	button1->update();
	
	TpButton *button2 = new TpButton(vScreen);
	button2->setRect(470, 150, 30, 30);
	button2->setText("-");
	button2->setVisible(true);
	button2->update();
	
	testProgress *progress = new testProgress(vScreen);
	progress->setRect(100, 100, 400, 20);
	progress->setVisible(true);
	progress->update();
	
	connect(button1, SIGNALS(TpButton, onClicked, bool), progress, SLOTS(testProgress, inCrease, bool));
	connect(button2, SIGNALS(TpButton, onClicked, bool), progress, SLOTS(testProgress, deCrease, bool));

	return app.run();
}
