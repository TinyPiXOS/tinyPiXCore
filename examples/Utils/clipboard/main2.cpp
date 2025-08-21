#include <tpUtils.h>
#include <iostream>
#include <cstring>
#include "tpClipboard.h"

int32_t main(int32_t argc, char *argv[])
{
    tpClipboard *clipboard = tpClipboard::Inst();

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
