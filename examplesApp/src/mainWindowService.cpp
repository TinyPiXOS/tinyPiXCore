#include "mainWindowService.h"
#include "testLight.h"
#include "TpPainter.h"
#include "TpTimer.h"

mainWindowService::mainWindowService()
    : TpFixScreen()
{
    setStyleSheet(applicationDirPath() + "/../data/style.css");

    // setBackGroundColor(_RGB(128, 128, 128));

    //     TpString testCSSStr = "TpButton {   \
//     gap: 5dp;   \
//     font-size: 15dp;    \
//     color: rgb(255, 128, 128);  \
//     background-color: rgb(255, 255, 255);   \
// }   \
// TpButton:hover {    \
//     background-color: rgb(128, 255, 128);   \
// }   \
// TpButton[type=\"TestCssType\"] {  \
//     height: 64dp;   \
//     width: 305dp;   \
//     font-size: 18dp;    \
//     color: rgb(128, 128, 255);  \
//     background-color: rgb(255, 255, 255);   \
//     border-radius: 32;  \
// }   \
// TpButton[type=\"TestCssType\"]:hover {    \
//     background-color: rgb(55, 55, 55);  \
// }";

    //     setStyleSheet(testCSSStr);

    //     testButton_ = new TpButton("测试按钮1", this);
    //     testButton_->setSize(300, 70);
    //     testButton_->move(150, 150);

    //     TpButton* testButton2_ = new TpButton("测试按钮2", this);
    //     testButton2_->setProperty("type", "TestCssType");
    //     testButton2_->setSize(300, 70);
    //     testButton2_->move(500, 150);

    //     testLight *light = new testLight(this);
    //     light->setSize(200, 50);
    //     light->move(150, 300);

    // TpTimer *animationTimer = new TpTimer(50);
    // connect(animationTimer, timeout, [=]()
    //         { update(); });
    // animationTimer->start();

    setBackGroundColor(_RGB(100, 100, 100));
    setBackGroundImage(TpImage("/home/hawk/Public/tinyPiXCore/examples/SingleGUI/thorVGTest/icon.png"), false);
}

mainWindowService::~mainWindowService()
{
}

bool mainWindowService::appChange(int32_t id, int32_t pid, int32_t visible, int32_t active, int32_t color, uint8_t alpha, int32_t require)
{
    std::cout << "mainWindowService::appChange" << std::endl;

    return true;
}

bool mainWindowService::onResizeEvent(TpResizeEvent *event)
{
    std::cout << "mainWindowService::onResizeEvent" << std::endl;

    return true;
}

bool mainWindowService::onActiveEvent(TpActiveEvent *event)
{
    std::cout << "mainWindowService::onActiveEvent" << std::endl;

    return true;
}

bool mainWindowService::onPaintEvent(TpPaintEvent *event)
{
    TpFixScreen::onPaintEvent(event);

    // TpPainter *paintCanvas = event->painter();

    // paintCanvas->sync();

    return true;
}
