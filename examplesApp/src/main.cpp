#include "tpApp.h"
#include "mainWindowService.h"

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);

	mainWindowService *mainWindow = new mainWindowService();
	app.bindVScreen(mainWindow);
	mainWindow->update();

	return app.run();
}
