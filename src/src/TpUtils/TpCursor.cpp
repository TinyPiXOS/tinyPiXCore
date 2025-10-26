#include "TpCursor.h"
#include "tinyPiXWF.h"

TpCursor::TpCursor()
{
}

TpCursor::~TpCursor()
{
}

TpPoint TpCursor::pos()
{
    TpPoint curPos;

    int x = 0;
    int y = 0;
    tinyPiX_wf_get_global_mouse_position(&x, &y);

    curPos.setX(x);
    curPos.setY(y);

    return curPos;
}
