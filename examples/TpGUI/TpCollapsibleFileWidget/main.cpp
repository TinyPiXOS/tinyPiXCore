#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpDialog.h"
#include "TpCollapsibleFileWidget.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpMainWindow *vScreen = new TpMainWindow();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	
	

    TpCollapsibleFileWidget *testFileWidget = new TpCollapsibleFileWidget(vScreen);
    testFileWidget->setTitle("今天");
    testFileWidget->setSubTitle("2月14日");
    testFileWidget->setBackGroundColor(_RGB(0, 255, 0));

	for (int i = 0; i < 10; ++i)
    {
        TpCollapsibleFileItem *item = new TpCollapsibleFileItem();
        // item->setFixedSize(100, 100);
        // item->setBackGroundColor(_RGB(255, 0, 0));
        item->setName(TpString::number(i) + ".png");
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
