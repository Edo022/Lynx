#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Memory/Ram/Memory.h"
#include "LuxEngine/macros.h"
#include "LuxEngine/Types/Containers/LuxContainer.h"
#include "LuxEngine/Types/Nothing.h"
#include "LuxEngine/Types/Nothing_sc_p.h"






namespace lux {
	static inline constexpr uint32 strlenl(const char8* str) { for(uint32 len = 0; ; ++len) if(str[len] == '\0') return len; }
	static inline constexpr uint32 strlenl(const wchar8* str) { for(uint32 len = 0; ; ++len) if(str[len] == '\0') return len; }




	//TODO add constructor that takes the size in input. IF it's already known, there is no reason to recalculate it
	//TODO improve concatenation performance
	class String : public ContainerBase<char8, uint32>{
	private:
		lux_sc_generate_debug_structure_body_func_only;
		ram::ptr<char8> str;

		inline void __vectorcall concatenate(const char8* vString, const uint32 size) {
			uint64 oldSize = str.cell->cellSize;
			ram::reallocDB(str, str.cell->cellSize + size - 1);
			ram::cpy(vString, str + oldSize - 1, size);
		}

	public:




		// Constructors -------------------------------------------------------------------------------------------------------- //




		lux_sc_generate_nothing_constructor(String) str{ str } { }

		//String size cannot be 0. the '\0' is always present and occupies one byte
		inline String( ) : str{ ram::AllocBck<char8>(1, CellClass::AT_LEAST_CLASS_B) }	{ str[0] = '\0'; }
		inline String(const String& pString) : str{ ram::allocBck(pString.size( )) }		{ ram::cpy(pString.str, str, pString.size( )); }
		inline String(const char8* vString) : str{ ram::allocBck(strlenl(vString) + 1) }	{ ram::cpy(vString, str, str.size( )); }
		inline String(const char8* vString, uint64 vSize) : str{ ram::allocBck(vSize) }	{ ram::cpy(vString, str, str.size( )); }

		//TODO remove
		inline String(const wchar8* vString) : str{ ram::allocBck(strlenl(vString) + 1) }	{ ram::cpy(vString, str, str.size( )); }




		// Get ----------------------------------------------------------------------------------------------------------------- //




		inline uint32	__vectorcall size( )	const override { checkInit; return (uint32)str.size( );	}
		inline uint64	__vectorcall bytes( )	const override { checkInit; return str.size( );			}
		inline bool		__vectorcall empty( )	const override { checkInit; return !str.size( );		}
		inline char8*	__vectorcall begin( )	const override { checkInit; return str.begin( );		}
		inline char8*	__vectorcall end( )		const override { checkInit; return str.end( );			}

		inline char8& __vectorcall	operator[](const uint32 vIndex) const		   {
			checkInit; param_error_2(vIndex < 0, vIndex, "Index cannot be negative"); param_error_2(vIndex > size(), vIndex, "Index is out of range");
			return str[vIndex];
		}




		// String concatenation ------------------------------------------------------------------------------------------------ //




		#pragma warning ( disable : 4996  )
		inline void __vectorcall operator += (const String& pString)	{ checkInit; concatenate(pString.str.address, pString.size( ));				}
		inline void __vectorcall operator += (const char8* vString)		{ checkInit; concatenate(vString, strlenl(vString) + 1);					}
		inline void __vectorcall operator += (const uint64 vValue)		{ checkInit; char b[20 + 1]; _ui64toa(vValue, b, 10);	operator += (b);	}
		inline void __vectorcall operator += (const int64 vValue)		{ checkInit; char b[20 + 1]; _i64toa(vValue, b, 10);	operator += (b);	}
		inline void __vectorcall operator += (const uint32 vValue)		{ checkInit; char b[10 + 1]; ultoa(vValue, b, 10);		operator += (b);	}
		inline void __vectorcall operator += (const int32 vValue)		{ checkInit; char b[10 + 1]; ltoa(vValue, b, 10);		operator += (b);	}
		inline void __vectorcall operator += (const char8 vChar)		{ checkInit; ram::reallocUB(str, str.size( ) + 1); *str.end( ) = vChar;		}
		#pragma warning ( default : 4996  )

		#define __lp_strcat_body(var) String vLuxString(str.address); vLuxString += var; return vLuxString;
		inline String __vectorcall operator + (const String& pString)	const { checkInit; __lp_strcat_body(pString);	}
		inline String __vectorcall operator + (const char8* vString)	const { checkInit; __lp_strcat_body(vString);	}
		inline String __vectorcall operator + (const uint64 vValue)		const { checkInit; __lp_strcat_body(vValue);	}
		inline String __vectorcall operator + (const int64 vValue)		const { checkInit; __lp_strcat_body(vValue);	}
		inline String __vectorcall operator + (const uint32 vValue)		const { checkInit; __lp_strcat_body(vValue);	}
		inline String __vectorcall operator + (const int32 vValue)		const { checkInit; __lp_strcat_body(vValue);	}
		inline String __vectorcall operator + (const char8 vChar)		const { checkInit; __lp_strcat_body(vChar);		}




		// Assignment and comparison ------------------------------------------------------------------------------------------- //




		inline void __vectorcall operator = (const String& pString) {
			checkInit; isInit(pString);
			ram::reallocDB(str, pString.size( ), CellClass::AUTO);
			str.address = (char8*)str.cell->address;
			ram::cpy(pString.str, str, pString.size( ));
		}


		inline void __vectorcall operator = (const char8* vString) {
			checkInit;
			ram::reallocDB(str, strlenl(vString) + 1, CellClass::AUTO);
			str.address = (char8*)str.cell->address;
			ram::cpy(vString, str, str.size( ));
		}




		inline bool __vectorcall operator == (const String& pString) const {
			checkInit; isInit(pString);
			return ((str.size( ) == pString.size( )) && (memcmp(pString.str, str.address, str.size( )) == 0));
		}


		inline bool __vectorcall operator == (const char* vString) const {
			checkInit;
			return ((str.size( ) == strlenl(vString) + 1) && (memcmp(vString, str.address, str.size( )) == 0));
		}
	};
}

