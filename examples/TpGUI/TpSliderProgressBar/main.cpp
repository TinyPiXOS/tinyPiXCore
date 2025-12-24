#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpSliderProgressBar.h"
#include "TpLabel.h"

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);

    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));

    // 声音进度条
    TpSliderProgressBar *bar1 = new TpSliderProgressBar(vScreen);
    bar1->setRect(80, 80, 400, 30);
    bar1->setRange(0, 100);
    bar1->setValue(30);

    TpLabel *label1 = new TpLabel(vScreen);
    label1->setAlign(Tp::AlignCenter);
    label1->setText(TpString::number(bar1->value()));
    label1->setRect(500, 80, 80, 30);

    connect(bar1, valueChanged, [=](int32_t value)
            { label1->setText(TpString::number(value)); });

    TpSliderProgressBar *bar2 = new TpSliderProgressBar(vScreen);
    bar2->setRange(0, 100);
    bar2->setValue(80);
    bar2->setRect(80, 150, 400, 30);

    TpLabel *label2 = new TpLabel(vScreen);
    label2->setAlign(Tp::AlignCenter);
    label2->setText(TpString::number(bar2->value()));
    label2->setRect(500, 150, 80, 30);

    connect(bar2, valueChanged, [=](int32_t value)
            { label2->setText(TpString::number(value)); });

    return app.run();
}
