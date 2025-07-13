#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpCarouselButton.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	tpCarouselButton* carouselButton = new tpCarouselButton(vScreen);
	carouselButton->setCount(3);
	carouselButton->move(100, 100);

	vScreen->update();
	return app.run();
}
