#pragma once
#include "Lynx/Types/Integers/Integers.hpp"




namespace lnx::ram{
    enum class CellClass : uint32 {
		eA = 512,					//512B per cell (~0.5KB). The minimum size of a cell
		eB = eA * 2,				//2x  eA. 1024 B per cell (~1KB)
		eC = eB * 16,				//16x eB. 16384 B per cell (~16KB)
		eD = eC * 8,				//8x  eC. 131072 B per cell (~131KB)
		eQ = eD * 16,				//16x eD. 2097152 B per cell (~2.1MB)
		eL = eQ * 8,				//8x  eQ. 16777216 B per cell (~16.8MB)
		e0 = 0,						//Dedicated buffer for cells larger than CellClass::eL
		eAuto   = (uint32)-1,		//Chooses a class large enough to contain the memory block
	};


    namespace __pvt{
		#ifdef LNX_DEBUG
		enum class CellState : uint32{
			eAlloc   = 1,
			eFreed   = 2,
			eNullptr = 4,
		};
		#endif

		//Indices corresponding to lnx::CellClass values
		enum class CellClassIndex : uint16 {
			eA = 0b000,
			eB = 0b001,
			eC = 0b010,
			eD = 0b011,
			eQ = 0b100,
			eL = 0b101,
			eNum
		};
        //Size of each buffer. 100663296 B (~100MB)
		static const uint32 buffSize = (uint32)CellClass::eL * 6;
        static const uint32 incSize  = (uint32)CellClass::eL / 2;
        static const uint32 memOffset = 512;


		//Returns the CellClassIndex value of a CellClass
		static constexpr inline uint16 classIndexFromEnum(const CellClass vClass) {
			switch(vClass) {
				#define _case(n) case CellClass::e##n: return (uint16)CellClassIndex::e##n;
				case CellClass::e0: return (uint16)-1;				//Custom size class
				_case(A) _case(B) _case(C) _case(D) _case(Q) _case(L)	//Fixed size classes
				default: return (uint16)-2; 							//This is just to suppress the warning
				#undef _case
			}
		}

		static constexpr CellClass classes[] = { CellClass::eA, CellClass::eB, CellClass::eC, CellClass::eD, CellClass::eQ, CellClass::eL };
		static constexpr inline CellClass classEnumFromIndex(const CellClassIndex vIndex) { return classes[(uint64)vIndex]; }
		static constexpr inline CellClass classEnumFromIndex(const uint64         vIndex) { return classes[        vIndex]; }
    }
}