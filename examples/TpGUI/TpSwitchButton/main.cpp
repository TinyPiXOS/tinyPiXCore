#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpColors.h"
#include "TpLabel.h"
#include "TpDialog.h"
#include "TpFont.h"
#include "TpSwitchButton.h"

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);

    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));

    TpSwitchButton *onOffBtn1 = new TpSwitchButton(vScreen, TpSwitchButton::TP_HORIZONTAL);
    onOffBtn1->setRect(120, 120, 150, 75);

    TpSwitchButton *onOffBtn2 = new TpSwitchButton(vScreen, TpSwitchButton::TP_VERTICAL);
    onOffBtn2->setRect(280, 120, 75, 150);

    return app.run();
}
