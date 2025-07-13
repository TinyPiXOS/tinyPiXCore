#include <tpJsonDocument.h>

tpJsonDocument::tpJsonDocument()
    : doc_()
{
    doc_.SetNull();
}

tpJsonDocument::tpJsonDocument(const tpJsonObject &object)
{
    doc_.CopyFrom(object.doc_, doc_.GetAllocator());
}

tpJsonDocument::tpJsonDocument(const tpJsonArray &array)
{
    doc_.CopyFrom(array.doc_, doc_.GetAllocator());
}

tpJsonDocument::tpJsonDocument(const tpJsonDocument &other)
{
    *this = other;
}

tpJsonDocument tpJsonDocument::fromJson(const tpString &json)
{
    tpJsonDocument tmpJsonDoc;

    // 重置 Document 状态
    tmpJsonDoc.doc_.SetNull();
    // tmpJsonDoc.doc_.Clear();

    // rapidjson::MemoryPoolAllocator<> allocator;
    rapidjson::ParseResult result = tmpJsonDoc.doc_.Parse(json.c_str());
    if (result.IsError())
    {
        // 抛出异常或记录错误
        std::cerr << "JSON 解析错误: " << rapidjson::GetParseErrorFunc(result.Code())
                  << " (偏移量: " << result.Offset() << ")" << std::endl;
        return tpJsonDocument(); // 返回空文档
    }

    // std::cout << "Parsed JSON in fromJson: " << tmpJsonDoc.toJson() << std::endl;

    return tmpJsonDoc;
}

tpString tpJsonDocument::toJson()
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    rapidjson::Value &json_value = this->doc_;

    json_value.Accept(writer);
    // std::string ret = std::string(buffer.GetString(), buffer.GetSize()) + '\0';
    std::string ret = std::string(buffer.GetString(), buffer.GetSize());

    return ret;
}

tpJsonObject tpJsonDocument::object() const
{
    tpJsonObject tmpJsonObj;

    if (this->doc_.IsObject())
        tmpJsonObj.doc_.CopyFrom(this->doc_, tmpJsonObj.doc_.GetAllocator());

    return tmpJsonObj;
}

tpJsonArray tpJsonDocument::array() const
{
    tpJsonArray tmpJsonArr;

    if (this->doc_.IsArray())
        tmpJsonArr.doc_.CopyFrom(this->doc_, tmpJsonArr.doc_.GetAllocator());

    return tmpJsonArr;
}

tpJsonDocument &tpJsonDocument::operator=(const tpJsonDocument &_others)
{
    if (this != &_others)
    {
        // doc_.Clear();
        doc_.CopyFrom(_others.doc_, doc_.GetAllocator());
    }

    return *this;
}
