/*
    内部 全局数据定义，禁止include目录下的文件包含此文件
*/

#ifndef __TP_DEF_H
#define __TP_DEF_H

#include <TpGUI.h>
#include "TpList.h"
#include "TpEvent.h"
#include "TpCDef.h"
#include "keyboard.h"
#include <tinyPiXWF.h>
#include <mutex>
#include <list>
#include "TpHash.h"
#include "TpVariant.h"
#include "TpWidget.h"
#include "TpImage.h"
#include "TpBrush.h"
#include "TpGraphicsBlurEffect.h"

class TpLayout;
class TpObjectStack;
class TpEvent;

struct ItpTempDef
{
    TpWidget *curfocus = nullptr;
    TpWidget *lstfocus = nullptr;
    TpWidget *curmotion = nullptr;
    TpWidget *lstmotion = nullptr;
    TpWidget *curObject = nullptr;
    // TpWidget *dragObject = nullptr;

    // 鼠标移动前的上一个坐标
    TpPoint lastPoint;

    ItpTempDef()
    {
    }

    /// @brief 移除对象时，如果缓存了obj，需要同步置空
    /// @param delObj
    void deleteObject(TpObject *delObj)
    {
        TpWidget *needDelObj = dynamic_cast<TpWidget *>(delObj);
        if (!needDelObj)
            return;

        if (curfocus == needDelObj)
            curfocus = nullptr;

        if (lstfocus == needDelObj)
            lstfocus = nullptr;

        if (curmotion == needDelObj)
            curmotion = nullptr;

        if (lstmotion == needDelObj)
            lstmotion = nullptr;

        if (curObject == needDelObj)
            curObject = nullptr;

        // if (dragObject == needDelObj)
        // dragObject = nullptr;
    }
};



struct ItpMouseSet
{
    uint32_t which;
    MouseEventType button;
    bool state;

    TpPoint pos;
    TpPoint globalPos;

    ItpMouseSet() : which(0), button(BUTTON_INVALIDATE_VALUE), state(false)
    {
    }
};

/// @brief 键盘事件数据
struct ItpKeyboardSet
{
    /// @brief 测试注释
    uint8_t which;
    bool state;
    uint32_t scancode;
    uint32_t virtualKey;
    uint32_t symbol;
    char shortCut[KEYBOARD_STRING_LEN];
    KeyModeType keyMod;
};

struct ItpFingerSet
{
    int32_t touchFingerType;
    uint32_t timestamp;
    int64_t fingerID;
    int64_t touchID;
    int32_t x;
    int32_t y;
    int32_t dx;
    int32_t dy;
    float pressure;
};

struct ItpDollarSet
{
    int32_t dollarType;
    uint32_t timestamp;
    int64_t touchID;
    int64_t GestureID;
    uint32_t numFingers;
    int32_t x;
    int32_t y;

    ItpDollarSet()
    {
    }
};

struct ItpMultiGestureSet
{
    uint32_t timestamp;
    int64_t touchID;
    float dtheta;
    float ddist;
    int32_t x, y;
    uint16_t numfingers;
    uint16_t padding;

    ItpMultiGestureSet()
    {
    }
};

struct ItpObjectMoveSet
{
    TpObject *object;
    int32_t nx;
    int32_t ny;

    ItpObjectMoveSet()
    {
    }
};

struct ItpObjectResizeSet
{
    TpObject *object;
    int32_t nw;
    int32_t nh;
    int32_t question;

    ItpObjectResizeSet()
    {
    }
};

struct ItpObjectFocusSet
{
    TpObject *object;
    bool focused;

    ItpObjectFocusSet()
    {
    }
};

struct ItpObjectLeaveSet
{
    TpObject *object;
    bool leaved;
};

struct ItpObjectVisibleSet
{
    TpObject *object;
    bool visible;
};

struct ItpObjectRotateSet
{
    TpObject *object;
    ItpRotateType rotate;
};

struct ItpObjectPaintInput
{
    TpObject *object;
    TpRect updateRect;
};

struct ItpObjectPaintSet
{
    TpObject *object;
    TpPainter *painter;
    TpRect updateRect;

    bool canDraw;

    ItpObjectPaintSet()
    {
        int a = 0;
    }
};

struct ItpObjectActiveSet
{
    TpObject *object;
    bool actived;
};

#endif

// (触发事件的对象指针，事件数据，事件函数名，对象是否是可用态（外部传入，这样可以控制某些事件在disbled状态下也可触发事件，例如resize）)
#ifndef IssueObjEvent
#define IssueObjEvent(Obj, keyEvent, eventFuncName, enabled)                                                                                                                \
    do                                                                                                                                                                      \
    {                                                                                                                                                                       \
        if (!enabled)                                                                                                                                                       \
            break;                                                                                                                                                          \
        TpObject *_current = (Obj)->eventFilterObject();                                                                                                                    \
        TpObject *_filterStack[16] = {0};                                                                                                                                   \
        int _depth = 0; /*std::cout << "计数： "<< testCount++ << "  " << #eventFuncName << std::endl;*/                                                                    \
        while (_current && _depth < 16)                                                                                                                                     \
        {                                                                                                                                                                   \
            _filterStack[_depth++] = _current;                                                                                                                              \
            _current = _current->eventFilterObject();                                                                                                                       \
        } /*std::cout << "过滤层数 " << _depth << std::endl;*/                                                                                                              \
        bool _handled = false;                                                                                                                                              \
        for (int _i = _depth - 1; _i >= 0; --_i)                                                                                                                            \
        { /*TimePoint start = Clock::now();*/                                                                                                                               \
            TpObject *lastObj = (_i > 0) ? _filterStack[_i - 1] : (Obj);                                                                                                    \
            if (_filterStack[_i]->eventFilter((lastObj), &keyEvent))                                                                                                        \
            {                                                                                                                                                               \
                _handled = true;                                                                                                                                            \
                break;                                                                                                                                                      \
            } /*TimePoint end = Clock::now();Duration elapsed = end - start;*/                                                                                              \
            /*std::cout << " 第 " << _i << " 层 执行耗时: " << elapsed.count() * 1000000 << " 微秒 (us)" << std::endl;                                              \
            std::cout << " 第 " << _i << " 层 执行耗时: " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() << " 毫秒 (ms)" << std::endl;*/ \
        } /*std::cout << std::endl<< std::endl<< std::endl<< std::endl;*/                                                                                                   \
        if (!_handled)                                                                                                                                                      \
        {                                                                                                                                                                   \
            (Obj)->eventFuncName(&keyEvent);                                                                                                                                \
            _handled = true;                                                                                                                                                \
        }                                                                                                                                                                   \
    } while (0);
#endif