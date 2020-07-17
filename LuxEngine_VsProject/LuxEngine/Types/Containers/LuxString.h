#pragma once
#include "LuxArray.h"

//TODO use faster memcpy
//TODO dont use initializer 


//32-bit integer strlen
namespace lux {
	static inline constexpr uint32 strlenl(const char8* str) {
		for (uint32 len = 0; ; ++len) if (str[len] == '\0') return len;
	}


	class String : public ContainerBase<char8, uint32> {
		Array<char8> str;

	public:
		//Constructors
		inline String() {
			str.resize(0);
		}
		inline String(const String& pString) {
			str.resize(pString.size());
			memcpy(str.begin(), pString.begin(), pString.size());
		}
		inline String(const char8* vString) {
			int32 len = strlenl(vString) + 1;
			str.resize(len);
			memcpy(str.begin(), vString, len);
		}
		inline String(const std::initializer_list<char8>& vArray) {
			str.resize((uint32)vArray.size());
			memcpy(str.begin(), vArray.begin(), vArray.size());
		}



		//Inherted from LuxContainer and operators
		inline uint32 __vectorcall size() const override { return str.size(); }
		inline bool __vectorcall empty() const override { return str.size() == 0; }
		inline char8* __vectorcall begin() const override { return str.begin(); }
		inline char8* __vectorcall end() const override { return str.end(); }
		inline char8& __vectorcall operator[](const uint32 vIndex) const { return str[vIndex]; }




		//String concatenation
		inline void __vectorcall operator += (const String& pString) {
			uint32 oldSize = str.size();
			str.resize(str.size() + pString.size() - 1);
			memcpy(str.begin() + oldSize - 1, pString.begin(), pString.size());
		}
		inline void __vectorcall operator += (const char8* vString) {
			int32 len = strlenl(vString) + 1;
			uint32 oldSize = str.size();
			str.resize(str.size() + len - 1);
			memcpy(str.begin() + oldSize - 1, vString, len);
		}


		inline String __vectorcall operator + (const String& pString) const {
			String vLuxString(str.begin());
			vLuxString += pString;
			return vLuxString;
		}
		inline String __vectorcall operator + (const char8* vString) const {
			String vLuxString(str.begin());
			vLuxString += vString;
			return vLuxString;
		}



		inline void __vectorcall operator = (const String& pString) {
			str.resize(pString.size());
			memcpy(str.begin(), pString.begin(), pString.size());
		}
		inline void __vectorcall operator = (const char8* vString) {
			int32 len = strlenl(vString) + 1;
			str.resize(len);
			memcpy(str.begin(), vString, len);
		}




		//Compare strings
		inline bool __vectorcall operator == (const String& vString) const {
			return ((str.__lp_size == vString.size()) && (memcmp(vString.begin(), str.begin(), str.__lp_size) == 0));
		}
		inline bool __vectorcall operator == (const char* vString) const {
			return ((str.__lp_size == strlenl(vString) + 1) && (memcmp(vString, str.begin(), str.__lp_size) == 0));
		}
	};
}

