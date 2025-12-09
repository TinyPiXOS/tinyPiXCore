#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpSignalSlot.h"
#include "TpVirtualKeyboard.h"
#include "TpLineEdit.h"

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);

    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));

    TpLineEdit *testEdit = new TpLineEdit(vScreen);
    testEdit->setRect(30, 30, 300, 60);
    testEdit->setVisible(true);

    // TpVirtualKeyboard *virtualKeyboard = new TpVirtualKeyboard();
    // virtualKeyboard->show();

    return app.run();
}
