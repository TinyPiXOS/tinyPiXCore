#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpColors.h"
#include "TpLabel.h"
#include "TpDialog.h"
#include "TpFont.h"
#include "TpImage.h"

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);
    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(226, 226, 226, 255));

    TpLabel *label1 = new TpLabel(vScreen);
    label1->font()->setFontColor(_RGB(255, 0, 0));
    label1->setText("默认字体");
    label1->setRect(50, 50, label1->font()->pixelWidth(), 30);

    TpLabel *label2 = new TpLabel(vScreen);
    label2->font()->setFontColor(_RGB(0, 255, 0));
    label2->font()->setBold(true);
    label2->setAlign(Tp::AlignCenter);
    label2->setText("加粗字体");
    label2->setRect(50, 80, 250, 30);

    TpLabel *label3 = new TpLabel(vScreen);
    label3->font()->setFontColor(_RGB(0, 255, 0));
    label3->font()->setItalic(true);
    label3->setAlign(Tp::AlignRight);
    label3->setText("斜体");
    label3->setRect(50, 110, 250, 30);

    TpLabel *label6 = new TpLabel(vScreen);
    label6->setBackGroundImage(TpImage(applicationDirPath() + "/test.svg"));
    label6->setRect(300, 300, 300, 300);

    vScreen->update();

    return app.run();
}
