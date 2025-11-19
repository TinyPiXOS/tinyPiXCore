#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpGraphicsBlurEffect.h"
#include "TpButton.h"
#include "TpLabel.h"

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);

    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
    

    TpLabel *imgLabel = new TpLabel(vScreen);
    imgLabel->setRect(30, 30, 500, 500);
    imgLabel->setBackGroundImage(TpImage(applicationDirPath() + "/icon.png"));

    TpGraphicsBlurEffect btnBlurEffect;
    btnBlurEffect.setBlurRadius(15);
    imgLabel->setGraphicsEffect(btnBlurEffect);
    imgLabel->setEnableGraphicsEffect(false);

    TpButton *changeBtn = new TpButton("模糊/还原", vScreen);
    changeBtn->setRect(560, 265, 120, 40);
    connect(changeBtn, onClicked, [=](bool) {
        imgLabel->setEnableGraphicsEffect(!imgLabel->enableGraphicsEffect());
    });

    vScreen->update();
    return app.run();
}
