#include "TpImage.h"
#include "thorVG/thorvg.h"
#include "TpFileInfo.h"
#include "TpImage_p.h"

#include <thread>
#include <cmath>

TpImage::TpImage(const TpString &fileName) : data_(nullptr)
{
    // 根据CPU核心数；分配绘图引擎线程数
    uint32_t cores = std::thread::hardware_concurrency();
    tvg::Initializer::init(cores / 2);

    TpImageData *imageData = new TpImageData();
    imageData->tvgPicture = tvg::Picture::gen();

    data_ = imageData;

    load(fileName);
}

TpImage::TpImage(const TpImage &other)
{
    TpImageData *otherData = static_cast<TpImageData *>(other.data_);

    TpImageData *imageData = new TpImageData();
    imageData->tvgPicture = tvg::Picture::gen();

    if (otherData)
    {
        TpImageData *otherData = static_cast<TpImageData *>(other.data_);

        // 深拷贝所有成员
        imageData->fileName = otherData->fileName;
        imageData->tvgPicture = static_cast<tvg::Picture *>(otherData->tvgPicture->duplicate());
    }

    data_ = imageData;
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

bool TpImage::load(const TpString &filename)
{
    if (filename.empty())
        return false;

    TpFileInfo loadFile(filename);
    if (!loadFile.exists())
        return false;

    TpString fileSuffix = loadFile.suffix();
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

TpImage TpImage::scaled(const ItpSize &size, bool keepAspectRatio)
{
    TpImageData *imageData = static_cast<TpImageData *>(data_);
    if (!imageData)
        return TpImage(imageData->fileName);

    return scaled(size.w, size.h, keepAspectRatio);
}

TpImage TpImage::scaled(const int32_t &width, const int32_t &height, bool keepAspectRatio)
{
    TpImageData *imageData = static_cast<TpImageData *>(data_);
    TpImage newImageObj = *this;
    if (!imageData)
        return newImageObj;

    if (width <= 0)
        return newImageObj;

    if (height <= 0)
        return newImageObj;

    TpImageData *newImageData = static_cast<TpImageData *>(newImageObj.data_);

    if (keepAspectRatio)
    {
        newImageData->tvgPicture->size(width, height);
    }
    else
    {
        // 获取原始尺寸
        float originalW, originalH;
        newImageData->tvgPicture->size(&originalW, &originalH);

        // 计算缩放因子
        float scaleX = width / originalW;
        float scaleY = height / originalH;

        // 应用不同的 X 和 Y 缩放因子
        tvg::Matrix stretchMatrix = {scaleX, 0, 0, 0, scaleY, 0, 0, 0, 1};
        newImageData->tvgPicture->transform(stretchMatrix);
    }

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

    TpImage newCopyImage(imageData->fileName);
    TpImageData *newImageData = static_cast<TpImageData *>(newCopyImage.data_);

    newImageData->tvgPicture->translate(0, 0);

    // 创建裁剪形状
    auto clipper = tvg::Shape::gen();
    clipper->appendRect(x, y, w, h); // 裁剪区域

    // 应用裁剪
    newImageData->tvgPicture->clip(clipper);

    return newCopyImage;
}

bool TpImage::save(const TpString &filename, ImageType type, int32_t jpguality)
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

TpImage TpImage::rotate(const float &angle)
{
    TpImageData *imageData = static_cast<TpImageData *>(data_);
    if (!imageData)
        return TpImage();

    TpImage newCopyImage = *this;

    TpImageData *newImageData = static_cast<TpImageData *>(newCopyImage.data_);
    newImageData->tvgPicture->rotate(angle);

    return newCopyImage;
}

float TpImage::rotateAngle() const
{
    TpImageData *imageData = static_cast<TpImageData *>(data_);
    if (!imageData || !imageData->tvgPicture)
        return 0.0f;

    auto matrix = imageData->tvgPicture->transform();
    return atan2(matrix.e21, matrix.e11) * 180.0f / M_PI;
}

bool TpImage::isRotated() const
{
    TpImageData *imageData = static_cast<TpImageData *>(data_);
    if (!imageData || !imageData->tvgPicture)
        return false;

    auto matrix = imageData->tvgPicture->transform();

    // 检查是否有旋转分量
    return (matrix.e12 != 0.0f || matrix.e21 != 0.0f);
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
