#include "tpFilePathWidget.h"
#include "tpSurface.h"
#include "tpFont.h"
#include "tpCanvas.h"
#include "tpEvent.h"
#include "tpFileInfo.h"

struct tpFilePathWidgetData
{
    // 路径lable和箭头label
    tpVector<tpFilePathWidgetItem *> pathLabelList;
    // tpVector<tpLabel *> arrowLabelList;

    tpShared<tpSurface> arrowSurface = tpMakeShared<tpSurface>();

    tpFilePathWidgetItem *curClickedItem = nullptr;

    tpString rootPath = "";

    tpString curPath = "";
};

tpFilePathWidget::tpFilePathWidget(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    tpFilePathWidgetData *buttonData = new tpFilePathWidgetData();
    buttonData->arrowSurface->fromFile("/usr/res/tinyPiX/箭头-右.png");

    data_ = buttonData;

    refreshBaseCss();
}

tpFilePathWidget::~tpFilePathWidget()
{
    tpFilePathWidgetData *buttonData = static_cast<tpFilePathWidgetData *>(data_);
    if (buttonData)
    {
        delete buttonData;
        buttonData = nullptr;
        data_ = nullptr;
    }
}

void tpFilePathWidget::setRootPath(const tpString &rootPath)
{
    tpFilePathWidgetData *buttonData = static_cast<tpFilePathWidgetData *>(data_);

    tpFileInfo curPath(rootPath);
    buttonData->rootPath = curPath.canonicalFilePath();
}

tpString tpFilePathWidget::rootPath()
{
    tpFilePathWidgetData *buttonData = static_cast<tpFilePathWidgetData *>(data_);
    return buttonData->rootPath;
}

void tpFilePathWidget::setPath(const tpString &path)
{
    tpFileInfo curPath(path);
    tpString dealPath = curPath.canonicalFilePath();

    tpFilePathWidgetData *buttonData = static_cast<tpFilePathWidgetData *>(data_);

    buttonData->curPath = dealPath;

    tpString curAllPath = "";
    dealPath = dealPath.replace(buttonData->rootPath, "/");

    int startFindIndex = 0;

    // 清空之前的路径item
    for (const auto &oldItem : buttonData->pathLabelList)
    {
        oldItem->deleteLater();
    }
    buttonData->pathLabelList.clear();

    tpList<tpString> pathList = dealPath.split('/');
    for (const auto &subPath : pathList)
    {
        if (subPath.empty())
            continue;

        curAllPath += "/" + subPath;

        tpFilePathWidgetItem *newPathLabel = new tpFilePathWidgetItem(this);
        newPathLabel->setChecked(false);
        newPathLabel->setEnableBackGroundColor(true);
        newPathLabel->setAlign(tinyPiX::AlignHCenter);
        newPathLabel->setText(subPath);
        newPathLabel->setProperty("Path", curAllPath);
        newPathLabel->installEventFilter(this);
        newPathLabel->setHeight(height());

        buttonData->pathLabelList.emplace_back(newPathLabel);
    }

    // tpList<tpString> pathList = dealPath.split('/');
    // for (const auto &subPath : pathList)
    // {
    //     if (subPath.empty())
    //         continue;

    //     curAllPath += "/" + subPath;

    //     bool isCreateNewLabel = true;
    //     for (; startFindIndex < buttonData->pathLabelList.size(); ++startFindIndex)
    //     {
    //         tpFilePathWidgetItem *curShowLabel = buttonData->pathLabelList.at(startFindIndex);
    //         curShowLabel->setChecked(false);

    //         tpString curShowPath = curShowLabel->property("Path").toString();
    //         if (curShowPath.compare(curAllPath) == 0)
    //         {
    //             isCreateNewLabel = false;
    //             ++startFindIndex;
    //             break;
    //         }
    //     }

    //     if (isCreateNewLabel == false)
    //         continue;

    //     tpFilePathWidgetItem *newPathLabel = new tpFilePathWidgetItem(this);
    //     newPathLabel->setChecked(false);
    //     newPathLabel->setEnableBackGroundColor(true);
    //     newPathLabel->setAlign(tinyPiX::AlignHCenter);
    //     newPathLabel->setText(subPath);
    //     newPathLabel->setProperty("Path", curAllPath);
    //     newPathLabel->installEventFilter(this);
    //     newPathLabel->setHeight(height());

    //     if (startFindIndex == buttonData->pathLabelList.size())
    //         ++startFindIndex;

    //     buttonData->pathLabelList.emplace_back(newPathLabel);
    // }

    // // 释放不需要的路径
    // uint32_t removeCount = buttonData->pathLabelList.size() - startFindIndex;
    // for (int i = 0; i < removeCount; ++i)
    // {
    //     tpFilePathWidgetItem *curShowLabel = buttonData->pathLabelList.back();
    //     buttonData->pathLabelList.pop_back();

    //     curShowLabel->deleteLater();
    // }

    // 最后一个item选中
    if (buttonData->pathLabelList.size() > 0)
        buttonData->pathLabelList.back()->setChecked(true);

    update();
}

tpString tpFilePathWidget::path()
{
    tpFilePathWidgetData *buttonData = static_cast<tpFilePathWidgetData *>(data_);
    return buttonData->curPath;
}

bool tpFilePathWidget::onMousePressEvent(tpMouseEvent *event)
{
    tpChildWidget::onMousePressEvent(event);

    return true;
}

bool tpFilePathWidget::onMouseRleaseEvent(tpMouseEvent *event)
{
    tpChildWidget::onMouseRleaseEvent(event);

    tpFilePathWidgetData *buttonData = static_cast<tpFilePathWidgetData *>(data_);
    if (buttonData->curClickedItem)
    {
        tpString curLastPath = buttonData->rootPath + buttonData->pathLabelList.back()->property("Path").toString();
        tpString curClickedPath = buttonData->rootPath + buttonData->curClickedItem->property("Path").toString();

        if (curClickedPath.compare(curLastPath) != 0)
        {
            setPath(curClickedPath);
            onPathChanged.emit(curLastPath, curClickedPath);
        }
    }

    return true;
}

bool tpFilePathWidget::onPaintEvent(tpObjectPaintEvent *event)
{
    tpChildWidget::onPaintEvent(event);

    tpFilePathWidgetData *buttonData = static_cast<tpFilePathWidgetData *>(data_);
    if (buttonData->pathLabelList.size() == 0)
        return true;

    tpCanvas *paintCanvas = event->canvas();

    tpShared<tpCssData> curCssData = currentStatusCss();

    // 计算当前显示的路径需要的宽度，算出偏移量
    uint32_t offsetX = 0;

    uint32_t allWidth = 0;
    uint32_t iconSize = height() * 0.62;
    for (int i = 0; i < buttonData->pathLabelList.size(); ++i)
    {
        tpFilePathWidgetItem *curItem = buttonData->pathLabelList.at(i);

        allWidth += curItem->width();

        if (i != (buttonData->pathLabelList.size() - 1))
        {
            // 添加间隔以及间隔的箭头
            allWidth += curCssData->gap();

            allWidth += iconSize + curCssData->gap() * 2;
        }
    }

    if (allWidth > width())
    {
        offsetX = allWidth - width();
    }

    tpShared<tpSurface> scaledSurface = buttonData->arrowSurface->scaled(iconSize, iconSize);

    int32_t curStartX = -offsetX;
    uint32_t iconY = (height() - iconSize) / 2.0;

    for (int i = 0; i < buttonData->pathLabelList.size(); ++i)
    {
        tpFilePathWidgetItem *curItem = buttonData->pathLabelList.at(i);

        curItem->move(curStartX, 0);
        curStartX += curItem->width() + curCssData->gap();

        if (i != (buttonData->pathLabelList.size() - 1))
        {
            paintCanvas->paintSurface(curStartX, iconY, scaledSurface);
            curStartX += iconSize + curCssData->gap();
        }
    }

    return true;
}

bool tpFilePathWidget::onResizeEvent(tpObjectResizeEvent *event)
{
    tpChildWidget::onResizeEvent(event);

    tpFilePathWidgetData *buttonData = static_cast<tpFilePathWidgetData *>(data_);
    for (int i = 0; i < buttonData->pathLabelList.size(); ++i)
    {
        buttonData->pathLabelList.at(i)->setHeight(height());
    }

    return true;
}

bool tpFilePathWidget::eventFilter(tpObject *watched, tpEvent *event)
{
    if (event->eventType() == tpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        tpFilePathWidgetData *buttonData = static_cast<tpFilePathWidgetData *>(data_);

        tpFilePathWidgetItem *pathItem = dynamic_cast<tpFilePathWidgetItem *>(watched);
        if (pathItem)
        {
            buttonData->curClickedItem = pathItem;
            tpMouseEvent *mouseEvent = dynamic_cast<tpMouseEvent *>(event);
            onMousePressEvent(mouseEvent);
        }
    }
    else if (event->eventType() == tpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        tpFilePathWidgetData *buttonData = static_cast<tpFilePathWidgetData *>(data_);

        tpFilePathWidgetItem *pathItem = dynamic_cast<tpFilePathWidgetItem *>(watched);
        if (pathItem)
        {
            buttonData->curClickedItem = pathItem;
            tpMouseEvent *mouseEvent = dynamic_cast<tpMouseEvent *>(event);
            onMouseRleaseEvent(mouseEvent);
        }
    }
    else
    {

    }

    return true;
}

void tpFilePathWidget::onThemeChangeEvent(tpThemeChangeEvent *event)
{
}
