#pragma once


template <class cntType, class alloc>
class LuxContainer {
public:
	virtual inline cntType* __vectorcall begin() const { return 0; }
	virtual inline cntType* __vectorcall end() const { return 0; }
	virtual inline alloc __vectorcall size() const { return 0; }
};