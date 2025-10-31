#include "TpDialog.h"
#include "TpApp.h"
#include "TpDefaultCss.h"
#include "TpDef.h"
#include <semaphore.h>
#include "TpMainWindow.h"

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

    // 模态窗口，
    // TpWidget *execWindow;
};

TpDialog::TpDialog(const char *type)
    : TpScreen(type)
{
    TpDialogData *dialogData = new TpDialogData();
    data_ = dialogData;
    // TpApp::Inst()->sendRegister(this);

    if (this->objectType() != Tp::TP_FLOAT_OBJECT)
    {
        TpApp::Inst()->sendDelete(this);
    }

    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
    set->top = this->topObject();

    refreshBaseCss();
}

TpDialog::~TpDialog()
{
}

uint32_t TpDialog::exec()
{
    TpDialogData *dialogData = static_cast<TpDialogData *>(data_);
    if (!dialogData)
        return 0;

    // 调整窗口到居中位置
    TpMainWindow *mainScreen = TpApp::Inst()->mainWindow();
    move((mainScreen->width() - width()) / 2.0, mainScreen->pos().y() + (mainScreen->height() - height()) / 2.0);

    setVisible(true);
    update();

    // dialogData->sema.wait();

    return 1;
}

void TpDialog::close()
{
    TpDialogData *dialogData = static_cast<TpDialogData *>(data_);
    if (!dialogData)
        return;

    TpScreen::setVisible(false);
    // setVisible(false);
    // update();

    dialogData->sema.post();
}

Tp::TpObjectType TpDialog::objectType()
{
    return Tp::TP_FLOAT_OBJECT;
}

bool TpDialog::onPaintEvent(TpPaintEvent *event)
{
    // dialog使用模拟宽高；即每个dialog起始都是全屏的，只是根据设置的尺寸绘制对应的背景

    return TpScreen::onPaintEvent(event);
}
