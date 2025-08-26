#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpButton.h"
#include "TpAnimation.h"
#include "TpDialog.h"
#include "TpLabel.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	// 效果展示窗体
	TpDialog *exampleWidget = new TpDialog();
	// TpChildWidget *exampleWidget = new TpChildWidget(vScreen);
	exampleWidget->setBackGroundColor(_RGB(255, 255, 255));
	exampleWidget->setRect(350, 20, 100, 100);

	TpButton *posMoveBtn = new TpButton("平移", vScreen);
	posMoveBtn->setProperty("type", "ControlPanelPowerButton");
	posMoveBtn->setSize(150, 40);
	posMoveBtn->move(20, 20);
	posMoveBtn->update();

	connect(posMoveBtn, onClicked, [=](bool)
			{
				TpAnimation* widgetAnimation = new TpAnimation(exampleWidget, TpAnimation::Pos);
				widgetAnimation->setStartValue(ItpPoint(350, 20));
				widgetAnimation->setEndValue(ItpPoint(700, 300));
				widgetAnimation->setDuration(1000);
				widgetAnimation->start(); });

	TpButton *returnMoveBtn = new TpButton("往返平移", vScreen);
	returnMoveBtn->setProperty("type", "ControlPanelPowerButton");
	returnMoveBtn->setSize(150, 40);
	returnMoveBtn->move(180, 20);
	returnMoveBtn->update();

	connect(returnMoveBtn, onClicked, [=](bool)
			{
				TpAnimation* widgetAnimation = new TpAnimation(exampleWidget, TpAnimation::Pos);
				widgetAnimation->setStartValue(ItpPoint(350, 20));
				widgetAnimation->setKeyValueAt(0.5, ItpPoint(700, 300));
				widgetAnimation->setEndValue(ItpPoint(350, 20));
				// widgetAnimation->setEndValue(ItpPoint(700, 300));
				widgetAnimation->setDuration(1500);
				widgetAnimation->start(); });

	TpButton *loopMoveBtn = new TpButton("循环往返", vScreen);
	loopMoveBtn->setProperty("type", "ControlPanelPowerButton");
	loopMoveBtn->setSize(150, 40);
	loopMoveBtn->move(20, 80);
	loopMoveBtn->update();

	TpButton *stopLoopMoveBtn = new TpButton("停止", vScreen);
	stopLoopMoveBtn->setProperty("type", "ControlPanelPowerButton");
	stopLoopMoveBtn->setSize(150, 40);
	stopLoopMoveBtn->move(180, 80);
	stopLoopMoveBtn->setEnabled(false);
	stopLoopMoveBtn->update();

	TpAnimation *loopAnimation = nullptr;

	connect(loopMoveBtn, onClicked, [&](bool)
			{
				loopAnimation = new TpAnimation(exampleWidget, TpAnimation::Pos);
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

	TpButton *hideBtn = new TpButton("淡入", vScreen);
	hideBtn->setProperty("type", "ControlPanelPowerButton");
	hideBtn->setSize(150, 40);
	hideBtn->move(20, 140);
	hideBtn->update();

	connect(hideBtn, onClicked, [=](bool)
			{
		TpAnimation* widgetAnimation = new TpAnimation(exampleWidget, TpAnimation::WindowOpacity);
		widgetAnimation->setStartValue(255);
		widgetAnimation->setEndValue(50);
		widgetAnimation->setDuration(1000);
		widgetAnimation->start(); });

	TpButton *showBtn = new TpButton("淡出", vScreen);
	showBtn->setProperty("type", "ControlPanelPowerButton");
	showBtn->setSize(150, 40);
	showBtn->move(180, 140);
	showBtn->update();

	connect(showBtn, onClicked, [=](bool)
			{
		TpAnimation* widgetAnimation = new TpAnimation(exampleWidget, TpAnimation::WindowOpacity);
		widgetAnimation->setStartValue(0);
		widgetAnimation->setEndValue(255);
		widgetAnimation->setDuration(1000);
		widgetAnimation->start(); });

	TpButton *widthResizeBtn = new TpButton("宽度变化", vScreen);
	widthResizeBtn->setProperty("type", "ControlPanelPowerButton");
	widthResizeBtn->setSize(150, 40);
	widthResizeBtn->move(20, 200);
	widthResizeBtn->update();

	connect(widthResizeBtn, onClicked, [=](bool)
			{
		TpAnimation* widgetAnimation = new TpAnimation(exampleWidget, TpAnimation::Width);
		widgetAnimation->setStartValue(100);
		widgetAnimation->setEndValue(300);
		widgetAnimation->setDuration(1000);
		widgetAnimation->start(); });

	TpButton *heightResizeBtn = new TpButton("高度变化", vScreen);
	heightResizeBtn->setProperty("type", "ControlPanelPowerButton");
	heightResizeBtn->setSize(150, 40);
	heightResizeBtn->move(20, 260);
	heightResizeBtn->update();

	connect(heightResizeBtn, onClicked, [=](bool)
			{
		TpAnimation* widgetAnimation = new TpAnimation(exampleWidget, TpAnimation::Height);
		widgetAnimation->setStartValue(100);
		widgetAnimation->setEndValue(300);
		widgetAnimation->setDuration(1000);
		widgetAnimation->start(); });

	TpButton *resizeBtn = new TpButton("大小变化", vScreen);
	resizeBtn->setProperty("type", "ControlPanelPowerButton");
	resizeBtn->setSize(150, 40);
	resizeBtn->move(20, 320);
	resizeBtn->update();

	connect(resizeBtn, onClicked, [=](bool)
			{
		TpAnimation* widgetAnimation = new TpAnimation(exampleWidget, TpAnimation::Size);
		widgetAnimation->setStartValue(ItpSize(100, 100));
		widgetAnimation->setEndValue(ItpSize(300, 500));
		widgetAnimation->setDuration(1000);
		widgetAnimation->start(); });

	TpButton *posResizeBtn = new TpButton("平移&&大小", vScreen);
	posResizeBtn->setProperty("type", "ControlPanelPowerButton");
	posResizeBtn->setSize(150, 40);
	posResizeBtn->move(20, 380);
	posResizeBtn->update();

	connect(posResizeBtn, onClicked, [=](bool)
			{
		TpAnimation* widgetAnimation = new TpAnimation(exampleWidget, TpAnimation::Geometry);
		widgetAnimation->setStartValue(ItpRect(350, 20, 100, 100));
		widgetAnimation->setEndValue(ItpRect(700, 300, 300, 300));
		widgetAnimation->setDuration(1000);
		widgetAnimation->start(); });

	TpButton *bgColorBtn = new TpButton("背景颜色", vScreen);
	bgColorBtn->setProperty("type", "ControlPanelPowerButton");
	bgColorBtn->setSize(150, 40);
	bgColorBtn->move(20, 440);
	bgColorBtn->update();

	connect(bgColorBtn, onClicked, [=](bool)
			{
		TpAnimation* widgetAnimation = new TpAnimation(exampleWidget, TpAnimation::BackgroundColor);
		widgetAnimation->setStartValue(_RGB(255, 255, 255));
		widgetAnimation->setEndValue(_RGB(100, 255, 100));
		widgetAnimation->setDuration(1000);
		widgetAnimation->start(); });

	exampleWidget->update();
	vScreen->update();

	return app.run();
}
