#include "MainWindowService.h"
#include "TpPainter.h"

MainWindowService::MainWindowService()
    : TpFixScreen()
{
    setStyleSheet(applicationDirPath() + "/../data/style.css");
    setBackGroundColor(_RGB(100, 100, 100));

    testLight_ = new TpTestLight(this);
    testLight_->setRect(100, 100, 200, 50);
}

MainWindowService::~MainWindowService()
{
}

bool MainWindowService::appChange(int32_t id, int32_t pid, int32_t visible, int32_t active, int32_t color, uint8_t alpha, int32_t require)
{
    std::cout << "mainWindowService::appChange" << std::endl;

    return true;
}

bool MainWindowService::onResizeEvent(TpResizeEvent *event)
{
    std::cout << "mainWindowService::onResizeEvent" << std::endl;

    return true;
}

bool MainWindowService::onActiveEvent(TpActiveEvent *event)
{
    std::cout << "mainWindowService::onActiveEvent" << std::endl;

    return true;
}

bool MainWindowService::onPaintEvent(TpPaintEvent *event)
{
    TpFixScreen::onPaintEvent(event);

    return true;
}
