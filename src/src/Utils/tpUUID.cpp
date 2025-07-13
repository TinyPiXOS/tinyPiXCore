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
#include "tpUUID.h"
#include <cstring>

#define UUID_MAX_LENGTH	37

typedef struct{
	char uuidString[UUID_MAX_LENGTH];
	int32_t length;
}ItpUUIDSet;

static unsigned long long seed[2];

static inline unsigned long long xorshift128plus(unsigned long long *s) 
{
  unsigned long long s1 = s[0];
  const unsigned long long s0 = s[1];
  s[0] = s0;
  s1 ^= s1 << 23;
  s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
  
  return s[1] + s0;
}


static inline bool uuid4_init() 
{
#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
  int32_t res;
  FILE *fp = fopen("/dev/urandom", "rb");
  if (!fp) {
    return false;
  }
  res = fread(seed, 1, sizeof(seed), fp);
  fclose(fp);
  if ( res != sizeof(seed) ) {
    return false;
  }

#elif defined(_WIN32)
  int32_t res;
  HCRYPTPROV hCryptProv;
  res = CryptAcquireContext(
    &hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
  if (!res) {
    return false;
  }
  res = CryptGenRandom(hCryptProv, (DWORD) sizeof(seed), (PBYTE) seed);
  CryptReleaseContext(hCryptProv, 0);
  if (!res) {
    return false;
  }

#else
  #error "unsupported platform"
#endif
  return true;
}

static inline void uuid4_generate(char *dst) 
{
  static const char *ptemplate = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
  static const char *chars = "0123456789abcdef";
  
  union{
  	uint8_t b[16]; 
  	unsigned long long word[2]; 
  } s;
  
  const char *p;
  int32_t i, n;
  /* get random */
  s.word[0] = xorshift128plus(seed);
  s.word[1] = xorshift128plus(seed);
  /* build string */
  p = ptemplate;
  i = 0;
  while (*p) {
    n = s.b[i >> 1];
    n = (i & 1) ? (n >> 4) : (n & 0xf);
    switch (*p) {
      case 'x'  : *dst = chars[n];              i++;  break;
      case 'y'  : *dst = chars[(n & 0x3) + 8];  i++;  break;
      default   : *dst = *p;
    }
    dst++, p++;
  }
  *dst = '\0';
}

tpUUID::tpUUID()
{
	ItpUUIDSet *set = new ItpUUIDSet();
	
	if(set){
		memset(set, 0, sizeof(ItpUUIDSet));
		this->uuidSet = set;
	}
}

tpUUID::~tpUUID()
{
	ItpUUIDSet *set = (ItpUUIDSet*)this->uuidSet;
	
	if(set){
		delete set;
	}
}

const char *tpUUID::create()
{
	ItpUUIDSet *set = (ItpUUIDSet*)this->uuidSet;
	const  char *result = nullptr;
	
	if(set){
		bool ret = uuid4_init();
		
		if(ret){
			uuid4_generate(set->uuidString);
			set->length = UUID_MAX_LENGTH - 1;
			result = set->uuidString;
		}
	}
	
	return result;
}

const char *tpUUID::uuidString()
{
	ItpUUIDSet *set = (ItpUUIDSet*)this->uuidSet;
	const  char *result = nullptr;
	
	if(set){
		if(set->length){
			result = set->uuidString;
		}
	}
	
	return result;
}

int32_t tpUUID::uuidLength()
{
	ItpUUIDSet *set = (ItpUUIDSet*)this->uuidSet;
	int32_t length = 0;
	
	if(set){
		length = set->length;
	}
	
	return length;
}

void tpUUID::release()
{
	ItpUUIDSet *set = (ItpUUIDSet*)this->uuidSet;
	
	if(set){
		memset(set, 0, sizeof(ItpUUIDSet));
	}
}

const char *tpUUID::generateUUID()
{
	static char uuid[UUID_MAX_LENGTH];
	bool ret = uuid4_init();
	const char *result = nullptr;
	
	if(ret){
		uuid4_generate(uuid);
		result = uuid;
	}
	
	return result;
}
