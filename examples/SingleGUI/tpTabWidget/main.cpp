#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpDialog.h"
#include "tpColors.h"
#include "tpSurface.h"
#include "tpButton.h"
#include "tpGridLayout.h"
#include "tpVariant.h"
#include "tpLabel.h"
#include "tpFont.h"
#include "tpTabWidget.h"
#include "tpVBoxLayout.h"
#include "tpDialog.h"
#include "tpTabBar.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true);//vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	tpTabWidget *tabWidget = new tpTabWidget(vScreen);
	tabWidget->addTab(new tpChildWidget(), "Tab1");
	tabWidget->addTab(new tpChildWidget(), "Tab2");
	
	tpTabBar *tabBar = tabWidget->tabBar();
    tabBar->setIcon(0, applicationDirPath() + "/最近-normal.png", applicationDirPath() + "/最近-focus.png");
    tabBar->setIcon(1, applicationDirPath() + "/浏览-normal.png", applicationDirPath() + "/浏览-focus.png");

	tpVBoxLayout *mainLayout = new tpVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);

    mainLayout->addWidget(tabWidget);
	
	vScreen->setLayout(mainLayout);
	
	vScreen->update();

	return app.run();
}