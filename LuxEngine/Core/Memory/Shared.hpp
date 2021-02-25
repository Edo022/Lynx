#pragma once
#define LUX_H_CELL_T_SHARED
#include "LuxEngine/Debug/Debug.hpp"
#include "LuxEngine/Types/Integers/Integers.hpp"



namespace lux{
	//Bytes allocated to each cell
	//Buffer classes and addresses are 32-byte aligned to allow the use of AVX2 and match the GPU minimum offsets //FIXME align on 32 bytes
	enum class CellClass : uint32 {
		CLASS_A = 512,					//512B per cell (~0.5KB). The minimum size of a cell
		CLASS_B = CLASS_A * 2,			//2x  CLASS_A. 1024 B per cell (~1KB)
		CLASS_C = CLASS_B * 16,			//16x CLASS_B. 16384 B per cell (~16KB)
		CLASS_D = CLASS_C * 8,			//8x  CLASS_C. 131072 B per cell (~131KB)
		CLASS_Q = CLASS_D * 16,			//16x CLASS_D. 2097152 B per cell (~2.1MB)
		CLASS_L = CLASS_Q * 8,			//8x  CLASS_Q. 16777216 B per cell (~16.8MB)
		CLASS_0 = 0,					//Dedicated buffer for cells larger than CellClass::CLASS_L
		AUTO = (uint32)-1,				//Chooses a class large enough to contain the memory block

		// AT_LEAST_CLASS_B = 1 + CLASS_B,	//Like AUTO, but the class is guaranteed to be at least CLASS_B
		// AT_LEAST_CLASS_C = 1 + CLASS_C,	//Like AUTO, but the class is guaranteed to be at least CLASS_C
		// AT_LEAST_CLASS_D = 1 + CLASS_D,	//Like AUTO, but the class is guaranteed to be at least CLASS_D
		// AT_LEAST_CLASS_Q = 1 + CLASS_Q,	//Like AUTO, but the class is guaranteed to be at least CLASS_Q
		// AT_LEAST_CLASS_L = 1 + CLASS_L,	//Like AUTO, but the class is guaranteed to be at least CLASS_L
	}; //TODO add options to change this from the GUI
	enum class VCellClass : uint32 {
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
		enum class CellClassIndex : uint32 { //TODO DIVIDE GRAPHICS
			INDEX_A = 0b000,
			INDEX_B = 0b001,
			INDEX_C = 0b010,
			INDEX_D = 0b011,
			INDEX_Q = 0b100,
			INDEX_L = 0b101,
			NUM 						//Number of LUX_CELL_CLASS values
		};
		//Size of each buffer. 100663296 B (~100MB)
		static const uint32 bufferSize = (uint32)CellClass::CLASS_L * 6;


		//Returns the CellClassIndex value of a CellClass
		static constexpr inline uint16 classIndexFromEnum(const CellClass vClass) {
			switch(vClass) {
				#define _case(n) case CellClass::CLASS_##n: return (uint32)CellClassIndex::INDEX_##n;
				_case(A) _case(B) _case(C) _case(D) _case(Q) _case(L)	//Fixed size classes
				case CellClass::CLASS_0: return (uint16)-1;				//Custom size class
				default: return -2; //This is just to suppress the warning
			}
		}
		//Returns the CellClass value of a CellClassIndex
		static constexpr CellClass classes[] = { CellClass::CLASS_A, CellClass::CLASS_B, CellClass::CLASS_C, CellClass::CLASS_D, CellClass::CLASS_Q, CellClass::CLASS_L };
		static constexpr inline CellClass classEnumFromIndex(const CellClassIndex vIndex) { return classes[(uint64)vIndex]; }
		static constexpr inline CellClass classEnumFromIndex(const uint64         vIndex) { return classes[        vIndex]; }









//FIXME REMOVE

		//DEPRECATED VERSION OF classIndexFromEnum
		static constexpr inline uint32 classIndexFromEnum__old(const CellClass vClass) {
			switch(vClass) {
				#define _case(n) case CellClass::CLASS_##n: return (uint32)CellClassIndex::INDEX_##n;
				_case(A) _case(B) _case(C) _case(D) _case(Q) _case(L) //_case(0)
				default:
					if((uint64)vClass == 0) return 0b110; //old class 0 index
					else { dbg::printError("Invalid cell class %d", (uint32)vClass); return (uint32)-1;}
			}
		}
		//DEPRECATED VERSION OF classEnumFromIndex
		static constexpr CellClass classes__old[] = { CellClass::CLASS_A, CellClass::CLASS_B, CellClass::CLASS_C, CellClass::CLASS_D, CellClass::CLASS_Q, CellClass::CLASS_L, CellClass::CLASS_0 };
		//DEPRECATED VERSION OF classEnumFromIndex
		static constexpr inline CellClass classEnumFromIndex__old(const CellClassIndex vIndex) { return classes__old[(uint64)vIndex]; }
		//DEPRECATED VERSION OF classEnumFromIndex
		static constexpr inline CellClass classEnumFromIndex__old(const uint64         vIndex) { return classes__old[        vIndex]; }
	}
}