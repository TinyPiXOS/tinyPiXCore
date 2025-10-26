#include "TpApp.h"
#include "TpMainWindow.h"
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
#include "TpGraphicsBlurEffect.h"
#include "TpButton.h"
#include "tinyPiXSys.h"
#include "tinyPiXUtils.h"
#include "TpSurface.h"
#include "png.h"
#include "TpMainWindow.h"

// class ThorVgPaintWidget : public TpWidget
class ThorVgPaintWidget : public TpDialog
{
public:
    ThorVgPaintWidget(TpWidget *parent)
    // : TpWidget(parent)
    {
        // setBackGroundColor(_RGBA(100, 100, 100, 200));
        // setBackGroundImage(TpImage(applicationDirPath() + "/test.svg"));
        setBackGroundImage(TpImage(applicationDirPath() + "/icon.png"));
        // setAlpha(150);
        testBattery_ = new TpBattery(this);
        testBattery_->setValue(100);
        testBattery_->setRect(10, 100, 200, 80);

        testBattery_->setVisible(false);
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

        std::cout << "pos: " << pos().x() << " , " << pos().y() << std::endl;

        move(pos().x() + 10, pos().y());
        if (pos().x() + width() > 1080)
        {
            move(150, pos().y());
        }

        TpImage grabImage = grabWindow();

        // static int32_t saveIndexS = 0;
        // TpString savePngPath = "/home/hawk/Public/TinyPiXOS/examples/TpGUI/test/grapWindow_" + std::to_string(saveIndexS++) + ".png";
        // grabImage.save(savePngPath, TpImage::PNG_FMT);

        // update();

        return true;
    }

    virtual bool onPaintEvent(TpPaintEvent *event) override
    {
        // static uint64_t paintCount = 0;
        // std::cout << "ThorVgPaintWidget::onPaintEvent " << paintCount++ << std::endl;

        // TpWidget::onPaintEvent(event);
        TpDialog::onPaintEvent(event);

        TpPainter *painter = event->painter();
        // painter->paintTest();

        // 测试 1: 默认构造函数和 moveTo/lineTo
        {
            TpPainterPath path;
            path.moveTo(TpPoint(50, 50));
            path.lineTo(TpPoint(150, 50));
            path.lineTo(TpPoint(150, 150));
            path.lineTo(TpPoint(50, 150));
            path.closeSubpath();

            painter->setPen(TpPen(TpColors::Red, 2));
            painter->setBrush(TpBrush(TpColors::LightGray));
            painter->drawPath(path);
        }

        // 测试 2: 带起始点的构造函数和 cubicTo
        {
            TpPainterPath path(TpPoint(200, 100));
            path.cubicTo(TpPoint(250, 50), TpPoint(300, 150), TpPoint(350, 100));

            painter->setPen(TpPen(TpColors::Blue, 3));
            painter->setBrush(TpBrush(TpColors::Transparent));
            painter->drawPath(path);
        }

        // 测试 3: addRect
        {
            TpPainterPath path;
            path.addRect(TpRect(250, 200, 100, 80));

            painter->setPen(TpPen(TpColors::Green, 2));
            painter->setBrush(TpBrush(TpColors::Yellow));
            painter->drawPath(path);
        }

        // 测试 4: addEllipse
        {
            TpPainterPath path;
            path.addEllipse(TpRect(100, 250, 120, 80));

            painter->setPen(TpPen(TpColors::Purple, 2));
            painter->setBrush(TpBrush(TpColors::LightBlue));
            painter->drawPath(path);
        }

        // 测试 5: addRoundedRect
        {
            TpPainterPath path;
            path.addRoundedRect(TpRect(300, 300, 120, 80), 20);

            painter->setPen(TpPen(TpColors::DarkGreen, 2));
            painter->setBrush(TpBrush(TpColors::LightGreen));
            painter->drawPath(path);
        }

        // 测试 6: addArc
        {
            TpPainterPath path;
            path.addArc(TpPoint(400, 200), 50, 0, 270);

            painter->setPen(TpPen(TpColors::Orange, 3));
            painter->setBrush(TpBrush(TpColors::Transparent));
            painter->drawPath(path);
        }

        // 测试 7: addPie
        {
            TpPainterPath path;
            path.addPie(TpPoint(150, 400), 60, 45, 270);

            painter->setPen(TpPen(TpColors::DarkRed, 2));
            painter->setBrush(TpBrush(TpColors::Pink));
            painter->drawPath(path);
        }

        // 测试 8: 运算符重载 (+)
        {
            TpPainterPath path1;
            path1.addRect(TpRect(350, 400, 50, 50));

            TpPainterPath path2;
            path2.addEllipse(TpRect(375, 425, 50, 50));

            TpPainterPath combinedPath = path1 + path2;

            painter->setPen(TpPen(TpColors::DarkBlue, 2));
            painter->setBrush(TpBrush(TpColors::LightYellow));
            painter->drawPath(combinedPath);
        }

        // 测试 9: 复杂路径组合
        {
            TpPainterPath path;
            path.moveTo(TpPoint(200, 50));
            path.lineTo(TpPoint(250, 50));
            path.cubicTo(TpPoint(275, 75), TpPoint(275, 125), TpPoint(250, 150));
            path.lineTo(TpPoint(200, 150));
            path.cubicTo(TpPoint(175, 125), TpPoint(175, 75), TpPoint(200, 50));
            path.closeSubpath();

            painter->setPen(TpPen(TpColors::Black, 2));
            painter->setBrush(TpBrush(TpColors::Cyan));
            painter->drawPath(path);
        }

        return true;
    }

private:
    TpBattery *testBattery_;
};

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);
    app.setStyle(Tp::SmartDeviceGUIStyle);

    TpMainWindow *vScreen = new TpMainWindow();

    // TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
    // vScreen->setBackGroundImage(TpImage(applicationDirPath() + "/test.svg"));
    // vScreen->setBackGroundImage(TpImage(applicationDirPath() + "/icon.png"));
    app.bindVScreen(vScreen);

    TpDialog *dia = new TpDialog();
    dia->setBackGroundColor(_RGBA(243, 243, 243, 100));
    dia->setRect(0, 0, 300, 300);
    dia->setAlpha(128);
    dia->setRoundCorners(50);
    dia->setVisible(true);
    dia->setBeMoved(true);

    // TpLabel *bgLabel = new TpLabel(vScreen);
    // bgLabel->setBackGroundColor(_RGB(200, 80, 80));
    // bgLabel->setRect(300, 50, 500, 500);

    // TestClass *testObj = new TestClass();
    // testObj->setBgLabel(bgLabel);

    // TpButton *testBtn = new TpButton(vScreen);
    // testBtn->setText("获取截图");
    // testBtn->setRect(50, 50, 150, 50);
    // connect(testBtn, onClicked, testObj, &TestClass::SlotTestFunc);

    // ThorVgPaintWidget *thorVGPaint = new ThorVgPaintWidget(vScreen);
    // thorVGPaint->setRect(600, 100, 500, 500);
    // TpGraphicsBlurEffect btnBlurEffect;
    // btnBlurEffect.setBlurRadius(15);
    // thorVGPaint->setGraphicsEffect(btnBlurEffect);

    vScreen->update();

    return app.run();
}
