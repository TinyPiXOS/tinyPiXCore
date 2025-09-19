/***
 * @Author: hywang
 * @Date: 2024-06-04 16:14:43
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-06 16:57:18
 * @FilePath: /pix-singlegui/TinyPiX-V7.0.2/demo/SingleGUI/listwidget/main.cpp
 * @Description: ListWidget测试主函数
 * @
 * @PiXOS
 */

#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpColors.h"
#include "TpListWidget.h"
#include "TpDialog.h"
#include "TpFont.h"
#include "TpString.h"
#include "TpVector.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display weekly
	app.bindVScreen(vScreen);
	vScreen->update();

	TpListWidget *edit = new TpListWidget(vScreen);
	// edit->setTextColor(_RGBA(255, 0, 0, 255));
	// edit->setText("This Is LineEdit");

	std::vector<TpString> itemStrList =
		{
			TpString("Test Item1"),
			TpString("Test Item2"),
			TpString("Test Item3"),
			TpString("Test Item4"),
			TpString("Test Item5"),
			TpString("Test Item6"),
			TpString("Test Item7"),
			TpString("Test Item8"),
			TpString("Test Item9"),
			TpString("Test Item10"),
			TpString("Test Item11"),
			TpString("Test Item12"),
			TpString("Test Item13"),
			TpString("Test Item14"),
			TpString("Test Item15"),
			TpString("Test Item16"),
			TpString("Test Item17"),
			TpString("Test Item18"),
			TpString("Test Item19"),
			TpString("Test Item20"),
			TpString("Test Item21"),
			TpString("Test Item22"),
		};

	for (const auto &itemText : itemStrList)
	{
		TpListWidgetItem *testListItem = new TpListWidgetItem(itemText);

		edit->addItem(testListItem);
	}

	std::string testItemStr = "Test Item1";
	TpString testItemStr2("Test Item1");

	// edit->addItem(new TpListWidgetItem("Test Item1"));
	// edit->addItem("Test Item1");

	edit->setRect(30, 30, 200, 400);
	edit->setVisible(true);
	edit->update();

	edit->setSelectionModel(Tp::ExtendedSelection);
	// edit->sortItems(Tp::AscendingOrder);
	edit->sortItems(Tp::DescendingOrder);

	// TpLabel *label2 = new TpLabel(vScreen);
	// label2->font()->setFontColor(_RGBA(0, 255, 0, 255), 0);
	// label2->font()->setAntialias(TpFont::TINY_FONT_ANTIALIAS_GOOD);
	// label2->setAlign(TpLabel::TP_ALIGN_CENTER);
	// label2->setText("传说中的标签:align-center");
	// label2->setRect(0, 30, 250, 30);
	// label2->setVisible(true);
	// label2->update();

	// TpLabel *label3 = new TpLabel(vScreen);
	// //label3->font()->setFontColor(_RGBA(0, 0, 255, 255), 0);
	// label3->font()->setAntialias(TpFont::TINY_FONT_ANTIALIAS_GOOD);
	// label3->setAlign(TpLabel::TP_ALIGN_RIGHT);
	// label3->setText("传说中的标签:align-right");
	// label3->setRect(0, 60, 250, 30);
	// label3->setVisible(true);
	// label3->update();

	// TpLabel *label4 = new TpLabel(vScreen);
	// label4->font()->setFontSize(50);
	// label4->setText("TinyPiX");
	// label4->setRect(0, 200, 500, 120);
	// label4->setVisible(true);
	// label4->update();

	// TpLabel *label5 = new TpLabel(vScreen);
	// label5->font()->setAntialias(TpFont::TINY_FONT_ANTIALIAS_GOOD);
	// label5->setAlign(TpLabel::TP_ALIGN_RIGHT);
	// label5->setAutoFit(true);
	// label5->setText("传说中的标签:align-auto fit");
	// label5->setRect(0, 90, 250, 30);
	// label5->setVisible(true);
	// label5->update();

	return app.run();
}
