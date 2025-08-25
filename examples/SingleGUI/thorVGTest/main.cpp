#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpEvent.h"
#include "TpCanvas.h"
#include "tpUtils.h"
#include "TpImage.h"
#include "tpBattery.h"
#include "tpLabel.h"
#include "tpTimer.h"
#include "tpDialog.h"

// class ThorVgPaintWidget : public tpChildWidget
class ThorVgPaintWidget : public tpDialog
{
public:
    ThorVgPaintWidget(tpChildWidget *parent) //: tpDialog(parent)
    {
        setBackGroundColor(_RGB(100, 100, 100));
        // setBackGroundImage(TpImage(applicationDirPath() + "/test.svg"));
        // setBackGroundImage(TpImage(applicationDirPath() + "/icon.png").rotate(45));

        // testBattery_ = new tpBattery(this);
        // testBattery_->setValue(100);

        // testLabel_ = new tpLabel(this);
        // testLabel_->setText("qqq");

        // testBattery_->setRect(10, 100, 200, 80);
        // testLabel_->setRect(10, 200, 300, 150);

        // testTimer_ = new tpTimer(3000);
        // connect(testTimer_, timeout, [=]()
        //         {
        //             int32_t batteryValue = testBattery_->value();
        //             batteryValue -= 10;
        //             if (batteryValue < 0)
        //             batteryValue = 100;

        //             std::cout << "刷新电量；当前值：" << batteryValue <<std::endl;
        //             testBattery_->setValue(batteryValue);

        //             testLabel_->setText(tpString::number(batteryValue)); });

        // testTimer_->start();
    }
    ~ThorVgPaintWidget()
    {
    }

    virtual bool onMousePressEvent(tpMouseEvent *event) override
    {
        // int32_t batteryValue = testBattery_->value();
        // batteryValue -= 10;
        // if (batteryValue < 0)
        //     batteryValue = 100;
        // testBattery_->setValue(batteryValue);

        // testLabel_->setText(tpString::number(batteryValue));

        update();

        return true;
    }

    virtual bool onMouseRleaseEvent(tpMouseEvent *event) override
    {
        tpDialog::onMouseRleaseEvent(event);
        return true;
    }

    virtual bool onPaintEvent(tpObjectPaintEvent *event) override
    {
        // static uint64_t paintCount = 0;
        // std::cout << "ThorVgPaintWidget::onPaintEvent " << paintCount++ << std::endl;

        tpDialog::onPaintEvent(event);

        TpCanvas *painter = event->canvas();
        painter->paintTest();

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

        // tpVector<ItpPoint> polygonPointList;
        // polygonPointList.emplace_back(ItpPoint(10, 330));
        // polygonPointList.emplace_back(ItpPoint(110, 350));
        // polygonPointList.emplace_back(ItpPoint(60, 400));

        // tpVector<ItpPoint> polygonPointList2;
        // polygonPointList2.emplace_back(ItpPoint(140, 330));
        // polygonPointList2.emplace_back(ItpPoint(170, 330));
        // polygonPointList2.emplace_back(ItpPoint(180, 360));
        // polygonPointList2.emplace_back(ItpPoint(120, 390));
        // polygonPointList2.emplace_back(ItpPoint(130, 350));

        // painter->polygon(polygonPointList, _RGB(150, 200, 168), 2);
        // painter->filledPolygon(polygonPointList2, _RGB(150, 200, 168));

        // painter->pie(260, 380, 50, 45, 170, _RGB(150, 200, 168), 5);
        // painter->filledPie(330, 450, 50, 225, 315, _RGB(150, 200, 168));

        // HollowMask hallowMask;
        // hallowMask.addRectHollow(ItpRect(10, 10, 30, 30));
        // hallowMask.addRoundRectHollow(ItpRect(50, 10, 30, 30), 10);
        // hallowMask.addCircleHollow(HollowMask::CircleHollow(110, 20, 15));
        // hallowMask.addPieHollow(HollowMask::PieHollow(40, 50, 0, 45, 30));
        // painter->hollowBox(320, 220, 480, 310, _RGB(150, 200, 168), hallowMask);

        // TpImage pngImage(applicationDirPath() + "/icon.png");
        // painter->paintImage(400, 415, pngImage.scaled(50, 50));
        // painter->paintImage(400, 340, pngImage.scaled(50, 50), 15);

        return true;
    }

private:
    tpBattery *testBattery_;
    tpLabel *testLabel_;

    tpTimer *testTimer_;
};

int32_t main(int32_t argc, char *argv[])
{
    tpApp app(argc, argv);

    tpFixScreen *vScreen = new tpFixScreen();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
    vScreen->setVisible(true); // vScreen setvisible will be update display
    app.bindVScreen(vScreen);

    ThorVgPaintWidget *thorVGPaint = new ThorVgPaintWidget(vScreen);
    thorVGPaint->setRect(100, 100, 500, 500);

    // tpBattery* testBattery = new tpBattery(vScreen);
    // testBattery->setRect(100, 100, 500, 500);

    vScreen->update();

    return app.run();
}
