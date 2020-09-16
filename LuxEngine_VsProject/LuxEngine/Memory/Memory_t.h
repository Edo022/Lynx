#ifndef LUX_H_MEMORY_T
#  define LUX_H_MEMORY_T


#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxMap_NMP_S.h"
#include "LuxEngine/Core/ConsoleOutput.h"
#include "LuxEngine/Core/ConsoleOutput.h"
#include "LuxEngine/Types/Nothing.h"
#include "LuxEngine/Types/Nothing_sc_p.h"







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
	static constexpr inline CellClass classEnumFromIndex(const CellClassIndex vIndex){
		switch(vIndex){ _case2(A) _case2(B) _case2(C) _case2(D) _case2(Q) _case2(L) _case2(0) default: return (CellClass)-1; }
	};








//TODO print warning if something is called on an uninitialized structure
	//luxDebug(void __lp_printWarning(const char* text));
	namespace ram{
		struct Cell_t;
		struct MemBuffer {
			void* memory = nullptr;					//Address of the buffer
			Map_NMP_S<Cell_t, uint32> cells;		//Cells in the buffer
			uint32 bufferIndex;						//Index of the buffer in the MemBufferType buffers array
		};
		struct MemBufferType {
			CellClass cellClass;					//Class of the cells
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








		//Like a normal pointer, but better
		//Allocate memory to the pointer with lux::ram::alloc
		//This pointer will automatically be freed once it is not longer used by any thread
		//You can also manually free it with the lux::mem:free function
		//    Accessing the memory of a freed pointer is ub
		template<class type> struct ptr{
			lux_sc_generate_debug_structure_body;
			Cell_t* cell;			//A pointer to a lux::ram::Cell_t object that contains the cell informations
			type* address;			//The address the pointer points to


			//####################################################################################################################################################################//

			//! PLS READ ME
			//Due to the lux::Nothing constructor property of initializing the class members with their own value, pointers must be initialized before it gets called
			//, to prevent read access violation errors
			//In order to do so, a class constructor, assignment/init function must be called
			//Those functions/constructors could accidentally access the memory of said uninitialized pointer, taking for granted that it has been initialized and breaking everything

			//This leads to the necessity of checking the pointer validity before executing any line of any functions that needs to access its memory
			//Because an uninitialized pointer/variable can contain any value, a uint64 with the value 0x94FFD489B48994FF is used as confirmation that the pointers have been initialized
			//Any other value means that they're not initialized and the program exeuction must be stopped (if you use it without initializing it, it's not the engine's fault)
			//This check is only performed in debug mode for performance reasons
			//(and if it's only in debug mode, you cannot just initialize the pointers in any function, or else it won't work in release mode)

			//So be chareful to check it. Or you will waste 2 weeks trying to fix non sense runtime errors
			//It's over 0.04% of a century

			//####################################################################################################################################################################//



			//TODO add check in other classes
			inline ptr( ) :								cell{ nullptr },		address{ nullptr }					{ }
			lux_sc_generate_nothing_constructor(ptr)	cell{ cell },			address{ address }					{ }
			inline ptr(Cell vCell) :					cell{ vCell },			address{ (type*)vCell->address }	{ cell->owners++; }
			inline ptr(Cell vCell, type* vAddress) :	cell{ vCell },			address{ vAddress }					{ cell->owners++; }
			inline ptr(ptr<type>& pPtr) :				cell{ pPtr.cell },		address{ pPtr.address }				{ cell->owners++; }
			//TODO pointer is not checked bu used
			template<class pType>
			explicit inline ptr(ptr<pType>& pPtr) :		cell{ pPtr.cell },		address{ (type*)pPtr.address }		{ cell->owners++; }


			inline void operator=(Cell_t* const vCell){		/*lux_sc_F*/
							//TODO ASSIGNMENT OPERATOR BREAKS EVEERYTHING IF THE STRUCT IS NOT INITIALIZED

			if(cell){
				cell->owners--;
			}
			cell = vCell;
			address = (type*)vCell->address;
			cell->owners++;
			}
			inline void operator=(const ptr<type>& pPtr){	/*lux_sc_F*/ if(cell) cell->owners--;	cell = pPtr.cell;	address = pPtr.address;				cell->owners++; }


			template<class pType> inline bool operator==(const ptr<pType>& pPtr) const {
				lux_sc_F;
				luxDebug(printWarning("Comparison operator should not be used with different pointer types"));
				return (void*)address == (void*)pPtr.address;
			}
			//TODO print warning if using a raw pointer
			//TODO or just use sparse pointers idk
			inline bool operator==(const type* vPtr) const { return (void*)address == (void*)vPtr; }




			//TODO print warning if using a raw pointer //??
			inline ptr<type>	operator+(const uint64		v	) const { lux_sc_F; return ptr<type>{cell,	address	+	v};						}
			inline ptr<type>	operator+(const uint32		v	) const { lux_sc_F; return ptr<type>{cell,	address	+	v};						}
			inline ptr<type>	operator+(const int64		v	) const { lux_sc_F; return ptr<type>{cell,	address	+	v};						}
			inline ptr<type>	operator+(const int32		v	) const { lux_sc_F; return ptr<type>{cell,	address	+	v};						}
			inline uint64		operator+(const type*		vPtr) const { lux_sc_F; return (uint64)			address	+	(uint64)vPtr;			}
			inline uint64		operator+(const ptr<type>&	vPtr) const { lux_sc_F; return (uint64)			address	+	(uint64)vPtr.address;	}
			inline ptr<type>	operator-(const uint64		v	) const { lux_sc_F; return ptr<type>{cell,	address	-	v};						}
			inline ptr<type>	operator-(const uint32		v	) const { lux_sc_F; return ptr<type>{cell,	address	-	v};						}
			inline ptr<type>	operator-(const int64		v	) const { lux_sc_F; return ptr<type>{cell,	address	-	v};						}
			inline ptr<type>	operator-(const int32		v	) const { lux_sc_F; return ptr<type>{cell,	address	-	v};						}
			inline uint64		operator-(const type*		vPtr) const { lux_sc_F; return (uint64)			address	-	(uint64)vPtr;			}
			inline uint64		operator-(const ptr<type>&	vPtr) const { lux_sc_F; return (uint64)			address	-	(uint64)vPtr.address;	}


			#define checkp luxDebug(if((uint64)address >= ((uint64)cell->address) + cell->cellSize) printWarning("A lux::ram::ptr has probably been increased too much and now points to an unallocated address. Reading or writing to this address is undefined behaviour and can cause runtime errors"))
			#define checkm luxDebug(if((uint64)address < (uint64)cell->address)                     printWarning("A lux::ram::ptr has probably been decreased too much and now points to an unallocated address. Reading or writing to this address is undefined behaviour and can cause runtime errors"))
			inline void			operator+=(uint64	v	)	{ lux_sc_F; address	+= v;	checkp; }
			inline void			operator+=(uint32	v	)	{ lux_sc_F; address	+= v;	checkp; }
			inline void			operator+=(int64	v	)	{ lux_sc_F; address	+= v;	checkp; }
			inline void			operator+=(int32	v	)	{ lux_sc_F; address	+= v;	checkp; }
			inline void			operator-=(uint64	v	)	{ lux_sc_F; address	-= v;	checkm; }
			inline void			operator-=(uint32	v	)	{ lux_sc_F; address	-= v;	checkm; }
			inline void			operator-=(int64	v	)	{ lux_sc_F; address	-= v;	checkm; }
			inline void			operator-=(int32	v	)	{ lux_sc_F; address	-= v;	checkm; }
			inline void			operator++(				)	{ lux_sc_F; address	++;		checkp; }
			inline void			operator--(				)	{ lux_sc_F; address	--;		checkm; }

			//TODO improve warnings and output object address or nanme
			inline type& operator*( ) const {
				lux_sc_F;
				luxDebug(if(!cell) printWarning("Unable to access the memory of an uninitialized lux::ram::ptr. A pointer should always be initialized with the lux::ram::alloc function or using one of its constructors"));
				luxDebug(if(!address) printWarning("Unable to access the memory of a null lux::ram::ptr"));
				return *address;
			}




			~ptr( ){ if(address) { if(!--cell->owners) cell->freeCell( ); } }		//Decrease the cell's owner count when the pointer is destroyed
			inline operator type*( ) const;											//ram::ptr<type> to type* implicit conversion
			inline operator bool( ) const;											//ram::ptr<type> to bool implicit conversion (e.g. if(ptr) is the same as if(ptr != nullptr), like normal pointers)
			inline type& operator [](const uint64 i)	const { lux_sc_F; return address[i]; }
			inline type& operator [](const uint32 i)	const { lux_sc_F; return address[i]; }
			inline type& operator [](const int64 i)		const { lux_sc_F; return address[i]; }
			inline type& operator [](const int32 i)		const { lux_sc_F; return address[i]; }

			//Returns the first address of the allocated memory block as a lux::ptr
			inline ptr<type> begin( ) const { lux_sc_F; return ptr<type>(cell); }
			//Returns the last address of the allocated memory block as a lux::ptr
			inline ptr<type> end( ) const { lux_sc_F; return ptr<type>(cell, (type*)((uint64)cell->address + cell->cellSize)); }
			//Returns the total size of the allocated memory
			inline uint64 __vectorcall size( )   const { lux_sc_F; return cell->cellSize; }
			//Returns the number of allocated bytes before the pointer
			inline uint64 __vectorcall prior( )  const { lux_sc_F; return (uint64)address - (uint64)cell->address; }
			//Returns the number of allocated bytes after the pointer
			inline uint64 __vectorcall latter( ) const { lux_sc_F; return ((uint64)cell->address + cell->cellSize) - (uint64)address;}
		};

		template<class type> ptr<type>::operator type*( )	const { lux_sc_F; return address; }
		template<class type> ptr<type>::operator bool( )	const { lux_sc_F; return address; }
	}
}
#endif