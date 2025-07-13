#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpButton.h"
#include "tpAnimation.h"
#include "tpDialog.h"
#include "tpLabel.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	// 效果展示窗体
	tpDialog *exampleWidget = new tpDialog();
	// tpChildWidget *exampleWidget = new tpChildWidget(vScreen);
	exampleWidget->setBackGroundColor(_RGB(255, 255, 255));
	exampleWidget->setRect(350, 20, 100, 100);

	tpButton *posMoveBtn = new tpButton("平移", vScreen);
	posMoveBtn->setProperty("type", "ControlPanelPowerButton");
	posMoveBtn->setSize(150, 40);
	posMoveBtn->move(20, 20);

	connect(posMoveBtn, onClicked, [=](bool)
			{
				tpAnimation* widgetAnimation = new tpAnimation(exampleWidget, tpAnimation::Pos);
				widgetAnimation->setStartValue(ItpPoint(350, 20));
				widgetAnimation->setEndValue(ItpPoint(700, 300));
				widgetAnimation->setDuration(1000);
				widgetAnimation->start(); });

	tpButton *returnMoveBtn = new tpButton("往返平移", vScreen);
	returnMoveBtn->setProperty("type", "ControlPanelPowerButton");
	returnMoveBtn->setSize(150, 40);
	returnMoveBtn->move(180, 20);

	connect(returnMoveBtn, onClicked, [=](bool)
			{
				tpAnimation* widgetAnimation = new tpAnimation(exampleWidget, tpAnimation::Pos);
				widgetAnimation->setStartValue(ItpPoint(350, 20));
				widgetAnimation->setKeyValueAt(0.5, ItpPoint(700, 300));
				widgetAnimation->setEndValue(ItpPoint(350, 20));
				// widgetAnimation->setEndValue(ItpPoint(700, 300));
				widgetAnimation->setDuration(1500);
				widgetAnimation->start(); });

	tpButton *loopMoveBtn = new tpButton("循环往返", vScreen);
	loopMoveBtn->setProperty("type", "ControlPanelPowerButton");
	loopMoveBtn->setSize(150, 40);
	loopMoveBtn->move(20, 80);
	tpButton *stopLoopMoveBtn = new tpButton("停止", vScreen);
	stopLoopMoveBtn->setProperty("type", "ControlPanelPowerButton");
	stopLoopMoveBtn->setSize(150, 40);
	stopLoopMoveBtn->move(180, 80);
	stopLoopMoveBtn->setEnabled(false);

	tpAnimation *loopAnimation = nullptr;

	connect(loopMoveBtn, onClicked, [&](bool)
			{
				loopAnimation = new tpAnimation(exampleWidget, tpAnimation::Pos);
				loopAnimation->setStartValue(ItpPoint(350, 20));
				loopAnimation->setKeyValueAt(0.5, ItpPoint(700, 300));
				loopAnimation->setEndValue(ItpPoint(350, 20));
				// loopAnimation->setEndValue(ItpPoint(700, 300));
				loopAnimation->setDuration(1500);
				loopAnimation->setLoopCount(-1);
				loopAnimation->start(); 
				
				stopLoopMoveBtn->setEnabled(true);
				loopMoveBtn->setEnabled(false); });

	connect(stopLoopMoveBtn, onClicked, [&](bool)
			{ 
				loopAnimation->stop();
				stopLoopMoveBtn->setEnabled(false);
				loopMoveBtn->setEnabled(true); });

	tpButton *hideBtn = new tpButton("淡入", vScreen);
	hideBtn->setProperty("type", "ControlPanelPowerButton");
	hideBtn->setSize(150, 40);
	hideBtn->move(20, 140);

	connect(hideBtn, onClicked, [=](bool)
			{
		tpAnimation* widgetAnimation = new tpAnimation(exampleWidget, tpAnimation::WindowOpacity);
		widgetAnimation->setStartValue(255);
		widgetAnimation->setEndValue(50);
		widgetAnimation->setDuration(1000);
		widgetAnimation->start(); });

	tpButton *showBtn = new tpButton("淡出", vScreen);
	showBtn->setProperty("type", "ControlPanelPowerButton");
	showBtn->setSize(150, 40);
	showBtn->move(180, 140);

	connect(showBtn, onClicked, [=](bool)
			{
		tpAnimation* widgetAnimation = new tpAnimation(exampleWidget, tpAnimation::WindowOpacity);
		widgetAnimation->setStartValue(0);
		widgetAnimation->setEndValue(255);
		widgetAnimation->setDuration(1000);
		widgetAnimation->start(); });

	tpButton *widthResizeBtn = new tpButton("宽度变化", vScreen);
	widthResizeBtn->setProperty("type", "ControlPanelPowerButton");
	widthResizeBtn->setSize(150, 40);
	widthResizeBtn->move(20, 200);

	connect(widthResizeBtn, onClicked, [=](bool)
			{
		tpAnimation* widgetAnimation = new tpAnimation(exampleWidget, tpAnimation::Width);
		widgetAnimation->setStartValue(100);
		widgetAnimation->setEndValue(300);
		widgetAnimation->setDuration(1000);
		widgetAnimation->start(); });

	tpButton *heightResizeBtn = new tpButton("高度变化", vScreen);
	heightResizeBtn->setProperty("type", "ControlPanelPowerButton");
	heightResizeBtn->setSize(150, 40);
	heightResizeBtn->move(20, 260);

	connect(heightResizeBtn, onClicked, [=](bool)
			{
		tpAnimation* widgetAnimation = new tpAnimation(exampleWidget, tpAnimation::Height);
		widgetAnimation->setStartValue(100);
		widgetAnimation->setEndValue(300);
		widgetAnimation->setDuration(1000);
		widgetAnimation->start(); });

	tpButton *resizeBtn = new tpButton("大小变化", vScreen);
	resizeBtn->setProperty("type", "ControlPanelPowerButton");
	resizeBtn->setSize(150, 40);
	resizeBtn->move(20, 320);

	connect(resizeBtn, onClicked, [=](bool)
			{
		tpAnimation* widgetAnimation = new tpAnimation(exampleWidget, tpAnimation::Size);
		widgetAnimation->setStartValue(ItpSize(100, 100));
		widgetAnimation->setEndValue(ItpSize(300, 500));
		widgetAnimation->setDuration(1000);
		widgetAnimation->start(); });

	tpButton *posResizeBtn = new tpButton("平移&&大小", vScreen);
	posResizeBtn->setProperty("type", "ControlPanelPowerButton");
	posResizeBtn->setSize(150, 40);
	posResizeBtn->move(20, 380);

	connect(posResizeBtn, onClicked, [=](bool)
			{
		tpAnimation* widgetAnimation = new tpAnimation(exampleWidget, tpAnimation::Geometry);
		widgetAnimation->setStartValue(ItpRect(350, 20, 100, 100));
		widgetAnimation->setEndValue(ItpRect(700, 300, 300, 300));
		widgetAnimation->setDuration(1000);
		widgetAnimation->start(); });

	tpButton *bgColorBtn = new tpButton("背景颜色", vScreen);
	bgColorBtn->setProperty("type", "ControlPanelPowerButton");
	bgColorBtn->setSize(150, 40);
	bgColorBtn->move(20, 440);

	connect(bgColorBtn, onClicked, [=](bool)
			{
		tpAnimation* widgetAnimation = new tpAnimation(exampleWidget, tpAnimation::BackgroundColor);
		widgetAnimation->setStartValue(_RGB(255, 255, 255));
		widgetAnimation->setEndValue(_RGB(100, 255, 100));
		widgetAnimation->setDuration(1000);
		widgetAnimation->start(); });

	exampleWidget->update();
	vScreen->update();

	return app.run();
}
