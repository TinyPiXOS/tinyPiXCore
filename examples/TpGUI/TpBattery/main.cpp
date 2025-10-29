#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpBattery.h"
#include "TpSlider.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpMainWindow *vScreen = new TpMainWindow();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	
	

	TpBattery *battertWidget = new TpBattery(vScreen);
	battertWidget->setValue(50);
	battertWidget->setWidth(600);
	battertWidget->setHeight(400);
	battertWidget->move(20, 200);

	TpSlider *slider = new TpSlider(vScreen);
	slider->setValue(50);
	slider->setSize(500, 10);
	slider->move(20, 20);

	TpSlider *vSlider = new TpSlider(vScreen);
	vSlider->setDirection(TpSlider::Vertical);
	vSlider->setValue(50);
	vSlider->setSize(10, 500);
	vSlider->move(650, 20);

	connect(slider, valueChanged, [=](int32_t value)
			{ battertWidget->setValue(value);
				vSlider->setValue(value); });

	connect(vSlider, valueChanged, [=](int32_t value)
			{ battertWidget->setValue(value);
				slider->setValue(value); });

	vScreen->update();

	return app.run();
}
