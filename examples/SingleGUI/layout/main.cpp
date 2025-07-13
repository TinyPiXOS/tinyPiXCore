#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpDialog.h"
#include "tpButton.h"
#include "tpHBoxLayout.h"
#include "tpVBoxLayout.h"
#include "tpVariant.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 200));
	app.bindVScreen(vScreen);

	tpDialog *floatScreenH = new tpDialog();
	floatScreenH->setBackGroundColor(_RGBA(255, 255, 255, 200));
	floatScreenH->setRect(0, 0, 900, 600);
	floatScreenH->setAlpha(128);
	floatScreenH->setBeMoved(true);

	tpDialog *floatScreenV = new tpDialog();
	floatScreenV->setBackGroundColor(_RGBA(255, 255, 255, 200));
	floatScreenV->setRect(10, 100, 400, 650);
	floatScreenV->setAlpha(128);
	floatScreenV->setBeMoved(true);

	tpHBoxLayout *hLayout = new tpHBoxLayout();
	hLayout->setDirection(tpBoxLayout::RightToLeft);

	tpVBoxLayout *vLayout = new tpVBoxLayout();
	vLayout->setDirection(tpBoxLayout::BottomToTop);

	tpVBoxLayout *sonLayout = new tpVBoxLayout();
	sonLayout->setContentsMargins(0, 0, 0, 0);

	tpHBoxLayout *son2Layout = new tpHBoxLayout();
	son2Layout->setContentsMargins(0, 0, 0, 0);

	for (int i = 0; i < 4; ++i)
	{
		tpButton *testButton = new tpButton("测试按钮" + tpString::number(i));
		tpButton *testButton2 = new tpButton("测试按钮" + tpString::number(i));
		tpButton *testButton3 = new tpButton("子按钮" + tpString::number(i));
		tpButton *testButton4 = new tpButton("二级子按钮" + tpString::number(i));

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

	hLayout->addSpacer(new tpSpacerItem(100, 20, tpSpacerItem::Expanding, tpSpacerItem::Minimum));
	vLayout->addSpacer(new tpSpacerItem(100, 20, tpSpacerItem::Minimum, tpSpacerItem::Expanding));

	floatScreenH->setLayout(hLayout);
	floatScreenV->setLayout(vLayout);

	floatScreenH->update();
	floatScreenV->update();

	vScreen->update();

	return app.run();
}
