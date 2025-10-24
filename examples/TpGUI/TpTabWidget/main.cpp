#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpDialog.h"
#include "TpColors.h"
#include "TpButton.h"
#include "TpGridLayout.h"
#include "TpVariant.h"
#include "TpLabel.h"
#include "TpFont.h"
#include "TpTabWidget.h"
#include "TpVBoxLayout.h"
#include "TpDialog.h"
#include "TpTabBar.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true);//vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	TpTabWidget *tabWidget = new TpTabWidget(vScreen);
	tabWidget->addTab(new TpWidget(), "Tab1");
	tabWidget->addTab(new TpWidget(), "Tab2");
	
	TpTabBar *tabBar = tabWidget->tabBar();
    tabBar->setIcon(0, applicationDirPath() + "/最近-normal.png", applicationDirPath() + "/最近-focus.png");
    tabBar->setIcon(1, applicationDirPath() + "/浏览-normal.png", applicationDirPath() + "/浏览-focus.png");

	TpVBoxLayout *mainLayout = new TpVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);

    mainLayout->addWidget(tabWidget);
	
	vScreen->setLayout(mainLayout);
	
	vScreen->update();

	return app.run();
}