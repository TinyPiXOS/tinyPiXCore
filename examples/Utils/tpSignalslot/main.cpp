#include "tpApp.h"
#include "tpSignalSlot.h"
#include "tpTimer.h"
#include "tpFixScreen.h"
#include "tpButton.h"

#include <thread>

class senderClass
{
public:
	senderClass() {}
	~senderClass() {}

public
signals:
	declare_signal(directSignal);
	declare_signal(queueSignal);
};

int main(int argc, char *argv[])
{
	tpApp app(argc, argv);

	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	std::thread::id mainThreadId = std::this_thread::get_id();

	senderClass *sender = new senderClass();

	tpButton *button1 = new tpButton("北京市", vScreen);
	button1->setProperty("type", "ControlPanelPowerButton");
	button1->setSize(300, 64);
	button1->move(150, 150);
	connect(button1, onClicked, [=](bool)
	{
		std::cout << " 发送按钮信号 " << std::endl;
		// 发送直连信号
		sender->directSignal.emit();
	});

	connect(sender, directSignal, [=]()
			{
		std::cout << " 接收到按钮信号 --- ";

		if (std::this_thread::get_id() == mainThreadId)
		{
			std::cout << " 当前 是 主线程" << std::endl;
		}
		else
		{
			std::cout << " 当前 不是 主线程" << std::endl;
		} });

	connect(sender, queueSignal, [=]()
			{
		std::cout << " 接收到线程信号 --- " ;

		if (std::this_thread::get_id() == mainThreadId)
		{
			std::cout << " 当前 是 主线程" << std::endl;
		}
		else
		{
			std::cout << " 当前 不是 主线程" << std::endl;
		} });

	std::thread sendSignalThread([&]()
								 { 
		while(1)
		{
			std::cout << " 线程发送信号 " << std::endl;
			sender->queueSignal.emit();
			tpTimer::sleep(1000);
		} });


	vScreen->update();
	return app.run();
}
