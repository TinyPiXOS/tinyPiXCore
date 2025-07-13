#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpColors.h"
#include "tpLineEdit.h"
#include "tpDialog.h"
#include "tpFont.h"
#include "tpVariant.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display weekly
	app.bindVScreen(vScreen);
	vScreen->update();

	tpLineEdit *edit = new tpLineEdit(vScreen);
	edit->setProperty("type", "fileManageEdit");
	edit->setPlaceholderText("请输入搜索内容");
	edit->setAlign(tinyPiX::AlignLeft);
	edit->setRect(20, 20, 300, 100);

	tpLineEdit *edit2 = new tpLineEdit(vScreen);
	edit2->setAlign(tinyPiX::AlignHCenter);
	edit2->setProperty("type", "fileManageEdit");
	edit2->setPlaceholderText("请输入搜索内容");
	edit2->setRect(20, 140, 300, 100);

	tpLineEdit *edit3 = new tpLineEdit(vScreen);
	edit3->setAlign(tinyPiX::AlignRight);
	edit3->setProperty("type", "fileManageEdit");
	edit3->setPlaceholderText("请输入搜索内容");
	edit3->setRect(20, 260, 300, 100);

	tpLineEdit *edit4 = new tpLineEdit(vScreen);
	edit4->setProperty("type", "fileManageEdit");
	edit4->setAlign(tinyPiX::AlignLeft);
	edit4->setIcon(applicationDirPath() + "/搜索.png");
	edit4->setPlaceholderText("请输入搜索内容");
	edit4->setRect(340, 20, 300, 100);

	tpLineEdit *edit5 = new tpLineEdit(vScreen);
	edit5->setAlign(tinyPiX::AlignHCenter);
	edit5->setProperty("type", "fileManageEdit");
	edit5->setIcon(applicationDirPath() + "/搜索.png");
	edit5->setPlaceholderText("请输入搜索内容");
	edit5->setRect(340, 140, 300, 100);

	tpLineEdit *edit6 = new tpLineEdit(vScreen);
	edit6->setAlign(tinyPiX::AlignRight);
	edit6->setProperty("type", "fileManageEdit");
	edit6->setIcon(applicationDirPath() + "/搜索.png");
	edit6->setPlaceholderText("请输入搜索内容");
	edit6->setRect(340, 260, 300, 100);

	vScreen->update();

	return app.run();
}
