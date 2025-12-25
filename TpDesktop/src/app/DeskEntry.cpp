#include "DeskEntry.h"
#include "TpApp.h"
#include "DeskScreen.h"
#include <csignal>
#include <TpAppManager.h>

void signalHandler(int signal)
{
    std::cout << "signal: " << signal << std::endl;
    TpAppManager::Instance()->killAllApp();
}

// int deskEntry_Start(int argc, char *argv[])
int main(int argc, char *argv[])
{
    // 注册信号处理
    // signal(SIGINT, signalHandler);
    // signal(SIGTERM, signalHandler);

    TpApp app(argc, argv, "tinyPiX_DeskTop_0x43ef3dc14");
    DeskScreen *deskTopMainWindow = new DeskScreen();
    deskTopMainWindow->update();

    return app.run();
}
