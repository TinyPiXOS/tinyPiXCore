#include "tpCollapsibleFileItem.h"
#include "tpLabel.h"
#include "tpFont.h"
#include "tpString.h"
#include "tpVBoxLayout.h"
#include "tpSurface.h"
#include "tpDisplay.h"
#include "tpFileInfo.h"
#include "tpDir.h"
#include "tpCheckBox.h"

struct tpCollapsibleFileItemData
{
    tpLabel *iconLabel;
    tpLabel *nameLabel;
    tpLabel *sizeLabel;
    tpLabel *typeLabel;

    tpString filePath;

    tpVBoxLayout *mainLayout = nullptr;

    // 是否触发item事件，如果鼠标按下后拖动，不再处理事件
    bool isTrigger = true;

    tpCheckBox *selectCbx;

    tpCollapsibleFileItemData() : filePath("")
    {
    }

    ~tpCollapsibleFileItemData()
    {
        iconLabel->setParent(nullptr);
        delete iconLabel;
        iconLabel = nullptr;

        nameLabel->setParent(nullptr);
        delete nameLabel;
        nameLabel = nullptr;

        sizeLabel->setParent(nullptr);
        delete sizeLabel;
        sizeLabel = nullptr;

        typeLabel->setParent(nullptr);
        delete typeLabel;
        typeLabel = nullptr;
    }
};

tpString parseFileSuffix(const tpString &suffix, const bool &isDir)
{
    if (isDir)
        return "文件夹";

    if (suffix.compare("txt") == 0)
        return "文本文件";
    else if (suffix.compare("exe") == 0)
        return "程序";
    else if (suffix.compare("doc") == 0 || suffix.compare("docx") == 0)
        return "word文档";
    else if (suffix.compare("xls") == 0 || suffix.compare("xlsx") == 0)
        return "excel表格";
    else if (suffix.compare("ppt") == 0 || suffix.compare("pptx") == 0)
        return "PPT";
    else if (suffix.compare("mp3") == 0)
        return "音频文件";
    else if (suffix.compare("mp4") == 0 || suffix.compare("avi") == 0)
        return "视频文件";
    else if (suffix.compare("zip") == 0 || suffix.compare("rar") == 0 || suffix.compare("7z") == 0)
        return "压缩包";
    else if (suffix.compare("png") == 0 || suffix.compare("jpg") == 0 || suffix.compare("jpeg") == 0)
        return "图片";
    else
        return "未知";
}

tpString parseIconPath(const tpString &suffix, const bool &isDir)
{
    tpString resPath = "/usr/res/tinyPiX/fileIcon/";

    // return resPath + "pdf.png";

    if (isDir)
        return resPath + "文件夹.png";

    if (suffix.compare("txt") == 0)
        return resPath + "文本.png";
    else if (suffix.compare("exe") == 0)
        return resPath + "程序.png";
    else if (suffix.compare("doc") == 0 || suffix.compare("docx") == 0)
        return resPath + "word.png";
    else if (suffix.compare("xls") == 0 || suffix.compare("xlsx") == 0)
        return resPath + "excel.png";
    else if (suffix.compare("ppt") == 0 || suffix.compare("pptx") == 0)
        return resPath + "ppt.png";
    else if (suffix.compare("mp3") == 0 || suffix.compare("MP3") == 0)
        return resPath + "音频.png";
    else if (suffix.compare("mp4") == 0 || suffix.compare("MP4") == 0 || suffix.compare("avi") == 0)
        return resPath + "视频.png";
    else if (suffix.compare("zip") == 0 || suffix.compare("rar") == 0 || suffix.compare("7z") == 0)
        return resPath + "压缩包.png";
    else if (suffix.compare("png") == 0 || suffix.compare("jpg") == 0 || suffix.compare("jpeg") == 0)
        return resPath + "图片.png";
    else
        return resPath + "未知.png";
}

tpCollapsibleFileItem::tpCollapsibleFileItem(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    tpShared<tpCssData> curCssData = currentStatusCss();

    tpCollapsibleFileItemData *itemData = new tpCollapsibleFileItemData();
    data_ = itemData;

    itemData->iconLabel = new tpLabel(this);
    itemData->iconLabel->setFixedSize(curCssData->iconSize(), curCssData->iconSize());
    itemData->iconLabel->installEventFilter(this);

    // itemData->iconLabel->setBackGroundColor(_RGB(255, 0, 0));

    itemData->nameLabel = new tpLabel(this);
    itemData->nameLabel->setAlign(tinyPiX::AlignHCenter);
    itemData->nameLabel->font()->setFontSize(curCssData->fontSize());
    itemData->nameLabel->font()->setFontColor(curCssData->color(), curCssData->color());
    itemData->nameLabel->setWordWrap(false);
    // itemData->nameLabel->setBackGroundColor(_RGB(255, 0, 0));
    itemData->nameLabel->installEventFilter(this);

    itemData->sizeLabel = new tpLabel(this);
    itemData->sizeLabel->setAlign(tinyPiX::AlignHCenter);
    itemData->sizeLabel->font()->setFontSize(curCssData->fontSize());
    itemData->sizeLabel->font()->setFontColor(_RGB(140, 140, 140), _RGB(140, 140, 140));
    itemData->sizeLabel->setText("0Kb");
    itemData->sizeLabel->installEventFilter(this);

    itemData->typeLabel = new tpLabel(this);
    itemData->typeLabel->setAlign(tinyPiX::AlignHCenter);
    itemData->typeLabel->font()->setFontSize(curCssData->fontSize());
    itemData->typeLabel->font()->setFontColor(_RGB(140, 140, 140), _RGB(140, 140, 140));
    itemData->typeLabel->setText("未知");
    itemData->typeLabel->installEventFilter(this);

    itemData->selectCbx = new tpCheckBox(this);
    itemData->selectCbx->setVisible(false);

    itemData->mainLayout = new tpVBoxLayout();
    itemData->mainLayout->setContentsMargins(0, 0, 0, 0);
    itemData->mainLayout->setSpacing(2);
    itemData->mainLayout->addWidget(itemData->iconLabel);
    itemData->mainLayout->addWidget(itemData->nameLabel);
    itemData->mainLayout->addWidget(itemData->sizeLabel);
    itemData->mainLayout->addWidget(itemData->typeLabel);

    refreshBaseCss();

    setLayout(itemData->mainLayout);

    // setBackGroundColor(_RGB(0, 255, 0));
}

tpCollapsibleFileItem::~tpCollapsibleFileItem()
{
    tpCollapsibleFileItemData *itemData = static_cast<tpCollapsibleFileItemData *>(data_);
    if (itemData)
    {
        delete itemData;
        itemData = nullptr;
        data_ = nullptr;
    }
}

void tpCollapsibleFileItem::setSelectEable(const bool &enable)
{
    tpCollapsibleFileItemData *itemData = static_cast<tpCollapsibleFileItemData *>(data_);
    itemData->selectCbx->setVisible(enable);
}

void tpCollapsibleFileItem::setName(const tpString &name)
{
    tpCollapsibleFileItemData *itemData = static_cast<tpCollapsibleFileItemData *>(data_);

    // tpFont* nameFont = itemData->nameLabel->font();
    // nameFont->setText(name);

    // int rowCount = nameFont->pixelWidth() % tpDisplay::dp2Px(131);

    itemData->nameLabel->setText(name);
    // itemData->nameLabel->update();

    setMinumumHeight(itemData->mainLayout->minumumSize().h);

    update();
}

void tpCollapsibleFileItem::setPath(const tpString &filePath)
{
    tpCollapsibleFileItemData *itemData = static_cast<tpCollapsibleFileItemData *>(data_);
    itemData->filePath = filePath;

    // 解析文件类型，获取资源图片
    tpFileInfo fileInfo(filePath);

    bool isDir = false;
    tpString suffix = fileInfo.suffix();
    if (fileInfo.isDir())
    {
        isDir = true;

        tpDir pathDir(filePath);
        itemData->sizeLabel->setText(tpString::number(pathDir.entryInfoList().size()) + "项");
    }
    else
    {
        itemData->sizeLabel->setText(tpString::number(fileInfo.size()) + "Kb");
    }
    // std::cout << "suffix  " << suffix << std::endl;

    tpString typeStr = parseFileSuffix(suffix, isDir);

    tpString iconPath = parseIconPath(suffix, isDir);

    itemData->typeLabel->setText(typeStr);

    tpShared<tpSurface> iconSurface = tpMakeShared<tpSurface>();
    iconSurface->fromFile(iconPath);
    itemData->iconLabel->setBackGroundImage(iconSurface);

    update();
}

tpString tpCollapsibleFileItem::path()
{
    tpCollapsibleFileItemData *itemData = static_cast<tpCollapsibleFileItemData *>(data_);
    return itemData->filePath;
}

bool tpCollapsibleFileItem::selected()
{
    tpCollapsibleFileItemData *itemData = static_cast<tpCollapsibleFileItemData *>(data_);
    return itemData->selectCbx->checked();
}

void tpCollapsibleFileItem::setSelected(const bool &selected)
{
    tpCollapsibleFileItemData *itemData = static_cast<tpCollapsibleFileItemData *>(data_);
    itemData->selectCbx->setChecked(selected);
}

bool tpCollapsibleFileItem::onMousePressEvent(tpMouseEvent *event)
{
    tpChildWidget::onMousePressEvent(event);

    tpCollapsibleFileItemData *itemData = static_cast<tpCollapsibleFileItemData *>(data_);

    itemData->isTrigger = true;

    return false;
}

bool tpCollapsibleFileItem::onMouseRleaseEvent(tpMouseEvent *event)
{
    tpChildWidget::onMouseRleaseEvent(event);

    tpCollapsibleFileItemData *itemData = static_cast<tpCollapsibleFileItemData *>(data_);

    if (itemData->isTrigger)
        onClicked.emit(this);

    return false;
}

bool tpCollapsibleFileItem::onMouseMoveEvent(tpMouseEvent *event)
{
    tpChildWidget::onMouseMoveEvent(event);

    if (event->state())
    {
        tpCollapsibleFileItemData *itemData = static_cast<tpCollapsibleFileItemData *>(data_);
        itemData->isTrigger = false;
    }

    return true;
}

bool tpCollapsibleFileItem::onLeaveEvent(tpObjectLeaveEvent *event)
{
    tpChildWidget::onLeaveEvent(event);

    tpCollapsibleFileItemData *itemData = static_cast<tpCollapsibleFileItemData *>(data_);
    itemData->isTrigger = false;

    return true;
}

bool tpCollapsibleFileItem::onMoveEvent(tpObjectMoveEvent *event)
{
    tpChildWidget::onMoveEvent(event);

    // tpCollapsibleFileItemData *itemData = static_cast<tpCollapsibleFileItemData *>(data_);
    // itemData->selectCbx->move(itemData->iconLabel->width() - itemData->selectCbx->width() - 6, itemData->iconLabel->height() - itemData->selectCbx->height() - 6);

    return true;
}

bool tpCollapsibleFileItem::onResizeEvent(tpObjectResizeEvent *event)
{
    tpChildWidget::onResizeEvent(event);

    tpCollapsibleFileItemData *itemData = static_cast<tpCollapsibleFileItemData *>(data_);

    tpShared<tpCssData> normalCss = currentStatusCss();

    uint32_t minSize = normalCss->iconSize();
    minSize *= 0.33333;

    itemData->selectCbx->setSize(minSize, minSize);

    int32_t cbxX = itemData->iconLabel->pos().x + itemData->iconLabel->width() - itemData->selectCbx->width() - 9;
    int32_t cbxY = itemData->iconLabel->pos().y + itemData->iconLabel->height() - itemData->selectCbx->height() - 8;

    // std::cout << "selectCbx Move " << cbxX << " " << cbxY << std::endl;

    itemData->selectCbx->move(cbxX, cbxY);

    // std::cout << "tpCollapsibleFileItem::onResizeEvent " << width() << " h " << height() << std::endl;

    return true;
}

bool tpCollapsibleFileItem::eventFilter(tpObject *watched, tpEvent *event)
{
    if (event->eventType() == tpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        tpMouseEvent *mouseEvent = dynamic_cast<tpMouseEvent *>(event);
        onMousePressEvent(mouseEvent);
    }
    else if (event->eventType() == tpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        tpMouseEvent *mouseEvent = dynamic_cast<tpMouseEvent *>(event);
        onMouseRleaseEvent(mouseEvent);
    }
    else
    {

    }

    return true;
}
