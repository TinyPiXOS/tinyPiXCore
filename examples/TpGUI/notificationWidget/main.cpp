#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpDialog.h"
#include "TpFont.h"
#include "TpNotificationWidget.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(0, 255, 255, 255));
	vScreen->setVisible(true);//vScreen setvisible will be update display
	app.bindVScreen(vScreen);
	
	TpNotificationWidget* notificationWidget = new TpNotificationWidget(vScreen);
	notificationWidget->setRect(10, 10, 400, 600);
	
	TpNotificationItem* testItem1 = new TpNotificationItem();
	TpNotificationItem* testItem2 = new TpNotificationItem();

	testItem1->setItemData(TpNotificationItem::NotificationItemData("messageId1", "AppID1", "微信", "12:00", "震惊", "张三在路边捡到1块钱！"));
	testItem2->setItemData(TpNotificationItem::NotificationItemData("messageId2", "AppID2", "微博", "12:05", "原来是这样", "李四在路边捡到10块钱！"));

	notificationWidget->addItem(testItem1);
	notificationWidget->addItem(testItem2);

	// TpDialog *floatScreen = new TpDialog();
	// // floatScreen->setEnableBackGroundColor(true);
	// floatScreen->setBackGroundColor(_RGBA(255, 0, 0, 200));//or use TpColors
	// floatScreen->setRect(0, 0, 300, 300);
	// floatScreen->setAlpha(128);
	// floatScreen->setVisible(true);
	// floatScreen->setBeMoved(true);
	

	vScreen->update();
	// floatScreen->update();

	return app.run();
}
