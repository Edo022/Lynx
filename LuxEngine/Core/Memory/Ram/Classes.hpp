#pragma once
#include "LuxEngine/Types/Integers/Integers.hpp"




namespace lux::ram{
    	enum class CellClass : uint32 {
		CLASS_A = 512,					//512B per cell (~0.5KB). The minimum size of a cell
		CLASS_B = CLASS_A * 2,			//2x  CLASS_A. 1024 B per cell (~1KB)
		CLASS_C = CLASS_B * 16,			//16x CLASS_B. 16384 B per cell (~16KB)
		CLASS_D = CLASS_C * 8,			//8x  CLASS_C. 131072 B per cell (~131KB)
		CLASS_Q = CLASS_D * 16,			//16x CLASS_D. 2097152 B per cell (~2.1MB)
		CLASS_L = CLASS_Q * 8,			//8x  CLASS_Q. 16777216 B per cell (~16.8MB)
		CLASS_0 = 0,					//Dedicated buffer for cells larger than CellClass::CLASS_L
		AUTO = (uint32)-1,				//Chooses a class large enough to contain the memory block
	};


    namespace __pvt{
		#ifdef LUX_DEBUG
		enum class CellState : uint32{
			ALLOC      = 1,
			FREED      = 2,
			NULLPTR    = 4,
		};
		#endif

		//Indices corresponding to lux::CellClass values
		enum class CellClassIndex : uint16 {
			INDEX_A = 0b000,
			INDEX_B = 0b001,
			INDEX_C = 0b010,
			INDEX_D = 0b011,
			INDEX_Q = 0b100,
			INDEX_L = 0b101,
			NUM
		};
        //Size of each buffer. 100663296 B (~100MB)
		static const uint32 buffSize = (uint32)CellClass::CLASS_L * 6;
        static const uint32 incSize  = (uint32)CellClass::CLASS_L / 2;
        static const uint32 memOffset = 512;


		//Returns the CellClassIndex value of a CellClass
		static constexpr inline uint16 classIndexFromEnum(const CellClass vClass) {
			switch(vClass) {
				#define _case(n) case CellClass::CLASS_##n: return (uint16)CellClassIndex::INDEX_##n;
				case CellClass::CLASS_0: return (uint16)-1;				//Custom size class
				_case(A) _case(B) _case(C) _case(D) _case(Q) _case(L)	//Fixed size classes
				default: return -2; 									//This is just to suppress the warning
				#undef _case
			}
		}

		static constexpr CellClass classes[] = { CellClass::CLASS_A, CellClass::CLASS_B, CellClass::CLASS_C, CellClass::CLASS_D, CellClass::CLASS_Q, CellClass::CLASS_L };
		static constexpr inline CellClass classEnumFromIndex(const CellClassIndex vIndex) { return classes[(uint64)vIndex]; }
		static constexpr inline CellClass classEnumFromIndex(const uint64         vIndex) { return classes[        vIndex]; }
    }
}