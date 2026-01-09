#include "TpDialog.h"
#include "TpApp.h"
#include "TpDefaultCss.h"
#include "TpDef.h"
#include "TpMainWindow.h"
#include "TpPainter.h"
#include "thorVG/thorvg.h"
#include "TpObject_p.h"
#include "TpApp.h"
#include "TpScreen_p.h"
#include "TpApp_def.h"
#include "TpApp_p.h"

struct TpDialogData
{
    // 遮罩窗体，模态显示时用于遮罩屏幕
    TpWidget *maskWidget = nullptr;

    // 鼠标点击坐标
    TpPoint originPoint; // 窗口原始坐标
    TpPoint pressPoint;

    bool beMoved = false;
};

TpDialog::TpDialog(const char *type)
    : TpWidget(TpApp::Inst()->mainWindow())
{
    TpDialogData *dialogData = new TpDialogData();

    TpWidget *mainScreen = TpApp::Inst()->mainWindow();
    dialogData->maskWidget = new TpWidget(mainScreen);
    dialogData->maskWidget->setBackGroundColor(_RGBA(255, 255, 255, 100));
    dialogData->maskWidget->setRect(mainScreen->pos().x(), mainScreen->pos().y(), mainScreen->width(), mainScreen->height());
    dialogData->maskWidget->setVisible(false);

    data_ = dialogData;

    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
    set->top = this->topObject();

    refreshBaseCss();

    setVisible(false);
}

TpDialog::~TpDialog()
{
    TpDialogData *dialogData = static_cast<TpDialogData *>(data_);
    if (dialogData)
    {
        delete dialogData;
        dialogData = nullptr;
        data_ = nullptr;
    }
}

void TpDialog::exec()
{
    TpDialogData *dialogData = static_cast<TpDialogData *>(data_);
    if (!dialogData)
        return;

    dialogData->maskWidget->setVisible(true);
    dialogData->maskWidget->bringToTop();
    bringToTop();

    // 调整窗口到居中位置
    TpWidget *mainScreen = TpApp::Inst()->mainWindow();
    move((mainScreen->width() - width()) / 2.0, mainScreen->pos().y() + (mainScreen->height() - height()) / 2.0);

    setVisible(true);
    update();

    // dialogData->sema.wait();
}

void TpDialog::close()
{
    TpDialogData *dialogData = static_cast<TpDialogData *>(data_);
    if (!dialogData)
        return;

    setVisible(false);
    // update();

    // dialogData->sema.post();
}

void TpDialog::setVisible(bool visible)
{
    TpDialogData *dialogData = static_cast<TpDialogData *>(data_);
    if (dialogData->maskWidget && (visible == false))
        dialogData->maskWidget->setVisible(false);

    TpWidget::setVisible(visible);
}

void TpDialog::setBeMoved(bool moved)
{
    TpDialogData *dialogData = static_cast<TpDialogData *>(data_);
    dialogData->beMoved = moved;
}

bool TpDialog::moved()
{
    TpDialogData *dialogData = static_cast<TpDialogData *>(data_);
    return dialogData->beMoved;
}

bool TpDialog::onMousePressEvent(TpMouseEvent *event)
{
    TpWidget::onMousePressEvent(event);

    TpDialogData *dialogData = static_cast<TpDialogData *>(data_);
    if (dialogData->beMoved)
    {
        dialogData->originPoint = pos();
        dialogData->pressPoint = event->globalPos();
        // std::cout << "TpDialog::onMousePressEvent: " << dialogData->pressPoint.x() << ", " << dialogData->pressPoint.y() << std::endl;
    }

    return true;
}

bool TpDialog::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpWidget::onMouseRleaseEvent(event);

    return true;
}

bool TpDialog::onMouseMoveEvent(TpMouseEvent *event)
{
    TpWidget::onMouseMoveEvent(event);

    TpDialogData *dialogData = static_cast<TpDialogData *>(data_);
    if (event->state() && dialogData->beMoved)
    {
        TpPoint movePoint = event->globalPos();
        TpPoint curPoint = dialogData->originPoint + (movePoint - dialogData->pressPoint);

        // std::cout << "TpDialog::onMouseMoveEvent: " << movePoint.x() << ", " << movePoint.y() << " --> " << curPoint.x() << ", " << curPoint.y() << std::endl;
        move(curPoint.x(), curPoint.y());
    }

    return true;
}

bool TpDialog::onPaintEvent(TpPaintEvent *event)
{
    TpWidget::onPaintEvent(event);
    return true;
}

Tp::TpObjectType TpDialog::objectType()
{
    return Tp::TP_FLOAT_OBJECT;
}
