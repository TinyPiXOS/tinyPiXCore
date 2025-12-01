#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpSlideProgressBar.h"

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);

    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));

    // 声音进度条
    TpSlideProgressBar *voiceProgessBar = new TpSlideProgressBar(vScreen);
    voiceProgessBar->setIcon(applicationDirPath() + "/音量.png");
    voiceProgessBar->setRange(0, 100);
    voiceProgessBar->setValue(50);
    // voiceProgessBar->setRect(80, 80, 400, 65);
    voiceProgessBar->setRect(80, 80, 600, 400);

    // 亮度进度条
    // TpSlideProgressBar *lightProgessBar = new TpSlideProgressBar(vScreen);
    // lightProgessBar->setIcon(applicationDirPath() + "/亮度.png");
    // lightProgessBar->setRange(0, 100);
    // lightProgessBar->setValue(50);
    // lightProgessBar->setRect(80, 150, 400, 65);

    return app.run();
}
