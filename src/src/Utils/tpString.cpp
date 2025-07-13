#include "tpString.h"
#include <sstream>
#include <regex>
#include <iomanip>
#include <iostream>

tpString tpString::number(const int32_t &num)
{
    return std::to_string(num);
}

tpString tpString::number(const uint32_t &num)
{
    return std::to_string(num);
}

tpString tpString::number(const uint64_t &num)
{
    return std::to_string(num);
}

tpString tpString::number(const double &num, const uint32_t &precision)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << num;
    std::string result = out.str();
    return result;
}

tpList<tpString> tpString::split(const char &_ch) const
{
    tpList<tpString> resList;
    if (this->empty())
        return resList;

    std::string token;
    std::istringstream tokenStream(*this);

    while (std::getline(tokenStream, token, _ch))
    {
        resList.emplace_back(token);
    }

    return resList;
}

tpString tpString::simplified() const
{
    tpString result;
    bool inWhitespace = false;

    for (char ch : *this)
    {
        if (std::isspace(static_cast<uint8_t>(ch)))
        {
            if (!inWhitespace && !result.empty())
            {
                result += ' ';
                inWhitespace = true;
            }
        }
        else
        {
            result += ch;
            inWhitespace = false;
        }
    }

    // 移除末尾可能添加的空格
    if (!result.empty() && result.back() == ' ')
    {
        result.pop_back();
    }

    return result;
}

tpString tpString::replace(const tpString &from, const tpString &to) const
{
    if (from.length() == 0)
        return *this;

    if (from.compare(to) == 0)
        return *this;

    if (this->compare(to) == 0)
        return *this;

    tpString result = *this;
    size_type start_pos = result.find(from, 0);

    // 循环遍历字符串，查找所有出现的子字符串
    while (start_pos != std::string::npos)
    {
        // 将子字符串之前的部分添加到结果中
        result = result.std::string::replace(start_pos, from.length(), to);

        // 查找下一个出现的子字符串
        start_pos = result.find(from, 0);
    }

    return result;
}

tpString tpString::replace(const tpRegex &regex, const tpString &to) const
{
    tpString result = *this;
    tpString regexStr = regex.regexStr();

    try
    {
        std::regex reg(regexStr); // 尝试构造正则表达式对象

        result = std::regex_replace(result, std::regex(regexStr), to);
    }
    catch (const std::regex_error &e)
    {
        // 捕获并处理正则表达式错误
        std::cerr << "Regex error: " << e.what() << std::endl;
        // 可以根据需要处理错误，例如返回错误代码、退出程序等
        return "";
    }

    return result;
}

tpString tpString::replace(uint64_t pos, uint64_t len, const tpString &str)
{
    if (pos >= this->length())
        return "";

    if ((pos + len) > this->length())
        return "";

    // 调用基类 std::string 的 replace 方法
    std::string::replace(pos, len, str);
    return *this;
}

bool tpString::contains(const tpString &find) const
{
    size_type start_pos = std::string::find(find, 0);

    return start_pos != std::string::npos;
}

tpString tpString::mid(const uint32_t &_pos, const int32_t &_count) const
{
    size_t startByte = logicalPosToBytePos(_pos);
    if (startByte > size())
        return tpString();

    size_t remaining = (_count == -1) ? (logicalLength() - _pos) : static_cast<size_t>(_count);
    size_t endByte = startByte;
    for (size_t i = 0; i < remaining && endByte < size(); ++i)
    {
        endByte = nextCharBytePos(endByte);
    }
    return substr(startByte, endByte - startByte);
}

void tpString::remove(const uint32_t &_pos, const uint32_t &_count)
{
    // 获取字符串的逻辑长度（字符数）
    uint32_t len = logicalLength();

    // 边界检查：如果起始位置超出字符串长度，直接返回
    if (_pos >= len)
    {
        return;
    }

    // 计算实际要移除的字符数
    uint32_t count = std::min(_count, len - _pos); // 确保不越界

    // 转换为字节位置
    uint32_t startByte = logicalPosToBytePos(_pos);
    uint32_t endByte = logicalPosToBytePos(_pos + count);

    // 调用基类的 erase 方法移除字节区间
    std::string::erase(startByte, endByte - startByte);
}

int16_t tpString::toShort(int32_t base) const
{
    return stoi(*this, nullptr, base);
}

int32_t tpString::toInt(int32_t base) const
{
    return stoi(*this, nullptr, base);
}

double tpString::toDouble() const
{
    return stof(*this);
}

void tpString::insert(const uint32_t &pos, const tpString &str)
{
    // 获取字节位置
    size_t bytePos = logicalPosToBytePos(pos > logicalLength() ? logicalLength() : pos);
    // 调用基类的insert方法插入内容
    std::string::insert(bytePos, str);
}

void tpString::append(const tpString &str)
{
    // 调用基类的append方法追加内容
    std::string::append(str);
}

tpString tpString::left(const uint32_t &_length)
{
    if (_length > length())
        return mid(0, length());
    return mid(0, _length);
}

tpString tpString::right(const uint32_t &_length)
{
    if (_length > length())
        return mid(0, length());
    return mid(length() - _length);
}

bool tpString::startsWith(const tpString &prefix) const
{
    // 如果前缀长度大于当前字符串长度，直接返回false
    if (prefix.size() > this->size())
        return false;

    // 比较字符串的前缀部分
    return std::equal(prefix.begin(), prefix.end(), this->begin());
}

bool tpString::endsWith(const tpString &suffix) const
{
    const size_t suffix_len = strlen(suffix.c_str());
    const size_t this_len = this->size();

    // 如果后缀比原字符串长，直接返回 false
    if (suffix_len > this_len)
        return false;

    // 从原字符串末尾开始比较
    return (this->compare(this_len - suffix_len, suffix_len, suffix.c_str()) == 0);
}

uint32_t tpString::logicalLength() const
{
    uint32_t len = 0;
    uint32_t bytePos = 0;
    while (bytePos < size())
    {
        bytePos += getCharByteLength(bytePos);
        len++;
    }
    return len;
}

uint32_t tpString::logicalPosToBytePos(uint32_t logicalPos) const
{
    size_t bytePos = 0;
    size_t currentLogical = 0;
    while (bytePos < size() && currentLogical < logicalPos)
    {
        bytePos += getCharByteLength(bytePos);
        currentLogical++;
    }
    return bytePos;
}

uint32_t tpString::nextCharBytePos(uint32_t currentBytePos) const
{
    if (currentBytePos >= size())
        return size();
    return currentBytePos + getCharByteLength(currentBytePos);
}

uint32_t tpString::prevCharBytePos(uint32_t currentBytePos) const
{
    if (currentBytePos == 0)
        return 0;
    size_t pos = currentBytePos - 1;
    while (pos > 0 && ((*this)[pos] & 0xC0) == 0x80)
    {
        --pos;
    }
    return pos;
}

uint32_t tpString::getCharByteLength(uint32_t bytePos) const
{
    if (bytePos >= size())
        return 0;
    unsigned char ch = (*this)[bytePos];
    if ((ch & 0x80) == 0)
        return 1; // 单字节字符
    else if ((ch & 0xE0) == 0xC0)
        return 2; // 双字节字符
    else if ((ch & 0xF0) == 0xE0)
        return 3; // 三字节字符
    else if ((ch & 0xF8) == 0xF0)
        return 4; // 四字节字符
    return 1;     // 默认处理为单字节
}

uint32_t tpString::bytePosToLogicalPos(uint32_t bytePos) const
{
    if (bytePos >= size())
    {
        return logicalLength();
    }
    uint32_t logicalPos = 0;
    uint32_t currentByte = 0;
    while (currentByte < bytePos)
    {
        currentByte += getCharByteLength(currentByte);
        logicalPos++;
    }
    return logicalPos;
}
