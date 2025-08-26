#include "TpApp.h"
#include "mainWindowService.h"

int32_t main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);

	mainWindowService *mainWindow = new mainWindowService();
	app.bindVScreen(mainWindow);
	mainWindow->update();

	return app.run();
}
