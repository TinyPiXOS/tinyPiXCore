#include "TpSplashScreen.h"
#include "TpGateway.h"
#include "TpInteractDataDef/TpDesktopData.h"

struct TpSplashScreenData
{
};

TpSplashScreen *TpSplashScreen::Instance()
{
    static TpSplashScreen instance;
    return &instance;
}

void TpSplashScreen::closeSplashScreen()
{
    bool publishRes = false;
    publishGatewayData(TpAppInitFinishKey, &publishRes, sizeof(publishRes));
}

TpSplashScreen::TpSplashScreen()
{
    TpSplashScreenData *apiData = new TpSplashScreenData();
    data_ = apiData;
    initializeGateway();
}

TpSplashScreen::~TpSplashScreen()
{
}
