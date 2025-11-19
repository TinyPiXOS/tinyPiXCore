#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpTemperatureWidget.h"
#include "TpHumidityWidget.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpMainWindow *vScreen = new TpMainWindow();
	vScreen->setBackGroundColor(_RGBA(226, 226, 226, 255));
	
	vScreen->update();

	TpTemperatureWidget *temperatureWidget_ = new TpTemperatureWidget(vScreen);
	temperatureWidget_->setRange(-10, 40);
	temperatureWidget_->setValue(20);
	temperatureWidget_->setFixedSize(150, 200);
	temperatureWidget_->move(50, 50);

	TpHumidityWidget* humidityWidget_ = new TpHumidityWidget(vScreen);
    humidityWidget_->setValue(20);
    humidityWidget_->setFixedSize(120, 180);
    humidityWidget_->move(300, 50);

	return app.run();
}
