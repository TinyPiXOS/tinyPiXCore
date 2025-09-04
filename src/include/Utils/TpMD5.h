#ifndef __TP_MD5_H
#define __TP_MD5_H

#include "TpUtils.h"
#include <string>

TP_DEF_VOID_TYPE_VAR(IPitpMD5);

class TpMD5
{
public:
    typedef enum
    {
        GEN_32_BYTES,
        GEN_64_BYTES,
        GEN_128_BYTES,
        GEN_256_BYTES,
        GEN_512_BYTES,
    } ItpMD5Type;

public:
    TpMD5();

public:
    ~TpMD5();

public:
    virtual const char *create(std::string &srcString, ItpMD5Type type = GEN_32_BYTES);
    virtual const char *create(const char *srcString, ItpMD5Type type = GEN_32_BYTES);

public:
    virtual const char *MD5String();
    virtual int32_t MD5Length();

public:
    virtual void release();

public:
    static const char *getnerateMD5(const char *input, int32_t length); // default 32BYTES
    
private:
    IPitpMD5 *md5Set;
};

#endif
