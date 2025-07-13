#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpPercentProgressBar.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	tpPercentProgressBar *progress = new tpPercentProgressBar(vScreen);
	progress->setWidth(700);
	progress->move(0, 100);

	progress->setTotalValue(100);
	progress->addItem("图片", 8);
	progress->addItem("音频", 15);
	progress->addItem("视频", 30);
	progress->addItem("文本", 23);

	vScreen->update();
	return app.run();
}
