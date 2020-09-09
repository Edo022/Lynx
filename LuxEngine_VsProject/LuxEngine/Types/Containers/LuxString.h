#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Memory/Ram/Memory.h"
#include "LuxEngine/macros.h"
#include "LuxEngine/Types/Nothing.h"
#include "LuxEngine/Types/Containers/LuxContainer.h"


//TODO compile with clang
//TODO add stack/heap allocation option to string constructor


//TODO check if the buffers and the str pointer do not waste memory

//TODO add a function to lux pointers that calculates the remaining allocated space after the pointer address





namespace lux {
	static inline constexpr uint32 strlenl(const char8* str) { for(uint32 len = 0; ; ++len) if(str[len] == '\0') return len; }
	static inline constexpr uint32 strlenl(const wchar8* str) { for(uint32 len = 0; ; ++len) if(str[len] == '\0') return len; }



	class String : public ContainerBase<char8, uint32>{
		ram::ptr<char8> str;

		inline void __vectorcall concatenate(const char8* vString, const uint32 size) {
			uint64 oldSize = str.cell->cellSize;
			ram::realloc(str.cell, str.cell->cellSize + size - 1);
			ram::cpy(vString, str + oldSize - 1, size);
		}



	public:
		//Constructors
		inline String(const Nothing) : str{ str } { }
		inline String( ){
			str = ram::alloc(1, CellClass::AUTO);
			str[0] = '\0';
		}
		inline String(const String& pString) {
			str = ram::alloc(pString.size( ), CellClass::AUTO);
			ram::cpy(pString.begin( ), str, pString.size( ));
		}
		inline String(const char8* vString) {
			int32 len = strlenl(vString) + 1;
			str = ram::alloc(len, CellClass::AUTO);
			ram::cpy(vString, str, len);
		}
		inline String(const wchar8* vString) {
			int32 len = strlenl(vString) + 1;
			str = ram::alloc(len, CellClass::AUTO);
			ram::cpy(vString, str, len);
		}




		//Inherted from LuxContainer and operators
		inline uint32 __vectorcall size( ) const override { return (str + str.cell->cellSize) - str; }
		//inline uint32 __vectorcall size( ) const override { return (uint32)str.cell->cellSize; }
		inline bool __vectorcall empty( ) const override { return str.cell->cellSize == 0; }
		inline char8* __vectorcall begin( ) const override { return str.address; }
		inline char8* __vectorcall end( ) const override { return str.address + str.cell->cellSize; }
		inline char8& __vectorcall operator[](const uint32 vIndex) const { return str[vIndex]; }




		//TODO add realloc for lux ptrs
		//String concatenation
		inline void __vectorcall operator += (const String& pString) { concatenate(pString.str.address, pString.size( )); }
		inline void __vectorcall operator += (const char8* vString) { concatenate(vString, strlenl(vString) + 1); }
		#pragma warning ( disable : 4996  )
		inline void __vectorcall operator += (const uint64 vValue)	{ char b[20 + 1]; _ui64toa(vValue, b, 10);	operator += (b); }
		inline void __vectorcall operator += (const int64 vValue)	{ char b[20 + 1]; _i64toa(vValue, b, 10);	operator += (b); }
		inline void __vectorcall operator += (const uint32 vValue)	{ char b[10 + 1]; ultoa(vValue, b, 10);		operator += (b); }
		inline void __vectorcall operator += (const int32 vValue)	{ char b[10 + 1]; ltoa(vValue, b, 10);		operator += (b); }
		#pragma warning ( default : 4996  )
		inline void __vectorcall operator += (const char8 vChar) { ram::realloc(str.cell, str.cell->cellSize + 1); *(str + str.cell->cellSize) = vChar; }



		//TODO improve concatenation performance
		#define __lp_strcat_body(var) String vLuxString(str.address); vLuxString += var; return vLuxString;
		inline String __vectorcall operator + (const String& pString)	const { __lp_strcat_body(pString); }
		inline String __vectorcall operator + (const char8* vString)	const { __lp_strcat_body(vString); }
		inline String __vectorcall operator + (const uint64 vValue)		const { __lp_strcat_body(vValue); }
		inline String __vectorcall operator + (const int64 vValue)		const { __lp_strcat_body(vValue); }
		inline String __vectorcall operator + (const uint32 vValue)		const { __lp_strcat_body(vValue); }
		inline String __vectorcall operator + (const int32 vValue)		const { __lp_strcat_body(vValue); }
		inline String __vectorcall operator + (const char8 vChar)		const { __lp_strcat_body(vChar); }




		//TODO automatically reassign address in realloc function
		inline void __vectorcall operator = (const String& pString) {
			ram::realloc(str.cell, pString.size( ), CellClass::AUTO);
			str.address = (char8*)str.cell->address;
			ram::cpy(pString.begin( ), str, pString.size( ));
		}
		inline void __vectorcall operator = (const char8* vString) {
			int32 len = strlenl(vString) + 1;
			ram::realloc(str.cell, len, CellClass::AUTO);
			str.address = (char8*)str.cell->address;
			ram::cpy(vString, str, len);
		}




		//Compare strings
		inline bool __vectorcall operator == (const String& vString) const {
			return ((str.cell->cellSize == vString.size( )) && (memcmp(vString.begin( ), str.address, str.cell->cellSize) == 0));
		}
		inline bool __vectorcall operator == (const char* vString) const {
			return ((str.cell->cellSize == strlenl(vString) + 1) && (memcmp(vString, str.address, str.cell->cellSize) == 0));
		}
	};
}

