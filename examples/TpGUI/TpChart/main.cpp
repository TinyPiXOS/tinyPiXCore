#include <TpApp.h>
#include "MainWindowService.h"
#include <signal.h>
#include <stdlib.h>

static TpApp *g_app = nullptr;

static void signal_handler(int signum) {
    if (signum == SIGINT && g_app) {
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);
    signal(SIGINT, signal_handler);

    TpApp app(argc, argv);
    g_app = &app;
    TpApp::Inst()->cssParser()->parseCss("./chart_style.css");

    MainWindowService mainWindow;
    mainWindow.show();

    return app.run();
}
