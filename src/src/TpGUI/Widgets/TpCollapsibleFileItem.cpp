#include "TpCollapsibleFileItem.h"
#include "TpLabel.h"
#include "TpFont.h"
#include "TpString.h"
#include "TpVBoxLayout.h"
#include "TpImage.h"
#include "SystemInfo/TpDisplay.h"
#include "TpFileInfo.h"
#include "TpDir.h"
#include "TpCheckBox.h"

struct TpCollapsibleFileItemData
{
    TpLabel *iconLabel;
    TpLabel *nameLabel;
    TpLabel *sizeLabel;
    TpLabel *typeLabel;

    TpString filePath;

    TpVBoxLayout *mainLayout = nullptr;

    // 是否触发item事件，如果鼠标按下后拖动，不再处理事件
    bool isTrigger = true;

    TpCheckBox *selectCbx;

    TpCollapsibleFileItemData() : filePath("")
    {
    }

    ~TpCollapsibleFileItemData()
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

TpString parseFileSuffix(const TpString &suffix, const bool &isDir)
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

TpString parseIconPath(const TpString &suffix, const bool &isDir)
{
    TpString resPath = "/usr/res/TinyPiX/fileIcon/";

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

TpCollapsibleFileItem::TpCollapsibleFileItem(TpWidget *parent)
    : TpWidget(parent)
{
    tpShared<TpCssData> curCssData = currentStatusCss();

    TpCollapsibleFileItemData *itemData = new TpCollapsibleFileItemData();
    data_ = itemData;

    itemData->iconLabel = new TpLabel(this);
    itemData->iconLabel->setFixedSize(curCssData->iconSize(), curCssData->iconSize());
    itemData->iconLabel->installEventFilter(this);

    // itemData->iconLabel->setBackGroundColor(_RGB(255, 0, 0));

    itemData->nameLabel = new TpLabel(this);
    itemData->nameLabel->setAlign(Tp::AlignHCenter);
    itemData->nameLabel->font()->setFontSize(curCssData->fontSize());
    itemData->nameLabel->font()->setFontColor(curCssData->color(), curCssData->color());
    itemData->nameLabel->setWordWrap(false);
    // itemData->nameLabel->setBackGroundColor(_RGB(255, 0, 0));
    itemData->nameLabel->installEventFilter(this);

    itemData->sizeLabel = new TpLabel(this);
    itemData->sizeLabel->setAlign(Tp::AlignHCenter);
    itemData->sizeLabel->font()->setFontSize(curCssData->fontSize());
    itemData->sizeLabel->font()->setFontColor(_RGB(140, 140, 140), _RGB(140, 140, 140));
    itemData->sizeLabel->setText("0Kb");
    itemData->sizeLabel->installEventFilter(this);

    itemData->typeLabel = new TpLabel(this);
    itemData->typeLabel->setAlign(Tp::AlignHCenter);
    itemData->typeLabel->font()->setFontSize(curCssData->fontSize());
    itemData->typeLabel->font()->setFontColor(_RGB(140, 140, 140), _RGB(140, 140, 140));
    itemData->typeLabel->setText("未知");
    itemData->typeLabel->installEventFilter(this);

    itemData->selectCbx = new TpCheckBox(this);
    itemData->selectCbx->setVisible(false);

    itemData->mainLayout = new TpVBoxLayout();
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

TpCollapsibleFileItem::~TpCollapsibleFileItem()
{
    TpCollapsibleFileItemData *itemData = static_cast<TpCollapsibleFileItemData *>(data_);
    if (itemData)
    {
        delete itemData;
        itemData = nullptr;
        data_ = nullptr;
    }
}

void TpCollapsibleFileItem::setSelectEable(const bool &enable)
{
    TpCollapsibleFileItemData *itemData = static_cast<TpCollapsibleFileItemData *>(data_);
    itemData->selectCbx->setVisible(enable);
}

void TpCollapsibleFileItem::setName(const TpString &name)
{
    TpCollapsibleFileItemData *itemData = static_cast<TpCollapsibleFileItemData *>(data_);

    // TpFont* nameFont = itemData->nameLabel->font();
    // nameFont->setText(name);

    // int rowCount = nameFont->pixelWidth() % TpDisplay::dp2Px(131);

    itemData->nameLabel->setText(name);
    // itemData->nameLabel->update();

    setMinumumHeight(itemData->mainLayout->minumumSize().height());

    update();
}

void TpCollapsibleFileItem::setPath(const TpString &filePath)
{
    TpCollapsibleFileItemData *itemData = static_cast<TpCollapsibleFileItemData *>(data_);
    itemData->filePath = filePath;

    // 解析文件类型，获取资源图片
    TpFileInfo fileInfo(filePath);

    bool isDir = false;
    TpString suffix = fileInfo.suffix();
    if (fileInfo.isDir())
    {
        isDir = true;

        TpDir pathDir(filePath);
        itemData->sizeLabel->setText(TpString::number(pathDir.entryInfoList().size()) + "项");
    }
    else
    {
        itemData->sizeLabel->setText(TpString::number(fileInfo.size()) + "Kb");
    }
    // std::cout << "suffix  " << suffix << std::endl;

    TpString typeStr = parseFileSuffix(suffix, isDir);

    TpString iconPath = parseIconPath(suffix, isDir);

    itemData->typeLabel->setText(typeStr);

    itemData->iconLabel->setBackGroundImage(TpImage(iconPath));

    update();
}

TpString TpCollapsibleFileItem::path()
{
    TpCollapsibleFileItemData *itemData = static_cast<TpCollapsibleFileItemData *>(data_);
    return itemData->filePath;
}

bool TpCollapsibleFileItem::selected()
{
    TpCollapsibleFileItemData *itemData = static_cast<TpCollapsibleFileItemData *>(data_);
    return itemData->selectCbx->checked();
}

void TpCollapsibleFileItem::setSelected(const bool &selected)
{
    TpCollapsibleFileItemData *itemData = static_cast<TpCollapsibleFileItemData *>(data_);
    itemData->selectCbx->setChecked(selected);
}

bool TpCollapsibleFileItem::onMousePressEvent(TpMouseEvent *event)
{
    TpWidget::onMousePressEvent(event);

    TpCollapsibleFileItemData *itemData = static_cast<TpCollapsibleFileItemData *>(data_);

    itemData->isTrigger = true;

    return false;
}

bool TpCollapsibleFileItem::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpWidget::onMouseRleaseEvent(event);

    TpCollapsibleFileItemData *itemData = static_cast<TpCollapsibleFileItemData *>(data_);

    if (itemData->isTrigger)
        onClicked.emit(this);

    return false;
}

bool TpCollapsibleFileItem::onMouseMoveEvent(TpMouseEvent *event)
{
    TpWidget::onMouseMoveEvent(event);

    if (event->state())
    {
        TpCollapsibleFileItemData *itemData = static_cast<TpCollapsibleFileItemData *>(data_);
        itemData->isTrigger = false;
    }

    return true;
}

bool TpCollapsibleFileItem::onLeaveEvent(TpLeaveEvent *event)
{
    TpWidget::onLeaveEvent(event);

    TpCollapsibleFileItemData *itemData = static_cast<TpCollapsibleFileItemData *>(data_);
    itemData->isTrigger = false;

    return true;
}

bool TpCollapsibleFileItem::onMoveEvent(TpMoveEvent *event)
{
    TpWidget::onMoveEvent(event);

    // TpCollapsibleFileItemData *itemData = static_cast<TpCollapsibleFileItemData *>(data_);
    // itemData->selectCbx->move(itemData->iconLabel->width() - itemData->selectCbx->width() - 6, itemData->iconLabel->height() - itemData->selectCbx->height() - 6);

    return true;
}

bool TpCollapsibleFileItem::onResizeEvent(TpResizeEvent *event)
{
    TpWidget::onResizeEvent(event);

    TpCollapsibleFileItemData *itemData = static_cast<TpCollapsibleFileItemData *>(data_);

    tpShared<TpCssData> normalCss = currentStatusCss();

    uint32_t minSize = normalCss->iconSize();
    minSize *= 0.33333;

    itemData->selectCbx->setSize(minSize, minSize);

    int32_t cbxX = itemData->iconLabel->pos().x() + itemData->iconLabel->width() - itemData->selectCbx->width() - 9;
    int32_t cbxY = itemData->iconLabel->pos().y() + itemData->iconLabel->height() - itemData->selectCbx->height() - 8;

    // std::cout << "selectCbx Move " << cbxX << " " << cbxY << std::endl;

    itemData->selectCbx->move(cbxX, cbxY);

    // std::cout << "TpCollapsibleFileItem::onResizeEvent " << width() << " h " << height() << std::endl;

    return true;
}

bool TpCollapsibleFileItem::eventFilter(TpObject *watched, TpEvent *event)
{
    if (event->eventType() == TpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        onMousePressEvent(mouseEvent);
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        onMouseRleaseEvent(mouseEvent);
    }
    else
    {

    }

    return true;
}
