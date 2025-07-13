#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpDialog.h"
#include "tpFont.h"
#include "tpNotificationWidget.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(0, 255, 255, 255));
	vScreen->setVisible(true);//vScreen setvisible will be update display
	app.bindVScreen(vScreen);
	
	tpNotificationWidget* notificationWidget = new tpNotificationWidget(vScreen);
	notificationWidget->setRect(10, 10, 400, 600);
	
	tpNotificationItem* testItem1 = new tpNotificationItem();
	tpNotificationItem* testItem2 = new tpNotificationItem();

	testItem1->setItemData(tpNotificationItem::NotificationItemData("messageId1", "AppID1", "微信", "12:00", "震惊", "张三在路边捡到1块钱！"));
	testItem2->setItemData(tpNotificationItem::NotificationItemData("messageId2", "AppID2", "微博", "12:05", "原来是这样", "李四在路边捡到10块钱！"));

	notificationWidget->addItem(testItem1);
	notificationWidget->addItem(testItem2);

	// tpDialog *floatScreen = new tpDialog();
	// // floatScreen->setEnableBackGroundColor(true);
	// floatScreen->setBackGroundColor(_RGBA(255, 0, 0, 200));//or use tpColors
	// floatScreen->setRect(0, 0, 300, 300);
	// floatScreen->setAlpha(128);
	// floatScreen->setVisible(true);
	// floatScreen->setBeMoved(true);
	

	vScreen->update();
	// floatScreen->update();

	return app.run();
}
