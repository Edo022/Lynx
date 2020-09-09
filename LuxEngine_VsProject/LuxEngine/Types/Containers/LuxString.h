#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Memory/Ram/Memory.h"
#include "LuxEngine/macros.h"
#include "LuxEngine/Types/Nothing.h"
#include "LuxEngine/Types/Containers/LuxContainer.h"

//TODO use faster memcpy
//TODO dont use initializer

//TODO compile with clang

//TODO add stack/heap allocation option to string constructor


//32-bit integer strlen
namespace lux {
	static inline constexpr uint32 strlenl(const char8* str) { for(uint32 len = 0; ; ++len) if(str[len] == '\0') return len; }
	static inline constexpr uint32 strlenl(const wchar8* str) { for(uint32 len = 0; ; ++len) if(str[len] == '\0') return len; }



	class String : public ContainerBase<char8, uint32>{
		ram::ptr<char8> str;
	public:
		//Constructors
		inline String(const Nothing) : str{ str } {
			int TODO_delete_this = 0;
		}
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
		inline String(const std::initializer_list<char8>& vArray) {
			str = ram::alloc((uint32)vArray.size( ), CellClass::AUTO);
			ram::cpy(vArray.begin( ), str, vArray.size( ));
		}



		//Inherted from LuxContainer and operators
		inline uint32 __vectorcall size( ) const override { return (uint32)str.cell->cellSize; }
		inline bool __vectorcall empty( ) const override { return str.cell->cellSize == 0; }
		inline char8* __vectorcall begin( ) const override { return str.address; }
		inline char8* __vectorcall end( ) const override { return str.address + str.cell->cellSize; }
		inline char8& __vectorcall operator[](const uint32 vIndex) const { return str[vIndex]; }




		//String concatenation
		inline void __vectorcall operator += (const String& pString) {
			uint32 oldSize = (uint32)str.cell->cellSize;
			ram::realloc(str.cell, str.cell->cellSize + pString.size( ) - 1);
			ram::cpy(pString.str, str + oldSize - 1, pString.size( ));
		}
		inline void __vectorcall operator += (const char8* vString) {
			int32 len = strlenl(vString) + 1;
			uint32 oldSize = (uint32)str.cell->cellSize;
			ram::realloc(str.cell, str.cell->cellSize + len - 1);
			ram::cpy(vString, str + oldSize - 1, len);
		}


		inline String __vectorcall operator + (const String& pString) const {
			String vLuxString(str.address);
			vLuxString += pString;
			return vLuxString;
		}
		inline String __vectorcall operator + (const char8* vString) const {
			String vLuxString(str.address);
			vLuxString += vString;
			return vLuxString;
		}
		inline String __vectorcall operator + (const uint32 vValue) const {
			String vLuxString(str.address);
			char b[20];
			#pragma warning ( disable : 4996  )
			vLuxString += _itoa(vValue, b, 10);
			#pragma warning ( default : 4996  )
			return vLuxString;
		}


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

