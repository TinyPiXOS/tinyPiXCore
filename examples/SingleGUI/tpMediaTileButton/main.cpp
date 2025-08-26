#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpMediaTileButton.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	TpMediaTileButton* pictureTileBtn = new TpMediaTileButton(vScreen);
	pictureTileBtn->setText("图片");
	pictureTileBtn->setIcon(applicationDirPath() + "/图片.png");
	connect(pictureTileBtn, onClicked, [=](bool) {});
	pictureTileBtn->move(20, 20);

	TpMediaTileButton* videoTileBtn = new TpMediaTileButton(vScreen);
	videoTileBtn->setText("视频");
	videoTileBtn->setIcon(applicationDirPath() + "/视频.png");
	connect(videoTileBtn, onClicked, [=](bool) {});
	videoTileBtn->move(20, 150);

	vScreen->update();
	return app.run();
}
