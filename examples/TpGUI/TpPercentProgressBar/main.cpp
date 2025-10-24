#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpPercentProgressBar.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpMainWindow *vScreen = new TpMainWindow();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	TpPercentProgressBar *progress = new TpPercentProgressBar(vScreen);
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
