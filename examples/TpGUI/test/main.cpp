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
#include "TpSurface.h"
#include "png.h"
#include "TpMainWindow.h"
#include "Service/TpSystemApi.h"

class ThorVgPaintWidget : public TpWidget
// class ThorVgPaintWidget : public TpDialog
{
public:
    ThorVgPaintWidget(TpWidget *parent)
        : TpWidget(parent)
    {
        setBackGroundColor(_RGBA(100, 100, 100, 200));
        // setBackGroundImage(TpImage(applicationDirPath() + "/test.svg"));
        // setBackGroundImage(TpImage(applicationDirPath() + "/icon.png"));
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

        TpFont testFont;
        testFont.setText("这是一个测试字符串");
        testFont.setFontSize(15);
        testFont.setFontColor(_RGB(220, 0, 0));

        painter->drawText(testFont, 20, 20);

        return true;
    }

private:
};

IPiSysApiAgent *globalAgent = tinyPiX_sys_create();

TpImage getImage()
{
    IPiWFSurface *surfacePtr = tinyPiX_sys_get_process_surface(globalAgent, getpid());
    if (!surfacePtr)
        return TpImage();

    tpShared<TpSurface> appDisplayImage = tpMakeShared<TpSurface>(surfacePtr);

    TpImage resImage;
    resImage.load(appDisplayImage->matrix(), TpSize(appDisplayImage->width(), appDisplayImage->height()),
                  TpRect(0, 36, appDisplayImage->width(), appDisplayImage->height() - 36));

    TpImage copyImage = resImage;

    tinyPiX_surface_free(surfacePtr);

    return copyImage;
}

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);
    app.setStyle(Tp::SmartDeviceGUIStyle);

    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
    // vScreen->setBackGroundImage(TpImage(applicationDirPath() + "/icon.png"));

    // TpLabel *bgLabel = new TpLabel(vScreen);
    // bgLabel->setBorderColor(_RGB(255, 0, 0));
    // bgLabel->setRect(250, 50, 450, 450);

    // TpButton *testBtn = new TpButton(vScreen);
    // testBtn->setText("获取当前进程截图");
    // testBtn->setRect(50, 50, 150, 50);
    // connect(testBtn, onClicked, [=](bool)
    //         { bgLabel->setBackGroundImage(getImage()); });

    // ThorVgPaintWidget *thorVGPaint = new ThorVgPaintWidget(vScreen);
    // thorVGPaint->setRect(0, 0, 500, 500);

    TpLabel *textTestLabel = new TpLabel("自动获取", vScreen);
    textTestLabel->setBackGroundColor(_RGB(255, 0, 0));
    textTestLabel->setAlign(Tp::AlignLeft);
    textTestLabel->font()->setFontSize(19);
    textTestLabel->setRect(520, 20, textTestLabel->font()->pixelWidth(), textTestLabel->font()->pixelHeight());

    TpLabel *textTestLabel2 = new TpLabel("以太网", vScreen);
    textTestLabel2->setBackGroundColor(_RGB(255, 0, 0));
    textTestLabel2->font()->setFontSize(19);
    textTestLabel2->setAlign(Tp::AlignBottom);
    textTestLabel2->setRect(520, 200, textTestLabel2->font()->pixelWidth(), textTestLabel2->font()->pixelHeight());

    TpVariant testVariant(10);
    std::cout << ":::" << testVariant.toInt8() << std::endl;
    std::cout << ":::" << testVariant.toInt16() << std::endl;
    std::cout << ":::" << testVariant.toInt32() << std::endl;
    std::cout << ":::" << testVariant.toInt64() << std::endl;

    vScreen->update();

    return app.run();
}
