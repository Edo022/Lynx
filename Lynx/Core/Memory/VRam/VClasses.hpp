#pragma once
#define LNX_H_VCLASSES
#include "Lynx/Types/Integers/Integers.hpp"




namespace lnx::vram{
	enum class VCellClass : uint32 {
		eA = 512,			//512B per cell (~0.5KB). The minimum size of a cell
		eB = eA * 2,			//2x  eA. 1024 B per cell (~1KB)
		eC = eB * 16,			//16x eB. 16384 B per cell (~16KB)
		eD = eC * 8,			//8x  eC. 131072 B per cell (~131KB)
		eQ = eD * 16,			//16x eD. 2097152 B per cell (~2.1MB)
		eL = eQ * 8,			//8x  eQ. 16777216 B per cell (~16.8MB)
		e0 = 0,					//Dedicated buffer for cells larger than CellClass::eL
		eAuto = (uint32)-1,		//Chooses a class large enough to contain the memory block
	};




	namespace __pvt{
		enum class VCellClassIndex : uint16 {
			eA = 0b000,
			eB = 0b001,
			eC = 0b010,
			eD = 0b011,
			eQ = 0b100,
			eL = 0b101,
			eNum
		};
		static const uint32 buffSize = (uint32)VCellClass::eL * 6;	//Size of each buffer. 100663296 B (~100MB)
        static const uint32 incSize  = (uint32)VCellClass::eL / 2;	//Custom size buffers size step in bytes
        static const uint32 memOffset = 512;						//TODO find out what memOffset actually is and rename it




		/**
		 * @brief Returns the corresponding VCellClassIndex of a CellClass value	\n
		 *     This function should only be used by the engine						\n
		 * Complexity: O(1)
		 * @param vClass The VCellClass value
		 * @return The index corresponding to the class
		 */
		static constexpr inline uint16 classIndexFromEnum(const VCellClass vClass) {
			switch(vClass) {
				//Custom size class
				case VCellClass::e0: return (uint16)-1;

				//Fixed size classes
				case VCellClass::eA: return (uint16)VCellClassIndex::eA;
				case VCellClass::eB: return (uint16)VCellClassIndex::eB;
				case VCellClass::eC: return (uint16)VCellClassIndex::eC;
				case VCellClass::eD: return (uint16)VCellClassIndex::eD;
				case VCellClass::eQ: return (uint16)VCellClassIndex::eQ;
				case VCellClass::eL: return (uint16)VCellClassIndex::eL;

				//This is just to suppress the warning
				default: return (uint16)-2;
			}
		}

		//Returns the CellClass value of a CellClassIndex
		static constexpr VCellClass classes[] = { VCellClass::eA, VCellClass::eB, VCellClass::eC, VCellClass::eD, VCellClass::eQ, VCellClass::eL };
		/**
		 * @brief Returns the corresponding VCellClass value of a VCellClassIndex	\n
		 *     This function should only be used by the engine						\n
		 * Complexity: O(1)
		 * @param vClass The class index
		 * @return The VCellClass value corresponding to the index
		 */
		static constexpr inline VCellClass classEnumFromIndex(const uint64 vIndex) {
			return classes[ vIndex];
		}
    }
}