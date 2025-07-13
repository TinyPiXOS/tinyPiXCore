#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpColors.h"
#include "tpLabel.h"
#include "tpDialog.h"
#include "tpFont.h"
#include "tpSurface.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(226, 226, 226, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display weekly
	app.bindVScreen(vScreen);
	vScreen->update();

	tpLabel *label1 = new tpLabel(vScreen);
	label1->font()->setFontColor(_RGBA(255, 0, 0, 255), 0);
	label1->font()->setAntialias(tpFont::TINY_FONT_ANTIALIAS_BEST);
	label1->font()->setFontWeight(tpFont::TINY_FONT_WEIGHT_THIN);
	label1->setText("TINY_FONT_WEIGHT_THIN");
	label1->setRect(0, 0, 250, 30);
	label1->setVisible(true);
	label1->update();

	tpLabel *label2 = new tpLabel(vScreen);
	label2->font()->setFontColor(_RGBA(0, 255, 0, 255), 0);
	label2->font()->setAntialias(tpFont::TINY_FONT_ANTIALIAS_GOOD);
	label1->font()->setFontWeight(tpFont::TINY_FONT_WEIGHT_ULTRALIGHT);
	label2->setAlign(tinyPiX::AlignCenter);
	label2->setText("TINY_FONT_WEIGHT_ULTRALIGHT");
	label2->setRect(0, 30, 250, 30);
	label2->setVisible(true);
	label2->update();

	tpLabel *label3 = new tpLabel(vScreen);
	// label3->font()->setFontColor(_RGBA(0, 0, 255, 255), 0);
	label3->font()->setAntialias(tpFont::TINY_FONT_ANTIALIAS_GOOD);
	label1->font()->setFontWeight(tpFont::TINY_FONT_WEIGHT_LIGHT);
	label3->setAlign(tinyPiX::AlignRight);
	label3->setText("TINY_FONT_WEIGHT_LIGHT");
	label3->setRect(0, 60, 250, 30);
	label3->setVisible(true);
	label3->update();

	tpLabel *label4 = new tpLabel(vScreen);
	// label4->font()->setFontSize(50);
	label1->font()->setFontWeight(tpFont::TINY_FONT_WEIGHT_SEMILIGHT);
	label4->setText("TINY_FONT_WEIGHT_SEMILIGHT");
	label4->setRect(0, 90, 250, 30);
	label4->setVisible(true);
	label4->update();

	tpList<tpFont::tpFontWeight> fontWeightValue =
		{
			tpFont::TINY_FONT_WEIGHT_BOOK,
			tpFont::TINY_FONT_WEIGHT_NORMAL,
			tpFont::TINY_FONT_WEIGHT_MEDIUM,
			tpFont::TINY_FONT_WEIGHT_SEMIBOLD,
			tpFont::TINY_FONT_WEIGHT_BOLD,
			tpFont::TINY_FONT_WEIGHT_ULTRABOLD,
			tpFont::TINY_FONT_WEIGHT_HEAVY,
			tpFont::TINY_FONT_WEIGHT_ULTRAHEAVY};
	tpList<tpString> fontWeightText =
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
		tpLabel *labelTmp = new tpLabel(vScreen);
		labelTmp->font()->setFontWeight(fontWeightValue.at(i));
		labelTmp->setText(fontWeightText.at(i));
		labelTmp->setRect(0, 120 + i * 30, 250, 30);
		labelTmp->setVisible(true);
		labelTmp->update();
	}

	tpLabel *label6 = new tpLabel(vScreen);
	tpShared<tpSurface> bgSurface = tpMakeShared<tpSurface>();
	// bgSurface->fromFile(applicationDirPath() + "/icon2.png");
	bgSurface->fromFile(applicationDirPath() + "/test.svg");
	label6->setBackGroundImage(bgSurface);
	// label6->setBackGroundColor(_RGB(255, 0, 0));
	// label6->setBlurRadius(20);
	label6->setRect(300, 300, 300, 300);
	// label6->setRoundCorners(100);
	label6->setVisible(true);
	label6->update();

	return app.run();
}
