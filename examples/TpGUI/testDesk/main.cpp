#include "TpApp.h"
#include "TpFixScreen.h"
#include "navigationBar.h"

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);
    app.setStyle(Tp::SmartDeviceGUIStyle);

    TpFixScreen *vScreen = new TpFixScreen();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
    // vScreen->setBackGroundImage(TpImage(applicationDirPath() + "/test.svg"));
    // vScreen->setBackGroundImage(TpImage(applicationDirPath() + "/icon.png"));
    app.bindVScreen(vScreen);

    navigationBar *navifgaBar = new navigationBar();
    navifgaBar->move((vScreen->width() - navifgaBar->width()) / 2.0, vScreen->height() - navifgaBar->height());

    navifgaBar->update();
    vScreen->update();

    return app.run();
}
