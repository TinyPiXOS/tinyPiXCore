#include <TpUtils.h>
#include <iostream>
#include <cstring>
#include "TpClipboard.h"

int32_t main(int32_t argc, char *argv[])
{
    TpClipboard *clipboard = TpClipboard::Inst();

    if (clipboard)
    {
        bool ret = clipboard->hasText();

        if (ret)
        {
            puts("has text");
        }
        else
        {
            puts("has no text");
        }

        clipboard->clear();

        delete clipboard;
    }
    return 0;
}
