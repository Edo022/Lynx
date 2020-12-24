#pragma once
#define LUX_H_RAARRAYC
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Tests/StructureInit.hpp"



template<class type, class iter = uint32> struct RaArrayC{
	genInitCheck;
	type* data;
	iter* lnkd;
	iter head, tail, count_;


	inline RaArrayC(){}
	void init(const iter vCount){
		luxCheckParam(vCount < 0, vCount, "Count cannot be negative");
		data = (type*)malloc(sizeof(type) * vCount);
		lnkd = (iter*)malloc(sizeof(iter) * vCount);
		tail = 0; head = vCount - 1;
		for(int i = 0; i < vCount - 1;) lnkd[i] = ++i;
		count_ = vCount;
	}



	inline iter add(const type& pElm){
		checkInit();
		iter tail_ = tail;		//Cache tail
		data[tail_] = pElm;		//Set element
		tail = lnkd[tail_];		//Update tail
		return tail_;			//Return old tail
	}

	inline void remove(const iter vIndex){
		checkInit(); luxCheckParam(vIndex < 0 || vIndex >= count(), vIndex, "Index is out of range");
		// lnkd[head] = head = vIndex;
		lnkd[head] = vIndex;
		head = vIndex;
	}

	inline type& operator[](const iter vIndex) const {
		checkInit(); luxCheckParam(vIndex < 0 || vIndex >= count(), vIndex, "Index is out of range");
		return data[vIndex];
	}

	inline iter count() const noexcept { checkInit(); return count_; }

	inline ~RaArrayC(){
		free(data);
		free(lnkd);
	}
};