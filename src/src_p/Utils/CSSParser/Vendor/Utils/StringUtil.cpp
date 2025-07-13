/*
 * StringUtil.cpp
 *
 *  Created on: 2017-11-11
 *      Author: bingjian
 */

#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <list>
#include "StringUtil.h"

namespace future {
    
    StringUtil::StringUtil() {
        
    }
    
    StringUtil::~StringUtil() {
    }
    
    std::string StringUtil::longlong2str(long long in) {
        std::stringstream stream;
        stream << in;
        return stream.str();
    }
    
    std::string StringUtil::int2str(int in) {
        std::stringstream stream;
        stream << in;
        return stream.str();
    }
    
    int StringUtil::str2int(const std::string &in) {
        int out = 0;
        std::stringstream stream(in);
        stream >> out;
        return out;
    }
    
    bool StringUtil::startWith(std::string &str, std::string &start) {
        return str.compare(0, start.size(), start) == 0;
    }
    
    bool StringUtil::endWith(std::string &str, std::string &end) {
        return str.compare(str.size() - end.size(), end.size(), end) == 0;
    }
    
    std::string StringUtil::tolower(const std::string &str) {
        std::string ret(str);
        std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
        return ret;
    }
    
    std::string StringUtil::toupper(const std::string &str) {
        std::string ret(str);
        std::transform(ret.begin(), ret.end(), ret.begin(), ::toupper);
        return ret;
    }
    
    bool StringUtil::contains(const std::string str1, const std::string str2) {
        if (str1.find(str2) != std::string::npos) {
            return true;
        }
        return false;
    }
    
    std::string StringUtil::tostring(long long in) {
        std::stringstream ss;
        std::string ret;
        ss << in;
        ss >> ret;
        return ret;
    }
    
    void StringUtil::StringReplace(std::string &srcStr, const std::string &findStr,
                                   const std::string &replaceStr) {
        std::size_t replaceStrLen = replaceStr.length();
        for (std::size_t pos = 0; pos != std::string::npos; pos += replaceStrLen) {
            if ((pos = srcStr.find(findStr, pos)) != std::string::npos) {
                srcStr.replace(pos, findStr.length(), replaceStr);
            } else {
                break;
            }
        }
    }
    
    std::string &StringUtil::trim (std::string &s){
        if(s.empty()){
            return s;
        }
        s.erase(0,s.find_first_not_of(" "));
        s.erase(s.find_last_not_of(" ") + 1);
        return s;
    }
    
    std::list<std::string> StringUtil::SeperatorBy(const std::string& s, char seperator)
    {
        std::list<std::string>container;
        std::istringstream f(s);
        std::istringstream& stream = f;
        std::string out;
        while(std::getline(stream, out, seperator)) {
            container.push_back(out);
        }
        return container;
    }
    
    std::string StringUtil::DeleteCharacter(const std::string &source, char target)
    {
        std::string dest = source;
        dest.erase(std::remove(dest.begin(), dest.end(), target));
        return dest;
    }
    
}
