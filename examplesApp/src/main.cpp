#include "TpApp.h"
#include "MainWindowService.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);

	MainWindowService *mainWindow = new MainWindowService();

	return app.run();
}
