#pragma once
#define LNX_H_INTEGERS





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

