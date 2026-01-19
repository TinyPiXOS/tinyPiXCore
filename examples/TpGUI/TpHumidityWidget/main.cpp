#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpHumidityWidget.h"

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);
    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(226, 226, 226, 255));

    TpHumidityWidget *humidityWidget_ = new TpHumidityWidget(vScreen);
    humidityWidget_->setRange(0, 100);
    humidityWidget_->setValue(99);
    humidityWidget_->setFixedSize(120, 100);
    humidityWidget_->move(300, 50);

    return app.run();
}
