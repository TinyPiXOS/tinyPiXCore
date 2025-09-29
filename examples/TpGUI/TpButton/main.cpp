#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpButton.h"
#include "TpVariant.h"
#include "TpCursor.h"

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);

    TpFixScreen *vScreen = new TpFixScreen();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
    app.bindVScreen(vScreen);

    TpButton *button1 = new TpButton("北京市", vScreen);
    button1->setProperty("type", "ControlPanelPowerButton");
    button1->setSize(300, 64);
    button1->move(150, 150);
    connect(button1, onClicked, [=](bool checked)
            { std::cout << "按钮被点击" << std::endl; std::cout << "当前鼠标坐标" << TpCursor::pos().x() << " , " <<TpCursor::pos().y()<<std::endl; });

    TpButton *buttonEnable = new TpButton("河北省", vScreen);
    buttonEnable->setProperty("type", "VirtualKeyboardButton");
    buttonEnable->setSize(305, 64);
    buttonEnable->move(460, 150);

    TpButton *button2 = new TpButton(vScreen);
    button2->setButtonStyle(TpButton::TextBesideIcon);
    button2->setProperty("type", "FunctionButton");
    button2->setIcon(applicationDirPath() + "/icon.png");
    button2->setText("吉林省2");
    button2->setRect(50, 250, 250, 50);

    TpButton *button3 = new TpButton(vScreen);
    button3->setButtonStyle(TpButton::IconOnly);
    button3->setIcon(applicationDirPath() + "/icon2.png");
    button3->setRoundCorners(13);
    button3->setRect(200, 350, 200, 200);

    vScreen->update();
    return app.run();
}
