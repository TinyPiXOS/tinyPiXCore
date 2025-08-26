#include "TpApp.h"
#include "TpFixScreen.h"
#include "TpDialog.h"
#include "TpButton.h"
#include "TpHBoxLayout.h"
#include "TpVBoxLayout.h"
#include "TpVariant.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 200));
	app.bindVScreen(vScreen);

	TpDialog *floatScreenH = new TpDialog();
	floatScreenH->setBackGroundColor(_RGBA(255, 255, 255, 200));
	floatScreenH->setRect(0, 0, 900, 600);
	floatScreenH->setAlpha(128);
	floatScreenH->setBeMoved(true);

	TpDialog *floatScreenV = new TpDialog();
	floatScreenV->setBackGroundColor(_RGBA(255, 255, 255, 200));
	floatScreenV->setRect(10, 100, 400, 650);
	floatScreenV->setAlpha(128);
	floatScreenV->setBeMoved(true);

	TpHBoxLayout *hLayout = new TpHBoxLayout();
	hLayout->setDirection(TpBoxLayout::RightToLeft);

	TpVBoxLayout *vLayout = new TpVBoxLayout();
	vLayout->setDirection(TpBoxLayout::BottomToTop);

	TpVBoxLayout *sonLayout = new TpVBoxLayout();
	sonLayout->setContentsMargins(0, 0, 0, 0);

	TpHBoxLayout *son2Layout = new TpHBoxLayout();
	son2Layout->setContentsMargins(0, 0, 0, 0);

	for (int i = 0; i < 4; ++i)
	{
		TpButton *testButton = new TpButton("测试按钮" + TpString::number(i));
		TpButton *testButton2 = new TpButton("测试按钮" + TpString::number(i));
		TpButton *testButton3 = new TpButton("子按钮" + TpString::number(i));
		TpButton *testButton4 = new TpButton("二级子按钮" + TpString::number(i));

		testButton->setMinimumSize(100, 40);
		testButton2->setMinimumSize(100, 40);
		testButton3->setMinimumSize(100, 40);
		testButton4->setMinimumSize(100, 40);

		testButton->setBackGroundColor(_RGB(255, 15, 15));
		testButton2->setBackGroundColor(_RGB(255, 15, 15));

		hLayout->addWidget(testButton, i + 1);
		vLayout->addWidget(testButton2, i + 1);

		sonLayout->addWidget(testButton3, i + 1);
		son2Layout->addWidget(testButton4, i + 1);
	}
	sonLayout->addLayout(son2Layout);
	hLayout->addLayout(sonLayout, 3);

	hLayout->addSpacer(new TpSpacerItem(100, 20, TpSpacerItem::Expanding, TpSpacerItem::Minimum));
	vLayout->addSpacer(new TpSpacerItem(100, 20, TpSpacerItem::Minimum, TpSpacerItem::Expanding));

	floatScreenH->setLayout(hLayout);
	floatScreenV->setLayout(vLayout);

	floatScreenH->update();
	floatScreenV->update();

	vScreen->update();

	return app.run();
}
