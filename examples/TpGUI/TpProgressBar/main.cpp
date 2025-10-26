#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpProgressBar.h"
#include "TpButton.h"

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);
    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
    vScreen->setVisible(true); // vScreen setvisible will be update display1234567498

    app.bindVScreen(vScreen);
    vScreen->update();

    TpButton *button1 = new TpButton(vScreen);
    button1->setRect(100, 150, 50, 50);
    button1->setText("+");
    button1->update();

    TpButton *button2 = new TpButton(vScreen);
    button2->setRect(470, 150, 50, 50);
    button2->setText("-");
    button2->update();

    TpProgressBar *progressH = new TpProgressBar(vScreen);
    progressH->setRect(100, 100, 400, 20);

    TpProgressBar *progressV = new TpProgressBar(vScreen, TpProgressBar::Vertical);
    progressV->setRect(300, 140, 20, 400);

    connect(button1, onClicked, [=](bool)
            { 
                progressH->setValue(progressH->value() + 1);
                progressV->setValue(progressV->value() + 1); });
    connect(button2, onClicked, [=](bool)
            { 
                progressH->setValue(progressH->value() - 1);
                progressV->setValue(progressV->value() - 1); });

    return app.run();
}
