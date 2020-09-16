#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Memory/Ram/Memory.h"
#include "LuxEngine/macros.h"
#include "LuxEngine/Types/Containers/LuxContainer.h"
#include "LuxEngine/Types/Nothing.h"
#include "LuxEngine/Types/Nothing_sc_p.h"



//TODO compile with clang
//TODO add stack/heap allocation option to string constructor


//TODO check if the buffers and the str pointer do not waste memory

//TODO add a function to lux pointers that calculates the remaining allocated space after the pointer address





namespace lux {
	static inline constexpr uint32 strlenl(const char8* str) { for(uint32 len = 0; ; ++len) if(str[len] == '\0') return len; }
	static inline constexpr uint32 strlenl(const wchar8* str) { for(uint32 len = 0; ; ++len) if(str[len] == '\0') return len; }



	class String : public ContainerBase<char8, uint32>{
		lux_sc_generate_debug_structure_body;
		ram::ptr<char8> str;

		inline void __vectorcall concatenate(const char8* vString, const uint32 size) {
			uint64 oldSize = str.cell->cellSize;
			ram::realloc(str, str.cell->cellSize + size - 1);
			ram::cpy(vString, str + oldSize - 1, size);
		}



	public:
		//Constructors
		lux_sc_generate_nothing_constructor(String) str{ str } { }
		inline String( ){
			str = ram::alloc(1, CellClass::AUTO);
			str[0] = '\0';
		}
		inline String(const String& pString) {
			str = ram::alloc(pString.size( ), CellClass::AUTO);
			ram::cpy(pString.str, str, pString.size( ));
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




		//Inherited from LuxContainer and operators
		inline uint32 __vectorcall	size( )							const override { lux_sc_F; return (uint32)str.size( );		}
		inline uint64 __vectorcall bytes( )							const override { lux_sc_F; return str.size( ) /* *sizeof(char8)*/; }
		inline bool __vectorcall	empty( )						const override { lux_sc_F; return str.size( ) == 0;			}
		inline char8* __vectorcall	begin( )						const override { lux_sc_F; return str.begin( );				}
		inline char8* __vectorcall	end( )							const override { lux_sc_F; return str.end( );				}
		inline char8& __vectorcall	operator[](const uint32 vIndex) const		   { lux_sc_F; return str[vIndex];				}




		//String concatenation
		#pragma warning ( disable : 4996  )
		inline void __vectorcall operator += (const String& pString)	{ lux_sc_F; concatenate(pString.str.address, pString.size( ));				}
		inline void __vectorcall operator += (const char8* vString)		{ lux_sc_F; concatenate(vString, strlenl(vString) + 1);						}
		inline void __vectorcall operator += (const uint64 vValue)		{ lux_sc_F; char b[20 + 1]; _ui64toa(vValue, b, 10);	operator += (b);	}
		inline void __vectorcall operator += (const int64 vValue)		{ lux_sc_F; char b[20 + 1]; _i64toa(vValue, b, 10);		operator += (b);	}
		inline void __vectorcall operator += (const uint32 vValue)		{ lux_sc_F; char b[10 + 1]; ultoa(vValue, b, 10);		operator += (b);	}
		inline void __vectorcall operator += (const int32 vValue)		{ lux_sc_F; char b[10 + 1]; ltoa(vValue, b, 10);		operator += (b);	}
		inline void __vectorcall operator += (const char8 vChar)		{ lux_sc_F; ram::realloc(str, str.size( ) + 1); *str.end( ) = vChar;		}
		#pragma warning ( default : 4996  )



		//TODO improve concatenation performance
		#define __lp_strcat_body(var) String vLuxString(str.address); vLuxString += var; return vLuxString;
		inline String __vectorcall operator + (const String& pString)	const { lux_sc_F; __lp_strcat_body(pString);	}
		inline String __vectorcall operator + (const char8* vString)	const { lux_sc_F; __lp_strcat_body(vString);	}
		inline String __vectorcall operator + (const uint64 vValue)		const { lux_sc_F; __lp_strcat_body(vValue);		}
		inline String __vectorcall operator + (const int64 vValue)		const { lux_sc_F; __lp_strcat_body(vValue);		}
		inline String __vectorcall operator + (const uint32 vValue)		const { lux_sc_F; __lp_strcat_body(vValue);		}
		inline String __vectorcall operator + (const int32 vValue)		const { lux_sc_F; __lp_strcat_body(vValue);		}
		inline String __vectorcall operator + (const char8 vChar)		const { lux_sc_F; __lp_strcat_body(vChar);		}




		//Assignment
		inline void __vectorcall operator = (const String& pString) {
			lux_sc_F;
			ram::realloc(str, pString.size( ), CellClass::AUTO);
			str.address = (char8*)str.cell->address;
			ram::cpy(pString.str, str, pString.size( ));
		}
		inline void __vectorcall operator = (const char8* vString) {
			lux_sc_F;
			int32 len = strlenl(vString) + 1;
			ram::realloc(str, len, CellClass::AUTO);
			str.address = (char8*)str.cell->address;
			ram::cpy(vString, str, len);
		}




		//Comparison
		inline bool __vectorcall operator == (const String& vString) const {
			lux_sc_F;
			return ((str.size( ) == vString.size( )) && (memcmp(vString.str, str.address, str.size( )) == 0));
		}
		inline bool __vectorcall operator == (const char* vString) const {
			lux_sc_F;
			return ((str.size( ) == strlenl(vString) + 1) && (memcmp(vString, str.address, str.size( )) == 0));
		}
	};
}

