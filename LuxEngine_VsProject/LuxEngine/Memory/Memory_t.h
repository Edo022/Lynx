#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxMap.h"







namespace lux{
	//Bytes to allocate for each cell
	//Buffer classes and addresses are 32-byte aligned to allow the use of AVX2 and match the GPU minimum offsets
	enum class CellClass : uint32 {
		CLASS_A = 32,				//32 bytes. The minimum size of a cell
		CLASS_B = CLASS_A * 16,		//16x CLASS_A. 512 B per cell (~0.5KB)
		CLASS_C = CLASS_A * 32,		//32x CLASS_A. 1024 B per cell (~1KB)
		CLASS_D = CLASS_A * 64,		//64x CLASS_A. 2048 B per cell (~2KB)
		CLASS_Q = CLASS_D * 64,		//64x CLASS_D. 131072 B per cell (~130KB)
		CLASS_L = CLASS_Q * 64,		//64x CLASS_Q. 8388608 B per cell (~8.4MB)
		CLASS_0 = 0,				//Dedicated buffer for cells larger than CellClass::CLASS_L
		AUTO = (uint32)-1,			//Choose a class large enough to contain the cell
	};
	enum class CellClassIndex : uint32 {
		INDEX_A = 0b000,
		INDEX_B = 0b001,
		INDEX_C = 0b010,
		INDEX_D = 0b011,
		INDEX_Q = 0b100,
		INDEX_L = 0b101,
		INDEX_0 = 0b110,
		NUM 						//Number of LUX_CELL_CLASS values
	};
	enum class AllocType : uint32 {
		DEDICATED_STORAGE = 0b00,	//Storage buffer in dedicated GPU memory
		DEDICATED_UNIFORM = 0b01,	//Uniform buffer in dedicated GPU memory
		SHARED_STORAGE =/**/0b10,	//Storage buffer in shared RAM memory
		SHARED_UNIFORM =/**/0b11,	//Uniform buffer in shared RAM memory
		NUM							//Number of LUX_ALLOC_TYPE values
	};
	static const uint32 bufferSize = (uint32)CellClass::CLASS_L * 6;	//Size of each buffer. 50331648 B (~50MB)
	#define _case(n) case CellClass::CLASS_##n: return (uint32)CellClassIndex::INDEX_##n;
	static constexpr inline uint32 classIndexFromEnum(const CellClass vClass){ switch(vClass){ _case(A) _case(B) _case(C) _case(D) _case(Q) _case(L) _case(0) default: return (uint32)-1; } };
	#define _case2(n) case CellClassIndex::INDEX_##n: return CellClass::CLASS_##n;
	static constexpr inline CellClass classEnumFromIndex(const CellClassIndex vIndex){ switch(vIndex){ _case2(A) _case2(B) _case2(C) _case2(D) _case2(Q) _case2(L) _case2(0) default: return (CellClass)-1; } };




	luxDebug(void __lp_printWarning(const char* text));
	namespace ram{
		struct Cell_t;
		struct MemBuffer {
			void* memory;					//Address of the buffer
			Map<Cell_t, uint32> cells;		//Cells in the buffer
			uint32 bufferIndex;				//Index of the buffer in the MemBufferType buffers array
		};
		struct MemBufferType {
			CellClass cellClass;			//Class of the cells
			Map<MemBuffer, uint32> buffers;	//Buffers containing the cells
		};
		struct Cell_t {
			uint32 owners;					//Number of lux::mem::ptr instances that point to an address of the cell
			uint64 cellSize;				//Size of the cell in bytes
			void* address;					//Address of the cell. The same as you would get with malloc
			MemBufferType* bufferType;		//Type of buffer allocation
			MemBuffer* buffer;				//Index of the buffer where the cell is allocated
			uint32 cellIndex;				//Index of the cell in the buffer
			void freeCell();
		};
		//TODO remove alias
		typedef Cell_t* Cell;
		static inline uint32 getCellOffset(const Cell pCell){ return (uint32)pCell->bufferType->cellClass * pCell->cellIndex; }




		//TODO use only ptr s

		//Like a normal pointer, but better
		//Allocate the pointer with lux::mem::alloc
		//This pointer will automatically be freed once it is not longer used by any thread
		//You can also manually free it with the lux::mem:free function
		//    Accessing the memory of a freed pointer will result in undefined behaviour
		template<class type> struct ptr{
			//TODO use dedicated address for pointer instances
			Cell_t* cell{ nullptr };
			type* address{ nullptr };
			luxDebug(bool initialized = false);
			#define checkInit if(!initialized __lp_printWarning("Unable to access the memory of an uninitialized pointer")
			//TODO warning when address is out of cell range


			inline void init( ){ cell->owners++; }
			inline ptr( ){ cell = nullptr; }
			inline ptr(Cell vCell) : cell{ vCell }, address{ (type*)vCell->address } { init( ); }
			inline ptr(Cell vCell, type* vAddress) : cell{ vCell }, address{ vAddress }{ init( ); }
			inline ptr(ptr<type>& pPtr) : cell{ pPtr.cell }, address{ pPtr.address }{ init( ); }
			template<class pType> explicit inline ptr(ptr<pType>& pPtr) : cell{ pPtr.cell }, address{ (type*)pPtr.address }{ init( ); }

			//TODO print warning if using a raw pointer
			inline void operator=(Cell vCell){ cell = vCell; init( ); }
			inline void operator=(ptr<type>& pPtr){ cell = pPtr.cell; init( ); }
			//inline void operator=(type* vPtr){ cell->address = (void*)vPtr; lux::core:: }

			template<class pType> inline bool operator==(ptr<pType>& pPtr){
				luxDebug(__lp_printWarning("Comparison operator should not be used with different pointer types"));
				return (void*)address == (void*)pPtr.address;
			}
			inline bool operator==(type* vPtr){ return (void*)address == (void*)vPtr; }




			//TODO print warning if using a raw pointer //??
			inline ptr<type> operator+(uint64 v){ return ptr<type>{cell, address + v}; }
			inline uint64 operator+(type* vPtr){ return (uint64)address + (uint64)vPtr; }
			inline ptr<type> operator-(uint64 v){ return ptr<type>{cell, address - v}; }
			inline uint64 operator-(type* vPtr){ return (uint64)address - (uint64)vPtr; }

			inline void operator+=(uint64 v){ address += v; }
			inline void operator-=(uint64 v){ address -= v; }
			inline void operator++( ){ address++; }
			inline void operator--( ){ address--; }

			inline type& operator*( ){ return *address; }




			~ptr( ){ if((--(cell->owners)) == 0) cell->freeCell( ); }
		};
	}
}