#include "TpDialog.h"
#include "TpApp.h"
#include "TpDefaultCss.h"
#include "TpDef.h"
#include <semaphore.h>
#include "TpMainWindow.h"
#include "TpPainter.h"
#include "thorVG/thorvg.h"

class Semaphore
{
public:
    explicit Semaphore(unsigned int initCount = 0)
    {
        sem_init(&m_sem, 0, initCount);
    }
    ~Semaphore()
    {
        sem_destroy(&m_sem);
    }

    bool wait()
    {
        return (sem_wait(&m_sem) == 0);
    }
    bool post(int n = 1)
    {
        while (n-- > 0)
        {
            if (sem_post(&m_sem) != 0)
                return false;
        }
        return true;
    }

private:
    sem_t m_sem;
};

struct TpDialogData
{
    // 对话框阻塞信号量
    Semaphore sema;

    // 遮罩窗体，模态显示时用于遮罩屏幕
    TpWidget *maskWidget = nullptr;
    // bool isExec = false;
};

TpDialog::TpDialog(const char *type)
    : TpScreen(type)
{
    TpDialogData *dialogData = new TpDialogData();

    TpWidget *mainScreen = TpApp::Inst()->mainWindow();
    dialogData->maskWidget = new TpWidget(mainScreen);
    dialogData->maskWidget->setBackGroundColor(_RGBA(255, 255, 255, 100));
    dialogData->maskWidget->setRect(mainScreen->pos().x(), mainScreen->pos().y(), mainScreen->width(), mainScreen->height());
    dialogData->maskWidget->setVisible(false);

    data_ = dialogData;

    TpApp::Inst()->sendRegister(this);

    if (this->objectType() != Tp::TP_FLOAT_OBJECT)
    {
        TpApp::Inst()->sendDelete(this);
    }

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

    TpScreen::setVisible(visible);
}

Tp::TpObjectType TpDialog::objectType()
{
    return Tp::TP_FLOAT_OBJECT;
}
