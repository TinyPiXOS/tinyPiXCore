#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpDialog.h"
#include "TpLabel.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true);//vScreen setvisible will be update display
	app.bindVScreen(vScreen);
	
	TpDialog *dia = new TpDialog();
	dia->setBackGroundColor(_RGBA(243, 243, 243, 100));
	dia->setRect(0, 0, 300, 300);
	dia->setAlpha(128);
    dia->setRoundCorners(50);
	dia->setVisible(true);
	dia->setBeMoved(true);
	
    TpLabel* testLabel = new TpLabel(dia);
    testLabel->setText("测试标签");
    testLabel->setRect(50, 50, 200, 100);
    
    TpWidget *childW = new TpWidget(vScreen);
	childW->setBackGroundColor(_RGBA(100, 255, 100, 100));
	childW->setRect(100, 100, 300, 300);
    childW->setRoundCorners(50);

	dia->update();
	vScreen->update();

	return app.run();
}
