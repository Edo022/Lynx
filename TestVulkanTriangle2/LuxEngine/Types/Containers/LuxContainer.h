#pragma once


template <class cntType>
class LuxContainer {
public:
	virtual inline cntType* __vectorcall begin() const { return 0; }
	virtual inline cntType* __vectorcall end() const { return 0; }
	virtual inline uint64 __vectorcall size() const { return 0; }
};