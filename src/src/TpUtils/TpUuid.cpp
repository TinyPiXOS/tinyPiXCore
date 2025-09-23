#include "TpUuid.h"
#include "sole/sole.hpp"

#define UUID_MAX_LENGTH 37

struct TpUuidData
{
    sole::uuid soleUuid;
};

TpUuid::TpUuid()
{
    TpUuidData *uuidData = new TpUuidData();
    // uuidData->soleUuid = sole::uuid0();
    data_ = uuidData;
}

TpUuid::TpUuid(const TpUuid &other)
{
    TpUuidData *otherData = static_cast<TpUuidData *>(other.data_);
    TpUuidData *newData = new TpUuidData();
    newData->soleUuid = otherData->soleUuid; // 复制 sole::uuid 对象
    data_ = newData;
}

TpUuid::~TpUuid()
{
    TpUuidData *uuidData = static_cast<TpUuidData *>(data_);
    if (uuidData)
    {
        delete uuidData;
        data_ = nullptr;
    }
}

TpUuid &TpUuid::operator=(const TpUuid &other)
{
    // 检查自赋值
    if (this == &other)
        return *this;

    TpUuidData *otherData = static_cast<TpUuidData *>(other.data_);
    TpUuidData *myData = static_cast<TpUuidData *>(data_);

    // 复制 sole::uuid 对象
    myData->soleUuid = otherData->soleUuid;

    return *this;
}

TpUuid TpUuid::createUuid()
{
    TpUuid createUuid;
    TpUuidData *uuidData = static_cast<TpUuidData *>(createUuid.data_);
    uuidData->soleUuid = sole::uuid0();
    return createUuid;
}

TpUuid TpUuid::createUuidV1()
{
    TpUuid createUuid;
    TpUuidData *uuidData = static_cast<TpUuidData *>(createUuid.data_);
    uuidData->soleUuid = sole::uuid1();
    return createUuid;
}

TpUuid TpUuid::createUuidV4()
{
    TpUuid createUuid;
    TpUuidData *uuidData = static_cast<TpUuidData *>(createUuid.data_);
    uuidData->soleUuid = sole::uuid4();
    return createUuid;
}

TpUuid TpUuid::fromString(const TpString &text)
{
    TpUuid createUuid;
    TpUuidData *uuidData = static_cast<TpUuidData *>(createUuid.data_);
    uuidData->soleUuid = sole::rebuild(text);
    return createUuid;
}

TpString TpUuid::toString()
{
    TpUuidData *uuidData = static_cast<TpUuidData *>(data_);
    return uuidData->soleUuid.str();
}

TpString TpUuid::toBase62()
{
    TpUuidData *uuidData = static_cast<TpUuidData *>(data_);
    return uuidData->soleUuid.base62();
}

TpString TpUuid::toPretty()
{
    TpUuidData *uuidData = static_cast<TpUuidData *>(data_);
    return uuidData->soleUuid.pretty();
}
