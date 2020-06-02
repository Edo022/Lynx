#pragma once


template <class cntType>
class LuxContainer {
public:
	virtual inline cntType* begin() { return 0; }
	virtual inline cntType* end() { return 0; }
	virtual inline uint64 size() { return 0; }
};