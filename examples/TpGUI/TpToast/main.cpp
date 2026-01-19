#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpToast.h"
#include "TpFont.h"
#include "TpButton.h"

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);

    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));

    TpToast *toast = new TpToast();
    toast->setText("这是一个消息提示框");
    toast->setIcon(applicationDirPath() + "/音量.png");

    TpButton *button1 = new TpButton("显示消息", vScreen);
    button1->setSize(300, 64);
    button1->move(150, 150);
    connect(button1, onClicked, [=](bool checked)
            { toast->show(); });

    return app.run();
}
