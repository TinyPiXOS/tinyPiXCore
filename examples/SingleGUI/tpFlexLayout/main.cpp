#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpDialog.h"
#include "tpFlexLayout.h"
#include "tpButton.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);

	tpFlexLayout *flexLayout = new tpFlexLayout();

	/// @brief 设置布局方向
	flexLayout->setFlexDirection(tpFlexLayout::Row);
	// flexLayout->setFlexDirection(tpFlexLayout::RowReverse);
	// flexLayout->setFlexDirection(tpFlexLayout::Column);
	// flexLayout->setFlexDirection(tpFlexLayout::ColumnReverse);

	/// @brief 设置主轴对齐方式
	// flexLayout->setJustifyContent(tpFlexLayout::MainFlexStart);   // 默认, 从主轴 开始位置 开始
	// flexLayout->setJustifyContent(tpFlexLayout::MainFlexEnd);      // 从主轴 结束位置 开始
	// flexLayout->setJustifyContent(tpFlexLayout::MainCenter);       // 居中对齐
	flexLayout->setJustifyContent(tpFlexLayout::MainSpaceBetween); // 两端对齐,两边贴着
	// flexLayout->setJustifyContent(tpFlexLayout::MainSpaceEvenly);  // 分散对齐,间距相同
	// flexLayout->setJustifyContent(tpFlexLayout::MainSpaceAround);   // 分散对齐,两边是中间的一半

	/// @brief 设置交叉轴对齐方式
	// flexLayout->setAlignItems(tpFlexLayout::CrossFlexStart); // 顶部对齐
	flexLayout->setAlignItems(tpFlexLayout::CrossFlexEnd);   // 底部对齐
	// flexLayout->setAlignItems(tpFlexLayout::CrossCenter);     // 居中对齐

	/// @brief 设置多行交叉轴对齐方式，注意设置后会覆盖交叉轴对齐样式
	// flexLayout->setAlignContent(tpFlexLayout::Stretch);      // 默认,拉伸，与交叉轴对齐
	// flexLayout->setAlignContent(tpFlexLayout::FlexStart);    // 顶部对齐
	flexLayout->setAlignContent(tpFlexLayout::FlexEnd);      // 底部对齐
	// flexLayout->setAlignContent(tpFlexLayout::Center);       // 居中对齐
	// flexLayout->setAlignContent(tpFlexLayout::SpaceBetween); // 分散对齐,上下贴着
	// flexLayout->setAlignContent(tpFlexLayout::SpaceEvenly);  //  分散对齐,上中下平均
	// flexLayout->setAlignContent(tpFlexLayout::SpaceAround);   // 分散对齐,上下是中一半

	for (int i = 0; i < 8; ++i)
	{
		tpButton *testButton = new tpButton("测试按钮" + tpString::number(i));

		testButton->setSize(150, 60);

		// testButton->setBackGroundColor(_RGB(255, 15, 15));

		flexLayout->addWidget(testButton);
	}

	vScreen->setLayout(flexLayout);

	vScreen->update();

	return app.run();
}
