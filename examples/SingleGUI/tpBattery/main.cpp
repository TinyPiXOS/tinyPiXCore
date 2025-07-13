#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpBattery.h"
#include "tpSlider.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	tpBattery *battertWidget = new tpBattery(vScreen);
	battertWidget->setValue(50);
	battertWidget->setWidth(600);
	battertWidget->setHeight(400);
	battertWidget->move(20, 200);

	tpSlider *slider = new tpSlider(vScreen);
	slider->setValue(50);
	slider->setSize(500, 10);
	slider->move(20, 20);

	tpSlider *vSlider = new tpSlider(vScreen);
	vSlider->setDirection(tpSlider::Vertical);
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
