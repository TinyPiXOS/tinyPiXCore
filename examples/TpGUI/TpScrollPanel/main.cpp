#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpRadioButton.h"
#include "TpFont.h"
#include "TpColors.h"
#include "TpScrollPanel.h"
#include "TpRadioButton.h"
#include "TpMediaTileButton.h"
#include "TpHBoxLayout.h"

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);
    app.setDisableEventType(TpApp::TP_DIS_KEYBOARD);
    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));

    TpScrollPanel *fastPathScroll_ = new TpScrollPanel(vScreen);
    fastPathScroll_->setScrollMode(false);
    fastPathScroll_->setBackGroundColor(_RGB(255, 0, 0));

    // 将快捷访问按钮放进中间widget，再放入滚动窗口
    TpWidget *fastScrollWidget = new TpWidget();
    fastScrollWidget->setBackGroundColor(_RGB(0, 255, 0));

    TpMediaTileButton *pictureTileBtn_ = new TpMediaTileButton();
    pictureTileBtn_->setText("图片");
    pictureTileBtn_->setIcon(applicationDirPath() + "/../res/fastAccess/图片.png");
    pictureTileBtn_->installEventFilter(fastScrollWidget);

    // TpMediaTileButton *videoTileBtn_ = new TpMediaTileButton();
    // videoTileBtn_->setText("视频");
    // videoTileBtn_->setIcon(applicationDirPath() + "/../res/fastAccess/视频.png");
    // videoTileBtn_->installEventFilter(fastScrollWidget);

    // TpMediaTileButton *textTileBtn_ = new TpMediaTileButton();
    // textTileBtn_->setText("文本");
    // textTileBtn_->setIcon(applicationDirPath() + "/../res/fastAccess/文本.png");
    // textTileBtn_->installEventFilter(fastScrollWidget);

    // TpMediaTileButton *musicTileBtn_ = new TpMediaTileButton();
    // musicTileBtn_->setText("音频");
    // musicTileBtn_->setIcon(applicationDirPath() + "/../res/fastAccess/音频.png");
    // musicTileBtn_->installEventFilter(fastScrollWidget);

    // TpMediaTileButton *applicationTileBtn_ = new TpMediaTileButton();
    // applicationTileBtn_->setText("应用");
    // applicationTileBtn_->setIcon(applicationDirPath() + "/../res/fastAccess/应用.png");
    // applicationTileBtn_->installEventFilter(fastScrollWidget);

    // 将快速访问按钮添加到中间窗体
    fastPathScroll_->setWidth(420);
    fastPathScroll_->setFixedHeight(pictureTileBtn_->height() + 25);
    std::cout << "fastPathScroll_ Height  " << fastPathScroll_->height() << std::endl;

    // fastScrollWidget->setLayout(new TpHBoxLayout());
    // fastScrollWidget->layout()->setSpacing(12);
    // fastScrollWidget->layout()->addWidget(pictureTileBtn_);
    // fastScrollWidget->layout()->addWidget(videoTileBtn_);
    // fastScrollWidget->layout()->addWidget(textTileBtn_);
    // fastScrollWidget->layout()->addWidget(musicTileBtn_);
    // fastScrollWidget->layout()->addWidget(applicationTileBtn_);
    // fastPathScroll_->setWidget(fastScrollWidget);

    fastPathScroll_->addObject(pictureTileBtn_);

    return app.run();
}
