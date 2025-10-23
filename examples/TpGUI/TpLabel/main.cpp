#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpColors.h"
#include "TpLabel.h"
#include "TpDialog.h"
#include "TpFont.h"
#include "TpImage.h"

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);
    TpFixScreen *vScreen = new TpFixScreen();
    vScreen->setBackGroundColor(_RGBA(226, 226, 226, 255));
    app.bindVScreen(vScreen);
    vScreen->update();

#if 0
	TpLabel *label1 = new TpLabel(vScreen);
	label1->font()->setFontColor(_RGBA(255, 0, 0, 255), 0);
	label1->font()->setAntialias(TpFont::TINY_FONT_ANTIALIAS_GOOD);
	label1->font()->setFontWeight(TpFont::TINY_FONT_WEIGHT_THIN);
	label1->setText("TINY_FONT_WEIGHT_THIN");
	label1->setRect(0, 0, 250, 30);
	label1->setVisible(true);
	label1->update();

	TpLabel *label2 = new TpLabel(vScreen);
	label2->font()->setFontColor(_RGBA(0, 255, 0, 255), 0);
	label2->font()->setAntialias(TpFont::TINY_FONT_ANTIALIAS_GOOD);
	label1->font()->setFontWeight(TpFont::TINY_FONT_WEIGHT_ULTRALIGHT);
	label2->setAlign(Tp::AlignCenter);
	label2->setText("TINY_FONT_WEIGHT_ULTRALIGHT");
	label2->setRect(0, 30, 250, 30);
	label2->setVisible(true);
	label2->update();

	TpLabel *label3 = new TpLabel(vScreen);
	// label3->font()->setFontColor(_RGBA(0, 0, 255, 255), 0);
	label3->font()->setAntialias(TpFont::TINY_FONT_ANTIALIAS_GOOD);
	label1->font()->setFontWeight(TpFont::TINY_FONT_WEIGHT_LIGHT);
	label3->setAlign(Tp::AlignRight);
	label3->setText("TINY_FONT_WEIGHT_LIGHT");
	label3->setRect(0, 60, 250, 30);
	label3->setVisible(true);
	label3->update();

	TpLabel *label4 = new TpLabel(vScreen);
	// label4->font()->setFontSize(50);
	label1->font()->setFontWeight(TpFont::TINY_FONT_WEIGHT_SEMILIGHT);
	label4->setText("TINY_FONT_WEIGHT_SEMILIGHT");
	label4->setRect(0, 90, 250, 30);
	label4->setVisible(true);
	label4->update();

	TpList<TpFont::TpFontWeight> fontWeightValue =
		{
			TpFont::TINY_FONT_WEIGHT_BOOK,
			TpFont::TINY_FONT_WEIGHT_NORMAL,
			TpFont::TINY_FONT_WEIGHT_MEDIUM,
			TpFont::TINY_FONT_WEIGHT_SEMIBOLD,
			TpFont::TINY_FONT_WEIGHT_BOLD,
			TpFont::TINY_FONT_WEIGHT_ULTRABOLD,
			TpFont::TINY_FONT_WEIGHT_HEAVY,
			TpFont::TINY_FONT_WEIGHT_ULTRAHEAVY};
	TpList<TpString> fontWeightText =
		{
			"TINY_FONT_WEIGHT_BOOK",
			"TINY_FONT_WEIGHT_NORMAL",
			"TINY_FONT_WEIGHT_MEDIUM",
			"TINY_FONT_WEIGHT_SEMIBOLD",
			"TINY_FONT_WEIGHT_BOLD",
			"TINY_FONT_WEIGHT_ULTRABOLD",
			"TINY_FONT_WEIGHT_HEAVY",
			"TINY_FONT_WEIGHT_ULTRAHEAVY"};
	for (int i = 0; i < fontWeightValue.size(); ++i)
	{
		TpLabel *labelTmp = new TpLabel(vScreen);
		labelTmp->font()->setFontWeight(fontWeightValue.at(i));
		labelTmp->setText(fontWeightText.at(i));
		labelTmp->setRect(0, 120 + i * 30, 250, 30);
		labelTmp->setVisible(true);
		labelTmp->update();
	}

	TpLabel *label6 = new TpLabel(vScreen);
	label6->setBackGroundImage(TpImage(applicationDirPath() + "/test.svg"));
	// label6->setBackGroundColor(_RGB(255, 0, 0));
	// label6->setBlurRadius(20);
	label6->setRect(300, 300, 300, 300);
	// label6->setRoundCorners(100);
	label6->setVisible(true);
	label6->update();
#endif

    return app.run();
}
