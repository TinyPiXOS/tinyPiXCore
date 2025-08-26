/*** 
 * @Author: hywang
 * @Date: 2024-06-18 09:05:57
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-21 10:39:56
 * @FilePath: /pix-singlegui/tinyPiX-V7.0.2/demo/SingleGUI/treewidget/main.cpp
 * @Description: 
 * @
 * @PiXOS
 */

#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpColors.h"
#include "TpTreeWidget.h"
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

	TpTreeWidget *edit = new TpTreeWidget(vScreen);
	// edit->setTextColor(_RGBA(255, 0, 0, 255));
	// edit->setText("This Is LineEdit");
	edit->setRect(30, 30, 200, 400);
	edit->setVisible(true);

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
			TpString("Test Item20")
		};

	for (const auto &itemText : itemStrList)
	{
		TpTreeWidgetItem *testListItem = new TpTreeWidgetItem(itemText);
		testListItem->addChild(new TpTreeWidgetItem("Child Item"));
		
		edit->addTopLevelItem(testListItem);
	}

	edit->addTopLevelItem(new TpTreeWidgetItem("Last Item"));

	std::string testItemStr = "Test Item1";
	TpString testItemStr2("Test Item1");

	// edit->addItem(new TpListWidgetItem("Test Item1"));
	// edit->addItem("Test Item1");

	edit->update();

	return app.run();
}
