#pragma once
#define LUX_H_STRING
#include "LuxEngine/Types/Containers/ContainerBase.hpp"








namespace lux {
	//FIXME REMOVE. USE NORMAL STRLEN
	static inline constexpr uint32 strlenl(const char8* str ) { for(uint32 len = 0; ; ++len) if(str[len] == '\0') return len; }
	static inline constexpr uint32 strlenl(const wchar8* str) { for(uint32 len = 0; ; ++len) if(str[len] == '\0') return len; }




	//TODO add constructor that takes the count in input. IF it's already known, there is no reason to recalculate it
	//TODO improve concatenation performance
	class String : public ContainerBase<char8, uint32>{
		#define Super ContainerBase<char8, uint32>
	private:
		genInitCheck;
		// ram::Alloc<char8> str;

		inline void cat(const char8* vString, const uint32 size) {
			uint64 oldSize = Super::data.size();
			Super::data.realloc(Super::data.size() + size - 1);
			ram::cpy(vString, Super::data + oldSize - 1, size);
		}

	public:




		// Constructors -------------------------------------------------------------------------------------------------------- //




		//String count cannot be 0. the '\0' is always present and occupies one byte
		//TODO the elements are probably uninitialized
		inline String(                                  ) : Super(1) { Super::data[0] = '\0'; }
		inline String(const String& pString             ) : Super(pString.count( ))	    { ram::cpy(pString.data, Super::data, pString.count( )); }
		inline String(const char8* vString              ) : Super(strlenl(vString) + 1) { ram::cpy(vString,      Super::data, Super::data.size( ));      }
		inline String(const char8* vString, uint64 vSize) : Super(vSize)	            { ram::cpy(vString,      Super::data, Super::data.size( ));      }

		//TODO remove
		// inline String(const wchar8* vString) : Super(strlenl(vString) + 1)	{ ram::cpy(vString, Super::data, Super::data.size( )); }




		// Get ----------------------------------------------------------------------------------------------------------------- //




		// inline uint32 count( ) const override { checkInit(); return (uint32)str.count( ); }
		//TODO move size to specific type
		inline uint64 size(  ) const { checkInit(); return Super::data.size( ); }
		// inline bool	  empty( ) const override { checkInit(); return !str.count( );        }
		// inline char8* begin( ) const override { checkInit(); return str.begin( );	        }
		// inline char8* end(   ) const override { checkInit(); return str.end( );           }

		inline char8& operator[](const uint32 vIndex) const {
			checkInit();
			luxCheckParam(vIndex < 0,       vIndex, "Index cannot be negative");
			luxCheckParam(vIndex > count(), vIndex, "Index is out of range");
			return Super::operator[](vIndex);
		}




		// String concatenation ------------------------------------------------------------------------------------------------ //




		#pragma warning ( disable : 4996  )
		inline void operator += (const String& pString) { checkInit(); cat(pString.Super::data, pString.count( )); }
		inline void operator += (const char8* vString ) { checkInit(); cat(vString, strlenl(vString) + 1); }

		//TODO write itoa functions
		// inline void operator += (const uint64 vValue)		{ checkInit(); char b[20 + 1]; _ui64toa(vValue, b, 10);	operator += (b);	}
		// inline void operator += (const int64 vValue)		{ checkInit(); char b[20 + 1]; _i64toa(vValue, b, 10);	operator += (b);	}
		// inline void operator += (const uint32 vValue)		{ checkInit(); char b[10 + 1]; ultoa(vValue, b, 10);		operator += (b);	}
		// inline void operator += (const int32 vValue)		{ checkInit(); char b[10 + 1]; ltoa(vValue, b, 10);		operator += (b);	}
		//inline void operator += (const char8 vChar)		{ checkInit(); ram::reallocBck(str, str.size( ) + 1); *str.end( ) = vChar;		}
		inline void operator += (const char8 vChar)		{ checkInit(); Super::data.realloc(Super::data.count( ) + 1); *Super::data.end( ) = vChar;		}


		#define __lp_strcat_body(var) String vLuxString(Super::data); vLuxString += var; return vLuxString;
		inline String operator + (const String& pString) const {
			checkInit();
			//TODO use sum chain struct instead of copying the string data
			__lp_strcat_body(pString);
		}
		inline String operator + (const char8* vString ) const {
			checkInit();
			//TODO use sum chain struct instead of copying the string data
			__lp_strcat_body(vString);
		}
		// inline String operator + (const uint64 vValue  ) const { checkInit(); __lp_strcat_body(vValue ); }
		// inline String operator + (const int64 vValue   ) const { checkInit(); __lp_strcat_body(vValue ); }
		// inline String operator + (const uint32 vValue  ) const { checkInit(); __lp_strcat_body(vValue ); }
		// inline String operator + (const int32 vValue   ) const { checkInit(); __lp_strcat_body(vValue ); }
		// inline String operator + (const char8 vChar    ) const { checkInit(); __lp_strcat_body(vChar  ); }




		// Assignment and comparison ------------------------------------------------------------------------------------------- //



		//copy assignment
		inline void operator = (const String& pString) {
			checkInit(); isInit(pString);
			Super::data.realloc(pString.count( ));
			// str.address = (char8*)str.cell->address;
			ram::cpy(pString.data, Super::data, Super::data.count( ));
			//FIXME dont copy old data
			//FIXME add parameter to realloc that prevents old data to be copied
		}


		//Copy from C-style string
		inline void operator = (const char8* vString) {
			checkInit();
			Super::data.realloc(strlenl(vString) + 1);
			// str.address = (char8*)str.cell->address;
			ram::cpy(vString, Super::data, Super::data.count( ));
			//FIXME dont copy old data
			//FIXME add parameter to realloc that prevents old data to be copied
		}




		inline bool operator == (const String& pString) const {
			checkInit(); isInit(pString);
			return ((Super::data.count( ) == pString.count( )) && (memcmp(pString.data, Super::data, Super::data.count( )) == 0));
		}


		inline bool operator == (const char* vString) const {
			checkInit();
			return ((Super::data.count( ) == strlenl(vString) + 1) && (memcmp(vString, Super::data, Super::data.count( )) == 0));
		}
		#undef Super
	};
}

