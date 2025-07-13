#ifndef	JSONSTRUCTBUFFER_H
#define	JSONSTRUCTBUFFER_H

#include <vector>
#include <string>

#include "Utils/JsonStructPackage/rapidjson/rapidjson.h"
#include "Utils/JsonStructPackage/rapidjson/document.h"
#include "Utils/JsonStructPackage/rapidjson/stringbuffer.h"
#include "Utils/JsonStructPackage/rapidjson/writer.h"

class JsonStructBuffer
{
	rapidjson::Document root_;

public:
	JsonStructBuffer()
	{
		root_.SetObject();
	}

	~JsonStructBuffer()
	{
	}

	std::string data() 
	{ 
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		rapidjson::Value& json_value = root_;

		json_value.Accept(writer);
		std::string ret = std::string(buffer.GetString(), buffer.GetSize()) + '\0';

		return ret;
	}

public:
	template<class _T>
	void append(const std::string& _name, _T _value)
	{
		rapidjson::Document::AllocatorType& allocator = root_.GetAllocator();

		rapidjson::Value json_key;
		json_key.SetString(_name.c_str(), _name.length(), allocator);

		root_.AddMember(json_key, _value, allocator);
	}

	void append(const std::string& _name, std::string _value)
	{
		rapidjson::Document::AllocatorType& allocator = root_.GetAllocator();

		rapidjson::Value json_key;
		json_key.SetString(_name.c_str(), _name.length(), allocator);

		rapidjson::Value json_value;
		json_value.SetString(_value.c_str(), _value.length(), allocator);

		root_.AddMember(json_key, json_value, allocator);
	}

	void appendObject(const std::string& _name, std::string _value)
	{
		rapidjson::Document obj_doc;
		obj_doc.Parse(_value.c_str());

		rapidjson::Document::AllocatorType& allocator = root_.GetAllocator();

		rapidjson::Value json_key;
		json_key.SetString(_name.c_str(), _name.length(), allocator);

		rapidjson::Value json_value(obj_doc, allocator);

		root_.AddMember(json_key, json_value, allocator);

	}

	// 判断字符串是json还是普通字符串
	void appendArray(const std::string& _name, const std::vector<std::string>& _value_list)
	{
		rapidjson::Document::AllocatorType& allocator = root_.GetAllocator();

		rapidjson::Value json_key;
		json_key.SetString(_name.c_str(), _name.length(), allocator);

		rapidjson::Value json_array;
		json_array.SetArray();

		for (const auto& str : _value_list)
		{
			rapidjson::Document obj_doc;
			obj_doc.Parse(str.c_str());

			if (obj_doc.IsObject())
			{
				rapidjson::Value json_value(obj_doc, allocator);

				json_array.PushBack(json_value, allocator);
			}
			else
			{
				rapidjson::Value json_value;
				json_value.SetString(str.c_str(), str.length(), allocator);

				json_array.PushBack(json_value, allocator);
			}
		}

		root_.AddMember(json_key, json_array, allocator);

	}

	template<class _T>
	void appendArray(const std::string& _name, std::vector<_T> _value_list)
	{
		rapidjson::Document::AllocatorType& allocator = root_.GetAllocator();

		rapidjson::Value json_key;
		json_key.SetString(_name.c_str(), _name.length(), allocator);

		rapidjson::Value json_array;
		json_array.SetArray();

		for (const auto& base_data : _value_list)
		{
			json_array.PushBack(base_data, allocator);
		}

		root_.AddMember(json_key, json_array, allocator);
	}

};


class JsonParser
{
	rapidjson::Document root_;

public:
	JsonParser(){}

	bool SetJsonString(const std::string& _json_str)
	{
		root_.Parse(_json_str.c_str());

		return root_.IsObject();
	}

	// 获取其他数据
	template<typename _T>
	void GetMember(const std::string& _key, _T& _value)
	{
		if (!root_.IsObject())
			return;

		if (!root_.HasMember(_key.c_str()))
			return;

		rapidjson::Value& json_value = root_[_key.c_str()];
		if (json_value.IsBool())
			_value = json_value.GetBool();
		else if (json_value.IsInt())
			_value = json_value.GetInt();
		else if (json_value.IsUint())
			_value = json_value.GetUint();
		else if (json_value.IsInt64())
			_value = json_value.GetInt64();
		else if (json_value.IsUint64())
			_value = json_value.GetUint64();
		else if (json_value.IsDouble())
			_value = json_value.GetDouble();
		else
		{
		}
	}

	// 获取字符串和嵌套json
	void GetMember(const std::string& _key, std::string& _value)
	{
		if (!root_.IsObject())
			return;

		if (!root_.HasMember(_key.c_str()))
			return;

		rapidjson::Value& json_value = root_[_key.c_str()];

		if (json_value.IsString())
		{
			_value = json_value.GetString();
		}
		else if (json_value.IsObject())
		{
			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

			rapidjson::Value res_value = json_value.GetObject();

			res_value.Accept(writer);
			_value = std::string(buffer.GetString(), buffer.GetSize()) + '\0';
		}
		else
		{
		}
	}

	// 获取列表
	void GetMember(const std::string& _key, std::vector<std::string>& _value)
	{
		if (!root_.IsObject())
			return;

		if (!root_.HasMember(_key.c_str()))
			return;

		rapidjson::Value& json_value = root_[_key.c_str()];

		if (!json_value.IsArray())
			return;

		for (int32_t i = 0; i < json_value.Size(); ++i)
		{
			rapidjson::Value& son_json_value = json_value[i];
			if (son_json_value.IsString())
			{
				_value.emplace_back(son_json_value.GetString());
			}
			else if (son_json_value.IsObject())
			{
				rapidjson::StringBuffer buffer;
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

				rapidjson::Value res_value = son_json_value.GetObject();

				res_value.Accept(writer);
				_value.emplace_back(std::string(buffer.GetString(), buffer.GetSize()) + '\0');
			}
			else
			{
			}
		}
	}

	template<typename _T>
	void GetMember(const std::string& _key, std::vector<_T>& _value)
	{
		if (!root_.IsObject())
			return;

		if (!root_.HasMember(_key.c_str()))
			return;

		rapidjson::Value& json_value = root_[_key.c_str()];
		if (!json_value.IsArray())
			return;

		for (int32_t i = 0; i < json_value.Size(); ++i)
		{
			rapidjson::Value& son_json_value = json_value[i];

			if (son_json_value.IsBool())
				_value.emplace_back(son_json_value.GetBool());
			else if (son_json_value.IsInt())
				_value.emplace_back(son_json_value.GetInt());
			else if (son_json_value.IsUint())
				_value.emplace_back(son_json_value.GetUint());
			else if (son_json_value.IsInt64())
				_value.emplace_back(son_json_value.GetInt64());
			else if (son_json_value.IsUint64())
				_value.emplace_back(son_json_value.GetUint64());
			else if (son_json_value.IsDouble())
				_value.emplace_back(son_json_value.GetDouble());
			else
			{ }
		}
	}

};

#endif // JSONSTRUCTBUFFER_H