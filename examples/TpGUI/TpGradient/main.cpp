#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpLinearGradient.h"
#include "TpRadialGradient.h"
#include "TpLabel.h"
#include "TpBrush.h"

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);

    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));

    TpLabel *lineGradientText = new TpLabel("线性渐变:", vScreen);
    lineGradientText->setRect(30, 30, 100, 50);

    TpLinearGradient lineGradient;
    lineGradient.setColorAt(0, _RGB(255, 0, 0));
    lineGradient.setColorAt(0.1, _RGB(252, 107, 1));
    lineGradient.setColorAt(0.2, _RGB(252, 226, 1));
    lineGradient.setColorAt(0.3, _RGB(158, 252, 1));
    lineGradient.setColorAt(0.4, _RGB(1, 252, 33));
    lineGradient.setColorAt(0.5, _RGB(2, 251, 201));
    lineGradient.setColorAt(0.6, _RGB(2, 139, 251));
    lineGradient.setColorAt(0.7, _RGB(1, 45, 252));
    lineGradient.setColorAt(0.8, _RGB(120, 1, 252));
    lineGradient.setColorAt(0.9, _RGB(252, 1, 252));
    lineGradient.setColorAt(1, _RGB(252, 1, 455));
    lineGradient.setStart(0, 25);
    lineGradient.setFinalStop(220, 25);

    TpLabel *lineGradientColor = new TpLabel(vScreen);
    lineGradientColor->setRect(150, 30, 220, 50);
    lineGradientColor->setBackGroundColor(TpBrush(&lineGradient));

    TpLabel *radialGradientText = new TpLabel("径向渐变:", vScreen);
    radialGradientText->setRect(30, 150, 100, 50);

    TpBrush tmpBrush;
    {
        {
            TpRadialGradient radialGradient;
            radialGradient.setColorAt(0, _RGB(255, 0, 0));
            radialGradient.setColorAt(0.1, _RGB(252, 107, 1));
            radialGradient.setColorAt(0.2, _RGB(252, 226, 1));
            radialGradient.setColorAt(0.3, _RGB(158, 252, 1));
            radialGradient.setColorAt(0.4, _RGB(1, 252, 33));
            radialGradient.setColorAt(0.5, _RGB(2, 251, 201));
            radialGradient.setColorAt(0.6, _RGB(2, 139, 251));
            radialGradient.setColorAt(0.7, _RGB(1, 45, 252));
            radialGradient.setColorAt(0.8, _RGB(120, 1, 252));
            radialGradient.setColorAt(0.9, _RGB(252, 1, 252));
            radialGradient.setColorAt(1, _RGB(252, 1, 455));
            radialGradient.setCenter(110, 110);
            radialGradient.setCenterRadius(110);
            radialGradient.setFocalPoint(110, 110);
            radialGradient.setFocalRadius(0);

            // TpLabel *radialGradientColor = new TpLabel(vScreen);
            // radialGradientColor->setRect(150, 150, 220, 220);
            // radialGradientColor->setRoundCorners(110);
            // radialGradientColor->setBackGroundColor(TpBrush(&radialGradient));

            TpBrush testBrush(&radialGradient);
            tmpBrush = testBrush;
        }

        TpLabel *radialGradientColor = new TpLabel(vScreen);
        radialGradientColor->setRect(150, 150, 220, 220);
        radialGradientColor->setRoundCorners(110);
        radialGradientColor->setBackGroundColor(tmpBrush);
    }

    vScreen->update();
    return app.run();
}
