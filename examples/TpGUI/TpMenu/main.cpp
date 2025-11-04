#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpFont.h"
#include "TpMenu.h"
#include "TpButton.h"

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);

    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));

    TpMenu *menu = new TpMenu();
    menu->addItem("Item11111");
    menu->addItem("Item22222");
    menu->addItem("Item33333");

    TpButton *showMenuBtn = new TpButton(vScreen);
    showMenuBtn->setText("显示弹出菜单");
    showMenuBtn->setProperty("type", "ControlPanelPowerButton");
    showMenuBtn->setSize(300, 64);
    showMenuBtn->move(20, 20);
    connect(showMenuBtn, onClicked, [&](bool)
            { menu->exec(330, 20); });

    vScreen->update();

    return app.run();
}
