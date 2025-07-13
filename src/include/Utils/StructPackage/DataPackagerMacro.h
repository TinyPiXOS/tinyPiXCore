#ifndef __DATA_PACKAGER_MACRO_HPP
#define __DATA_PACKAGER_MACRO_HPP

#if ! defined NI_SERIALIZATION_EXPAND
#define NI_SERIALIZATION_EXPAND
#define NI_EXPAND(args) args

#define NISERIALIZATION_0(c, p) convert(#p, c.p)
#define NISERIALIZATION_1(c, p) NISERIALIZATION_0(c, p)
#define NISERIALIZATION_2(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_1(c, __VA_ARGS__))
#define NISERIALIZATION_3(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_2(c, __VA_ARGS__))
#define NISERIALIZATION_4(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_3(c, __VA_ARGS__))
#define NISERIALIZATION_5(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_4(c, __VA_ARGS__))
#define NISERIALIZATION_6(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_5(c, __VA_ARGS__))
#define NISERIALIZATION_7(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_6(c, __VA_ARGS__))
#define NISERIALIZATION_8(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_7(c, __VA_ARGS__))
#define NISERIALIZATION_9(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_8(c, __VA_ARGS__))
#define NISERIALIZATION_10(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_9(c, __VA_ARGS__))
#define NISERIALIZATION_11(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_10(c, __VA_ARGS__))
#define NISERIALIZATION_12(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_11(c, __VA_ARGS__))
#define NISERIALIZATION_13(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_12(c, __VA_ARGS__))
#define NISERIALIZATION_14(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_13(c, __VA_ARGS__))
#define NISERIALIZATION_15(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_14(c, __VA_ARGS__))
#define NISERIALIZATION_16(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_15(c, __VA_ARGS__))
#define NISERIALIZATION_17(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_16(c, __VA_ARGS__))
#define NISERIALIZATION_18(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_17(c, __VA_ARGS__))
#define NISERIALIZATION_19(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_18(c, __VA_ARGS__))
#define NISERIALIZATION_20(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_19(c, __VA_ARGS__))
#define NISERIALIZATION_21(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_20(c, __VA_ARGS__))
#define NISERIALIZATION_22(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_21(c, __VA_ARGS__))
#define NISERIALIZATION_23(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_22(c, __VA_ARGS__))
#define NISERIALIZATION_24(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_23(c, __VA_ARGS__))
#define NISERIALIZATION_25(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_24(c, __VA_ARGS__))
#define NISERIALIZATION_26(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_25(c, __VA_ARGS__))
#define NISERIALIZATION_27(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_26(c, __VA_ARGS__))
#define NISERIALIZATION_28(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_27(c, __VA_ARGS__))
#define NISERIALIZATION_29(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_28(c, __VA_ARGS__))
#define NISERIALIZATION_30(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_29(c, __VA_ARGS__))
#define NISERIALIZATION_31(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_30(c, __VA_ARGS__))
#define NISERIALIZATION_32(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_31(c, __VA_ARGS__))
#define NISERIALIZATION_33(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_32(c, __VA_ARGS__))
#define NISERIALIZATION_34(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_33(c, __VA_ARGS__))
#define NISERIALIZATION_35(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_34(c, __VA_ARGS__))
#define NISERIALIZATION_36(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_35(c, __VA_ARGS__))
#define NISERIALIZATION_37(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_36(c, __VA_ARGS__))
#define NISERIALIZATION_38(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_37(c, __VA_ARGS__))
#define NISERIALIZATION_39(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_38(c, __VA_ARGS__))
#define NISERIALIZATION_40(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_39(c, __VA_ARGS__))
#define NISERIALIZATION_41(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_40(c, __VA_ARGS__))
#define NISERIALIZATION_42(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_41(c, __VA_ARGS__))
#define NISERIALIZATION_43(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_42(c, __VA_ARGS__))
#define NISERIALIZATION_44(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_43(c, __VA_ARGS__))
#define NISERIALIZATION_45(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_44(c, __VA_ARGS__))
#define NISERIALIZATION_46(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_45(c, __VA_ARGS__))
#define NISERIALIZATION_47(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_46(c, __VA_ARGS__))
#define NISERIALIZATION_48(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_47(c, __VA_ARGS__))
#define NISERIALIZATION_49(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_48(c, __VA_ARGS__))
#define NISERIALIZATION_50(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_49(c, __VA_ARGS__))
#define NISERIALIZATION_51(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_50(c, __VA_ARGS__))
#define NISERIALIZATION_52(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_51(c, __VA_ARGS__))
#define NISERIALIZATION_53(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_52(c, __VA_ARGS__))
#define NISERIALIZATION_54(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_53(c, __VA_ARGS__))
#define NISERIALIZATION_55(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_54(c, __VA_ARGS__))
#define NISERIALIZATION_56(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_55(c, __VA_ARGS__))
#define NISERIALIZATION_57(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_56(c, __VA_ARGS__))
#define NISERIALIZATION_58(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_57(c, __VA_ARGS__))
#define NISERIALIZATION_59(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_58(c, __VA_ARGS__))
#define NISERIALIZATION_60(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_59(c, __VA_ARGS__))
#define NISERIALIZATION_61(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_60(c, __VA_ARGS__))
#define NISERIALIZATION_62(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_61(c, __VA_ARGS__))
#define NISERIALIZATION_63(c, p, ...) NISERIALIZATION_0(c, p) . NI_EXPAND(NISERIALIZATION_62(c, __VA_ARGS__))


#define NI_MAKE_CALL(t, f) t . f

#define NI_PARAMS_COUNT(TAG, _63,_62,_61,_60,_59,_58,_57,_56,_55,_54,_53,_52,_51,\
                        _50,_49,_48,_47,_46,_45,_44,_43,_42,_41,\
                        _40,_39,_38,_37,_36,_35,_34,_33,_32,_31,\
                        _30,_29,_28,_27,_26,_25,_24,_23,_22,_21,\
                        _20,_19,_18,_17,_16,_15,_14,_13,_12,_11,\
                        _10,_9,_8,_7,_6,_5,_4,_3,_2,_1,N,...) TAG##N


#define NISERIALIZATION(t, c, ...) NI_MAKE_CALL(t, NI_EXPAND(NI_PARAMS_COUNT(NISERIALIZATION, __VA_ARGS__, \
                        _63,_62,_61,_60,_59,_58,_57,_56,_55,_54,_53,_52,_51,\
                        _50,_49,_48,_47,_46,_45,_44,_43,_42,_41,\
                        _40,_39,_38,_37,_36,_35,_34,_33,_32,_31,\
                        _30,_29,_28,_27,_26,_25,_24,_23,_22,_21,\
                        _20,_19,_18,_17,_16,_15,_14,_13,_12,_11,\
                        _10,_9,_8,_7,_6,_5,_4,_3,_2,_1))(c, __VA_ARGS__))


#endif 

#include <memory>
#include <cstdint>

class StructXBuffer{
    char*         m_data;
    char*         m_end;
    std::size_t m_cap;
public:
    StructXBuffer(){
        m_cap  = 512;
        m_data = new char[m_cap];
        m_end = m_data;
    }
    ~StructXBuffer(){
        m_cap  = 0;
        delete[] m_data;
        m_data = nullptr;
        m_end = nullptr;
    }

    char* data() const { return m_data; }
    std::size_t size() const { return std::size_t(m_end - m_data); }

    void append(const void* _data, std::size_t _size){
        std::size_t left_space = m_cap - size();
        if(left_space < _size){//空间不足
            for(;;){
                m_cap *= 2;
                left_space = m_cap - size();
                if(left_space > _size) break;
            }

            char* new_data = new char[m_cap];
            std::size_t old_size = size();
            ::memcpy(new_data, m_data, old_size);
            delete[] m_data;
            m_data = new_data;
            m_end = m_data + old_size;
        }
        ::memcpy(m_end, _data, _size);
        m_end += _size;
    }
};

template<typename _Ty> struct PStructTool{};
#define STRUCT_PACKAGE(structType, ...) \
    template<> struct PStructTool<structType>{ template<typename _Ty> static void serlize(_Ty& t, structType& data) \
        {NISERIALIZATION(t, data, __VA_ARGS__);}};   \
    typedef std::shared_ptr<structType> structType##SPtr;

#endif
