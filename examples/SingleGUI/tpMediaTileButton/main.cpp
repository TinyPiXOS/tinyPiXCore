#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpMediaTileButton.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	tpMediaTileButton* pictureTileBtn = new tpMediaTileButton(vScreen);
	pictureTileBtn->setText("图片");
	pictureTileBtn->setIcon(applicationDirPath() + "/图片.png");
	connect(pictureTileBtn, onClicked, [=](bool) {});
	pictureTileBtn->move(20, 20);

	tpMediaTileButton* videoTileBtn = new tpMediaTileButton(vScreen);
	videoTileBtn->setText("视频");
	videoTileBtn->setIcon(applicationDirPath() + "/视频.png");
	connect(videoTileBtn, onClicked, [=](bool) {});
	videoTileBtn->move(20, 150);

	vScreen->update();
	return app.run();
}
