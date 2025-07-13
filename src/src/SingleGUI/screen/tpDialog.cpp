#include "tpDialog.h"
#include "tpApp.h"
#include "tpDefaultCss.h"
#include "tpDef.h"
#include <semaphore.h>

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

struct tpDialogData
{
	// 对话框阻塞信号量
	Semaphore sema;
};

tpDialog::tpDialog(const char *type) : tpScreen(type)
{
	tpDialogData *dialogData = new tpDialogData();
	data_ = dialogData;
	// tpApp::Inst()->sendRegister(this);

	if (this->objectType() != TP_FLOAT_OBJECT)
	{
		tpApp::Inst()->sendDelete(this);
	}

	ItpObjectSet *set = (ItpObjectSet *)tpObject::objectSets();
	set->top = this->topObject();

	refreshBaseCss();
}

tpDialog::~tpDialog()
{
}

uint32_t tpDialog::exec()
{
	tpDialogData *dialogData = static_cast<tpDialogData *>(data_);
	if (!dialogData)
		return 0;

	tpScreen::setVisible(true);

	// setVisible(true);
	// update();

	dialogData->sema.wait();

	return 1;
}

void tpDialog::close()
{
	tpDialogData *dialogData = static_cast<tpDialogData *>(data_);
	if (!dialogData)
		return;

	tpScreen::setVisible(false);
	// setVisible(false);
	// update();

	dialogData->sema.post();
}

ItpObjectType tpDialog::objectType()
{
	return TP_FLOAT_OBJECT;
}
