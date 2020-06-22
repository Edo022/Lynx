#pragma once
#include "LuxArray.h"


//class LuxString : public LuxContainer<char> {
//	LuxArray<char> str;
//
//public:
//	inline uint64 __vectorcall size() const override {return }
//	inline char* __vectorcall begin() const override {return str.}
//	inline char* __vectorcall end() const override { return str.end(); }
//};



class LuxString : public LuxArray<char> {
private:

public:
	inline uint64 __vectorcall resize(const uint64 vNewSize) = delete;
	inline uint64 __vectorcall resize(const uint64 vNewSize, const type vInitValue) = delete;


	inline char* __vectorcall cStr() const { return begin(); }
};