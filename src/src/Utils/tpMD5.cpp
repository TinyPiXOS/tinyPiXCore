/* liucy has been here，but nothing to see and nothing left ^_^!*/

/*
** Copyright (c) 2007-2021 By Alexander.King.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/
#include "tpMD5.h"
#include <cstring>
#include "base/md5.h"

#define MAX_MD5_LENGTH	513
#define MD5_SIZE		16
#define MD5_STR_LEN		(MD5_SIZE * 2)

#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x^y^z)
#define I(x,y,z) (y ^ (x | ~z))
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))

#define FF(a,b,c,d,x,s,ac) \
{ \
	a += F(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}
#define GG(a,b,c,d,x,s,ac) \
{ \
	a += G(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}
#define HH(a,b,c,d,x,s,ac) \
{ \
	a += H(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}
#define II(a,b,c,d,x,s,ac) \
{ \
	a += I(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}

typedef struct{
	int32_t count[2];
	int32_t state[4];
	uint8_t buffer[64];   
} MD5_CTX;

typedef struct{
	char MD5String[MAX_MD5_LENGTH];
	int32_t length;
}ItpMD5Set;

// uint8_t PADDING[] =
// {
// 	0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
// 	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
// 	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
// 	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
// };

static inline void MD5Init(MD5_CTX *context)
{
	context->count[0] = 0;
	context->count[1] = 0;
	context->state[0] = 0x67452301;
	context->state[1] = 0xEFCDAB89;
	context->state[2] = 0x98BADCFE;
	context->state[3] = 0x10325476;
}

static inline void MD5Encode(uint8_t *output,int32_t *input,int32_t len)
{
	int32_t i = 0;
	int32_t j = 0;

	while(j < len)
	{
		output[j] = input[i] & 0xFF;  
		output[j+1] = (input[i] >> 8) & 0xFF;
		output[j+2] = (input[i] >> 16) & 0xFF;
		output[j+3] = (input[i] >> 24) & 0xFF;
		i++;
		j += 4;
	}
}

static inline void MD5Decode(int32_t *output, uint8_t *input, int32_t len)
{
	int32_t i = 0;
	int32_t j = 0;

	while(j < len)
	{
		output[i] = (input[j]) |
			(input[j+1] << 8) |
			(input[j+2] << 16) |
			(input[j+3] << 24);
		i++;
		j += 4; 
	}
}

static inline void MD5Transform(int32_t state[4], uint8_t block[64])
{
	int32_t a = state[0];
	int32_t b = state[1];
	int32_t c = state[2];
	int32_t d = state[3];
	int32_t x[64];

	MD5Decode(x,block,64);

	FF(a, b, c, d, x[ 0], 7, 0xd76aa478); /* 1 */
	FF(d, a, b, c, x[ 1], 12, 0xe8c7b756); /* 2 */
	FF(c, d, a, b, x[ 2], 17, 0x242070db); /* 3 */
	FF(b, c, d, a, x[ 3], 22, 0xc1bdceee); /* 4 */
	FF(a, b, c, d, x[ 4], 7, 0xf57c0faf); /* 5 */
	FF(d, a, b, c, x[ 5], 12, 0x4787c62a); /* 6 */
	FF(c, d, a, b, x[ 6], 17, 0xa8304613); /* 7 */
	FF(b, c, d, a, x[ 7], 22, 0xfd469501); /* 8 */
	FF(a, b, c, d, x[ 8], 7, 0x698098d8); /* 9 */
	FF(d, a, b, c, x[ 9], 12, 0x8b44f7af); /* 10 */
	FF(c, d, a, b, x[10], 17, 0xffff5bb1); /* 11 */
	FF(b, c, d, a, x[11], 22, 0x895cd7be); /* 12 */
	FF(a, b, c, d, x[12], 7, 0x6b901122); /* 13 */
	FF(d, a, b, c, x[13], 12, 0xfd987193); /* 14 */
	FF(c, d, a, b, x[14], 17, 0xa679438e); /* 15 */
	FF(b, c, d, a, x[15], 22, 0x49b40821); /* 16 */

	/* Round 2 */
	GG(a, b, c, d, x[ 1], 5, 0xf61e2562); /* 17 */
	GG(d, a, b, c, x[ 6], 9, 0xc040b340); /* 18 */
	GG(c, d, a, b, x[11], 14, 0x265e5a51); /* 19 */
	GG(b, c, d, a, x[ 0], 20, 0xe9b6c7aa); /* 20 */
	GG(a, b, c, d, x[ 5], 5, 0xd62f105d); /* 21 */
	GG(d, a, b, c, x[10], 9,  0x2441453); /* 22 */
	GG(c, d, a, b, x[15], 14, 0xd8a1e681); /* 23 */
	GG(b, c, d, a, x[ 4], 20, 0xe7d3fbc8); /* 24 */
	GG(a, b, c, d, x[ 9], 5, 0x21e1cde6); /* 25 */
	GG(d, a, b, c, x[14], 9, 0xc33707d6); /* 26 */
	GG(c, d, a, b, x[ 3], 14, 0xf4d50d87); /* 27 */
	GG(b, c, d, a, x[ 8], 20, 0x455a14ed); /* 28 */
	GG(a, b, c, d, x[13], 5, 0xa9e3e905); /* 29 */
	GG(d, a, b, c, x[ 2], 9, 0xfcefa3f8); /* 30 */
	GG(c, d, a, b, x[ 7], 14, 0x676f02d9); /* 31 */
	GG(b, c, d, a, x[12], 20, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH(a, b, c, d, x[ 5], 4, 0xfffa3942); /* 33 */
	HH(d, a, b, c, x[ 8], 11, 0x8771f681); /* 34 */
	HH(c, d, a, b, x[11], 16, 0x6d9d6122); /* 35 */
	HH(b, c, d, a, x[14], 23, 0xfde5380c); /* 36 */
	HH(a, b, c, d, x[ 1], 4, 0xa4beea44); /* 37 */
	HH(d, a, b, c, x[ 4], 11, 0x4bdecfa9); /* 38 */
	HH(c, d, a, b, x[ 7], 16, 0xf6bb4b60); /* 39 */
	HH(b, c, d, a, x[10], 23, 0xbebfbc70); /* 40 */
	HH(a, b, c, d, x[13], 4, 0x289b7ec6); /* 41 */
	HH(d, a, b, c, x[ 0], 11, 0xeaa127fa); /* 42 */
	HH(c, d, a, b, x[ 3], 16, 0xd4ef3085); /* 43 */
	HH(b, c, d, a, x[ 6], 23,  0x4881d05); /* 44 */
	HH(a, b, c, d, x[ 9], 4, 0xd9d4d039); /* 45 */
	HH(d, a, b, c, x[12], 11, 0xe6db99e5); /* 46 */
	HH(c, d, a, b, x[15], 16, 0x1fa27cf8); /* 47 */
	HH(b, c, d, a, x[ 2], 23, 0xc4ac5665); /* 48 */

	/* Round 4 */
	II(a, b, c, d, x[ 0], 6, 0xf4292244); /* 49 */
	II(d, a, b, c, x[ 7], 10, 0x432aff97); /* 50 */
	II(c, d, a, b, x[14], 15, 0xab9423a7); /* 51 */
	II(b, c, d, a, x[ 5], 21, 0xfc93a039); /* 52 */
	II(a, b, c, d, x[12], 6, 0x655b59c3); /* 53 */
	II(d, a, b, c, x[ 3], 10, 0x8f0ccc92); /* 54 */
	II(c, d, a, b, x[10], 15, 0xffeff47d); /* 55 */
	II(b, c, d, a, x[ 1], 21, 0x85845dd1); /* 56 */
	II(a, b, c, d, x[ 8], 6, 0x6fa87e4f); /* 57 */
	II(d, a, b, c, x[15], 10, 0xfe2ce6e0); /* 58 */
	II(c, d, a, b, x[ 6], 15, 0xa3014314); /* 59 */
	II(b, c, d, a, x[13], 21, 0x4e0811a1); /* 60 */
	II(a, b, c, d, x[ 4], 6, 0xf7537e82); /* 61 */
	II(d, a, b, c, x[11], 10, 0xbd3af235); /* 62 */
	II(c, d, a, b, x[ 2], 15, 0x2ad7d2bb); /* 63 */
	II(b, c, d, a, x[ 9], 21, 0xeb86d391); /* 64 */
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
}


static inline void MD5Update(MD5_CTX *context, uint8_t *input, int32_t inputlen)
{
	int32_t i = 0;
	int32_t index = 0;
	int32_t partlen = 0;

	index = (context->count[0] >> 3) & 0x3F;
	partlen = 64 - index;
	context->count[0] += inputlen << 3;

	if(context->count[0] < (inputlen << 3))
		context->count[1]++;
	context->count[1] += inputlen >> 29;

	if(inputlen >= partlen){
		memcpy(&context->buffer[index], input,partlen);
		MD5Transform(context->state, context->buffer);

		for(i = partlen; i+64 <= inputlen; i+=64)
			MD5Transform(context->state, &input[i]);

		index = 0;        
	}  
	else{
		i = 0;
	}
	memcpy(&context->buffer[index], &input[i], inputlen-i);
}

static inline void MD5Final(MD5_CTX *context, uint8_t digest[16])
{
	int32_t index = 0,padlen = 0;
	uint8_t bits[8];

	index = (context->count[0] >> 3) & 0x3F;
	padlen = (index < 56)?(56-index):(120-index);
	MD5Encode(bits, context->count, 8);
	MD5Update(context, PADDING, padlen);
	MD5Update(context, bits, 8);
	MD5Encode(digest, context->state, 16);
}


tpMD5::tpMD5()
{
	ItpMD5Set *set = new ItpMD5Set();
	
	if(set){
		memset(set->MD5String, 0, MAX_MD5_LENGTH);
		set->length = 0;
		this->md5Set = set;
	}
}

tpMD5::~tpMD5()
{
	ItpMD5Set *set = (ItpMD5Set*)this->md5Set;
	
	if(set){
		delete set;
	}
}

const char *tpMD5::create(std::string &srcString, tpMD5::ItpMD5Type type)
{
	return this->create(srcString.c_str(), type);
}

const char *tpMD5::create(const char *srcString, tpMD5::ItpMD5Type type)
{
	int32_t loopCount = 0, md5length[] = {0, 32, 64, 128, 256, 512};
	
	if(srcString == nullptr ||
		strlen(srcString) == 0){
		return nullptr;
	}

	switch(type)
	{
	case tpMD5::GEN_32_BYTES:
		{
			loopCount = 1;
		}
	break;
	case tpMD5::GEN_64_BYTES:
		{
			loopCount = 2;
		}
	break;
	case tpMD5::GEN_128_BYTES:
		{
			loopCount = 4;
		}
	break;
	case tpMD5::GEN_256_BYTES:
		{
			loopCount = 8;
		}
	break;
	case tpMD5::GEN_512_BYTES:
		{
			loopCount = 16;
		}
	break;
	default:
		return nullptr;
	}

	ItpMD5Set *set = (ItpMD5Set*)this->md5Set;
	
	if(set){
		int32_t count = loopCount, tmplength = 0, i;
		char *tmp_ptr = (char*)srcString, md5[64];
		
		while(count--){
			char *tmpString = (char*)tpMD5::getnerateMD5((const char*)tmp_ptr, strlen(tmp_ptr));
			memcpy(set->MD5String + tmplength, tmpString, MD5_STR_LEN);
			strcpy(md5, tmpString);		
			tmp_ptr = md5;
			tmplength += MD5_STR_LEN;
			set->MD5String[tmplength] = '\0';
		}
		
		set->length = md5length[loopCount];
		return set->MD5String;
	}
	
	return nullptr;
}

const char *tpMD5::MD5String()
{
	ItpMD5Set *set = (ItpMD5Set*)this->md5Set;
	const char *result = nullptr;
	
	if(set){
		if(set->length){
			result = set->MD5String;
		}
	}
	
	return result;
}

int32_t tpMD5::MD5Length()
{
	ItpMD5Set *set = (ItpMD5Set*)this->md5Set;
	int32_t length = 0;
	
	if(set){
		length = set->length;
	}
	
	return length;
}

void tpMD5::release()
{
	ItpMD5Set *set = (ItpMD5Set*)this->md5Set;
	int32_t length = 0;
	
	if(set){
		memset(set, 0, sizeof(ItpMD5Set));
	}
}

const char *tpMD5::getnerateMD5(const char *input, int32_t length)
{
	static char md5[MD5_STR_LEN + 1];
	uint8_t md5_value[16];
	int32_t i;

	if(input == NULL ||
		length <= 0){
		return nullptr;
	}

	MD5_CTX c_md5;

	MD5Init(&c_md5);
	MD5Update(&c_md5, (uint8_t *)input, length);
	MD5Final(&c_md5, md5_value);

	for(i = 0; i < MD5_SIZE; i++){
		snprintf(md5 + i*2, 2 + 1, "%02x", md5_value[i]);
	}

	md5[MD5_STR_LEN] = '\0';
	
	return md5;
}
