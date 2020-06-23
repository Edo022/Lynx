#pragma once
#include "LuxArray.h"

//TODO use faster memcpy
//TODO dont use initializer list
class LuxString : public LuxContainer<char> {
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
		int i = 0;
		for (; vString[i] != '\0'; ++i);
		str.resize(i + 1);
		memcpy(str.begin(), vString, i + 1);
	}
	inline LuxString(const std::initializer_list<char> vArray) {
		str.resize(vArray.size());
		memcpy(str.begin(), vArray.begin(), vArray.size());
	}



	//Inherted from LuxContainer and operators
	inline uint64 __vectorcall size() const override { return str.size(); }
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
		int i = 0;
		for (; vString[i] != '\0'; ++i);
		uint64 oldSize = str.size();
		str.resize(str.size() + i/*+1 -1*/);
		memcpy(str.begin() + oldSize - 1, vString, i + 1);
	}
	//inline void __vectorcall operator += (const std::initializer_list<char> vArray) {



	inline LuxString __vectorcall operator + (const char* vString) {
		LuxString vLuxString(str.begin());
		vLuxString += vString;
		return vLuxString;
	}
	inline LuxString __vectorcall operator + (const LuxString& pString) {
		LuxString vLuxString(str.begin());
		vLuxString += pString;
		return vLuxString;
	}
};
