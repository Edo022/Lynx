#pragma once
////#define LNX_H_RAARRAYC
#include "Lynx/Types/Integers/Integers.hpp"
#include "Lynx/Tests/StructureInit.hpp"
//FIXME

namespace lnx{
	template<class tType, class tIdxt = uint32> struct RaArrayC{
		genInitCheck;

		struct Elm{
			tType value;
			tIdxt next;
		};
		Elm* data;
		tIdxt head, tail, count_;


		inline RaArrayC() {};


		//Move constructor
		inline RaArrayC(RaArrayC&& rArray) :
			data{ rArray.data },
			head{ rArray.head }, tail{ rArray.tail }, count_{ rArray.count_ } {
			rArray.data = nullptr;
		}
		inline RaArrayC(RaArrayC& rArray) = delete;


		void init(const tIdxt vCount) {
			dbg::assertParam(vCount >= 0, "vCount", "Count cannot be negative");
			data = (Elm*)malloc(sizeof(Elm) * vCount);
			tail = 0; head = vCount - 1;
			for(tIdxt i = 0; i < vCount - 1; ++i) data[i].next = i + 1;
			count_ = vCount;
		}




		//Move assignment
		inline void operator=(RaArrayC&& rArray) {
			data = rArray.data;
			head = rArray.head; tail = rArray.tail; count_ = rArray.count_;
			rArray.data = nullptr;
		}
		inline void operator=(RaArrayC& rArray) = delete;




		inline tIdxt add(const tType& pElm) {
			checkInit();
			tIdxt tail_ = tail;			//Cache tail
			data[tail_].value = pElm;	//Set element
			tail = data[tail_].next;	//Update tail
			return tail_;				//Return old tail
		}

		inline void remove(const tIdxt vIndex) {
			checkInit(); dbg::assertIndex(vIndex, 0, count() - 1, "vIndex");
			data[head].next = vIndex;
			head = vIndex;
		}

		inline tType& operator[](const tIdxt vIndex) const {
			checkInit(); dbg::assertIndex(vIndex, 0, count() - 1, "vIndex");
			return data[vIndex].value;
		}

		inline tIdxt count() const { checkInit(); return count_; }

		inline ~RaArrayC() {
			if(data) free(data);
		}
	};
}