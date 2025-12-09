#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpVariant.h"
#include "TpLabel.h"
#include "TpTabWidget.h"
#include "TpTabBar.h"
#include "TpFont.h"
#include "TpVBoxLayout.h"

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);
    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));

    TpTabWidget *tabWidget = new TpTabWidget(vScreen);
    TpWidget *tab1 = new TpWidget();
    tab1->setBackGroundColor(_RGB(255, 150, 150));
    TpWidget *tab2 = new TpWidget();
    tab2->setBackGroundColor(_RGB(150, 255, 150));

    TpLabel *testLabel1 = new TpLabel(tab1);
    testLabel1->setBackGroundColor(_RGB(255, 0, 0));
    testLabel1->setAlign(Tp::AlignCenter);
    testLabel1->font()->setFontSize(19);
    testLabel1->setText("Tab111");
    testLabel1->setRect(50, 50, 100, 50);

    TpLabel *testLabel2 = new TpLabel(tab2);
    testLabel2->setBackGroundColor(_RGB(255, 0, 0));
    testLabel2->setAlign(Tp::AlignCenter);
    testLabel2->font()->setFontSize(19);
    testLabel2->setText("Tab222");
    testLabel2->setRect(50, 50, 100, 50);

    tabWidget->addTab(tab1, "Tab1");
    tabWidget->addTab(tab2, "Tab2");

    TpTabBar *tabBar = tabWidget->tabBar();
    tabBar->setIcon(0, applicationDirPath() + "/最近-normal.png", applicationDirPath() + "/最近-focus.png");
    tabBar->setIcon(1, applicationDirPath() + "/浏览-normal.png", applicationDirPath() + "/浏览-focus.png");

    TpVBoxLayout *mainLayout = new TpVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(tabWidget);

    vScreen->setLayout(mainLayout);

    return app.run();
}