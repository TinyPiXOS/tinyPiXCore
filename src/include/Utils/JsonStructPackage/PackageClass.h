#ifndef	JSON_STRUCT_PACKAGE_CLASS_H
#define	JSON_STRUCT_PACKAGE_CLASS_H

#include <cstdint>
#include <string.h>
#include <type_traits>
#include <string>
#include <vector>
#include "Utils/JsonStructPackage/DataRegisterMacro.h"

// bool i8 ui8 i16 ui16 i32 ui32 i64 ui64 float double enum struct array std::sting std::vector
class JsonStructPackager
{
	JsonStructBuffer buffer;

	JsonStructPackager(const JsonStructPackager&);
	JsonStructPackager(JsonStructPackager&&);
	void operator = (const JsonStructPackager&);
	bool operator == (const JsonStructPackager&);

	// isClass,isString
	template<bool, bool, typename _Ty> struct CaseVectorMember{};

	// vector放结构体
	template<typename _Ty> struct CaseVectorMember<true, false, std::vector<_Ty>>
	{
		static void run(const char* _name, JsonStructPackager& _tool, std::vector<_Ty>& _value)
		{
			std::vector<std::string> json_arr;
			for (int32_t i = 0; i < _value.size(); ++i)
			{
				JsonStructPackager struct_package;
				JsonTypeTool<_Ty>::serlize(struct_package, _value.at(i));

				json_arr.emplace_back(struct_package.data());
			}

			_tool.buffer.appendArray(_name, json_arr);
		}
	};

	// vector放pod数据
	template<typename _Ty> struct CaseVectorMember<false, false, std::vector<_Ty>>
	{
		static void run(const char* _name, JsonStructPackager& _tool, std::vector<_Ty>& _value)
		{
			_tool.buffer.appendArray(_name, _value);
		}
	};

	// vector放string
	template<typename _Ty> struct CaseVectorMember<true, true, std::vector<_Ty>>
	{
		static void run(const char* _name, JsonStructPackager& _tool, std::vector<_Ty>& _value)
		{
			_tool.buffer.appendArray(_name, _value);
		}
	};

	//     pod? enum? struct? array? string? vector?
	template<bool, bool, bool, bool, bool, bool, typename _Ty> struct Case{};

	// base pod
	template<typename _Ty> struct Case<true, false, false, false, false, false, _Ty>
	{
		static void run(const char* _name, JsonStructPackager& _tool, _Ty& _value)
		{
			_tool.buffer.append(_name, _value);
		}
	};

	// enum
	template<typename _Ty> struct Case<true, true, false, false, false, false, _Ty>
	{
		static void run(const char* _name, JsonStructPackager& _tool, _Ty& _value)
		{
			int64_t enum_value = _value;
			_tool.buffer.append(_name, enum_value);
		}
	};

	// pod struct
	template<typename _Ty> struct Case<true, false, true, false, false, false, _Ty>
	{
		static void run(const char* _name, JsonStructPackager& _tool, _Ty& _value)
		{
			JsonStructPackager struct_package;
			JsonTypeTool<_Ty>::serlize(struct_package, _value);
			_tool.buffer.appendObject(_name, struct_package.data());

		}
	};
	// not pod struct
	template<typename _Ty> struct Case<false, false, true, false, false, false, _Ty>
	{
		static void run(const char* _name, JsonStructPackager& _tool, _Ty& _value)
		{
			JsonStructPackager struct_package;
			JsonTypeTool<_Ty>::serlize(struct_package, _value);
			_tool.buffer.appendObject(_name, struct_package.data());
		}
	};

	// pod array
	template<typename _Ty, uint32_t _Nx> struct Case<true, false, false, true, false, false, _Ty[_Nx]>
	{
		static void run(const char* _name, JsonStructPackager& _tool, _Ty _value[])
		{
			if (typeid(_Ty) == typeid(char) || typeid(_Ty) == typeid(uint8_t))
			{
				std::string char_string(_value);
				_tool.buffer.append(_name, char_string);
			}
			else
			{
				std::vector<_Ty> vector_value;
				for (uint32_t i = 0; i < _Nx; ++i)
				{
					vector_value.emplace_back(_value[i]);
				}

				CaseVectorMember <std::is_class<_Ty>::value,
					std::is_same<_Ty, std::string>::value, std::vector < _Ty >> ::run(_name, _tool, vector_value);
			}
		}
	};

	// not pod array
	template<typename _Ty, uint32_t _Nx> struct Case<false, false, false, true, false, false, _Ty[_Nx]>
	{
		static void run(const char* _name, JsonStructPackager& _tool, _Ty _value[])
		{
			std::vector<_Ty> vector_value;
			for (uint32_t i = 0; i < _Nx; ++i)
			{
				vector_value.emplace_back(_value[i]);
			}

			CaseVectorMember <std::is_class<_Ty>::value,
				std::is_same<_Ty, std::string>::value, std::vector < _Ty >> ::run(_name, _tool, vector_value);
		}
	};

	// string
	template<typename _Ty> struct Case<false, false, true, false, true, false, _Ty>
	{
		static void run(const char* _name, JsonStructPackager& _tool, std::string& _value)
		{
			_tool.buffer.append(_name, _value);
		}
	};

	// vector
	template<typename _Ty> struct Case<false, false, true, false, false, true, std::vector<_Ty> >
	{
		static void run(const char* _name, JsonStructPackager& _tool, std::vector<_Ty>& _value)
		{
			CaseVectorMember<std::is_class<_Ty>::value,
				std::is_same<_Ty, std::string>::value, std::vector<_Ty>> 
				::run(_name, _tool, _value);

		}
	};

public:
	JsonStructPackager()
	{
	}

	template<typename _Ty>
	JsonStructPackager& operator << (_Ty& _value)
	{
		JsonTypeTool<_Ty>::serlize(*this, _value);

		return *this;
	}

	std::string data() { return buffer.data(); }

	template<typename _Ty>
	JsonStructPackager& convert(const char* _name, _Ty& _value)
	{
		Case<std::is_pod<_Ty>::value,
			std::is_enum<_Ty>::value,
			std::is_class<_Ty>::value,
			std::is_array<_Ty>::value,
			std::is_same<_Ty, std::string>::value,
			false, _Ty>::run(_name, *this, _value);

		return *this;
	}

	template<typename _Ty>
	JsonStructPackager& convert(const char* _name, std::vector<_Ty>& _value)
	{
		Case<false, false, true, false, false, true, std::vector<_Ty>>::run(_name, *this, _value);

		return *this;
	}

};


class JsonStructUnpackager
{
	JsonStructUnpackager(const JsonStructUnpackager&);
	JsonStructUnpackager(JsonStructUnpackager&&);
	void operator = (const JsonStructUnpackager&);
	bool operator == (const JsonStructUnpackager&);

	// isClass,isString
	template<bool, bool, typename _Ty> struct CaseVectorMember{};

	// vector放结构体
	template<typename _Ty> struct CaseVectorMember<true, false, std::vector<_Ty>>
	{
		static void run(const char* _name, JsonStructUnpackager& _tool, std::vector<_Ty>& _value)
		{
			std::vector<std::string> member_struct_json_list;

			_tool.json_parse_.GetMember(_name, member_struct_json_list);
			for (int32_t i = 0; i < member_struct_json_list.size(); ++i)
			{
				_Ty tmp_struct;

				JsonStructUnpackager struct_unpackage(member_struct_json_list.at(i));
				JsonTypeTool<_Ty>::serlize(struct_unpackage, tmp_struct);

				_value.emplace_back(tmp_struct);
			}
		}
	};

	// vector放pod数据
	template<typename _Ty> struct CaseVectorMember<false, false, std::vector<_Ty>>
	{
		static void run(const char* _name, JsonStructUnpackager& _tool, std::vector<_Ty>& _value)
		{
			_tool.json_parse_.GetMember(_name, _value);
		}
	};

	// vector放string
	template<typename _Ty> struct CaseVectorMember<true, true, std::vector<_Ty>>
	{
		static void run(const char* _name, JsonStructUnpackager& _tool, std::vector<_Ty>& _value)
		{
			_tool.json_parse_.GetMember(_name, _value);
		}
	};

	//     pod? enum? struct? array? string? vector?
	template<bool, bool, bool, bool, bool, bool, typename _Ty> struct Case{};

	// base pod
	template<typename _Ty> struct Case<true, false, false, false, false, false, _Ty>
	{
		static void run(const char* _name, JsonStructUnpackager& _tool, _Ty& _value)
		{
			_tool.json_parse_.GetMember(_name, _value);
		}
	};

	// enum
	template<typename _Ty> struct Case<true, true, false, false, false, false, _Ty>
	{
		static void run(const char* _name, JsonStructUnpackager& _tool, _Ty& _value)
		{
            int64_t enum_value = 0;
			_tool.json_parse_.GetMember(_name, enum_value);
            _value = (_Ty)enum_value;
		}
	};

	// pod struct
	template<typename _Ty> struct Case<true, false, true, false, false, false, _Ty>
	{
		static void run(const char* _name, JsonStructUnpackager& _tool, _Ty& _value)
		{
			std::string member_json_str;
			_tool.json_parse_.GetMember(_name, member_json_str);

			JsonStructUnpackager member_unpackage(member_json_str);

			JsonTypeTool<_Ty>::serlize(member_unpackage, _value);
		}
	};

	// not pod struct
	template<typename _Ty> struct Case<false, false, true, false, false, false, _Ty>
	{
		static void run(const char* _name, JsonStructUnpackager& _tool, _Ty& _value)
		{
			std::string member_json_str;
			_tool.json_parse_.GetMember(_name, member_json_str);

			JsonStructUnpackager member_unpackage(member_json_str);

			JsonTypeTool<_Ty>::serlize(member_unpackage, _value);
		}
	};

	// pod array
	template<typename _Ty, uint32_t _Nx> struct Case<true, false, false, true, false, false, _Ty[_Nx]>
	{
		static void run(const char* _name, JsonStructUnpackager& _tool, _Ty _value[])
		{
			if (typeid(_Ty) == typeid(char) || typeid(_Ty) == typeid(uint8_t))
			{
				std::string char_string;
				_tool.json_parse_.GetMember(_name, char_string);

				uint32_t array_size = (char_string.length() < _Nx) ? char_string.length() : _Nx;
				memcpy(_value, char_string.c_str(), array_size);
			}
			else
			{
				std::vector<_Ty> vector_value;

				CaseVectorMember < std::is_class<_Ty>::value,
					std::is_same<_Ty, std::string>::value, std::vector < _Ty >>
					::run(_name, _tool, vector_value);

				for (uint32_t i = 0; i < _Nx; ++i)
				{
					if (_Nx <= i)
						break;

					_value[i] = vector_value.at(i);
				}

			}
		}
	};

	// not pod array
	template<typename _Ty, uint32_t _Nx> struct Case<false, false, false, true, false, false, _Ty[_Nx]>
	{
		static void run(const char* _name, JsonStructUnpackager& _tool, _Ty _value[])
		{
			std::vector<_Ty> vector_value;

			CaseVectorMember < std::is_class<_Ty>::value,
				std::is_same<_Ty, std::string>::value, std::vector < _Ty >>
				::run(_name, _tool, vector_value);

			for (uint32_t i = 0; i < _Nx; ++i)
			{
				if (_Nx <= i)
					break;

				_value[i] = vector_value.at(i);
			}
		}
	};

	// string
	template<typename _Ty> struct Case<false, false, true, false, true, false, _Ty>
	{
		static void run(const char* _name, JsonStructUnpackager& _tool, std::string& _value)
		{
			_tool.json_parse_.GetMember(_name, _value);
		}
	};

	// vector
	template<typename _Ty> struct Case<false, false, true, false, false, true, std::vector<_Ty> >
	{
		static void run(const char* _name, JsonStructUnpackager& _tool, std::vector<_Ty>& _value)
		{
			CaseVectorMember < std::is_class<_Ty>::value,
				std::is_same<_Ty, std::string>::value, std::vector < _Ty >>
				::run(_name, _tool, _value);
		}
	};

	JsonParser json_parse_;

public:
	JsonStructUnpackager(const std::string& _json_str)
	{
		json_parse_.SetJsonString(_json_str);
		// std::cout << "JsonString Parse Error!" << std::endl;
	}

	template<typename _Ty>
	JsonStructUnpackager& operator >> (_Ty& _value)
	{
		JsonTypeTool<_Ty>::serlize(*this, _value);

		return *this;
	}

	template<typename _Ty>
	JsonStructUnpackager& convert(const char* _name, _Ty& _value)
	{
		Case<std::is_pod<_Ty>::value,
			std::is_enum<_Ty>::value,
			std::is_class<_Ty>::value,
			std::is_array<_Ty>::value,
			std::is_same<_Ty, std::string>::value,
			false, _Ty>::run(_name, *this, _value);

		return *this;
	}

	template<typename _Ty>
	JsonStructUnpackager& convert(const char* _name, std::vector<_Ty>& _value)
	{
		Case<false, false, true, false, false, true, std::vector<_Ty>>::run(_name, *this, _value);

		return *this;
	}

};

#endif // JSON_STRUCT_PACKAGE_CLASS_H