/***
 * @Author: hywang
 * @Date: 2024-05-31 10:02:28
 * @LastEditors: hywang
 * @LastEditTime: 2024-08-15 16:08:15
 * @FilePath: /pix-singlegui/PixSingleGUI/include/Utils/TpString.h
 * @Description: std::string字符串封装类
 * @
 * @PiXOS
 */

#ifndef TP_STRING_H
#define TP_STRING_H

#include <string>
#include "TpRegex.h"
#include "TpList.h"

class TpString : public std::string
{
    // private:
    //     std::string pString_;

public:
    TpString() {}

    TpString(const char &ch)
    {
        std::string::append(1, ch);
    }

    TpString(const char *value)
        : std::string(value)
    {
    }

    TpString(const std::string &value)
        : std::string(value)
    {
    }

    TpString operator=(const std::string &_others)
    {
        *this = TpString(_others);
        return *this;
    }

    TpString operator=(const char *_others)
    {
        *this = TpString(_others);
        return *this;
    }

    bool operator==(const TpString &_others) const
    {
        return (this->compare(_others) == 0) ? true : false;
    }

    /// @brief 将数字转换为字符串
    /// @param num 数字
    /// @return 返回转换后的字符串
    static TpString number(const int32_t &num);
    static TpString number(const uint32_t &num);
    static TpString number(const uint64_t &num);
    static TpString number(const double &num, const uint32_t &precision = 2);

    /// @brief 指定字符分割字符串
    /// @param _ch 分隔符
    /// @return 返回分割后的字符串列表
    TpList<TpString> split(const char &_ch) const;

    /// @brief 去除首尾空格，中间空格只保留一个
    /// @return 返回处理后的字符串
    TpString simplified() const;

    /// @brief 指定字符串替换
    /// @param from 要被替换的字符串
    /// @param to 替换后的字符串
    /// @return 返回替换后的结果
    TpString replace(const TpString &from, const TpString &to) const;

    /// @brief 使用正则表达式进行字符串替换
    /// @param regex 正则对象
    /// @param to 匹配的字符串替换对象
    /// @return 返回替换后的结果
    TpString replace(const TpRegex &regex, const TpString &to) const;

    // 替换字符串中的内容
    TpString replace(uint64_t pos, uint64_t len, const TpString &str);

    /// @brief 是否包含某个字符串
    /// @param find 查询字符串
    /// @return 当前字符串包含find返回true，否则返回false
    bool contains(const TpString &find) const;

    /// @brief 切割字符串
    /// @param _pos 起始索引，0开始
    /// @param _count 分割长度，-1为一直到字符串末尾
    /// @return 切割后字符串
    TpString mid(const uint32_t &_pos, const int32_t &_count = -1) const;

    /// @brief 指定索引移除字符
    /// @param _pos 索引值，从该索引开始移除
    /// @param _count 移除字符数
    void remove(const uint32_t & _pos, const uint32_t &_count = 1);

    /// @brief 字符串转整数
    /// @param base 字符串内容的进制，默认为10进制，包括 2/8/10/16
    /// @return 转换结果
    int16_t toShort(int32_t base = 10) const;
    /// @brief 字符串转整数
    /// @param base 字符串内容的进制，默认为10进制，包括 2/8/10/16
    /// @return 转换结果
    int32_t toInt(int32_t base = 10) const;
    double toDouble() const;

	bool toBool() const;

	// 转换为无符号短整型
    uint16_t toUShort(bool *ok = nullptr, int base = 10) const;
    
    // 转换为无符号整型
    uint32_t toUInt(bool *ok = nullptr, int base = 10) const;

	/// @brief 转换为大写
	/// @return 
	TpString toUpper() const;

	/// @brief 转换为小写
	/// @return 
	TpString toLower() const;
	
    /// @brief 	在字符串左侧填充字符
    /// @param width 填充数量
    /// @param fill 填充的字符
    /// @return 
    TpString leftJustified(uint32_t width, char fill = ' ') const;
    
    /// @brief 	在字符串右侧填充字符
    /// @param width 填充数量
    /// @param fill 填充的字符
    /// @return 
    TpString rightJustified(uint32_t width, char fill = ' ') const;
    
    // 重复字符串n次
    TpString repeated(uint32_t times) const;

	// 获取字符的十六进制表示
    TpString toHex(char separator = '\0') const;
    
    // 检查字符串是否包含任何指定字符集中的字符
    bool containsAnyOf(const TpString &charSet) const;
    
    // 检查字符串是否仅包含指定字符集中的字符
    bool containsOnly(const TpString &charSet) const;

	// 删除所有指定字符
    TpString removeChar(char ch) const;
    
    // 字符串反转
    TpString reversed() const;

    /// @brief 指定索引插入指定字符或字符串
    /// @param post 索引值，从0开始
    /// @param str 插入字符串
    void insert(const uint32_t& pos, const TpString& str);

    /// @brief 末尾追加字符串
    /// @param str 指定字符串
    void append(const TpString& str);

    /// @brief 从字符串起始位置，切割指定长度字符
    /// @param _length 切割长度
    /// @return 切割后字符串
    TpString left(const uint32_t &_length);

    /// @brief 从字符串终止位置，切割指定长度字符
    /// @param _length 切割长度
    /// @return 切割后字符串
    TpString right(const uint32_t &_length);

    /// @brief 判断字符串是否以指定字符串开头
    /// @param prefix 指定字符串
    /// @return 查询结果
    bool startsWith(const TpString &prefix) const;

    /// @brief 检查字符串是否以指定后缀结尾
    /// @param suffix 后缀
    /// @return 查询结果
    bool endsWith(const TpString &suffix) const;

    /// @brief 获取字符串的逻辑长度（中英文字符各算一个）
    uint32_t logicalLength() const;

    /// @brief 根据逻辑位置获取对应的字节位置
    uint32_t logicalPosToBytePos(uint32_t logicalPos) const;

    /// @brief 获取下一个字符的字节位置
    uint32_t nextCharBytePos(uint32_t currentBytePos) const;

    /// @brief 获取前一个字符的字节位置
    uint32_t prevCharBytePos(uint32_t currentBytePos) const;

private:
    /// @brief 计算字符的字节长度（基于UTF-8编码）
    uint32_t getCharByteLength(uint32_t bytePos) const;

    uint32_t bytePosToLogicalPos(uint32_t bytePos) const;
};

// 特化:hash模板为tpString类型
namespace std
{
    template <>
    struct hash<TpString>
    {
        std::size_t operator()(const TpString &str) const
        {
            // 使用std::string的哈希函数
            return hash<std::string>()(str);
        }
    };
}

#endif
