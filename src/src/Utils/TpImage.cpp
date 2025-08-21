#include "TpImage.h"
#include "thorVG/thorvg.h"
#include "tpFileInfo.h"
#include "TpImage_p.h"

#include <thread>

TpImage::TpImage(const tpString &fileName) : data_(nullptr)
{
    // 根据CPU核心数；分配绘图引擎线程数
    uint32_t cores = std::thread::hardware_concurrency();
    tvg::Initializer::init(cores / 2);

    TpImageData *imageData = new TpImageData();
    imageData->tvgPicture = tvg::Picture::gen();

    data_ = imageData;

    load(fileName);
}

TpImage::~TpImage()
{
    tvg::Initializer::term();

    TpImageData *imageData = static_cast<TpImageData *>(data_);
    if (imageData)
    {
        delete imageData;
        imageData = nullptr;
        data_ = nullptr;
    }
}

bool TpImage::load(const tpString &filename)
{
    if (filename.empty())
        return false;

    tpFileInfo loadFile(filename);
    if (!loadFile.exists())
        return false;

    tpString fileSuffix = loadFile.suffix();
    if (fileSuffix.compare("svg") != 0 && fileSuffix.compare("webp") != 0 &&
        fileSuffix.compare("png") != 0 && fileSuffix.compare("PNG") != 0 &&
        fileSuffix.compare("jpg") != 0 && fileSuffix.compare("JPG") != 0 &&
        fileSuffix.compare("jpeg") != 0 && fileSuffix.compare("JPEG") != 0)
        return false;

    TpImageData *imageData = static_cast<TpImageData *>(data_);
    if (!imageData)
        return false;

    imageData->fileName = filename;
    imageData->tvgPicture->load(filename.c_str());

    return true;
}

TpImage TpImage::scaled(const ItpSize &size)
{
    TpImageData *imageData = static_cast<TpImageData *>(data_);
    if (!imageData)
        return TpImage(imageData->fileName);

    return scaled(size.w, size.h);
}

TpImage TpImage::scaled(const int32_t &width, const int32_t &height)
{
    TpImageData *imageData = static_cast<TpImageData *>(data_);
    TpImage newImageObj(imageData->fileName);
    if (!imageData)
        return newImageObj;

    if (width <= 0)
        return newImageObj;

    if (height <= 0)
        return newImageObj;

    TpImageData *newImageData = static_cast<TpImageData *>(newImageObj.data_);
    newImageData->tvgPicture->size(width, height);
    return newImageObj;
}

TpImage TpImage::gaussianBlur(const int32_t &radius)
{
    return TpImage();
}

int32_t TpImage::width() const
{
    TpImageData *imageData = static_cast<TpImageData *>(data_);
    if (!imageData)
        return 0;

    float width = 0;
    float height = 0;

    imageData->tvgPicture->size(&width, &height);
    return width;
}

int32_t TpImage::height() const
{
    TpImageData *imageData = static_cast<TpImageData *>(data_);
    if (!imageData)
        return 0;

    float width = 0;
    float height = 0;

    imageData->tvgPicture->size(&width, &height);
    return height;
}

bool TpImage::isNull()
{
    // TpImage test11;

    // {
    //     TpImage test22("/home/hawk/Public/tinyPiXCore/examples/SingleGUI/thorVGTest/icon.png");
    //     test11 = test22;
    // }
    // TpImageData *test11Data = static_cast<TpImageData *>(test11.data_);

    TpImageData *imageData = static_cast<TpImageData *>(data_);
    if (!imageData)
        return true;

    float width, height;

    if (imageData->tvgPicture->size(&width, &height) == tvg::Result::InsufficientCondition)
    {
        // Picture 对象未加载任何图片资源
        // std::cout << "Picture 对象为空，未加载图片" << std::endl;
        return true;
    }
    else
    {
        // Picture 对象已加载图片资源
        // std::cout << "Picture 已加载，尺寸: " << width << " x " << height << std::endl;
        return false;
    }
}

TpImage TpImage::copy(const ItpRect &rect)
{
    return copy(rect.x, rect.y, rect.w, rect.h);
}

TpImage TpImage::copy(int32_t x, int32_t y, int32_t w, int32_t h)
{
    TpImageData *imageData = static_cast<TpImageData *>(data_);
    if (!imageData)
        return TpImage();

    // 创建深拷贝
    TpImage newCopyImage = *this;
    TpImageData *newImageData = static_cast<TpImageData *>(newCopyImage.data_);

    newImageData->tvgPicture->translate(0, 0);

    // 创建裁剪形状
    auto clipper = tvg::Shape::gen();
    clipper->appendRect(x, y, w, h); // 裁剪区域

    // 应用裁剪
    newImageData->tvgPicture->clip(clipper);

    return newCopyImage;
}

bool TpImage::save(const tpString &filename, ImageType type, int32_t jpguality)
{
    // TpImageData *imageData = static_cast<TpImageData *>(data_);
    // if (!imageData)
    //     return false;

    // // 创建保存器
    // auto saver = tvg::Saver::gen();

    // if (jpguality < 0)
    //     jpguality = 0;
    // if (jpguality > 100)
    //     jpguality = 100;

    // // 保存为文件
    // saver->save(imageData->tvgPicture, "output.tvg", jpguality); // 质量参数 0-100
    // saver->sync();                                               // 等待保存完成
    return true;
}

TpImage &TpImage::operator=(const TpImage &others)
{
    // 自赋值检查
    if (this == &others)
        return *this;

    TpImageData *imageData = static_cast<TpImageData *>(data_);
    TpImageData *othersImageData = static_cast<TpImageData *>(others.data_);

    imageData->fileName = othersImageData->fileName;

    if (imageData->tvgPicture)
    {
        delete imageData->tvgPicture;
        imageData->tvgPicture = nullptr;
    }

    imageData->tvgPicture = static_cast<tvg::Picture *>(othersImageData->tvgPicture->duplicate());

    return *this;
}
