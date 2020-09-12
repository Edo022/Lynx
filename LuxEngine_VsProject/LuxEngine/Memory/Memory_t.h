#ifndef LUX_H_MEMORY_T
#  define LUX_H_MEMORY_T


#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxMap_NMP_S.h"
#include "LuxEngine/Core/ConsoleOutput.h"
#include "LuxEngine/Types/Nothing.h"






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



//TODO print warning if something is called on an uninitialized structure


	//luxDebug(void __lp_printWarning(const char* text));
	namespace ram{
		struct Cell_t;
		struct MemBuffer {
			void* memory = nullptr;			//Address of the buffer
			Map_NMP_S<Cell_t, uint32> cells;		//Cells in the buffer
			uint32 bufferIndex;				//Index of the buffer in the MemBufferType buffers array
		};
		struct MemBufferType {
			CellClass cellClass;			//Class of the cells
			Map_NMP_S<MemBuffer, uint32> buffers;	//Buffers containing the cells
		};
		struct Cell_t {
			uint32 owners;					//Number of lux::ram::ptr instances that point to an address of the cell
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




		//TODO use only ptrs

		//Like a normal pointer, but better
		//Allocate memory to the pointer with lux::ram::alloc
		//This pointer will automatically be freed once it is not longer used by any thread
		//You can also manually free it with the lux::mem:free function
		//    Accessing the memory of a freed pointer will result in undefined behaviour
		template<class type> struct ptr{
			Cell_t* cell;			//A pointer to a lux::ram::Cell_t object that contains the cell informations
			type* address;			//The address the pointer points to



			inline ptr( ) : cell{ nullptr }, address{ nullptr } { }
			inline ptr(Nothing) : cell{ cell }, address{ address }{ }
			inline ptr(Cell vCell) : cell{ vCell }, address{ (type*)vCell->address } { cell->owners++; }
			inline ptr(Cell vCell, type* vAddress) : cell{ vCell }, address{ vAddress }{ cell->owners++; }

			//TODO check for self nullptrs in any struct with Nothing constructors
			//TODO or just print an error idk
			//inline ptr(ptr<type>& pPtr) :
			//	cell{ (pPtr) ? pPtr.cell : nullptr }, address{ (pPtr) ? pPtr.address : nullptr } {
			//	if(cell) cell->owners++;
			//}
			inline ptr(ptr<type>& pPtr) :
				cell{ pPtr.cell }, address{ pPtr.address } {
				cell->owners++;
			}
			template<class pType> explicit inline ptr(ptr<pType>& pPtr) : cell{ pPtr.cell }, address{ (type*)pPtr.address }{ cell->owners++; }

			inline void operator=(const Cell vCell){
				//Decrease the owner count if the pointer was bound to a cell
				if(cell) {
					cell->owners--;
				}
					cell = vCell;
					address = (type*)vCell->address;
					cell->owners++;
				//else printError("cell is uninitialized");
			}
			inline void operator=(const ptr<type>& pPtr){ if(cell) cell->owners--; cell = pPtr.cell; address = pPtr.address; cell->owners++; }

			template<class pType> inline bool operator==(const ptr<pType>& pPtr) const {
				luxDebug(printWarning("Comparison operator should not be used with different pointer types"));
				return (void*)address == (void*)pPtr.address;
			}
			//TODO print warning if using a raw pointer
			//TODO or just use sparse pointers idk
			inline bool operator==(const type* vPtr) const { return (void*)address == (void*)vPtr; }




			//TODO print warning if using a raw pointer //??
			inline ptr<type> operator+(const uint64 v) const { return ptr<type>{cell, address + v}; }
			inline uint64 operator+(const type* vPtr) const { return (uint64)address + (uint64)vPtr; }
			inline uint64 operator+(const ptr<type>& vPtr) const { return (uint64)address + (uint64)vPtr.address; }
			inline ptr<type> operator-(const uint64 v) const { return ptr<type>{cell, address - v}; }
			inline uint64 operator-(const type* vPtr) const { return (uint64)address - (uint64)vPtr; }
			inline uint64 operator-(const ptr<type>& vPtr) const { return (uint64)address - (uint64)vPtr.address; }


			#define checkp luxDebug(if((uint64)address >= ((uint64)cell->address) + cell->cellSize) printWarning("A lux::ram::ptr has probably been increased too much and now points to an unallocated address. Reading or writing to this address is undefined behaviour and can cause runtime errors"))
			#define checkm luxDebug(if((uint64)address < (uint64)cell->address)                     printWarning("A lux::ram::ptr has probably been decreased too much and now points to an unallocated address. Reading or writing to this address is undefined behaviour and can cause runtime errors"))
			inline void operator+=(uint64 v){ address += v; checkp; }
			inline void operator-=(uint64 v){ address -= v; checkm; }
			inline void operator++( ){ address++; checkp;}
			inline void operator--( ){ address--; checkm;}

			//TODO improve warnings and output object address or nanme
			inline type& operator*( ){
				luxDebug(if(!cell) printWarning("Unable to access the memory of an uninitialized lux::ram::ptr. A pointer should always be initialized with the lux::ram::alloc function or using one of its constructors"));
				luxDebug(if(!address) printWarning("Unable to access the memory of a null lux::ram::ptr"));
				return *address;
			}




			~ptr( ){ if(address) { if(!--cell->owners) cell->freeCell( ); } }
			inline operator type*( ) const;
			inline type& operator [](const uint64 i) const { return address[i]; }
			inline type& operator [](const uint32 i) const {
				return address[i];
			}
			inline type& operator [](const int i) const { return address[i]; }
		};

		template<class type> ptr<type>::operator type*( ) const {return address;}
	}
}
#endif