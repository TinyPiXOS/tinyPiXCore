#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpEvent.h"
#include "TpPainter.h"
#include "TpUtils.h"
#include "TpImage.h"
#include "TpBattery.h"
#include "TpLabel.h"
#include "TpTimer.h"
#include "TpDialog.h"
#include "TpFont.h"
#include "TpLinearGradient.h"
#include "TpRadialGradient.h"
#include "TpSlider.h"
// #include "SmartDeviceGUI/Widgets/TpButton.h"
#include "TpGraphicsBlurEffect.h"

class ThorVgPaintWidget : public TpChildWidget
// class ThorVgPaintWidget : public TpDialog
{
public:
    ThorVgPaintWidget(TpChildWidget *parent) : TpChildWidget(parent)
    {
        setBackGroundColor(_RGBA(100, 100, 100, 200));
        // setBackGroundImage(TpImage(applicationDirPath() + "/test.svg"));
        // setBackGroundImage(TpImage(applicationDirPath() + "/icon.png"));
        setAlpha(150);
        testBattery_ = new TpBattery(this);
        testBattery_->setValue(100);

        // testLabel_ = new TpLabel(this);
        // testLabel_->setText("qqq");

        testBattery_->setRect(10, 100, 200, 80);
        // testLabel_->setRect(10, 200, 300, 150);

        // testTimer_ = new TpTimer(3000);
        // connect(testTimer_, timeout, [=]()
        //         {
        //             int32_t batteryValue = testBattery_->value();
        //             batteryValue -= 10;
        //             if (batteryValue < 0)
        //             batteryValue = 100;

        //             std::cout << "刷新电量；当前值：" << batteryValue <<std::endl;
        //             testBattery_->setValue(batteryValue);

        //             testLabel_->setText(TpString::number(batteryValue)); });

        // testTimer_->start();
    }
    ~ThorVgPaintWidget()
    {
    }

    virtual bool onMousePressEvent(TpMouseEvent *event) override
    {
        int32_t batteryValue = testBattery_->value();
        batteryValue -= 10;
        if (batteryValue < 0)
            batteryValue = 100;
        testBattery_->setValue(batteryValue);

        // testLabel_->setText(TpString::number(batteryValue));

        update();

        move(pos().x() + 10, pos().y());
        if (pos().x() + width() > 1080)
        {
            move(150, pos().y());
        }

        return true;
    }

    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override
    {
        TpChildWidget::onMouseRleaseEvent(event);
        return true;
    }

    virtual bool onPaintEvent(TpPaintEvent *event) override
    {
        // static uint64_t paintCount = 0;
        // std::cout << "ThorVgPaintWidget::onPaintEvent " << paintCount++ << std::endl;

        TpChildWidget::onPaintEvent(event);

        TpPainter *painter = event->painter();
        // painter->paintTest();

        // TpFont testFont;
        // testFont.setFontColor(_RGB(0, 255, 0), _RGB(0, 255, 0));
        // testFont.setFontSize(30);
        // painter->drawText(testFont, 10, 10, "你好tinyPiXOS");

        // TpLinearGradient lineGradient(0, 0, 290, 90);
        // lineGradient.setColorAt(0, _RGB(204, 143, 237));
        // lineGradient.setColorAt(1, _RGB(107, 80, 246));

        TpRadialGradient radialGradient(220, 220, 220);
        radialGradient.setColorAt(0, _RGB(204, 143, 237));
        radialGradient.setColorAt(1, _RGB(107, 80, 246));

        // painter->setGradient(&radialGradient);

        // painter->roundedBox(10, 10, 450, 450, 50, _RGB(0, 0, 0));

        // static int32_t width = 100;
        // painter->box(10, 10, 10 + width, 60, _RGB(150, 200, 168));

        // width -= 10;
        // if (width < 0)
        //     width = 100;

        // painter->hline(10, 490, 10, _RGB(255, 0, 0));
        // painter->vline(490, 10, 490, _RGB(0, 255, 0), 3);
        // painter->line(10, 10, 490, 490, _RGB(0, 0, 255), 4);

        // painter->roundedBox(10, 10, 100, 100, 50, _RGB(150, 200, 168));

        // for (int i = 0; i < 100; ++i)
        // {
        //     painter->pixel(150 + i, 150, _RGB(0, 255, 0));
        // }

        // painter->rectangle(120, 10, 220, 100, _RGB(150, 200, 168), 5);
        // painter->roundedRectangle(240, 10, 340, 100, 30, _RGB(150, 200, 168), 2);

        // painter->circle(50, 160, 50, _RGB(150, 200, 168));
        // painter->filledCircle(160, 160, 50, _RGB(150, 200, 168));

        // painter->ellipse(320, 160, 100, 50, _RGB(150, 200, 168));
        // painter->filledEllipse(100, 270, 100, 50, _RGB(150, 200, 168));

        // painter->arc(260, 270, 50, 135, 405, _RGB(150, 200, 168), 10, true);

        // TpVector<TpPoint> polygonPointList;
        // polygonPointList.emplace_back(TpPoint(10, 330));
        // polygonPointList.emplace_back(TpPoint(110, 350));
        // polygonPointList.emplace_back(TpPoint(60, 400));

        // TpVector<TpPoint> polygonPointList2;
        // polygonPointList2.emplace_back(TpPoint(140, 330));
        // polygonPointList2.emplace_back(TpPoint(170, 330));
        // polygonPointList2.emplace_back(TpPoint(180, 360));
        // polygonPointList2.emplace_back(TpPoint(120, 390));
        // polygonPointList2.emplace_back(TpPoint(130, 350));

        // painter->polygon(polygonPointList, _RGB(150, 200, 168), 2);
        // painter->filledPolygon(polygonPointList2, _RGB(150, 200, 168));

        // painter->pie(260, 380, 50, 45, 170, _RGB(150, 200, 168), 5);
        // painter->filledPie(330, 450, 50, 225, 315, _RGB(150, 200, 168));

        // TpHollowMask hallowMask;
        // hallowMask.addRectHollow(TpRect(10, 10, 30, 30));
        // hallowMask.addRoundRectHollow(TpRect(50, 10, 30, 30), 10);
        // hallowMask.addCircleHollow(TpHollowMask::CircleHollow(110, 20, 15));
        // hallowMask.addPieHollow(TpHollowMask::PieHollow(40, 50, 0, 45, 30));
        // painter->hollowBox(320, 220, 480, 310, _RGB(150, 200, 168), hallowMask);

        // TpImage pngImage(applicationDirPath() + "/icon.png");
        // painter->drawImage(400, 415, pngImage.scaled(50, 50));
        // painter->drawImage(400, 340, pngImage.scaled(50, 50), 15);

        return true;
    }

private:
    TpBattery *testBattery_;
    TpLabel *testLabel_;

    TpTimer *testTimer_;
};

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);
    app.setStyle(Tp::SmartDeviceGUIStyle);

    TpFixScreen *vScreen = new TpFixScreen();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
    vScreen->setVisible(true); // vScreen setvisible will be update display
    app.bindVScreen(vScreen);

    TpSlider *vSlider = new TpSlider(vScreen);
    vSlider->setDirection(TpSlider::Vertical);
    vSlider->setValue(50);
    vSlider->setSize(10, 500);
    vSlider->move(950, 20);

    ThorVgPaintWidget *thorVGPaint = new ThorVgPaintWidget(vScreen);
    thorVGPaint->setRect(600, 100, 500, 500);
    TpGraphicsBlurEffect btnBlurEffect;
    btnBlurEffect.setBlurRadius(15);
    thorVGPaint->setGraphicsEffect(btnBlurEffect);

    // TpBattery* testBattery = new TpBattery(vScreen);
    // testBattery->setRect(100, 100, 500, 500);

    // smartDeviceGUI::TpButton* testButton = new smartDeviceGUI::TpButton(vScreen);
    // testButton->setCheckable(true);
    // testButton->setText("蓝牙");
    // testButton->setRect(10, 10, 150, 50);

    // testButton->setRoundCorners(20);

    vScreen->update();

    return app.run();
}
