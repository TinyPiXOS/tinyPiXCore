#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpDialog.h"
#include "tpCollapsibleFileWidget.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

    tpCollapsibleFileWidget *testFileWidget = new tpCollapsibleFileWidget(vScreen);
    testFileWidget->setTitle("今天");
    testFileWidget->setSubTitle("2月14日");
    testFileWidget->setBackGroundColor(_RGB(0, 255, 0));

	for (int i = 0; i < 10; ++i)
    {
        tpCollapsibleFileItem *item = new tpCollapsibleFileItem();
        // item->setFixedSize(100, 100);
        // item->setBackGroundColor(_RGB(255, 0, 0));
        item->setName(tpString::number(i) + ".png");
        // item->setSize(i * 1e3);
        item->setPath("");

        testFileWidget->addFileItem(item);
    }

	// testFileWidget->setFixedSize(700, 150);
	testFileWidget->setSize(700, 0);
    testFileWidget->move(10, 10);

	vScreen->update();
	return app.run();
}
