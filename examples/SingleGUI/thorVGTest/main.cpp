#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpEvent.h"
#include "tpCanvas.h"
#include "tpUtils.h"

class ThorVgPaintWidget : public tpChildWidget
{
public:
    ThorVgPaintWidget(tpChildWidget *parent) : tpChildWidget(parent)
    {
        // setBackGroundColor(_RGB(100, 100, 100));
    }
    ~ThorVgPaintWidget()
    {
    }

    virtual bool onPaintEvent(tpObjectPaintEvent *event) override
    {
        tpChildWidget::onPaintEvent(event);

        tpCanvas *painter = event->canvas();

        painter->hline(10, 490, 10, _RGB(255, 0, 0));
        painter->vline(490, 10, 490, _RGB(0, 255, 0), 3);
        painter->line(10, 10, 490, 490, _RGB(0, 0, 255), 4);

        painter->roundedBox(10, 10, 100, 100, 50, _RGB(150, 200, 168));

        for (int i = 0; i < 100; ++i)
        {
            painter->pixel(150 + i, 150, _RGB(0, 255, 0));
        }

        painter->rectangle(120, 10, 220, 100, _RGB(150, 200, 168), 5);
        painter->roundedRectangle(240, 10, 340, 100, 30, _RGB(150, 200, 168), 2);

        painter->circle(50, 160, 50, _RGB(150, 200, 168));
        painter->filledCircle(160, 160, 50, _RGB(150, 200, 168));

        painter->ellipse(320, 160, 100, 50, _RGB(150, 200, 168));
        painter->filledEllipse(100, 270, 100, 50, _RGB(150, 200, 168));

        painter->arc(260, 270, 50, 135, 405, _RGB(150, 200, 168), 10, true);

        tpVector<ItpPoint> polygonPointList;
        polygonPointList.emplace_back(ItpPoint(10, 330));
        polygonPointList.emplace_back(ItpPoint(110, 350));
        polygonPointList.emplace_back(ItpPoint(60, 400));

        tpVector<ItpPoint> polygonPointList2;
        polygonPointList2.emplace_back(ItpPoint(140, 330));
        polygonPointList2.emplace_back(ItpPoint(170, 330));
        polygonPointList2.emplace_back(ItpPoint(180, 360));
        polygonPointList2.emplace_back(ItpPoint(120, 390));
        polygonPointList2.emplace_back(ItpPoint(130, 350));

        painter->polygon(polygonPointList, _RGB(150, 200, 168), 2);
        painter->filledPolygon(polygonPointList2, _RGB(150, 200, 168));

        painter->pie(260, 380, 50, 45, 170, _RGB(150, 200, 168), 5);
        painter->filledPie(330, 380, 50, 225, 315, _RGB(150, 200, 168));

        return true;
    }
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

    vScreen->update();

    return app.run();
}
