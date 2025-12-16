#include "TpWidget.h"
#include "TpApp_p.h"
#include "TpWidget_p.h"
#include "TpScreen_p.h"

TpWidget::TpWidget(TpWidget *parent)
    : TpObject(parent)
{
    TpWidgetData *widgetData = new TpWidgetData();

    // 移除父类的数据；创建widget的指针
    TpObjectData *objData = static_cast<TpObjectData *>(TpObject::data_);
    if (objData)
    {
        delete objData;
        objData = nullptr;
        TpObject::data_ = nullptr;
    }
    TpObject::data_ = widgetData;

    initTpData();

    TpApp::Inst()->sendRegister(this);

    setParent(parent);
    setVisible(true);
}

TpWidget::~TpWidget()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(data_);
    widgetData->tvgScene->unref();

    tvg::Initializer::term();
}

void TpWidget::setProperty(const TpString &_name, const TpVariant &_value)
{
    TpObject::setProperty(_name, _value);

    // 如果更新控件type，更新样式
    if (_name.compare("type") == 0)
    {
        TpWidgetData *childData = static_cast<TpWidgetData *>(data_);

        childData->enabledCssData = readCss(pluginType(), TpCssParser::Enabled);
        childData->pressCssData = readCss(pluginType(), TpCssParser::Pressed);
        childData->hoverCssData = readCss(pluginType(), TpCssParser::Hover);
        childData->checkedCssData = readCss(pluginType(), TpCssParser::Checked);
        childData->disabledCssData = readCss(pluginType(), TpCssParser::Disabled);

        refreshBaseCss();

        update();
    }
}

void TpWidget::deleteLater()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);

    // 顶层窗口缓存清除
    if (widgetData->top)
    {
        TpWidgetData *topData = static_cast<TpWidgetData *>(widgetData->top->objectSets());
        topData->tmp.deleteObject(this);

        // 移除顶部节点缓存的widget节点
        for (const auto &child : widgetData->objectList)
        {
            topData->tmp.deleteObject(child);
        }
    }

    TpObject::deleteLater();

    setParent(nullptr);
    uninstallEventFilter();
}

void TpWidget::close()
{
    setVisible(false);
}

void TpWidget::show()
{
    setVisible(true);
}

void TpWidget::showMaximum()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    uint32_t sWidth = 0;
    uint32_t sHeight = 0;
    tinyPiX_wf_get_display_size(widgetData->agent, &sWidth, &sHeight);

    setRect(0, 0, sWidth, sHeight);
    setVisible(true);

    update();
}

void TpWidget::setVisible(bool visible)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(data_);
    if (!widgetData)
        return;

    if (visible == widgetData->visible)
        return;

    if (widgetData->tvgScene)
    {
        widgetData->tvgScene->visible(visible);
    }

    widgetData->visible = visible;
    TpVisibleEventData input;

    input.object = this;
    input.visible = visible;

    TpVisibleEvent event;
    bool ret = event.construct(&input);

    if (ret)
    {
        this->onVisibleEvent(&event);
    }

    update();
}

bool TpWidget::visible()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(data_);
    if (!widgetData)
        return false;

    return widgetData->visible;
}

void TpWidget::setEnabled(const bool &enable)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(data_);
    if (!widgetData)
        return;

    widgetData->enable = enable;

    // 所有子组件均要同步设置禁用状态
    for (const auto &childObj : TpObject::objectList())
    {
        TpWidget *childWidget = dynamic_cast<TpWidget *>(childObj);
        if (!childWidget)
            continue;
        childWidget->setEnabled(enable);
    }
    update();
}

bool TpWidget::enabled()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(data_);
    if (!widgetData)
        return false;

    return widgetData->enable;
}

int32_t TpWidget::offsetX()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return 0;

    return widgetData->offsetX;
}

int32_t TpWidget::offsetY()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return 0;

    return widgetData->offsetY;
}

void TpWidget::setRect(const TpRect &rect)
{
    setRect(rect.x(), rect.y(), rect.width(), rect.height());
}

void TpWidget::setRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    move(x, y);
    setSize(w, h);
}

TpRect TpWidget::toScreen()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return TpRect();

    return widgetData->absoluteRect;
}

TpRect TpWidget::rect()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return TpRect();

    return widgetData->logicalRect;
}

TpSize TpWidget::screenSize()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    uint32_t sWidth = 0;
    uint32_t sHeight = 0;
    tinyPiX_wf_get_display_size(widgetData->agent, &sWidth, &sHeight);

    return TpSize(sWidth, sHeight);
}

void TpWidget::setSize(const int32_t &width, const int32_t &height)
{
    setWidth(width);
    setHeight(height);
}

void TpWidget::setSize(const TpSize &size)
{
    setSize(size.width(), size.height());
}

TpSize TpWidget::size()
{
    return TpSize(width(), height());
}

void TpWidget::setWidth(const int32_t &width)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    changeWidth(this, widgetData, width);
}

void TpWidget::setHeight(const int32_t &height)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    changeHeight(this, widgetData, height);
}

int32_t TpWidget::width()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return 0;

    return widgetData->logicalRect.width();
}

int32_t TpWidget::height()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return 0;

    return widgetData->logicalRect.height();
    // return processDeskTopBarHeight(this, widgetData->logicalRect.height());
}

void TpWidget::setMinimumSize(const int32_t &width, const int32_t &height)
{
    setMinumumWidth(width);
    setMinumumHeight(height);
}

void TpWidget::setMinimumSize(const TpSize &minimumSize)
{
    setMinimumSize(minimumSize.width(), minimumSize.height());
}

TpSize TpWidget::minimumSize()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return TpSize();
    return TpSize(widgetData->minimumWidth, widgetData->minimumHeight);
}

void TpWidget::setMinumumWidth(const int32_t &width)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    widgetData->minimumWidth = width;

    if (this->width() < widgetData->minimumWidth)
        setWidth(widgetData->minimumWidth);
}

int32_t TpWidget::minumumWidth()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return 0;
    return widgetData->minimumWidth;
}

void TpWidget::setMinumumHeight(const int32_t &height)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    if (height == 1350)
        int a = 0;

    widgetData->minimumHeight = height;

    if (this->height() < widgetData->minimumHeight)
        setHeight(widgetData->minimumHeight);
}

int32_t TpWidget::minumumHeight()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return 0;
    return widgetData->minimumHeight;
}

void TpWidget::setMaximumSize(const int32_t &width, const int32_t &height)
{
    setMaxumumWidth(width);
    setMaxumumHeight(height);
}

void TpWidget::setMaximumSize(const TpSize &maximumSize)
{
    setMaximumSize(maximumSize.width(), maximumSize.height());
}

TpSize TpWidget::maximumSize()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return TpSize();
    return TpSize(widgetData->maximumWidth, widgetData->maximumHeight);
}

void TpWidget::setMaxumumWidth(const int32_t &width)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    widgetData->maximumWidth = width;

    if (this->width() < widgetData->maximumWidth)
        setWidth(widgetData->maximumWidth);
}

int32_t TpWidget::maxumumWidth()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return 0;
    return widgetData->maximumWidth;
}

void TpWidget::setMaxumumHeight(const int32_t &height)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;
    widgetData->maximumHeight = height;

    if (this->height() > widgetData->maximumHeight)
        setHeight(widgetData->maximumHeight);
}

int32_t TpWidget::maxumumHeight()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return 0;
    return widgetData->maximumHeight;
}

void TpWidget::setFixedSize(const int32_t &width, const int32_t &height)
{
    setFixedWidth(width);
    setFixedHeight(height);
}

void TpWidget::setFixedWidth(const int32_t &width)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    widgetData->minimumWidth = width;
    widgetData->maximumWidth = width;

    setMinumumWidth(width);
    setMaxumumWidth(width);
    setWidth(width);
}

void TpWidget::setFixedHeight(const int32_t &height)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    widgetData->minimumHeight = height;
    widgetData->maximumHeight = height;

    setMinumumHeight(height);
    setMaxumumHeight(height);
    setHeight(height);
}

bool TpWidget::isFixedSize()
{
    return (isFixedWidth() && isFixedHeight());
}

bool TpWidget::isFixedWidth()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return 0;
    return (widgetData->minimumWidth == widgetData->maximumWidth);
}

bool TpWidget::isFixedHeight()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return 0;
    return (widgetData->minimumHeight == widgetData->maximumHeight);
}

void TpWidget::move(int32_t x, int32_t y)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    changeXY(this, widgetData, x, y);
    // update();
}

const TpPoint TpWidget::pos()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return TpPoint();

    TpPoint point(widgetData->logicalRect.x(), widgetData->logicalRect.y());
    return point;
}

void TpWidget::setWindowOpacity(float opacity)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    if (opacity < 0)
        opacity = 0;
    if (opacity > 1)
        opacity = 1;

    widgetData->windowOpacity = opacity;

    // 遍历所有子窗口设置Alpha
    TpList<TpObject *> childList = TpObject::objectList();
    for (const auto &childPtr : childList)
    {
        TpWidget *childWidget = dynamic_cast<TpWidget *>(childPtr);
        if (!childWidget)
            continue;
        childWidget->setWindowOpacity(opacity);
    }

    update();
}

float TpWidget::windowOpacity()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return 0;

    return widgetData->windowOpacity;
}

void TpWidget::bringToTop()
{
    TpWidget *parentWidget = dynamic_cast<TpWidget *>(parent());
    if (!parentWidget)
        return;

    // 将本窗体添加至父窗体子节点链表末尾
    TpObjectData *objData = static_cast<TpObjectData *>(parentWidget->objectSets());
    if (objData->objectList.contains(this))
    {
        if (objData->objectList.back() != this)
        {
            objData->objectList.remove(this);
            objData->objectList.emplace_back(this);
        }
    }
}

void TpWidget::bringToBottom()
{
    TpWidget *parentWidget = dynamic_cast<TpWidget *>(parent());
    if (!parentWidget)
        return;

    // 将本窗体添加至父窗体子节点链表起始
    TpObjectData *objData = static_cast<TpObjectData *>(parentWidget->objectSets());
    if (objData->objectList.contains(this))
    {
        if (objData->objectList.front() != this)
        {
            objData->objectList.remove(this);
            objData->objectList.emplace_front(this);
        }
    }
}

bool TpWidget::setLayout(TpLayout *layout)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return false;

    if (!layout)
        return false;

    if (layout == widgetData->layout)
        return false;

    if (widgetData->layout)
        return false;

    if (widgetData->layoutMutex.try_lock())
    {
        widgetData->layout = layout;
        layout->setParent(this);
        widgetData->layout->update();

        widgetData->layoutMutex.unlock();
    }

    return true;
}

TpLayout *TpWidget::layout()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return nullptr;

    return widgetData->layout;
}

void TpWidget::update(const TpRect &rect, bool onlyBlit)
{
    update(rect.x(), rect.y(), rect.width(), rect.height(), onlyBlit);
}

void TpWidget::update(int32_t x, int32_t y, int32_t w, int32_t h, bool onlyBlit)
{
    if (!visible())
        return;

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    TpWidget *topScreenWidget = static_cast<TpWidget *>(widgetData->top);
    if (!topScreenWidget)
        return;

    TpPoint point = selfToScreenPoint(this, x, y);
    bool ret = true;

    x = point.x();
    y = point.y();

    TpRect blitRect(x, y, w, h);

    TpRect screenRect = topScreenWidget->toScreen();
    TpRect selfRect(screenRect);
    ret = blitRect.intersect(selfRect);

    if (ret)
    {
        // TpWidget *parentWidget = dynamic_cast<TpWidget *>(this->parent());
        // if (parentWidget)
        // {
        //     TpPoint parentPos = parentWidget->pos();
        //     TpApp::Inst()->postUpdateEvent(parentWidget, parentPos.x(), parentPos.y(), parentWidget->width(), parentWidget->height(), onlyBlit);
        // }

        // TpApp::Inst()->postUpdateEvent(this, x, y, width(), height(), onlyBlit);

        TpApp::Inst()->postUpdateEvent(topScreenWidget, topScreenWidget->toScreen().x(), topScreenWidget->toScreen().y(), topScreenWidget->width(), topScreenWidget->height(), onlyBlit);
    }
}

void TpWidget::update(bool onlyBlit)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    update(widgetData->logicalRect.x(), widgetData->logicalRect.y(), widgetData->logicalRect.width(), widgetData->logicalRect.height(), onlyBlit);
}

void TpWidget::setCheckable(const bool &_checkable)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    widgetData->checkable = _checkable;
}

bool TpWidget::checkable()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return false;

    return widgetData->checkable;
}

void TpWidget::setChecked(const bool &_isChecked)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    widgetData->isChecked = _isChecked;
    update();
}

bool TpWidget::checked()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return false;

    return widgetData->isChecked;
}

void TpWidget::setRoundCorners(const uint32_t &round)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    widgetData->round = round;

    // CSS解析完，初始化默认状态下CSS数据对象
    enabledCss()->setRoundCorners(round);
    pressedCss()->setRoundCorners(round);
    hoveredCss()->setRoundCorners(round);
    checkedCss()->setRoundCorners(round);
    disableCss()->setRoundCorners(round);
}

uint32_t TpWidget::roundCorners() const
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return 0;

    return widgetData->round;
}

TpImage TpWidget::backGroundCacheImage()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return TpImage();

    return widgetData->cacheImage;
}

void TpWidget::setBackGroundImage(TpImage image, bool keepAspectRatio)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    if (image.isNull())
        return;

    widgetData->reserveImage = image;
    widgetData->enableImage = true;
    widgetData->keepAspectRatio = keepAspectRatio;

    refreshCacheImage(widgetData);

    update();
}

TpImage TpWidget::backGroundImage()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return TpImage();

    return widgetData->reserveImage;
}

bool TpWidget::enableBackGroundImage()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return false;

    return widgetData->enableImage;
}

void TpWidget::setEnableBackGroundImage(bool enable)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    widgetData->enableImage = enable;
}

void TpWidget::setBackGroundColor(const TpColors &color, bool enable)
{
    setBackGroundColor(color.rgba(), enable);
}

void TpWidget::setBackGroundColor(int32_t color, bool enable)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    widgetData->backColor = color;
    widgetData->enableColor = enable;

    // CSS解析完，初始化默认状态下CSS数据对象
    enabledCss()->setBackgroundColor(color);
    pressedCss()->setBackgroundColor(color);
    hoveredCss()->setBackgroundColor(color);
    checkedCss()->setBackgroundColor(color);
    disableCss()->setBackgroundColor(color);

    update();
}

void TpWidget::setBackGroundColor(const TpBrush &bgBrush, bool enable)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    widgetData->backBrush = bgBrush;
    widgetData->enableColor = enable;

    // CSS解析完，初始化默认状态下CSS数据对象
    enabledCss()->setBackgroundColor(widgetData->backBrush.gradient());
    pressedCss()->setBackgroundColor(widgetData->backBrush.gradient());
    hoveredCss()->setBackgroundColor(widgetData->backBrush.gradient());
    checkedCss()->setBackgroundColor(widgetData->backBrush.gradient());
    disableCss()->setBackgroundColor(widgetData->backBrush.gradient());

    update();
}

uint32_t TpWidget::backGroundColor()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return 0;

    return widgetData->backColor;
}

bool TpWidget::enableBackGroundColor()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return false;

    return widgetData->enableColor;
}

void TpWidget::setEnableBackGroundColor(bool enable)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    widgetData->enableColor = enable;
}

void TpWidget::setBorderColor(const TpColors &color, bool enable)
{
    setBorderColor(color.rgba(), enable);
}

void TpWidget::setBorderColor(int32_t color, bool enable)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    widgetData->borderColor = color;
    widgetData->enableBorderColor = enable;

    // CSS解析完，初始化默认状态下CSS数据对象
    enabledCss()->setBorderColor(color);
    pressedCss()->setBorderColor(color);
    hoveredCss()->setBorderColor(color);
    checkedCss()->setBorderColor(color);
    disableCss()->setBorderColor(color);

    update();
}

void TpWidget::setBorderColor(const TpBrush &borderBrush, bool enable)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;
    widgetData->borderBrush = borderBrush;
    widgetData->enableBorderColor = enable;

    // CSS解析完，初始化默认状态下CSS数据对象
    enabledCss()->setBorderColor(widgetData->borderBrush.gradient());
    pressedCss()->setBorderColor(widgetData->borderBrush.gradient());
    hoveredCss()->setBorderColor(widgetData->borderBrush.gradient());
    checkedCss()->setBorderColor(widgetData->borderBrush.gradient());
    disableCss()->setBorderColor(widgetData->borderBrush.gradient());
}

uint32_t TpWidget::borderColor()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return _RGB(0, 0, 0);

    return widgetData->borderColor;
}

bool TpWidget::enableBorderColor()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return false;

    return widgetData->enableBorderColor;
}

void TpWidget::setEnabledBorderColor(bool enable)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    widgetData->enableBorderColor = enable;
}

void TpWidget::setGraphicsEffect(const TpGraphicsBlurEffect &blurEffect)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;
    widgetData->enableBlur = true;
    widgetData->blurEffect = blurEffect;
    update();
}

TpGraphicsBlurEffect TpWidget::graphicsEffect()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return TpGraphicsBlurEffect();
    return widgetData->blurEffect;
}

void TpWidget::setEnableGraphicsEffect(const bool &enable)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return;

    widgetData->enableBlur = enable;
    update();
}

bool TpWidget::enableGraphicsEffect()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return false;

    return widgetData->enableBlur;
}

void SetTopFunc(TpObject *topObj, TpObjectData *findSetData)
{
    for (const auto &setObj : findSetData->objectList)
    {
        TpObjectData *curSet = (TpObjectData *)setObj->objectSets();
        curSet->top = topObj;

        if (curSet->objectList.size() > 0)
            SetTopFunc(topObj, curSet);
    }
};

void TpWidget::setParent(TpObject *parent)
{
    // TpWidget 只能设置 TpWidget 类型的父对象
    TpWidget *parentWidget = dynamic_cast<TpWidget *>(parent);
    if (parent && !parentWidget)
        return;

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);

    // 如果已经有父窗口，需要将原先的scene链表中断
    if (widgetData->parent && (widgetData->parent != parent))
    {
        widgetData->tvgScene->remove();

        // TpScreen *topScreen = static_cast<TpScreen *>(topObject());
        // if (topScreen)
        // {
        //     TpScreenData *topScreenData = static_cast<TpScreenData *>(topScreen->data_);
        //     topScreenData->swCanvas->remove(widgetData->tvgScene);
        // }

        TpWidget *lastParentWidget = dynamic_cast<TpWidget *>(widgetData->parent);
        TpWidgetData *lastParentWidgetData = static_cast<TpWidgetData *>(lastParentWidget->data_);
        lastParentWidgetData->tvgScene->remove(widgetData->tvgScene);
    }

    TpObject::setParent(parent);

    if (parent)
    {
        broadObjectSetTop(this, this->topObject());
    }

    // 遍历this的所有子节点，所有子节点查询一下top
    if (widgetData->top)
    {
        SetTopFunc(widgetData->top, widgetData);
    }

    // 将自己的scene加入父组件的scene
    if (parentWidget)
    {
        TpWidgetData *parentWidgetData = static_cast<TpWidgetData *>(parentWidget->data_);
        parentWidgetData->tvgScene->push(widgetData->tvgScene);

        // 父节点改变后，重新计算裁剪区域
        // refreshSceneClipRect(this, widgetData);
        // ClipRectOptimizer::markWidgetForRefresh(this);

        // widgetData->tvgScene->visible(true);
        // std::cout << "parentWidget->pluginType() " << parentWidget->pluginType() << std::endl;
        // std::list<tvg::Paint *> canvasSceneList = parentWidgetData->tvgScene->paints();
        // std::cout << "11111111111topSceneChildList : " << canvasSceneList.size() << std::endl;
    }
}

bool TpWidget::onMousePressEvent(TpMouseEvent *event)
{
    if (event->button() != BUTTON_LEFT)
        return true;

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    widgetData->isPress = true;
    widgetData->pressPoint = event->globalPos();

    update();

    return true;
}

bool TpWidget::onMouseRleaseEvent(TpMouseEvent *event)
{
    if (event->button() != BUTTON_LEFT)
        return true;

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    widgetData->isPress = false;

    TpPoint mouseGlobalPos = event->globalPos();
    bool isUpdate = false;

    if ((std::abs(mouseGlobalPos.x() - widgetData->pressPoint.x()) <= 5) && (std::abs(mouseGlobalPos.y() - widgetData->pressPoint.y()) <= 5))
    {
        if (checkable())
        {
            setChecked(!checked());
            isUpdate = true;
        }
    }

    if (!isUpdate)
        update();

    return true;
}

bool TpWidget::onResizeEvent(TpResizeEvent *event)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);

    if (widgetData->layoutMutex.try_lock())
    {
        // widgetData->layoutMutex.lock();

        if (widgetData->layout)
        {
            widgetData->layout->update();
        }

        widgetData->layoutMutex.unlock();
    }

    return true;
}

bool TpWidget::onLeaveEvent(TpLeaveEvent *event)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    widgetData->isHover = event->leave();
    widgetData->isPress = false;
    update();

    return true;
}

bool TpWidget::onPaintEvent(TpPaintEvent *event)
{
    bool ret = event->isCanDraw();
    if (!ret)
        return false;

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(data_);
    if (!widgetData)
        return false;

    if (!widgetData->visible)
        return false;

    TpRect rect = this->rect();

    TpPainter *painter = event->painter();

    // 判断组件当前状态，决定取出哪个CSS样式
    tpShared<TpCssData> curCssData = currentStatusCss();

    // uint32_t minRad = (width() > height() ? height() : width()) * curCssData->roundCorners();
    uint32_t minRad = curCssData->roundCorners();

    if (widgetData->enableColor)
    {
        if (objectType() == Tp::TP_FLOAT_OBJECT)
        {
            int curAlpha = _A(curCssData->backgroundColor());
            curAlpha *= windowOpacity();
            if ((curAlpha & 0xff) != 0xff)
            {
                painter->erase();
            }
        }

        if (curCssData->backgroundColorIsGradient())
        {
            painter->setPen(curCssData->backgroundColor());
            painter->setBrush(TpBrush(curCssData->backgroundColorGradiant()));
        }
        else
        {
            painter->setPen(curCssData->backgroundColor());
            painter->setBrush(TpBrush(curCssData->backgroundColor()));
        }

        // std::cout << "背景颜色： " << 0 << " " << rect.width() << " " << rect.height() << std::endl;
        painter->drawRect(0, 0, rect.width(), rect.height(), minRad);
        painter->setBrush(TpBrush(Tp::NoBrush));
    }

    if (widgetData->enableImage && !widgetData->cacheImage.isNull())
    {
        int32_t imageX = 0;
        int32_t imageY = 0;

        // 如果保持了纵横比，保持图片居中显示
        if (widgetData->keepAspectRatio)
        {
            int32_t imageWidth = widgetData->cacheImage.width();
            int32_t imageHeight = widgetData->cacheImage.height();

            if (imageWidth > width())
            {
                imageX = -(imageWidth - width()) / 2.0;
            }
            else
            {
                imageX = (width() - imageWidth) / 2.0;
            }

            if (imageHeight > height())
            {
                imageY = -(imageHeight - height()) / 2.0;
            }
            else
            {
                imageY = (height() - imageHeight) / 2.0;
            }
        }

        painter->drawImage(imageX, imageY, widgetData->cacheImage, minRad);
    }

    if (widgetData->enableBorderColor)
    {
        painter->setPen(curCssData->borderColor());
        painter->setBrush(TpBrush(Tp::NoBrush));

        // painter->pen().setBrush(TpBrush(Tp::NoBrush));

        if (curCssData->borderColorIsGradient())
        {
            painter->pen().setBrush(TpBrush(curCssData->borderColorGradiant()));
        }

        painter->drawRect(0, 0, rect.width(), rect.height(), minRad);
    }

    // 窗体更新，如果有布局更新布局
    if (widgetData->layout)
    {
        if (widgetData->layoutMutex.try_lock())
        {
            widgetData->layout->update();

            widgetData->layoutMutex.unlock();
        }
    }

    return ret;
}

void TpWidget::onThemeChangeEvent(TpThemeChangeEvent *event)
{
    TpWidgetData *childData = static_cast<TpWidgetData *>(data_);
    if (!childData)
        return;

    // childData->enabledCssData = readCss(pluginType(), TpCssParser::Enabled);
    // childData->pressCssData = readCss(pluginType(), TpCssParser::Pressed);
    // childData->hoverCssData = readCss(pluginType(), TpCssParser::Hover);
    // childData->checkedCssData = readCss(pluginType(), TpCssParser::Checked);
    // childData->disabledCssData = readCss(pluginType(), TpCssParser::Disabled);

    // setRoundCorners(childData->enabledCssData->roundCorners());
}

Tp::TpObjectType TpWidget::objectType()
{
    return Tp::TP_CHILD_OBJECT;
}

TpWidget *TpWidget::find(const TpPoint &point)
{
    return find(point.x(), point.y());
}

TpWidget *TpWidget::find(int32_t x, int32_t y)
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return nullptr;

    TpWidget *object = this;

    TpRect absRect(widgetData->absoluteRect);

    bool ret = absRect.contains(x, y);

    if (ret)
        object = this;

    TpWidget *result = findObject(widgetData, x, y);
    if (result)
        object = result;

    return object;
}

void TpWidget::setStyleSheet(const TpString &_styleSheetStr)
{
    // 解析CSS字符串
    TpApp::Inst()->cssParser()->parseCss(_styleSheetStr);

    if (objectType() != Tp::TP_MAIN_WINDOW_OBJECT && objectType() != Tp::TP_FIXSCREEN_OBJECT)
        refreshBaseCss();
}

TpString TpWidget::styleSheet()
{
    return TpApp::Inst()->cssParser()->cssStr();
}

tpShared<TpCssData> TpWidget::readCss(const TpString &_className, const TpCssParser::MouseStatus &_status)
{
    TpString uiType = property("type").toString();

    return TpApp::Inst()->cssParser()->readCss(_className, uiType, _status);
}

TpImage TpWidget::grabWindow()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (!widgetData)
        return TpImage();

    return widgetData->grapImage;
}

void TpWidget::broadSetTop()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);
    if (widgetData)
    {
        broadObjectSetTop(this, widgetData->top);
    }
}

void *TpWidget::scenePtr()
{
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(data_);
    return widgetData->tvgScene;
}

tpShared<TpCssData> TpWidget::currentStatusCss()
{
    tpShared<TpCssData> curCssData = disableCss();
    if (enabled())
    {
        curCssData = enabledCss();

        TpWidgetData *widgetData = static_cast<TpWidgetData *>(TpObject::data_);

        if (widgetData->checkable && widgetData->isChecked)
        {
            curCssData = checkedCss();
        }
        else
        {
            if (widgetData->isHover)
            {
                curCssData = hoveredCss();
            }
            if (widgetData->isPress)
                curCssData = pressedCss();
        }
    }

    return curCssData;
}

tpShared<TpCssData> TpWidget::enabledCss()
{
    TpWidgetData *childData = static_cast<TpWidgetData *>(data_);
    if (childData == nullptr)
        return tpMakeShared<TpCssData>(TpHash<TpString, TpString>{});

    if (childData->enabledCssData == nullptr)
    {
        childData->enabledCssData = readCss(pluginType(), TpCssParser::Enabled);
    }

    return childData->enabledCssData;
}

tpShared<TpCssData> TpWidget::disableCss()
{
    TpWidgetData *childData = static_cast<TpWidgetData *>(data_);
    if (childData == nullptr)
        return tpMakeShared<TpCssData>(TpHash<TpString, TpString>{});

    if (childData->disabledCssData == nullptr)
    {
        childData->disabledCssData = readCss(pluginType(), TpCssParser::Disabled);
    }

    return childData->disabledCssData;
}

tpShared<TpCssData> TpWidget::hoveredCss()
{
    TpWidgetData *childData = static_cast<TpWidgetData *>(data_);
    if (childData == nullptr)
        return tpMakeShared<TpCssData>(TpHash<TpString, TpString>{});

    if (childData->hoverCssData == nullptr)
    {
        childData->hoverCssData = readCss(pluginType(), TpCssParser::Hover);
    }

    return childData->hoverCssData;
}

tpShared<TpCssData> TpWidget::pressedCss()
{
    TpWidgetData *childData = static_cast<TpWidgetData *>(data_);
    if (childData == nullptr)
        return tpMakeShared<TpCssData>(TpHash<TpString, TpString>{});

    if (childData->pressCssData == nullptr)
    {
        childData->pressCssData = readCss(pluginType(), TpCssParser::Pressed);
    }

    return childData->pressCssData;
}

tpShared<TpCssData> TpWidget::checkedCss()
{
    TpWidgetData *childData = static_cast<TpWidgetData *>(data_);
    if (childData == nullptr)
        return tpMakeShared<TpCssData>(TpHash<TpString, TpString>{});

    if (childData->checkedCssData == nullptr)
    {
        childData->checkedCssData = readCss(pluginType(), TpCssParser::Checked);
    }

    return childData->checkedCssData;
}

void TpWidget::refreshBaseCss()
{
    // 每次刷新CSS要从配置文件重新读取，避免产生继承关系时，子类未刷新正确自己的CSS数据
    TpWidgetData *widgetData = static_cast<TpWidgetData *>(data_);
    widgetData->enabledCssData = readCss(pluginType(), TpCssParser::Enabled);
    widgetData->disabledCssData = readCss(pluginType(), TpCssParser::Disabled);
    widgetData->hoverCssData = readCss(pluginType(), TpCssParser::Hover);
    widgetData->pressCssData = readCss(pluginType(), TpCssParser::Pressed);
    widgetData->checkedCssData = readCss(pluginType(), TpCssParser::Checked);

    tpShared<TpCssData> normalCss = currentStatusCss();
    setMinimumSize(normalCss->minimumWidth(), normalCss->minimumHeight());
    setMaximumSize(normalCss->maximumWidth(), normalCss->maximumHeight());
    setSize(normalCss->width(), normalCss->height());
    setRoundCorners(normalCss->roundCorners());
}

void TpWidget::initTpData()
{
    TpObject::initTpData();

    TpWidgetData *widgetData = static_cast<TpWidgetData *>(data_);

    widgetData->visible = false;

    widgetData->offsetX = 0;
    widgetData->offsetY = 0;

    widgetData->backColor = _RGB(248, 248, 248);
    widgetData->enableColor = true;
    widgetData->enableImage = true;

    widgetData->windowOpacity = 1.0;

    widgetData->layout = nullptr;

    widgetData->top = this->topObject();
    widgetData->tvgScene = tvg::Scene::gen();
    widgetData->tvgScene->ref();
}
