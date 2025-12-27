#include "TpDesktopDialog.h"
#include "TpApp.h"
#include "TpDefaultCss.h"
#include "TpDef.h"
#include "TpMainWindow.h"
#include "TpPainter.h"
#include "thorVG/thorvg.h"
#include <InteractData/TpDesktopData.h>
#include "TpDeskStatusInfo.h"

TpDesktopDialog::TpDesktopDialog()
    : TpDialog()
{
}

TpDesktopDialog::~TpDesktopDialog()
{
}

void TpDesktopDialog::move(int32_t x, int32_t y)
{
#if 1
    if (!TpApp::Inst()->isDesktop())
    {
        const TpDeskStatusBarInfo &statusBarInfo = TpDeskStatusInfo::Instance()->statusInfo();

        TpSize screenSize = this->screenSize();

        int32_t statusBarLocation = statusBarInfo.statusBarLocation;
        if (statusBarLocation == 0)
        {
            if (y < statusBarInfo.statusBarHeight)
                y = statusBarInfo.statusBarHeight;
        }
        else if (statusBarLocation == 1)
        {
            if ((x + width()) > (screenSize.width() - statusBarInfo.statusBarWidth))
                x = screenSize.width() - statusBarInfo.statusBarWidth - width();
        }
        else if (statusBarLocation == 2)
        {
            if ((y + height()) > (screenSize.height() - statusBarInfo.statusBarHeight))
                y = screenSize.height() - statusBarInfo.statusBarHeight - height();
        }
        else if (statusBarLocation == 3)
        {
            if (x < statusBarInfo.statusBarWidth)
                x = statusBarInfo.statusBarWidth;
        }
        else
        {
        }
    }
#endif

    TpDialog::move(x, y);
}
