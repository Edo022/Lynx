#pragma once
#define LUX_H_RAARRAYC
#include "Lynx/Types/Integers/Integers.hpp"
#include "Lynx/Tests/StructureInit.hpp"


namespace lux{
	template<class type, class iter = uint32> struct RaArrayC{
		genInitCheck;

		struct Elm{
			type value;
			iter next;
		};
		Elm* data;
		iter head, tail, count_;


		inline RaArrayC() {};


		//Move constructor
		inline RaArrayC(RaArrayC&& rArray) :
			data{ rArray.data },
			head{ rArray.head }, tail{ rArray.tail }, count_{ rArray.count_ } {
			rArray.data = nullptr;
		}
		inline RaArrayC(RaArrayC& rArray) = delete;


		void init(const iter vCount) {
			dbg::checkParam(vCount < 0, "vCount", "Count cannot be negative");
			data = (Elm*)malloc(sizeof(Elm) * vCount);
			tail = 0; head = vCount - 1;
			for(iter i = 0; i < vCount - 1; ++i) data[i].next = i + 1;
			count_ = vCount;
		}




		//Move assignment
		inline void operator=(RaArrayC&& rArray) {
			data = rArray.data;
			head = rArray.head; tail = rArray.tail; count_ = rArray.count_;
			rArray.data = nullptr;
		}
		inline void operator=(RaArrayC& rArray) = delete;




		inline iter add(const type& pElm) {
			checkInit();
			iter tail_ = tail;			//Cache tail
			data[tail_].value = pElm;	//Set element
			tail = data[tail_].next;	//Update tail
			return tail_;				//Return old tail
		}

		inline void remove(const iter vIndex) {
			checkInit(); dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			data[head].next = vIndex;
			head = vIndex;
		}

		inline type& operator[](const iter vIndex) const {
			checkInit(); dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			return data[vIndex].value;
		}

		inline iter count() const { checkInit(); return count_; }

		inline ~RaArrayC() {
			if(data) free(data);
		}
	};
}