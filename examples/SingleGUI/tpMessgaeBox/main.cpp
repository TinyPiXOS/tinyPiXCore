#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpButton.h"
#include "tpMessageBox.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	tpButton *button1 = new tpButton("消息框", vScreen);
	button1->setRect(20, 20, 100, 50);
	connect(button1, onClicked, [=](bool)
			{ tpMessageBox::information("消息弹出框"); });

	tpButton *button2 = new tpButton("警告框", vScreen);
	button2->setRect(20, 80, 100, 50);
	connect(button2, onClicked, [=](bool)
			{ tpMessageBox::warning("警告弹出框"); });

	tpButton *button3 = new tpButton("询问框", vScreen);
	button3->setRect(20, 140, 100, 50);
	connect(button3, onClicked, [=](bool)
			{ tpMessageBox::question("询问弹出框"); });

	tpButton *button4 = new tpButton("错误框", vScreen);
	button4->setRect(20, 200, 100, 50);
	connect(button4, onClicked, [=](bool)
			{ tpMessageBox::error("错误弹出框"); });

	vScreen->update();
	return app.run();
}
