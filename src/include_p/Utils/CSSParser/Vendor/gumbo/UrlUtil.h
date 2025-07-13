/*
 * UrlUtil.h
 *
 *  Created on: 2017-11-21
 *      Author: bingjian
 */

#ifndef URLUTIL_H_
#define URLUTIL_H_
#include <string>

namespace future {

class UrlUtil {
public:
	UrlUtil();
	virtual ~UrlUtil();
	static std::string UrlEncode(const std::string& str);
	static std::string UrlDecode(const std::string& str);

private:
	static unsigned char ToHex(unsigned char x);
	static unsigned char FromHex(unsigned char x);

};

}

#endif /* URLUTIL_H_ */
