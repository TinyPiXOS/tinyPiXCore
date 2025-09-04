/*
    内部 全局数据定义，禁止include目录下的文件包含此文件
*/

#ifndef __TP_DEF_H
#define __TP_DEF_H

#include "TpUtils.h"
#include "TpList.h"
#include "TpEvent.h"
#include "TpCDef.h"
#include "keyboard.h"
#include <tinyPiXWF.h>
#include <mutex>
#include <list>
#include "TpHash.h"
#include "TpVariant.h"
#include "TpChildWidget.h"
#include "TpImage.h"
#include "TpGraphicsBlurEffect.h"

#define OBJECT_MAX_TEXT_LENGTH 1024

class TpLayout;
class TpObjectStack;
class TpEvent;

struct ItpTempDef
{
    TpChildWidget *curfocus = nullptr;
    TpChildWidget *lstfocus = nullptr;
    TpChildWidget *curmotion = nullptr;
    TpChildWidget *lstmotion = nullptr;
    TpChildWidget *curObject = nullptr;
    // TpChildWidget *dragObject = nullptr;

    // 鼠标移动前的上一个坐标
    TpPoint lastPoint;

    // 新增悬停链状态
    // TpVector<TpChildWidget *> prevHoverChain; // 上一帧悬停链
    // TpVector<TpChildWidget *> currHoverChain; // 当前悬停链

    ItpTempDef()
    {
    }

    /// @brief 移除对象时，如果缓存了obj，需要同步置空
    /// @param delObj
    void deleteObject(TpObject *delObj)
    {
        TpChildWidget *needDelObj = dynamic_cast<TpChildWidget *>(delObj);
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

struct ItpObjectSet
{
    // 鼠标按下的对象，用于判断拖拽、等事件
    // 记录鼠标按下时的对象，最后鼠标无论在哪释放，都触发按下对象的release
    TpChildWidget *mousePressObject = nullptr;

    TpList<TpObject *> objectList;

    TpObject *filterObject = nullptr;

    TpObject *parent = nullptr;
    TpObject *top = nullptr;

    bool visible = true;
    bool enable = true;

    char text[OBJECT_MAX_TEXT_LENGTH];

    std::mutex layoutMutex;
    TpLayout *layout = nullptr;

    int32_t offsetX;
    int32_t offsetY;

    /// @brief 绝对坐标
    TpRect absoluteRect;
    /// @brief 逻辑坐标
    TpRect logicalRect;

    // 窗口最小宽高
    uint32_t minimumWidth = 0;
    uint32_t minimumHeight = 0;

    // 窗口最大宽高
    uint32_t maximumWidth = WIDGET_MAX_WIDTH;
    uint32_t maximumHeight = WIDGET_MAX_HEIGHT;

    int32_t objectID;

    // 背景图片
    bool enableImage;
    TpImage reserveImage;
    TpImage cacheImage;
    bool keepAspectRatio = true;

    // 背景颜色
    bool enableColor = true;
    uint32_t backColor;

    // 边框颜色
    bool enableBorderColor = false;
    uint32_t borderColor;

    // 是否可选中；选中状态
    bool checkable = false;
    bool isChecked = false;

    // 是否启用背景模糊，模糊半径 px
    bool enableBlur = false;
    TpGraphicsBlurEffect blurEffect;

    bool isHover = false;
    bool isPress = false;
    TpPoint pressPoint;

    // 圆角值，单位px
    uint32_t round = 0;

    uint8_t alpha;

    IPiWFApiAgent *agent = nullptr;
    std::mutex gMutex;
    // TpObjectStack *objectStack = nullptr;
    ItpTempDef tmp;

    // 对象属性信息
    TpHash<TpString, TpVariant> objPropertyMap;

    // 缓存有多少发送者信号绑定了自己
    TpVector<std::pair<void *, uintptr_t>> slotConnections_;
    std::mutex slotConnectMutex_;

    ItpObjectSet()
    {
    }
};

struct ItpMouseSet
{
    TpEvent::ItpEventType type;
    uint32_t which;
    MouseEventType button;
    bool state;

    TpPoint pos;
    TpPoint globalPos;

    ItpMouseSet() : type(TpEvent::EVENT_NONE_TYPE), which(0), button(BUTTON_INVALIDATE_VALUE), state(false)
    {
    }
};

/// @brief 键盘事件数据
struct ItpKeyboardSet
{
    /// @brief 事件类型
    TpEvent::ItpEventType type;
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
    TpEvent::ItpEventType type;
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
    TpEvent::ItpEventType type;
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
    TpEvent::ItpEventType type;
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
    TpEvent::ItpEventType type;
    TpObject *object;
    int32_t nx;
    int32_t ny;

    ItpObjectMoveSet()
    {
    }
};

struct ItpObjectResizeSet
{
    TpEvent::ItpEventType type;
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
    TpEvent::ItpEventType type;
    TpObject *object;
    bool focused;

    ItpObjectFocusSet()
    {
    }
};

struct ItpObjectLeaveSet
{
    TpEvent::ItpEventType type;
    TpObject *object;
    bool leaved;
};

struct ItpObjectVisibleSet
{
    TpEvent::ItpEventType type;
    TpObject *object;
    bool visible;
};

struct ItpObjectRotateSet
{
    TpEvent::ItpEventType type;
    TpObject *object;
    ItpRotateType rotate;
};

struct ItpObjectPaintInput
{
    TpObject *object;
    tpShared<TpSurface> surface;
    TpRect updateRect;
};

struct ItpObjectPaintSet
{
    TpEvent::ItpEventType type;

    TpObject *object;

    TpCanvas *canvas;
    tpShared<TpSurface> surface;
    ItpSufaceData *itpSurface;

    int32_t offsetX;
    int32_t offsetY;

    TpRect updateRect;
    TpRect rect;

    bool canDraw;

    ItpObjectPaintSet()
    {
        int a = 0;
    }
};

typedef struct
{
    TpEvent::ItpEventType type;
    struct
    {
        TpObject *object;
        struct
        {
            bool actived;
        };
    };
} ItpObjectActiveSet;

// static int64_t testCount = 0;

// // 使用 type alias 简化代码
// using Clock = std::chrono::high_resolution_clock;
// using Duration = std::chrono::duration<double>;
// using TimePoint = std::chrono::time_point<Clock>;

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