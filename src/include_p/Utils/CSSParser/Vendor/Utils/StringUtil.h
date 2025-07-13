/*
 * StringUtil.h
 *
 *  Created on: 2017-11-11
 *      Author: bingjian
 */

#ifndef STRINGUTIL_H_
#define STRINGUTIL_H_
#include <string>
#include <list>

namespace future {

class StringUtil {
public:
	StringUtil();
	virtual ~StringUtil();

	static std::string longlong2str(long long in);
	static std::string int2str(int in);
	static int str2int(const std::string &in);
	static bool startWith(std::string &str,std::string &strWith);
	static bool endWith(std::string &str,std::string &strWith);
	static std::string tolower(const std::string &str);
	static std::string toupper(const std::string &str);
	static bool contains(const std::string str1,const std::string str2);
	static std::string tostring(long long in);
	static void StringReplace(std::string &srcStr, const std::string &findStr,const std::string &replaceStr );
	static std::string &trim (std::string &s);
    static std::list<std::string> SeperatorBy(const std::string& , char c);
    static std::string DeleteCharacter(const std::string&, char);
};

}
#endif /* STRINGUTIL_H_ */
