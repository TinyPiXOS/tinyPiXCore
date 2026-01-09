#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpDialog.h"
#include "TpLabel.h"
#include "TpFont.h"

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);

    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 20));

    // TpDialog *dia = new TpDialog(vScreen);
    TpDialog *dia = new TpDialog();
    dia->setBackGroundColor(_RGB(100, 255, 100));
    dia->setRect(100, 100, 300, 300);
    // dia->setWindowOpacity(0.8);
    // dia->setRoundCorners(50);
    dia->setBeMoved(true);
    dia->setVisible(true);

    // TpLabel *testLabel = new TpLabel(dia);
    // testLabel->setText("测试标签");
    // testLabel->setRect(50, 50, 200, 100);

    // TpWidget *childW = new TpWidget(vScreen);
    // childW->setBackGroundColor(_RGBA(100, 255, 100, 100));
    // childW->setRect(100, 100, 300, 300);
    // childW->setRoundCorners(50);

    return app.run();
}
