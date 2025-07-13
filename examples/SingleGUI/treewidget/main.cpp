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

#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpColors.h"
#include "tpTreeWidget.h"
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

	tpTreeWidget *edit = new tpTreeWidget(vScreen);
	// edit->setTextColor(_RGBA(255, 0, 0, 255));
	// edit->setText("This Is LineEdit");
	edit->setRect(30, 30, 200, 400);
	edit->setVisible(true);

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
			tpString("Test Item20")
		};

	for (const auto &itemText : itemStrList)
	{
		tpTreeWidgetItem *testListItem = new tpTreeWidgetItem(itemText);
		testListItem->addChild(new tpTreeWidgetItem("Child Item"));
		
		edit->addTopLevelItem(testListItem);
	}

	edit->addTopLevelItem(new tpTreeWidgetItem("Last Item"));

	std::string testItemStr = "Test Item1";
	tpString testItemStr2("Test Item1");

	// edit->addItem(new tpListWidgetItem("Test Item1"));
	// edit->addItem("Test Item1");

	edit->update();

	return app.run();
}
