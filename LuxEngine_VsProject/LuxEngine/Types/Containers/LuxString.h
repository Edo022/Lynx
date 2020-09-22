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
		lux_sc_generate_debug_structure_body_func_only;
		ram::ptr<char8> str;

		inline void __vectorcall concatenate(const char8* vString, const uint32 size) {
			uint64 oldSize = str.cell->cellSize;
			ram::dRealloc(str, str.cell->cellSize + size - 1);
			ram::cpy(vString, str + oldSize - 1, size);
		}


		//TODO ALLOCATE INITIALIZED THE [SIZE] BYTES BUT NOT THE WHOLE CELL.
		//TODO AND RESIZE DOES NOT INITIALIZE DATA BUT ONLY CHANGE THE SIZE VARIABLE
		//TODO THIS PROBABLY CAUSES SOME OF THE RUNTIME ERRORS
	public:
		//Constructors
		lux_sc_generate_nothing_constructor(String) str{ str } { }
		// [#] No init required
		//OK

		//String size cannot be 0. the '\0' is always present and occupies one byte
		inline String( ) :							str{ ram::AllocDB<char8>(1, CellClass::AT_LEAST_CLASS_B)   }	{ str[0] = '\0';								}
		// [#] No init required
		//OK

		inline String(const String& pString) :		str{ ram::alloc(pString.size( )) }	{ ram::cpy(pString.str, str, pString.size( ));	}
		// [#] No init required
		//OK

		//TODO add constructor that takes the size in input. IF it's already known, there is no reason to recalculate it
		inline String(const char8* vString) :		str{ ram::alloc(strlenl(vString) + 1) }		{
			//int32 len = strlenl(vString) + 1;
			//str = ram::alloc(len, CellClass::AUTO);
			//ram::cpy(vString, str, len);
			ram::cpy(vString, str, str.size( ));
		}
		// [#] No init required
		//OK

		//TODO remove
		inline String(const wchar8* vString) :		str{ ram::alloc(strlenl(vString) + 1) } {
			//int32 len = strlenl(vString) + 1;
			//str = ram::alloc(len, CellClass::AUTO);
			//ram::cpy(vString, str, len);
			ram::cpy(vString, str, str.size( ));
		}
		// [#] No init required
		//OK












		//Inherited from LuxContainer and operators
		inline uint32 __vectorcall	size( )							const override { checkInit; return (uint32)str.size( );		}
		// [#] Structure is uninitialized  | k | print error
		//OK

		inline uint64 __vectorcall bytes( )							const override { checkInit; return str.size( ) /* *sizeof(char8)*/; }
		// [#] Structure is uninitialized  | k | print error
		//OK

		inline bool __vectorcall	empty( )						const override { checkInit; return str.size( ) == 0;		}
		// [#] Structure is uninitialized  | k | print error
		//OK

		inline char8* __vectorcall	begin( )						const override { checkInit; return str.begin( );			}
		// [#] Structure is uninitialized  | k | print error
		//OK

		inline char8* __vectorcall	end( )							const override { checkInit; return str.end( );				}
		// [#] Structure is uninitialized  | k | print error
		//OK

		inline char8& __vectorcall	operator[](const uint32 vIndex) const		   {
			checkInit; param_error_2(vIndex < 0, vIndex, "Index cannot be negative"); param_error_2(vIndex > size(), vIndex, "Index is out of range");
			return str[vIndex];
		}
		// [#] Structure is uninitialized  | k | print error
		// [#] Size is 0                   | k | ok. If it's 0, '\0' will be returned
		// [#] vIndex is negative          | k | print error
		// [#] vIndex is otu of range      | k | print error
		//OK









		//String concatenation
		#pragma warning ( disable : 4996  )
		inline void __vectorcall operator += (const String& pString)	{ checkInit; concatenate(pString.str.address, pString.size( ));				}
		// [#] Structure is uninitialized  | k | print error
		//OK

		inline void __vectorcall operator += (const char8* vString)		{ checkInit; concatenate(vString, strlenl(vString) + 1);					}
		// [#] Structure is uninitialized  | k | print error
		//OK

		inline void __vectorcall operator += (const uint64 vValue)		{ checkInit; char b[20 + 1]; _ui64toa(vValue, b, 10);	operator += (b);	}
		// [#] Structure is uninitialized  | k | print error
		//OK

		inline void __vectorcall operator += (const int64 vValue)		{ checkInit; char b[20 + 1]; _i64toa(vValue, b, 10);		operator += (b);}
		// [#] Structure is uninitialized  | k | print error
		//OK

		inline void __vectorcall operator += (const uint32 vValue)		{ checkInit; char b[10 + 1]; ultoa(vValue, b, 10);		operator += (b);	}
		// [#] Structure is uninitialized  | k | print error
		//OK

		inline void __vectorcall operator += (const int32 vValue)		{ checkInit; char b[10 + 1]; ltoa(vValue, b, 10);		operator += (b);	}
		// [#] Structure is uninitialized  | k | print error
		//OK

		inline void __vectorcall operator += (const char8 vChar)		{ checkInit; ram::realloc(str, str.size( ) + 1); *str.end( ) = vChar;		}
		// [#] Structure is uninitialized  | k | print error
		//OK

		#pragma warning ( default : 4996  )








		//TODO improve concatenation performance
		#define __lp_strcat_body(var) String vLuxString(str.address); vLuxString += var; return vLuxString;
		inline String __vectorcall operator + (const String& pString)	const { checkInit; __lp_strcat_body(pString);	}
		// [#] Structure is uninitialized  | k | print error
		//OK

		inline String __vectorcall operator + (const char8* vString)	const { checkInit; __lp_strcat_body(vString);	}
		// [#] Structure is uninitialized  | k | print error
		//OK

		inline String __vectorcall operator + (const uint64 vValue)		const { checkInit; __lp_strcat_body(vValue);	}
		// [#] Structure is uninitialized  | k | print error
		//OK

		inline String __vectorcall operator + (const int64 vValue)		const { checkInit; __lp_strcat_body(vValue);	}
		// [#] Structure is uninitialized  | k | print error
		//OK

		inline String __vectorcall operator + (const uint32 vValue)		const { checkInit; __lp_strcat_body(vValue);	}
		// [#] Structure is uninitialized  | k | print error
		//OK

		inline String __vectorcall operator + (const int32 vValue)		const { checkInit; __lp_strcat_body(vValue);	}
		// [#] Structure is uninitialized  | k | print error
		//OK

		inline String __vectorcall operator + (const char8 vChar)		const { checkInit; __lp_strcat_body(vChar);		}
		// [#] Structure is uninitialized  | k | print error
		//OK












		//Assignment
		inline void __vectorcall operator = (const String& pString) {
			checkInit; isInit(pString);
			ram::dRealloc(str, pString.size( ), CellClass::AUTO);
			//ram::realloc(str, pString.size( ), CellClass::AUTO);
			str.address = (char8*)str.cell->address;
			ram::cpy(pString.str, str, pString.size( ));
		}
		// [#] Structure is uninitialized  | k | print error
		// [#] pString is uninitialized    | k | print error
		//OK

		inline void __vectorcall operator = (const char8* vString) {
			checkInit;
			//int32 len = strlenl(vString) + 1;
			ram::dRealloc(str, strlenl(vString) + 1, CellClass::AUTO);
			//ram::realloc(str, len, CellClass::AUTO);
			str.address = (char8*)str.cell->address;
			//ram::cpy(vString, str, len);
			ram::cpy(vString, str, str.size( ));
		}
		// [#] Structure is uninitialized  | k | print error
		//OK





		//Comparison
		inline bool __vectorcall operator == (const String& pString) const {
			checkInit; isInit(pString);
			return ((str.size( ) == pString.size( )) && (memcmp(pString.str, str.address, str.size( )) == 0));
		}
		// [#] Structure is uninitialized  | k | print error
		// [#] pString is uninitialized    | k | print error
		//OK

		inline bool __vectorcall operator == (const char* vString) const {
			checkInit;
			return ((str.size( ) == strlenl(vString) + 1) && (memcmp(vString, str.address, str.size( )) == 0));
		}
		// [#] Structure is uninitialized  | k | print error
		//OK

	};
}

