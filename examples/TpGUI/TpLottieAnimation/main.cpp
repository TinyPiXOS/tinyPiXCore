#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpLottieAnimation.h"
#include <TpGUI.h>

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);

    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
    
    

    TpLottieAnimation *lottieAnimation1 = new TpLottieAnimation(vScreen);
    // lottieAnimation1->setBackGroundColor(_RGB(100, 255, 100));
    lottieAnimation1->load(applicationDirPath() + "/旋转加载.json");
    lottieAnimation1->setRect(20, 20, 200, 200);

    TpLottieAnimation *lottieAnimation2 = new TpLottieAnimation(vScreen);
    lottieAnimation2->load(applicationDirPath() + "/散点加载.json");
    lottieAnimation2->setRect(250, 20, 200, 200);

    TpLottieAnimation *lottieAnimation3 = new TpLottieAnimation(vScreen);
    lottieAnimation3->load(applicationDirPath() + "/成功.json");
    lottieAnimation3->setRect(20, 270, 200, 200);

    TpLottieAnimation *lottieAnimation4 = new TpLottieAnimation(vScreen);
    lottieAnimation4->load(applicationDirPath() + "/圆形加载.json");
    lottieAnimation4->setRect(250, 270, 200, 200);

    vScreen->update();

    return app.run();
}
