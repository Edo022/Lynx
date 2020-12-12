#pragma once
#define LUX_H_STRING
#include "LuxEngine/Types/Containers/ContainerBase.hpp"








namespace lux {
	static inline constexpr uint32 strlenl(const char8* str ) { for(uint32 len = 0; ; ++len) if(str[len] == '\0') return len; }
	static inline constexpr uint32 strlenl(const wchar8* str) { for(uint32 len = 0; ; ++len) if(str[len] == '\0') return len; }




	//TODO add constructor that takes the count in input. IF it's already known, there is no reason to recalculate it
	//TODO improve concatenation performance
	class String : public ContainerBase<char8, uint32>{
	private:
		genInitCheck;
		ram::ptr<char8> str;

		inline void concatenate(const char8* vString, const uint32 size) {
			uint64 oldSize = str.cell->cellSize;
			str.realloc(str.cell->cellSize + size - 1);
			ram::cpy(vString, str + oldSize - 1, size);
		}

	public:




		// Constructors -------------------------------------------------------------------------------------------------------- //




		//String count cannot be 0. the '\0' is always present and occupies one byte
		//TODO the elements are probably uninitialized
		inline String(                                  ) : str(1, char8(), CellClass::AT_LEAST_CLASS_B)	{ str[0] = '\0'; }
		inline String(const String& pString             ) : str(pString.count( ))	  { ram::cpy(pString.str, str, pString.count( )); }
		inline String(const char8* vString              ) : str(strlenl(vString) + 1) { ram::cpy(vString,     str, str.size( ));      }
		inline String(const char8* vString, uint64 vSize) : str(vSize)	              { ram::cpy(vString,     str, str.size( ));      }

		//TODO remove
		inline String(const wchar8* vString) : str(strlenl(vString) + 1)	{ ram::cpy(vString, str, str.size( )); }




		// Get ----------------------------------------------------------------------------------------------------------------- //




		inline uint32 count( ) const override { checkInit(); return (uint32)str.count( ); }
		inline uint64 size(  ) const override { checkInit(); return str.count( );         }
		inline bool	  empty( ) const override { checkInit(); return !str.count( );        }
		inline char8* begin( ) const override { checkInit(); return str.begin( );	        }
		inline char8* end(   ) const override { checkInit(); return str.end( );           }

		inline char8&	operator[](const uint32 vIndex) const {
			checkInit(); luxCheckParam(vIndex < 0, vIndex, "Index cannot be negative"); luxCheckParam(vIndex > count(), vIndex, "Index is out of range");
			return str[vIndex];
		}




		// String concatenation ------------------------------------------------------------------------------------------------ //




		#pragma warning ( disable : 4996  )
		inline void operator += (const String& pString)	{ checkInit(); concatenate(pString.str.address, pString.count( ));			}
		inline void operator += (const char8* vString)		{ checkInit(); concatenate(vString, strlenl(vString) + 1);					}
		//TODO write itoa functions
		// inline void operator += (const uint64 vValue)		{ checkInit(); char b[20 + 1]; _ui64toa(vValue, b, 10);	operator += (b);	}
		// inline void operator += (const int64 vValue)		{ checkInit(); char b[20 + 1]; _i64toa(vValue, b, 10);	operator += (b);	}
		// inline void operator += (const uint32 vValue)		{ checkInit(); char b[10 + 1]; ultoa(vValue, b, 10);		operator += (b);	}
		// inline void operator += (const int32 vValue)		{ checkInit(); char b[10 + 1]; ltoa(vValue, b, 10);		operator += (b);	}
		//inline void operator += (const char8 vChar)		{ checkInit(); ram::reallocBck(str, str.size( ) + 1); *str.end( ) = vChar;		}
		inline void operator += (const char8 vChar)		{ checkInit(); str.realloc(str.count( ) + 1); *str.end( ) = vChar;		}
		#pragma warning ( default : 4996  )

		#define __lp_strcat_body(var) String vLuxString(str.address); vLuxString += var; return vLuxString;
		inline String operator + (const String& pString) const { checkInit(); __lp_strcat_body(pString); }
		inline String operator + (const char8* vString ) const { checkInit(); __lp_strcat_body(vString); }
		inline String operator + (const uint64 vValue  ) const { checkInit(); __lp_strcat_body(vValue ); }
		inline String operator + (const int64 vValue   ) const { checkInit(); __lp_strcat_body(vValue ); }
		inline String operator + (const uint32 vValue  ) const { checkInit(); __lp_strcat_body(vValue ); }
		inline String operator + (const int32 vValue   ) const { checkInit(); __lp_strcat_body(vValue ); }
		inline String operator + (const char8 vChar    ) const { checkInit(); __lp_strcat_body(vChar  ); }




		// Assignment and comparison ------------------------------------------------------------------------------------------- //




		inline void operator = (const String& pString) {
			checkInit(); isInit(pString);
			str.realloc(pString.count( ));
			str.address = (char8*)str.cell->address;
			ram::cpy(pString.str, str, pString.count( ));
		}


		inline void operator = (const char8* vString) {
			checkInit();
			str.realloc(strlenl(vString) + 1);
			str.address = (char8*)str.cell->address;
			ram::cpy(vString, str, str.count( ));
		}




		inline bool operator == (const String& pString) const {
			checkInit(); isInit(pString);
			return ((str.count( ) == pString.count( )) && (memcmp(pString.str, str.address, str.count( )) == 0));
		}


		inline bool operator == (const char* vString) const {
			checkInit();
			return ((str.count( ) == strlenl(vString) + 1) && (memcmp(vString, str.address, str.count( )) == 0));
		}
	};
}

