#pragma once
#include "LuxArray.h"

//TODO use faster memcpy
//TODO dont use initializer list
class LuxString : public LuxContainer<char, uint32> {
	LuxArray<char> str;

public:
	//Constructors
	inline LuxString() {
		str.resize(0);
	}
	inline LuxString(const LuxString& pString) {
		str.resize(pString.size());
		memcpy(str.begin(), pString.begin(), pString.size());
	}
	inline LuxString(const char* vString) {
		int64 len = strlen(vString) + 1;
		str.resize(len);
		memcpy(str.begin(), vString, len);
	}
	inline LuxString(const std::initializer_list<char>& vArray) {
		str.resize(vArray.size());
		memcpy(str.begin(), vArray.begin(), vArray.size());
	}



	//Inherted from LuxContainer and operators
	inline uint32 __vectorcall size() const override { return str.size(); }
	inline char* __vectorcall begin() const override { return str.begin(); }
	inline char* __vectorcall end() const override { return str.end(); }
	inline char& __vectorcall operator[](const uint64 vIndex) const { return str[vIndex]; }




	//String concatenation
	inline void __vectorcall operator += (const LuxString& pString) {
		uint64 oldSize = str.size();
		str.resize(str.size() + pString.size() - 1);
		memcpy(str.begin() + oldSize - 1, pString.begin(), pString.size());
	}
	inline void __vectorcall operator += (const char* vString) {
		int64 len = strlen(vString) + 1;
		uint64 oldSize = str.size();
		str.resize(str.size() + len - 1);
		memcpy(str.begin() + oldSize - 1, vString, len);
	}


	inline LuxString __vectorcall operator + (const LuxString& pString) const {
		LuxString vLuxString(str.begin());
		vLuxString += pString;
		return vLuxString;
	}
	inline LuxString __vectorcall operator + (const char* vString) const {
		LuxString vLuxString(str.begin());
		vLuxString += vString;
		return vLuxString;
	}




	//Compare strings
	inline bool __vectorcall operator == (const LuxString& vString) const {
		return ((str.__lp_size == vString.size()) && (memcmp(vString.begin(), str.begin(), str.__lp_size) == 0));
	}
	inline bool __vectorcall operator == (const char* vString) const {
		return ((str.__lp_size == strlen(vString) + 1) && (memcmp(vString, str.begin(), str.__lp_size) == 0));
	}
};


