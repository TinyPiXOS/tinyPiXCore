#include <TpJsonDocument.h>
#include <sstream>

TpJsonDocument::TpJsonDocument()
    : doc_()
{
    doc_.SetNull();
}

TpJsonDocument::TpJsonDocument(const TpJsonObject &object)
{
    doc_.CopyFrom(object.doc_, doc_.GetAllocator());
}

TpJsonDocument::TpJsonDocument(const TpJsonArray &array)
{
    doc_.CopyFrom(array.doc_, doc_.GetAllocator());
}

TpJsonDocument::TpJsonDocument(const TpJsonDocument &other)
{
    *this = other;
}

TpJsonDocument TpJsonDocument::fromJson(const TpString &json)
{
    TpJsonDocument tmpJsonDoc;

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
        return TpJsonDocument(); // 返回空文档
    }

    // std::cout << "Parsed JSON in fromJson: " << tmpJsonDoc.toJson() << std::endl;

    return tmpJsonDoc;
}

TpString TpJsonDocument::toJson()
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    rapidjson::Value &json_value = this->doc_;

    json_value.Accept(writer);
    // std::string ret = std::string(buffer.GetString(), buffer.GetSize()) + '\0';
    std::string ret = std::string(buffer.GetString(), buffer.GetSize());

    return ret;
}

TpString TpJsonDocument::toFormattedJson()
{  
    rapidjson::StringBuffer buffer;  
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);  
      
    rapidjson::Value &json_value = this->doc_;  
      
    json_value.Accept(writer);  
    std::string ret = std::string(buffer.GetString(), buffer.GetSize());  
      
    return ret;  
}

TpJsonObject TpJsonDocument::object() const
{
    TpJsonObject tmpJsonObj;

    if (this->doc_.IsObject())
        tmpJsonObj.doc_.CopyFrom(this->doc_, tmpJsonObj.doc_.GetAllocator());

    return tmpJsonObj;
}

TpJsonArray TpJsonDocument::array() const
{
    TpJsonArray tmpJsonArr;

    if (this->doc_.IsArray())
        tmpJsonArr.doc_.CopyFrom(this->doc_, tmpJsonArr.doc_.GetAllocator());

    return tmpJsonArr;
}

TpJsonDocument &TpJsonDocument::operator=(const TpJsonDocument &_others)
{
    if (this != &_others)
    {
        // doc_.Clear();
        doc_.CopyFrom(_others.doc_, doc_.GetAllocator());
    }

    return *this;
}
