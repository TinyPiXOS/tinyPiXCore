#include "TpFilePathWidget.h"
#include "TpImage.h"
#include "TpFont.h"
#include "TpPainter.h"
#include "TpEvent.h"
#include "TpFileInfo.h"

struct TpFilePathWidgetData
{
    // 路径lable和箭头label
    TpVector<TpFilePathWidgetItem *> pathLabelList;
    // TpVector<TpLabel *> arrowLabelList;

    TpImage arrowSurface;

    TpFilePathWidgetItem *curClickedItem = nullptr;

    TpString rootPath = "";

    TpString curPath = "";
};

TpFilePathWidget::TpFilePathWidget(TpWidget *parent)
    : TpWidget(parent)
{
    TpFilePathWidgetData *buttonData = new TpFilePathWidgetData();
    buttonData->arrowSurface.load("/usr/res/TinyPiX/箭头-右.png");

    data_ = buttonData;

    refreshBaseCss();
}

TpFilePathWidget::~TpFilePathWidget()
{
    TpFilePathWidgetData *buttonData = static_cast<TpFilePathWidgetData *>(data_);
    if (buttonData)
    {
        delete buttonData;
        buttonData = nullptr;
        data_ = nullptr;
    }
}

void TpFilePathWidget::setRootPath(const TpString &rootPath)
{
    TpFilePathWidgetData *buttonData = static_cast<TpFilePathWidgetData *>(data_);

    TpFileInfo curPath(rootPath);
    buttonData->rootPath = curPath.canonicalFilePath();
}

TpString TpFilePathWidget::rootPath()
{
    TpFilePathWidgetData *buttonData = static_cast<TpFilePathWidgetData *>(data_);
    return buttonData->rootPath;
}

void TpFilePathWidget::setPath(const TpString &path)
{
    TpFileInfo curPath(path);
    TpString dealPath = curPath.canonicalFilePath();

    TpFilePathWidgetData *buttonData = static_cast<TpFilePathWidgetData *>(data_);

    buttonData->curPath = dealPath;

    TpString curAllPath = "";
    dealPath = dealPath.replace(buttonData->rootPath, "/");

    int startFindIndex = 0;

    // 清空之前的路径item
    for (const auto &oldItem : buttonData->pathLabelList)
    {
        oldItem->deleteLater();
    }
    buttonData->pathLabelList.clear();

    TpList<TpString> pathList = dealPath.split('/');
    for (const auto &subPath : pathList)
    {
        if (subPath.empty())
            continue;

        curAllPath += "/" + subPath;

        TpFilePathWidgetItem *newPathLabel = new TpFilePathWidgetItem(this);
        newPathLabel->setChecked(false);
        newPathLabel->setEnableBackGroundColor(true);
        newPathLabel->setAlign(Tp::AlignCenter);
        newPathLabel->setText(subPath);
        newPathLabel->setProperty("Path", curAllPath);
        newPathLabel->installEventFilter(this);
        newPathLabel->setHeight(height());

        buttonData->pathLabelList.emplace_back(newPathLabel);
    }

    // 最后一个item选中
    if (buttonData->pathLabelList.size() > 0)
        buttonData->pathLabelList.back()->setChecked(true);

    update();
}

TpString TpFilePathWidget::path()
{
    TpFilePathWidgetData *buttonData = static_cast<TpFilePathWidgetData *>(data_);
    return buttonData->curPath;
}

bool TpFilePathWidget::onPaintEvent(TpPaintEvent *event)
{
    TpWidget::onPaintEvent(event);

    TpFilePathWidgetData *buttonData = static_cast<TpFilePathWidgetData *>(data_);
    if (buttonData->pathLabelList.size() == 0)
        return true;

    TpPainter *paintCanvas = event->painter();

    tpShared<TpCssData> curCssData = currentStatusCss();

    // 计算当前显示的路径需要的宽度，算出偏移量
    uint32_t offsetX = 0;

    uint32_t allWidth = 0;
    uint32_t iconSize = height() * 0.62;
    for (int i = 0; i < buttonData->pathLabelList.size(); ++i)
    {
        TpFilePathWidgetItem *curItem = buttonData->pathLabelList.at(i);

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

    TpImage scaledSurface = buttonData->arrowSurface.scaled(iconSize, iconSize);

    int32_t curStartX = -offsetX;
    uint32_t iconY = (height() - iconSize) / 2.0;

    for (int i = 0; i < buttonData->pathLabelList.size(); ++i)
    {
        TpFilePathWidgetItem *curItem = buttonData->pathLabelList.at(i);

        curItem->move(curStartX, 0);
        curStartX += curItem->width() + curCssData->gap();

        if (i != (buttonData->pathLabelList.size() - 1))
        {
            paintCanvas->drawImage(curStartX, iconY, scaledSurface);
            curStartX += iconSize + curCssData->gap();
        }
    }

    return true;
}

bool TpFilePathWidget::onResizeEvent(TpResizeEvent *event)
{
    TpWidget::onResizeEvent(event);

    TpFilePathWidgetData *buttonData = static_cast<TpFilePathWidgetData *>(data_);
    for (int i = 0; i < buttonData->pathLabelList.size(); ++i)
    {
        buttonData->pathLabelList.at(i)->setHeight(height());
    }

    return true;
}

bool TpFilePathWidget::eventFilter(TpObject *watched, TpEvent *event)
{
    if (event->eventType() == TpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        TpFilePathWidgetData *buttonData = static_cast<TpFilePathWidgetData *>(data_);

        TpFilePathWidgetItem *pathItem = dynamic_cast<TpFilePathWidgetItem *>(watched);
        if (pathItem)
        {
            buttonData->curClickedItem = pathItem;
            // TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
            // onMousePressEvent(mouseEvent);
        }
        return true;
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        TpFilePathWidgetData *buttonData = static_cast<TpFilePathWidgetData *>(data_);

        TpFilePathWidgetItem *pathItem = dynamic_cast<TpFilePathWidgetItem *>(watched);
        if (pathItem)
        {
            buttonData->curClickedItem = pathItem;

            TpFilePathWidgetData *buttonData = static_cast<TpFilePathWidgetData *>(data_);
            if (buttonData->curClickedItem)
            {
                TpString curLastPath = buttonData->rootPath + buttonData->pathLabelList.back()->property("Path").toString();
                TpString curClickedPath = buttonData->rootPath + buttonData->curClickedItem->property("Path").toString();

                if (curClickedPath.compare(curLastPath) != 0)
                {
                    setPath(curClickedPath);
                    onPathChanged.emit(curLastPath, curClickedPath);
                }
            }
        }

        return true;
    }
    else
    {
    }

    return false;
}

void TpFilePathWidget::onThemeChangeEvent(TpThemeChangeEvent *event)
{
}
