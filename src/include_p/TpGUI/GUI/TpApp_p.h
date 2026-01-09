#ifndef PROCESS_MAX_NAME_LENGTH
#define PROCESS_MAX_NAME_LENGTH 1024
#endif

#ifndef __TP_APP_PRIVATE_H
#define __TP_APP_PRIVATE_H

#include "TpAutoObject.h"
#include "TpScreen.h"
#include "TpConfig.h"
#include "TpTimer.h"
#include "TpMD5.h"
#include "TpDefaultCss.h"
#include "TpEvent.h"
#include "TpDef.h"
#include "TpWidget.h"
#include "TpRect.h"
#include "TpObjectFunction.hpp"
#include "TpObject_p.h"
#include "TpShareMemory.h"
#include "TpMainWindow.h"
#include <TpDir.h>
#include "TpDef.h"
#include "TpApp_def.h"

#include <tinyPiXApi.h>
#include <unistd.h>
#include <getopt.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

// 刷新指令下发
static void DownUpdateCommand(std::queue<UpdateCommand> &updateCommandQueue)
{
    if (updateCommandQueue.size() == 0)
        return;

    TpMap<IPiWFApiAgent *, TpRect> pixwmMergeUpdateRect;
    TpMap<TpWidget *, TpPaintEventInput> mergeUpdateWidget;

    while (!updateCommandQueue.empty())
    {
        UpdateCommand task = updateCommandQueue.front();
        updateCommandQueue.pop();

        if (/*task.updateObj->objectType() == Tp::TP_FLOAT_OBJECT ||*/
            task.updateObj->objectType() == Tp::TP_FIXSCREEN_OBJECT ||
            task.updateObj->objectType() == Tp::TP_MAIN_WINDOW_OBJECT)
        {
            if (!task.updateObj->objectActive())
                continue;
        }

        TpObjectData *updateObjSet = static_cast<TpObjectData *>(task.updateObj->objectSets());
        TpObjectData *topScreenSet = static_cast<TpObjectData *>(updateObjSet->top->objectSets());

        if (pixwmMergeUpdateRect.contains(topScreenSet->agent))
        {
            TpRect taskRect(task.x, task.y, task.w, task.h);
            TpRect &hasRect = pixwmMergeUpdateRect[topScreenSet->agent];
            hasRect.unions(taskRect);
        }
        else
        {
            pixwmMergeUpdateRect[topScreenSet->agent] = TpRect(task.x, task.y, task.w, task.h);
        }

        // 存在该窗口则更新合并区域
        if (mergeUpdateWidget.contains(task.updateObj))
        {
            TpRect taskRect(task.x, task.y, task.w, task.h);

            TpPaintEventInput &paintInfo = mergeUpdateWidget[task.updateObj];
            paintInfo.updateRect.unions(taskRect);
        }
        else
        {
            // 不存在构建新数据
            TpPaintEventInput paintInput;

            // std::cout << "UpdateRect : " << task.updateObj->rect().x() << " , " << task.updateObj->rect().y()
            //           << " , " << task.updateObj->rect().width() << " , " << task.updateObj->rect().height() << std::endl;

            paintInput.object = task.updateObj;
            paintInput.updateRect.setX(task.x);
            paintInput.updateRect.setY(task.y);
            paintInput.updateRect.setWidth(task.w);
            paintInput.updateRect.setHeight(task.h);

            mergeUpdateWidget[task.updateObj] = paintInput;
        }
    }

    for (const auto &updateWidgetIter : mergeUpdateWidget)
    {
        TpObjectData *updateObjSet = static_cast<TpObjectData *>(updateWidgetIter.first->objectSets());
        TpObjectData *topScreenSet = static_cast<TpObjectData *>(updateObjSet->top->objectSets());

        TpPaintEventInput paintInput = updateWidgetIter.second;

        tinyPiX_wf_lock_mutex(topScreenSet->agent);

        // std::cout << "局部刷新：刷新区域： " << updateWidgetIter.first << " : " << paintInput.updateRect.x << " , " << paintInput.updateRect.y << " , "
        //           << paintInput.updateRect.w << " , " << paintInput.updateRect.h << std::endl;

        // int32_t surfaceWidth = paintInput.surface->width();
        // int32_t surfaceHeight = paintInput.surface->height();
        // std::cout << "Surface尺寸： " << surfaceWidth << "  " << surfaceHeight << std::endl;

        if (updateWidgetIter.first->visible())
            drawWidget(paintInput, updateWidgetIter.first);

        TpScreen *topScreen = dynamic_cast<TpScreen *>(updateWidgetIter.first->topObject());
        tvg::SwCanvas *topCanvas = (tvg::SwCanvas *)topScreen->canvasPtr();
        topCanvas->update();
        topCanvas->draw();
        topCanvas->sync();

        tinyPiX_wf_unlock_mutex(topScreenSet->agent);
    }

    // static int32_t testRefreshIndex = 0;

    for (const auto &updateInfo : pixwmMergeUpdateRect)
    {
        const TpRect &updateRect = updateInfo.second;

        // static int testRefreshIndex = 0;
        // std::cout << "TpWM刷新： " << testRefreshIndex++ << "  ;" << updateRect.x() << " : " << updateRect.y() << " , " << updateRect.width() << " , "
        //   << updateRect.height() << std::endl;

        tinyPiX_wf_update(updateInfo.first, updateRect.x(), updateRect.y(), updateRect.width(), updateRect.height(), true, false);
    }
}

static bool bindVScreen(TpAppData *appData, TpFixScreen *object)
{
    if (!appData)
        return false;

    if (!object)
        return false;

    if (object->objectType() != Tp::TP_FIXSCREEN_OBJECT)
    {
        std::cout << "bind screen type error !" << std::endl;
        return false;
    }

    if (appData->vScreen)
    {
        std::cout << "bind screen only once !" << std::endl;
        return false;
    }

    bool ret = (appData->vScreen != object);

    if (ret)
    {
        appData->gMutex.lock();
        appData->vScreen = object;
        appData->gMutex.unlock();
    }

    appData->appExecThread->start();

    return ret;
}

static void sendThemeChangedEvent(TpAppData *setData, const Tp::SystemTheme &sysTheme)
{
    TpThemeChangeEvent *themeEvent = new TpThemeChangeEvent();

    TpString cssFilePath = parseThemeFile(sysTheme);
    setData->cssParser_->parseCss(cssFilePath);

    // 在 app的run函数中，调用主题改变事件函数，通知所有组件
    TpWidget *screenWidget = dynamic_cast<TpWidget *>(setData->vScreen);
    if (screenWidget)
    {
        // 初始化CSS样式表
        // screenWidget->setStyleSheet(cssFilePath);
        screenWidget->onThemeChangeEvent(themeEvent);
        screenWidget->update();
    }

    for (const auto &floatScrenPtr : setData->floatScreenList)
    {
        // floatScrenPtr->setStyleSheet(cssFilePath);
        floatScrenPtr->onThemeChangeEvent(themeEvent);
        floatScrenPtr->update();
    }

    delete themeEvent;
    themeEvent = nullptr;
}

static void initVirtualKeyboard(TpAppData *set)
{
    if (set->virtualKeyboard)
        return;

    set->virtualKeyboard = new TpVirtualKeyboard();

    // 初始化虚拟键盘相关
    connect(set->virtualKeyboard, inputPinyin, [=](const TpString &pinyin)
            {
				if (set->curInputObj)
				{
					set->curInputObj->virtualKeyboardInput(Tp::Pinyin, pinyin);
				} });
    connect(set->virtualKeyboard, finishChinese, [=](const TpString &chinese)
            {
				if (set->curInputObj)
				{
					set->curInputObj->virtualKeyboardInput(Tp::Chinese, chinese);
				} });
    connect(set->virtualKeyboard, deleteSymbol, [=]()
            {
				if (set->curInputObj)
				{
					set->curInputObj->virtualKeyboardInput(Tp::Delete, "");
				} });
    connect(set->virtualKeyboard, inputCharacter, [=](const TpString &character)
            {
				if (set->curInputObj)
				{
					set->curInputObj->virtualKeyboardInput(Tp::Symbol, character);
				} });
}

#endif