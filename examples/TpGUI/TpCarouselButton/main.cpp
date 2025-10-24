#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpCarouselButton.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpMainWindow *vScreen = new TpMainWindow();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	TpCarouselButton* carouselButton = new TpCarouselButton(vScreen);
	carouselButton->setCount(3);
	carouselButton->move(100, 100);

	vScreen->update();
	return app.run();
}
