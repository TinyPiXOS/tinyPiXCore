#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpEvent.h"
#include "TpPainter.h"
#include "TpUtils.h"

class PaintPathWidget : public TpChildWidget
{
public:
    PaintPathWidget(TpChildWidget *parent) : TpChildWidget(parent)
    {
        setBackGroundColor(_RGBA(100, 100, 100, 200));
    }
    ~PaintPathWidget()
    {
    }

    virtual bool onPaintEvent(TpPaintEvent *event) override
    {
        TpChildWidget::onPaintEvent(event);

        TpPainter *painter = event->painter();

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
};

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);
    app.setStyle(Tp::SmartDeviceGUIStyle);

    TpFixScreen *vScreen = new TpFixScreen();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
    vScreen->setVisible(true); // vScreen setvisible will be update display
    app.bindVScreen(vScreen);

    PaintPathWidget *thorVGPaint = new PaintPathWidget(vScreen);
    thorVGPaint->setRect(100, 100, 500, 500);

    vScreen->update();

    return app.run();
}
