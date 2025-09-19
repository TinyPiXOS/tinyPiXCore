#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpCarouselButton.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	TpCarouselButton* carouselButton = new TpCarouselButton(vScreen);
	carouselButton->setCount(3);
	carouselButton->move(100, 100);

	vScreen->update();
	return app.run();
}
