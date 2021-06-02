#pragma once
#define LNX_H_INTEGERS
#include <type_traits>




namespace lnxc{
    typedef char      int8,  i8;        //Signed 8-bit integer
    typedef short     int16, i16;	    //Signed 16-bit integer
    typedef int       int32, i32;       //Signed 32-bit integer
    typedef long long int64, i64;		//Signed 64-bit integer
    //using int128_t int128;            //
    //using int256_t int256;            //
    //using int512_t int512;            //

    typedef unsigned char  uint8,  u8;	//Unsigned 8-bit integer. Same as char8, but specific to integer types
    typedef unsigned short uint16, u16;	//Unsigned 16-bit integer
    typedef unsigned int   uint32, u32;	//Unsigned 32-bit integer
    typedef unsigned long  uint64, u64;	//Unsigned 64-bit integer
    //using uint128 = uint128_t;        //
    //using uint256 = uint256_t;        //
    //using int512  = uint512_t;        //




    typedef char     char8,   c8;       //Normal 8 bit character
    typedef char8_t  wchar8,  w8;       //UTF8  character (normal unsigned char)
    typedef char16_t wchar16, w16;      //UTF16 character
    typedef char32_t wchar32, w32;      //UTF32 character

    typedef float  float32, f32;        //Single precision floating point
    typedef double float64, f64;        //Double precision floating point
    //using float128;                   //
    //using float256;                   //
    //using float512;                   //





    //has_conversion_operator helper struct
    template<bool c, class op> struct __has_conversion_operator_t {
        template<class type> static consteval std::true_type get(int32, decltype(type().operator op())* = 0){
            return std::true_type();
        }
        template<class type> static consteval std::false_type get(auto) {
            return std::false_type();
        }
    };
    //has_conversion_operator helper struct
    template<class op> struct __has_conversion_operator_t<false, op> {
        template<class type> static consteval std::false_type get(auto) {
            return std::false_type();
        }
    };

    /**
     * @brief Provides std::true_type if the type has a conversion operator of type op or an alias of it, std::false_type otherwise
     *    Trivial types always provide std::false_type
     *
     *    e.g.
     *    struct s{
     *        int n;
     *        operator int(){ return n; }
     *    };
     *
     *    has_conversion_operator<int, int>;    //false_type
     *    has_conversion_operator<s, int>;      //true_type
     *    has_conversion_operator<s, float>;    //false_type
     * @tparam type The type to test
     * @tparam op The type of the operator
     */
    template<class type, class op> using has_conversion_operator = decltype(__has_conversion_operator_t<std::is_class_v<type>, op>::template get<type>(0));
    template<class type, class op> static constexpr bool has_conversion_operator_v = has_conversion_operator<type, op>::value;
}
#ifndef LNX_NO_GLOBAL_NAMESPACE
	using namespace lnxc;
#endif