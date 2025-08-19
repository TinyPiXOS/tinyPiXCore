#include "TpImage.h"
#include "thorVG/thorvg.h"
#include "tpFileInfo.h"

struct TpImageData
{
    tpString fileName = "";
    tvg::Picture *tvgPicture = tvg::Picture::gen();
};

TpImage::TpImage(const tpString &fileName)
{
    TpImageData *imageData = new TpImageData();

    data_ = imageData;

    load(fileName);
}

TpImage::~TpImage()
{
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

    return false;
}

TpImage TpImage::scaled(const ItpSize &size)
{
    TpImageData *imageData = static_cast<TpImageData *>(data_);
    if (!imageData)
        return TpImage(imageData->fileName);

    return scaled(size.w, size.h);
}

TpImage TpImage::scaled(const uint32_t &width, const uint32_t &height)
{
    TpImageData *imageData = static_cast<TpImageData *>(data_);
    TpImage newImageObj(imageData->fileName);
    if (!imageData)
        return newImageObj;

    TpImageData *newImageData = static_cast<TpImageData *>(newImageObj.data_);
    newImageData->tvgPicture->size(width, height);
    return newImageObj;
}

TpImage TpImage::gaussianBlur(const int32_t &radius)
{
}

int32_t TpImage::width()
{
    TpImageData *imageData = static_cast<TpImageData *>(data_);
    if (!imageData)
        return 0;

    float width = 0;
    float height = 0;

    imageData->tvgPicture->size(&width, &height);
    return width;
}

int32_t TpImage::height()
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
}

TpImage TpImage::copy()
{
}

TpImage TpImage::copy(const ItpRect &rect)
{
}

TpImage TpImage::copy(int32_t x, int32_t y, int32_t w, int32_t h)
{
}

bool TpImage::save(const tpString &filename, ImageType type, int32_t jpguality)
{
}

TpImage TpImage::operator=(const TpImage &others)
{
    TpImageData *imageData = static_cast<TpImageData *>(others.data_);
    this->load(imageData->fileName);
    return *this;
}
