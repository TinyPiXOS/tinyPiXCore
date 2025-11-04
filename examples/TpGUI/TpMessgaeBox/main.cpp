#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpButton.h"
#include "TpMessageBox.h"

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);
    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));

    TpMessageBox infoMsg("消息弹出框");
    TpButton *button1 = new TpButton("消息框", vScreen);
    button1->setRect(20, 20, 100, 50);
    connect(button1, onClicked, [&](bool)
            { infoMsg.exec(); });

    TpMessageBox warningMsg("警告弹出框", TpMessageBox::Warning);
    TpButton *button2 = new TpButton("警告框", vScreen);
    button2->setRect(20, 80, 100, 50);
    connect(button2, onClicked, [&](bool)
            { warningMsg.exec(); });

    TpMessageBox questiongMsg("询问弹出框", TpMessageBox::Question);
    TpButton *button3 = new TpButton("询问框", vScreen);
    button3->setRect(20, 140, 100, 50);
    connect(button3, onClicked, [&](bool)
            { questiongMsg.exec(); });

    TpMessageBox errorMsg("错误弹出框", TpMessageBox::Error);
    TpButton *button4 = new TpButton("错误框", vScreen);
    button4->setRect(20, 200, 100, 50);
    connect(button4, onClicked, [&](bool)
            { errorMsg.exec(); });

    vScreen->update();
    return app.run();
}
