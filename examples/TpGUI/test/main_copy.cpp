#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpEvent.h"
#include "TpPainter.h"
#include <TpGUI.h>
#include "TpImage.h"
#include "TpBattery.h"
#include "TpLabel.h"
#include "TpTimer.h"
#include "TpDialog.h"
#include "TpFont.h"
#include "TpLinearGradient.h"
#include "TpRadialGradient.h"
#include "TpSlider.h"
#include "TpGraphicsBlurEffect.h"
#include "TpButton.h"
#include "tinyPiXSys.h"
#include "tinyPiXUtils.h"
#include "TpMainWindow.h"

class ThorVgPaintWidget : public TpWidget
// class ThorVgPaintWidget : public TpDialog
{
public:
    ThorVgPaintWidget(TpWidget *parent)
        : TpWidget(parent)
    {
        setBackGroundColor(_RGBA(100, 100, 100, 180));
    }
    ~ThorVgPaintWidget()
    {
    }

    virtual bool onMousePressEvent(TpMouseEvent *event) override
    {
        std::cout << "pos: " << pos().x() << " , " << pos().y() << std::endl;

        move(pos().x() + 10, pos().y());
        if (pos().x() + width() > 1080)
        {
            move(150, pos().y());
        }

        TpImage grabImage = grabWindow();

        return true;
    }

    virtual bool onPaintEvent(TpPaintEvent *event) override
    {
        // static uint64_t paintCount = 0;
        // std::cout << "ThorVgPaintWidget::onPaintEvent " << paintCount++ << std::endl;

        TpWidget::onPaintEvent(event);
        // TpDialog::onPaintEvent(event);

        TpPainter *painter = event->painter();
        // painter->paintTest();

        painter->setPen(_RGB(255, 100, 100));
        painter->setBrush(TpBrush(_RGB(255, 100, 100)));
        painter->drawRect(10, 10, 200, 200);
        painter->drawText(TpFont(),10,10,"Test");
        return true;
    }

private:
};

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);
    app.setStyle(Tp::SmartDeviceGUIStyle);

    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 0));
   
    ThorVgPaintWidget *thorVGPaint = new ThorVgPaintWidget(vScreen);
    // thorVGPaint->setWindowOpacity(0.3);
    thorVGPaint->setRect(0, 0, 500, 500);

    return app.run();
}
