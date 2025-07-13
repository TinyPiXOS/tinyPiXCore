#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpLabel.h"
#include "tpSlider.h"
#include "tpFont.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	tpLabel *valueText = new tpLabel(vScreen);
	valueText->setText(tpString::number(50));
	valueText->setAlign(tinyPiX::AlignCenter);
	valueText->font()->setFontColor(_RGB(255, 255, 255),_RGB(255, 255, 255));
	valueText->font()->setFontSize(30);
	valueText->setWidth(600);
	valueText->setHeight(400);
	valueText->move(20, 200);

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
			{ valueText->setText(tpString::number(value));
				vSlider->setValue(value); });

	connect(vSlider, valueChanged, [=](int32_t value)
			{ valueText->setText(tpString::number(value));
				slider->setValue(value); });

	vScreen->update();

	return app.run();
}
