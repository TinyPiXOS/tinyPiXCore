#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpLabel.h"
#include "TpSlider.h"
#include "TpFont.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	TpLabel *valueText = new TpLabel(vScreen);
	valueText->setText(TpString::number(50));
	valueText->setAlign(tinyPiX::AlignCenter);
	valueText->font()->setFontColor(_RGB(255, 255, 255),_RGB(255, 255, 255));
	valueText->font()->setFontSize(30);
	valueText->setWidth(600);
	valueText->setHeight(400);
	valueText->move(20, 200);

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
			{ valueText->setText(TpString::number(value));
				vSlider->setValue(value); });

	connect(vSlider, valueChanged, [=](int32_t value)
			{ valueText->setText(TpString::number(value));
				slider->setValue(value); });

	vScreen->update();

	return app.run();
}
