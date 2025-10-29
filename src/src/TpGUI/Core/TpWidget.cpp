#include "TpWidget.h"
#include "TpApp_p.h"
#include "TpChildWidget_p.h"

TpWidget::TpWidget(TpWidget *parent)
    : TpObject(parent)
{
    data_ = new TpWidgetCssData();

    TpApp::Inst()->sendRegister(this);

    setParent(parent);
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();

    if (set)
    {
        set->top = this->topObject();
    }

    setVisible(true);

    // 根据CPU核心数；分配绘图引擎线程数
    uint32_t cores = std::thread::hardware_concurrency();
    tvg::Initializer::init(cores / 2);
}

TpWidget::~TpWidget()
{
    tvg::Initializer::term();

    TpWidgetCssData *childData = static_cast<TpWidgetCssData *>(data_);
    if (childData)
    {
        delete childData;
        childData = nullptr;
        data_ = nullptr;
    }
}

void TpWidget::setProperty(const TpString &_name, const TpVariant &_value)
{
    TpObject::setProperty(_name, _value);

    // 如果更新控件type，更新样式
    if (_name.compare("type") == 0)
    {
        TpWidgetCssData *childData = static_cast<TpWidgetCssData *>(data_);

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
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());

    // 顶层窗口缓存清除
    if (set->top)
    {
        TpObjectData *topData = static_cast<TpObjectData *>(set->top->objectSets());
        topData->tmp.deleteObject(this);

        TpList<TpObject *> thisChildList = this->objectList();
        for (const auto &child : thisChildList)
        {
            child->uninstallEventFilter();
            topData->tmp.deleteObject(child);
        }
    }

    setParent(nullptr);

    // set->tmp.deleteObject(this);

    uninstallEventFilter();

    TpObject::deleteLater();
    // TpApp::Inst()->sendDelete(this);
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
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
    uint32_t sWidth = 0;
    uint32_t sHeight = 0;
    tinyPiX_wf_get_display_size(set->agent, &sWidth, &sHeight);

    setRect(0, 0, sWidth, sHeight);
    setVisible(true);

    update();
}

void TpWidget::setVisible(bool visible)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());

    if (!set)
        return;

    if (visible == set->visible)
        return;

    set->visible = visible;
    ItpObjectVisibleSet input;

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
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return false;

    return set->visible;
}

void TpWidget::setEnabled(const bool &enable)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    set->enable = enable;

    // 所有子组件均要同步设置禁用状态
    for (const auto &childObj : set->objectList)
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
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return false;

    return set->enable;
}

void TpWidget::setText(const TpString &text)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    if (text.length() < OBJECT_MAX_TEXT_LENGTH - 1)
    {
        memcpy(set->text, text.c_str(), text.length());
        set->text[text.length()] = '\0'; // 确保字符串以空字符结尾
    }
    else
    {
        memcpy(set->text, text.c_str(), OBJECT_MAX_TEXT_LENGTH - 1);
        set->text[OBJECT_MAX_TEXT_LENGTH - 1] = '\0'; // 防止缓冲区溢出
    }
}

TpString TpWidget::text()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return "";

    return set->text;
}

int32_t TpWidget::offsetX()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return 0;

    return set->offsetX;
}

int32_t TpWidget::offsetY()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return 0;

    return set->offsetY;
}

void TpWidget::setRect(const TpRect &rect)
{
    setRect(rect.x(), rect.y(), rect.width(), rect.height());
}

void TpWidget::setRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    changeXY(this, set, x, y);

    setSize(w, h);
}

TpRect TpWidget::toScreen()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return TpRect();

    return set->absoluteRect;
}

TpRect TpWidget::rect()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return TpRect();

    return set->logicalRect;
}

TpSize TpWidget::screenSize()
{
    TpObjectData *set = (TpObjectData *)TpObject::objectSets();
    uint32_t sWidth = 0;
    uint32_t sHeight = 0;
    tinyPiX_wf_get_display_size(set->agent, &sWidth, &sHeight);

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
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    changeWidth(this, set, width);
}

void TpWidget::setHeight(const int32_t &height)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    changeHeight(this, set, height);
}

int32_t TpWidget::width()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return 0;

    return set->logicalRect.width();
}

int32_t TpWidget::height()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return 0;

    return set->logicalRect.height();
    // return processDeskTopBarHeight(this, set->logicalRect.height());
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
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return TpSize();
    return TpSize(set->minimumWidth, set->minimumHeight);
}

void TpWidget::setMinumumWidth(const int32_t &width)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    set->minimumWidth = width;

    if (this->width() < set->minimumWidth)
        setWidth(set->minimumWidth);
}

int32_t TpWidget::minumumWidth()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return 0;
    return set->minimumWidth;
}

void TpWidget::setMinumumHeight(const int32_t &height)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    if (height == 1350)
        int a = 0;

    set->minimumHeight = height;

    if (this->height() < set->minimumHeight)
        setHeight(set->minimumHeight);
}

int32_t TpWidget::minumumHeight()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return 0;
    return set->minimumHeight;
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
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return TpSize();
    return TpSize(set->maximumWidth, set->maximumHeight);
}

void TpWidget::setMaxumumWidth(const int32_t &width)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    set->maximumWidth = width;

    if (this->width() < set->maximumWidth)
        setWidth(set->maximumWidth);
}

int32_t TpWidget::maxumumWidth()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return 0;
    return set->maximumWidth;
}

void TpWidget::setMaxumumHeight(const int32_t &height)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;
    set->maximumHeight = height;

    if (this->height() > set->maximumHeight)
        setHeight(set->maximumHeight);
}

int32_t TpWidget::maxumumHeight()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return 0;
    return set->maximumHeight;
}

void TpWidget::setFixedSize(const int32_t &width, const int32_t &height)
{
    setFixedWidth(width);
    setFixedHeight(height);
}

void TpWidget::setFixedWidth(const int32_t &width)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    set->minimumWidth = width;
    set->maximumWidth = width;

    setMinumumWidth(width);
    setMaxumumWidth(width);
    setWidth(width);
}

void TpWidget::setFixedHeight(const int32_t &height)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    set->minimumHeight = height;
    set->maximumHeight = height;

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
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return 0;
    return (set->minimumWidth == set->maximumWidth);
}

bool TpWidget::isFixedHeight()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return 0;
    return (set->minimumHeight == set->maximumHeight);
}

void TpWidget::move(int32_t x, int32_t y)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    changeXY(this, set, x, y);
    // update();
}

const TpPoint TpWidget::pos()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return TpPoint();

    TpPoint point(set->logicalRect.x(), set->logicalRect.y());
    return point;
}

void TpWidget::setAlpha(const uint8_t &alpha)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    set->alpha = alpha;
}

uint8_t TpWidget::alpha()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return 0;

    return set->alpha;
}

bool TpWidget::setLayout(TpLayout *layout)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return false;

    if (!layout)
        return false;

    if (layout == set->layout)
        return false;

    if (set->layout)
        return false;

    if (set->layoutMutex.try_lock())
    {
        // set->layoutMutex.lock();

        // if (set->layout)
        // {
        // 	delObject(set, set->layout);
        // 	set->layout = nullptr;
        // }

        set->layout = layout;
        layout->setParent(this);
        set->layout->update();

        // addObject(set, set->layout, this);

        set->layoutMutex.unlock();
    }

    return true;
}

TpLayout *TpWidget::layout()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return nullptr;

    return set->layout;
}

void TpWidget::update(const TpRect &rect, bool onlyBlit)
{
    update(rect.x(), rect.y(), rect.width(), rect.height(), onlyBlit);
}

void TpWidget::update(int32_t x, int32_t y, int32_t w, int32_t h, bool onlyBlit)
{
    if (!visible())
        return;

    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    TpWidget *topScreenWidget = static_cast<TpWidget *>(set->top);
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
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    update(set->logicalRect.x(), set->logicalRect.y(), set->logicalRect.width(), set->logicalRect.height(), onlyBlit);
}

void TpWidget::setCheckable(const bool &_checkable)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    set->checkable = _checkable;
}

bool TpWidget::checkable()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return false;

    return set->checkable;
}

void TpWidget::setChecked(const bool &_isChecked)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    set->isChecked = _isChecked;
    update();
}

bool TpWidget::checked()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return false;

    return set->isChecked;
}

void TpWidget::setRoundCorners(const uint32_t &round)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    set->round = round;

    // CSS解析完，初始化默认状态下CSS数据对象
    enabledCss()->setRoundCorners(round);
    pressedCss()->setRoundCorners(round);
    hoveredCss()->setRoundCorners(round);
    checkedCss()->setRoundCorners(round);
    disableCss()->setRoundCorners(round);
}

uint32_t TpWidget::roundCorners()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return 0;

    return set->round;
}

TpImage TpWidget::backGroundCacheImage()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return TpImage();

    return set->cacheImage;
}

void TpWidget::setBackGroundImage(TpImage image, bool keepAspectRatio)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    if (image.isNull())
        return;

    set->reserveImage = image;
    set->enableImage = true;
    set->keepAspectRatio = keepAspectRatio;

    refreshCacheImage(set);

    update();
}

TpImage TpWidget::backGroundImage()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return TpImage();

    return set->reserveImage;
}

bool TpWidget::enableBackGroundImage()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return false;

    return set->enableImage;
}

void TpWidget::setEnableBackGroundImage(bool enable)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    set->enableImage = enable;
}

void TpWidget::setBackGroundColor(TpColors &color, bool enable)
{
    setBackGroundColor(color.rgba(), enable);
}

void TpWidget::setBackGroundColor(int32_t color, bool enable)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    set->backColor = color;
    set->enableColor = enable;

    // TpWidgetCssData *childData = static_cast<TpWidgetCssData *>(data_);

    // CSS解析完，初始化默认状态下CSS数据对象
    enabledCss()->setBackgroundColor(color);
    pressedCss()->setBackgroundColor(color);
    hoveredCss()->setBackgroundColor(color);
    checkedCss()->setBackgroundColor(color);
    disableCss()->setBackgroundColor(color);
}

void TpWidget::setBackGroundColor(const TpBrush &bgBrush, bool enable)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;
    set->backBrush = bgBrush;
    set->enableColor = enable;

    // CSS解析完，初始化默认状态下CSS数据对象
    enabledCss()->setBackgroundColor(set->backBrush.gradient());
    pressedCss()->setBackgroundColor(set->backBrush.gradient());
    hoveredCss()->setBackgroundColor(set->backBrush.gradient());
    checkedCss()->setBackgroundColor(set->backBrush.gradient());
    disableCss()->setBackgroundColor(set->backBrush.gradient());

    update();
}

uint32_t TpWidget::backGroundColor()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return 0;

    return set->backColor;
}

bool TpWidget::enableBackGroundColor()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return false;

    return set->enableColor;
}

void TpWidget::setEnableBackGroundColor(bool enable)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    set->enableColor = enable;
}

void TpWidget::setBorderColor(TpColors &color, bool enable)
{
    setBorderColor(color.rgba(), enable);
}

void TpWidget::setBorderColor(int32_t color, bool enable)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    set->borderColor = color;
    set->enableBorderColor = enable;
}

void TpWidget::setBorderColor(const TpBrush &borderBrush, bool enable)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;
    set->borderBrush = borderBrush;
    set->enableBorderColor = enable;

    // CSS解析完，初始化默认状态下CSS数据对象
    enabledCss()->setBorderColor(set->borderBrush.gradient());
    pressedCss()->setBorderColor(set->borderBrush.gradient());
    hoveredCss()->setBorderColor(set->borderBrush.gradient());
    checkedCss()->setBorderColor(set->borderBrush.gradient());
    disableCss()->setBorderColor(set->borderBrush.gradient());
}

uint32_t TpWidget::borderColor()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return _RGB(0, 0, 0);

    return set->borderColor;
}

bool TpWidget::enableBorderColor()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return false;

    return set->enableBorderColor;
}

void TpWidget::setEnabledBorderColor(bool enable)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    set->enableBorderColor = enable;
}

void TpWidget::setGraphicsEffect(const TpGraphicsBlurEffect &blurEffect)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;
    set->enableBlur = true;
    set->blurEffect = blurEffect;
    update();
}

TpGraphicsBlurEffect TpWidget::graphicsEffect()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return TpGraphicsBlurEffect();
    return set->blurEffect;
}

void TpWidget::setEnableGraphicsEffect(const bool &enable)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return;

    set->enableBlur = enable;
    update();
}

bool TpWidget::enableGraphicsEffect()
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return false;

    return set->enableBlur;
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
    TpObject::setParent(parent);

    TpObjectData *set = (TpObjectData *)TpObject::objectSets();

    if (!set)
        return;

    set->top = this->topObject();

    if (parent == nullptr)
        set->top = nullptr;

    // 遍历this的所有子节点，所有子节点查询一下top
    if (set->top)
    {
        SetTopFunc(set->top, set);
    }
}

bool TpWidget::onMousePressEvent(TpMouseEvent *event)
{
    if (event->button() != BUTTON_LEFT)
        return true;

    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    set->isPress = true;
    set->pressPoint = event->globalPos();

    update();

    return true;
}

bool TpWidget::onMouseRleaseEvent(TpMouseEvent *event)
{
    if (event->button() != BUTTON_LEFT)
        return true;

    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    set->isPress = false;

    TpPoint mouseGlobalPos = event->globalPos();
    bool isUpdate = false;

    if ((std::abs(mouseGlobalPos.x() - set->pressPoint.x()) <= 5) && (std::abs(mouseGlobalPos.y() - set->pressPoint.y()) <= 5))
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
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return false;

    if (set->layoutMutex.try_lock())
    {
        // set->layoutMutex.lock();

        if (set->layout)
        {
            set->layout->update();
        }

        set->layoutMutex.unlock();
    }

    return true;
}

bool TpWidget::onLeaveEvent(TpLeaveEvent *event)
{
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    set->isHover = event->leave();
    set->isPress = false;
    update();

    return true;
}

bool TpWidget::onPaintEvent(TpPaintEvent *event)
{
    bool ret = event->isCanDraw();
    uint8_t alpha = 0xff;

    if (!ret)
        return false;

    TpPainter *painter = event->painter();
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return false;

    TpRect rect = event->rect();

    if (!set->visible)
        return ret;

    // 判断组件当前状态，决定取出哪个CSS样式
    tpShared<TpCssData> curCssData = currentStatusCss();

    // uint32_t minRad = (width() > height() ? height() : width()) * curCssData->roundCorners();
    uint32_t minRad = curCssData->roundCorners();

    if (set->enableColor)
    {
        if (objectType() == Tp::TP_FLOAT_OBJECT)
        {
            if ((curCssData->backgroundColor() & 0xff) != 0xff)
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

    if (set->enableBorderColor)
    {
        painter->pen().setColor(curCssData->borderColor());

        if (curCssData->borderColorIsGradient())
        {
            painter->pen().setBrush(TpBrush(curCssData->borderColorGradiant()));
        }
        else
        {
            painter->pen().setBrush(TpBrush(Tp::NoBrush));
        }

        painter->drawRect(0, 0, rect.width(), rect.height(), minRad);
    }

    if (set->enableImage && !set->cacheImage.isNull())
    {
        int32_t imageX = 0;
        int32_t imageY = 0;

        // 如果保持了纵横比，保持图片居中显示
        if (set->keepAspectRatio)
        {
            int32_t imageWidth = set->cacheImage.width();
            int32_t imageHeight = set->cacheImage.height();

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

        painter->drawImage(imageX, imageY, set->cacheImage, minRad);
    }

    // 窗体更新，如果有布局更新布局
    if (set->layout)
    {
        if (set->layoutMutex.try_lock())
        {
            set->layout->update();

            set->layoutMutex.unlock();
        }
    }

    return ret;
}

void TpWidget::onThemeChangeEvent(TpThemeChangeEvent *event)
{
    TpWidgetCssData *childData = static_cast<TpWidgetCssData *>(data_);
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
    TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());
    if (!set)
        return nullptr;

    TpWidget *object = this;

    TpRect absRect(set->absoluteRect);

    bool ret = absRect.contains(x, y);

    if (ret)
        object = this;

    TpWidget *result = findObject(set, x, y);

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
    TpObjectData *widgetData = static_cast<TpObjectData *>(this->objectSets());
    if (!widgetData)
        return TpImage();

    return widgetData->grapImage;
}

std::pair<void *, void *> TpWidget::canvasPtr()
{
    TpWidgetCssData *childData = static_cast<TpWidgetCssData *>(data_);
    if (childData->swCanvas == nullptr)
    {
        childData->swCanvas = tvg::SwCanvas::gen();
        childData->tvgScene = tvg::Scene::gen();
        childData->swCanvas->push(std::move(childData->tvgScene));
    }
    return std::pair<void *, void *>(childData->swCanvas, childData->tvgScene);
}

tpShared<TpCssData> TpWidget::currentStatusCss()
{
    tpShared<TpCssData> curCssData = disableCss();
    if (enabled())
    {
        curCssData = enabledCss();

        TpObjectData *set = static_cast<TpObjectData *>(TpObject::objectSets());

        if (set->checkable && set->isChecked)
        {
            curCssData = checkedCss();
        }
        else
        {
            if (set->isHover)
            {
                curCssData = hoveredCss();
            }
            if (set->isPress)
                curCssData = pressedCss();
        }
    }

    return curCssData;
}

tpShared<TpCssData> TpWidget::enabledCss()
{
    TpWidgetCssData *childData = static_cast<TpWidgetCssData *>(data_);
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
    TpWidgetCssData *childData = static_cast<TpWidgetCssData *>(data_);
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
    TpWidgetCssData *childData = static_cast<TpWidgetCssData *>(data_);
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
    TpWidgetCssData *childData = static_cast<TpWidgetCssData *>(data_);
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
    TpWidgetCssData *childData = static_cast<TpWidgetCssData *>(data_);
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
    TpWidgetCssData *childData = static_cast<TpWidgetCssData *>(data_);
    childData->enabledCssData = readCss(pluginType(), TpCssParser::Enabled);
    childData->disabledCssData = readCss(pluginType(), TpCssParser::Disabled);
    childData->hoverCssData = readCss(pluginType(), TpCssParser::Hover);
    childData->pressCssData = readCss(pluginType(), TpCssParser::Pressed);
    childData->checkedCssData = readCss(pluginType(), TpCssParser::Checked);

    tpShared<TpCssData> normalCss = currentStatusCss();
    setMinimumSize(normalCss->minimumWidth(), normalCss->minimumHeight());
    setMaximumSize(normalCss->maximumWidth(), normalCss->maximumHeight());
    setSize(normalCss->width(), normalCss->height());
    setRoundCorners(normalCss->roundCorners());
}

bool TpWidget::objectActive()
{
    return false;
}
