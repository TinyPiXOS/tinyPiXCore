#include "TpImage.h"
#include "thorVG/thorvg.h"
#include "TpFileInfo.h"
#include "TpImage_p.h"

#include <png.h>
#include <thread>
#include <cmath>
#include <fstream>

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

TpImage::TpImage(const TpImage &other) : data_(nullptr)
{
    // 根据CPU核心数；分配绘图引擎线程数
    uint32_t cores = std::thread::hardware_concurrency();
    tvg::Initializer::init(cores / 2);

    TpImageData *imageData = new TpImageData();
    imageData->tvgPicture = tvg::Picture::gen();

    data_ = imageData;

    TpImageData *otherData = static_cast<TpImageData *>(other.data_);
    if (!otherData)
        return;

    // 深拷贝所有成员
    // if (otherData->loadBuffer)
    // {
    //     load(otherData->loadBuffer, TpRect(0, 0, otherData->actualWidth, otherData->actualHeight));
    // }
    // else
    {
        imageData->fileName = otherData->fileName;
        imageData->actualWidth = otherData->actualWidth;
        imageData->actualHeight = otherData->actualHeight;
        imageData->tvgPicture = static_cast<tvg::Picture *>(otherData->tvgPicture->duplicate());
    }
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

    // if (imageData->loadBuffer)
    // {
    //     delete[] imageData->loadBuffer;
    //     imageData->loadBuffer = nullptr;
    // }

    imageData->fileName = filename;

    imageData->tvgPicture->load(filename.c_str());

    float picWidth, picHeight;
    imageData->tvgPicture->size(&picWidth, &picHeight);

    imageData->actualWidth = picWidth;
    imageData->actualHeight = picHeight;

    return true;
}

bool TpImage::load(void *martix, const TpRect &rect)
{
    if (!martix)
        return false;

    if (rect.width() == 0 || rect.height() == 0)
        return false;

    TpImageData *imageData = static_cast<TpImageData *>(data_);
    if (!imageData)
        return false;

    imageData->fileName = "";

    // if (imageData->loadBuffer)
    // {
    //     delete[] imageData->loadBuffer;
    //     imageData->loadBuffer = nullptr;
    // }

    // imageData->loadBuffer = new uint32_t[rect.width() * rect.height()];
    // memcpy(imageData->loadBuffer, martix, sizeof(uint32_t) * rect.width() * rect.height());

    imageData->tvgPicture->load((uint32_t*)martix, rect.width(), rect.height(), tvg::ColorSpace::ARGB8888);
    // imageData->tvgPicture->load(imageData->loadBuffer, rect.width(), rect.height(), tvg::ColorSpace::ARGB8888);

    imageData->actualWidth = rect.width();
    imageData->actualHeight = rect.height();

    // imageData->tvgPicture->size(&imageData->actualWidth, &imageData->actualHeight);

    return true;
}

TpImage TpImage::scaled(const TpSize &size, bool keepAspectRatio)
{
    TpImageData *imageData = static_cast<TpImageData *>(data_);
    if (!imageData)
        return TpImage(imageData->fileName);

    return scaled(size.width(), size.height(), keepAspectRatio);
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

    // 获取原始尺寸
    float originalW, originalH;
    newImageData->tvgPicture->size(&originalW, &originalH);

    // 计算缩放因子
    float scaleX = width / originalW;
    float scaleY = height / originalH;

    if (keepAspectRatio)
    {
        float actualScale = (scaleX < scaleY) ? scaleX : scaleY;

        // 计算实际渲染尺寸
        newImageData->actualWidth = originalW * actualScale;
        newImageData->actualHeight = originalH * actualScale;

        newImageData->tvgPicture->size(width, height);
    }
    else
    {
        // 计算实际渲染尺寸
        newImageData->actualWidth = originalW * scaleX;
        newImageData->actualHeight = originalH * scaleY;

        // 应用不同的 X 和 Y 缩放因子
        tvg::Matrix stretchMatrix = {scaleX, 0, 0, 0, scaleY, 0, 0, 0, 1};
        newImageData->tvgPicture->transform(stretchMatrix);
    }

    return newImageObj;
}

int32_t TpImage::width() const
{
    TpImageData *imageData = static_cast<TpImageData *>(data_);
    if (!imageData)
        return 0;

    return imageData->actualWidth;
}

int32_t TpImage::height() const
{
    TpImageData *imageData = static_cast<TpImageData *>(data_);
    if (!imageData)
        return 0;

    return imageData->actualHeight;
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

TpImage TpImage::copy(const TpRect &rect)
{
    return copy(rect.x(), rect.y(), rect.width(), rect.height());
}

TpImage TpImage::copy(int32_t x, int32_t y, int32_t w, int32_t h)
{
    TpImageData *imageData = static_cast<TpImageData *>(data_);
    if (!imageData)
        return TpImage();

    TpImage newCopyImage = *this;
    TpImageData *newImageData = static_cast<TpImageData *>(newCopyImage.data_);

    // newImageData->tvgPicture->translate(0, 0);

    // 创建裁剪形状
    auto clipper = tvg::Shape::gen();
    clipper->appendRect(x, y, w, h); // 裁剪区域

    // 应用裁剪
    newImageData->tvgPicture->clip(clipper);

    return newCopyImage;
}

// bool TpImage::save(const TpString &filename, ImageType type, int32_t jpguality)
// {
//     TpImageData *imageData = static_cast<TpImageData *>(data_);
//     if (!imageData)
//         return false;

//     if (!imageData->loadBuffer)
//         return false;

// #if 1
//     FILE *fp = fopen(filename.c_str(), "wb");
//     // 处理文件打开失败
//     if (!fp)
//         return false;

//     png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
//     if (!png)
//     {
//         fclose(fp);
//         return false;
//     }

//     png_infop info = png_create_info_struct(png);
//     if (!info)
//     {
//         png_destroy_write_struct(&png, nullptr);
//         fclose(fp);
//         return false;
//     }

//     // 设置错误处理
//     if (setjmp(png_jmpbuf(png)))
//     {
//         png_destroy_write_struct(&png, &info);
//         fclose(fp);
//         return false;
//     }

//     png_init_io(png, fp);

//     // 设置图像信息
//     png_set_IHDR(png, info,
//                  imageData->actualWidth, imageData->actualHeight,
//                  8,
//                  PNG_COLOR_TYPE_RGBA,
//                  PNG_INTERLACE_NONE,
//                  PNG_COMPRESSION_TYPE_DEFAULT,
//                  PNG_FILTER_TYPE_DEFAULT);

//     // 添加关键：设置字节顺序（RGBA）
//     png_set_swap(png); // 如果您的系统是小端序，可能需要这个

//     png_write_info(png, info);

//     // 写入像素数据
//     int32_t *buffer = reinterpret_cast<int32_t *>(imageData->loadBuffer);
//     const int width = imageData->actualWidth;
//     const int height = imageData->actualHeight;
//     const int rowbytes = width * 4; // 每个像素4字节 (RGBA)

//     // 分配行缓冲区
//     png_bytep row_buffer = new png_byte[rowbytes];

//     for (int y = 0; y < height; y++)
//     {
//         // 获取当前行数据
//         int32_t *src_row = buffer + y * width;

//         // 转换为字节数组
//         for (int x = 0; x < width; x++)
//         {
//             uint32_t pixel = static_cast<uint32_t>(src_row[x]);
//             row_buffer[x * 4 + 0] = (pixel >> 16) & 0xFF; // R
//             row_buffer[x * 4 + 1] = (pixel >> 8) & 0xFF;  // G
//             row_buffer[x * 4 + 2] = pixel & 0xFF;         // B
//             row_buffer[x * 4 + 3] = (pixel >> 24) & 0xFF; // A
//         }

//         png_write_row(png, row_buffer);
//     }

//     delete[] row_buffer;
//     png_write_end(png, nullptr);
//     png_destroy_write_struct(&png, &info);
//     fclose(fp);
// #endif

//     return true;
// }

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

    imageData->actualWidth = othersImageData->actualWidth;
    imageData->actualHeight = othersImageData->actualHeight;
    imageData->fileName = othersImageData->fileName;

    if (imageData->tvgPicture)
    {
        delete imageData->tvgPicture;
        imageData->tvgPicture = tvg::Picture::gen();
    }

    // if (imageData->loadBuffer)
    // {
    //     delete[] imageData->loadBuffer;
    //     imageData->loadBuffer = nullptr;
    // }

    // if (othersImageData->loadBuffer)
    // {
    //     if (othersImageData->actualWidth != 0 && othersImageData->actualHeight != 0)
    //     {
    //         imageData->loadBuffer = new uint32_t[othersImageData->actualWidth * othersImageData->actualHeight];
    //         memcpy(imageData->loadBuffer, othersImageData->loadBuffer, sizeof(uint32_t) * othersImageData->actualWidth * othersImageData->actualHeight);
    //         imageData->tvgPicture->load(imageData->loadBuffer, othersImageData->actualWidth, othersImageData->actualHeight, tvg::ColorSpace::ARGB8888);
    //     }
    // }
    // else
    {
        imageData->tvgPicture = static_cast<tvg::Picture *>(othersImageData->tvgPicture->duplicate());
    }

    return *this;
}
