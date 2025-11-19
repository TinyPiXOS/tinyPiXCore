#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpColors.h"
#include "TpLineEdit.h"
#include "TpDialog.h"
#include "TpFont.h"
#include "TpVariant.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpMainWindow *vScreen = new TpMainWindow();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	 weekly
	
	vScreen->update();

	TpLineEdit *edit = new TpLineEdit(vScreen);
	edit->setProperty("type", "fileManageEdit");
	edit->setPlaceholderText("请输入搜索内容");
	edit->setAlign(Tp::AlignLeft);
	edit->setRect(20, 20, 300, 100);

	TpLineEdit *edit2 = new TpLineEdit(vScreen);
	edit2->setAlign(Tp::AlignHCenter);
	edit2->setProperty("type", "fileManageEdit");
	edit2->setPlaceholderText("请输入搜索内容");
	edit2->setRect(20, 140, 300, 100);

	TpLineEdit *edit3 = new TpLineEdit(vScreen);
	edit3->setAlign(Tp::AlignRight);
	edit3->setProperty("type", "fileManageEdit");
	edit3->setPlaceholderText("请输入搜索内容");
	edit3->setRect(20, 260, 300, 100);

	TpLineEdit *edit4 = new TpLineEdit(vScreen);
	edit4->setProperty("type", "fileManageEdit");
	edit4->setAlign(Tp::AlignLeft);
	edit4->setIcon(applicationDirPath() + "/搜索.png");
	edit4->setPlaceholderText("请输入搜索内容");
	edit4->setRect(340, 20, 300, 100);

	TpLineEdit *edit5 = new TpLineEdit(vScreen);
	edit5->setAlign(Tp::AlignHCenter);
	edit5->setProperty("type", "fileManageEdit");
	edit5->setIcon(applicationDirPath() + "/搜索.png");
	edit5->setPlaceholderText("请输入搜索内容");
	edit5->setRect(340, 140, 300, 100);

	TpLineEdit *edit6 = new TpLineEdit(vScreen);
	edit6->setAlign(Tp::AlignRight);
	edit6->setProperty("type", "fileManageEdit");
	edit6->setIcon(applicationDirPath() + "/搜索.png");
	edit6->setPlaceholderText("请输入搜索内容");
	edit6->setRect(340, 260, 300, 100);

	vScreen->update();

	return app.run();
}
