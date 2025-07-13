/***
 * @Author: hywang
 * @Date: 2024-06-04 16:14:43
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-06 16:57:18
 * @FilePath: /pix-singlegui/tinyPiX-V7.0.2/demo/SingleGUI/listwidget/main.cpp
 * @Description: ListWidget测试主函数
 * @
 * @PiXOS
 */

#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpColors.h"
#include "tpListWidget.h"
#include "tpDialog.h"
#include "tpFont.h"
#include "tpString.h"
#include "tpVector.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display weekly
	app.bindVScreen(vScreen);
	vScreen->update();

	tpListWidget *edit = new tpListWidget(vScreen);
	// edit->setTextColor(_RGBA(255, 0, 0, 255));
	// edit->setText("This Is LineEdit");

	std::vector<tpString> itemStrList =
		{
			tpString("Test Item1"),
			tpString("Test Item2"),
			tpString("Test Item3"),
			tpString("Test Item4"),
			tpString("Test Item5"),
			tpString("Test Item6"),
			tpString("Test Item7"),
			tpString("Test Item8"),
			tpString("Test Item9"),
			tpString("Test Item10"),
			tpString("Test Item11"),
			tpString("Test Item12"),
			tpString("Test Item13"),
			tpString("Test Item14"),
			tpString("Test Item15"),
			tpString("Test Item16"),
			tpString("Test Item17"),
			tpString("Test Item18"),
			tpString("Test Item19"),
			tpString("Test Item20"),
			tpString("Test Item21"),
			tpString("Test Item22"),
		};

	for (const auto &itemText : itemStrList)
	{
		tpListWidgetItem *testListItem = new tpListWidgetItem(itemText);

		edit->addItem(testListItem);
	}

	std::string testItemStr = "Test Item1";
	tpString testItemStr2("Test Item1");

	// edit->addItem(new tpListWidgetItem("Test Item1"));
	// edit->addItem("Test Item1");

	edit->setRect(30, 30, 200, 400);
	edit->setVisible(true);
	edit->update();

	edit->setSelectionModel(tinyPiX::ExtendedSelection);
	// edit->sortItems(tinyPiX::AscendingOrder);
	edit->sortItems(tinyPiX::DescendingOrder);

	// tpLabel *label2 = new tpLabel(vScreen);
	// label2->font()->setFontColor(_RGBA(0, 255, 0, 255), 0);
	// label2->font()->setAntialias(tpFont::TINY_FONT_ANTIALIAS_GOOD);
	// label2->setAlign(tpLabel::TP_ALIGN_CENTER);
	// label2->setText("传说中的标签:align-center");
	// label2->setRect(0, 30, 250, 30);
	// label2->setVisible(true);
	// label2->update();

	// tpLabel *label3 = new tpLabel(vScreen);
	// //label3->font()->setFontColor(_RGBA(0, 0, 255, 255), 0);
	// label3->font()->setAntialias(tpFont::TINY_FONT_ANTIALIAS_GOOD);
	// label3->setAlign(tpLabel::TP_ALIGN_RIGHT);
	// label3->setText("传说中的标签:align-right");
	// label3->setRect(0, 60, 250, 30);
	// label3->setVisible(true);
	// label3->update();

	// tpLabel *label4 = new tpLabel(vScreen);
	// label4->font()->setFontSize(50);
	// label4->setText("tinyPiX");
	// label4->setRect(0, 200, 500, 120);
	// label4->setVisible(true);
	// label4->update();

	// tpLabel *label5 = new tpLabel(vScreen);
	// label5->font()->setAntialias(tpFont::TINY_FONT_ANTIALIAS_GOOD);
	// label5->setAlign(tpLabel::TP_ALIGN_RIGHT);
	// label5->setAutoFit(true);
	// label5->setText("传说中的标签:align-auto fit");
	// label5->setRect(0, 90, 250, 30);
	// label5->setVisible(true);
	// label5->update();

	return app.run();
}
