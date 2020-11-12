#ifndef LUX_H_MEMORY_T
#  define LUX_H_MEMORY_T


#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Types/Containers/LuxMap_NMP_S.hpp"
#include "LuxEngine/Core/ConsoleOutput.hpp"
#include "LuxEngine/Core/ConsoleOutput.hpp"
#include "LuxEngine/Types/Nothing.hpp"
#include "LuxEngine/Types/Nothing_sc_p.hpp"







namespace lux{
	//Bytes to allocate for each cell
	//Buffer classes and addresses are 32-byte aligned to allow the use of AVX2 and match the GPU minimum offsets
	enum class CellClass : uint32 {
		CLASS_A = 32U,					//32 bytes. The minimum count of a cell
		CLASS_B = CLASS_A * 16,			//16x CLASS_A. 512 B per cell (~0.5KB)
		CLASS_C = CLASS_A * 32,			//32x CLASS_A. 1024 B per cell (~1KB)
		CLASS_D = CLASS_A * 64,			//64x CLASS_A. 2048 B per cell (~2KB)
		CLASS_Q = CLASS_D * 64,			//64x CLASS_D. 131072 B per cell (~130KB)
		CLASS_L = CLASS_Q * 64,			//64x CLASS_Q. 8388608 B per cell (~8.4MB)
		CLASS_0 = 0,					//Dedicated buffer for cells larger than CellClass::CLASS_L
		AUTO = (uint32)-1,				//Choose a class large enough to contain the cell

		AT_LEAST_CLASS_A = 1 + CLASS_A,	//Like AUTO, but the class will only change if the specified one is too small
		AT_LEAST_CLASS_B = 1 + CLASS_B,	//Like AUTO, but the class will only change if the specified one is too small
		AT_LEAST_CLASS_C = 1 + CLASS_C,	//Like AUTO, but the class will only change if the specified one is too small
		AT_LEAST_CLASS_D = 1 + CLASS_D,	//Like AUTO, but the class will only change if the specified one is too small
		AT_LEAST_CLASS_Q = 1 + CLASS_Q,	//Like AUTO, but the class will only change if the specified one is too small
		AT_LEAST_CLASS_L = 1 + CLASS_L,	//Like AUTO, but the class will only change if the specified one is too small
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
	static constexpr inline uint32 classIndexFromEnum(const CellClass vClass){ switch(vClass){ _case(A) _case(B) _case(C) _case(D) _case(Q) _case(L) _case(0) default: return (uint32)-1; } }
	#define _case2(n) case CellClassIndex::INDEX_##n: return CellClass::CLASS_##n;
	static constexpr inline CellClass classEnumFromIndex(const CellClassIndex vIndex){
		switch(vIndex){ _case2(A) _case2(B) _case2(C) _case2(D) _case2(Q) _case2(L) _case2(0) default: return (CellClass)-1; }
	}








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
		static inline uint32 getCellOffset(const Cell_t* pCell){ return (uint32)pCell->bufferType->cellClass * pCell->cellIndex; }







		//struct


		//Like a normal pointer, but better
		//Allocate memory to the pointer with lux::ram::allocBck
		//This pointer will automatically be freed once it is not longer used by any thread
		//You can also manually free it with the lux::mem:free function
		//    Accessing the memory of a freed pointer is ub
		template<class type/*, class ptrType = ram::*/> struct ptr{
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

			#ifdef LUX_DEBUG
				bool __ptrC(const ptr<type>& pPtr) const {
					isInit(pPtr);
					//if(pPtr.lux_sc_initialized != lux_sc_v) param_error(pPtr, "An uninitialized lux::ram::ptr have been passed to a copy constructor of another lux::ram::ptr");
					return true;
				};
				void __ptrCf(const ptr<type>& pPtr) const {
					isInit(pPtr);
					//if(pPtr.lux_sc_initialized != lux_sc_v)
						//param_error(pPtr, "Uninitialized lux::ram::ptr passed to a function of a lux::ram::ptr");
				};


				bool __cellAdrC(Cell_t* vCell, type* vAddress) const {
					param_error_2(!vCell && vAddress, vCell, "nullptr cell used to initialize a lux::ram::ptr. If a pointer has a valid address, the cell must also be valid");
					param_error_2(vCell && !vAddress, vCell, "nullptr address used to initialize a lux::ram::ptr. If a pointer has a valid cell, the address must be in its range");
					//ok, if they're both nullptr the ptr is initialized with nullptr

					ptr_validity(vAddress, type, "Invalid address used to initialize a lux::ram::ptr");
					ptr_validity(vCell, Cell_t, "Invalid Cell_t pointer used to initialize a lux::ram::ptr");
					return true;
				};
			#endif





			#define checkNullptr lux_error(!address, "function %s  have been called on a lux::ram::ptr with value nullptr", "h")
			//#define checkNullptr lux_error(!address, "function %s  have been called on a lux::ram::ptr with value nullptr", __FUNCTION__)
			#define checkSizeD   lux_error(size( ) == 0, "Cannot dereference a pointer pointing to a 0-byte memory allocation")
			//#define checkCount  lux_error(count( ) == 0, "This function cannot be called on pointers pointing to a 0-byte memory allocation")



			#define checkp luxDebug(if(address >= end( ) + cell->cellSize) printWarning("A lux::ram::ptr has probably been increased too much and now points to an unallocated address. Reading or writing to this address is undefined behaviour and can cause runtime errors"))
			//#define checkp luxDebug(if((uint64)address >= ((uint64)cell->address) + cell->cellSize) printWarning("A lux::ram::ptr has probably been increased too much and now points to an unallocated address. Reading or writing to this address is undefined behaviour and can cause runtime errors"))
			#define checkm luxDebug(if(address < begin( ))                 printWarning("A lux::ram::ptr has probably been decreased too much and now points to an unallocated address. Reading or writing to this address is undefined behaviour and can cause runtime errors"))
			//#define checkm luxDebug(if((uint64)address < (uint64)cell->address)                     printWarning("A lux::ram::ptr has probably been decreased too much and now points to an unallocated address. Reading or writing to this address is undefined behaviour and can cause runtime errors"))

			#define __checka__err__ "The assigned address is out of the allocated block range. Reading or writing to this address is undefined behaviour and can cause runtime errors"
			#define checka luxDebug(																				\
				if(address && ((uint64)address < (uint64)cell->address)){											\
					if(cell->cellSize){																				\
						lux_error((uint64)address >= ((uint64)cell->address) + cell->cellSize, __checka__err__);	\
					}																								\
					else{																							\
						lux_error((uint64)address != (uint64)cell->address + cell->cellSize, __checka__err__);		\
					}																								\
				}																									\
			);













			//TODO #define macro(a,b) otherMacros(a,b)
			//TODO #define otherMacro(a,b) finalMacro(a##b)
			//TODO macro(__FUNC, TION__)

			//TODO add check in other classes
			//TODO check pointer validity with try catch on general exception
			//TODO check nullptr in every function. only in debug mode
			//TODO use dummy cell to improve performance and dont check for nullptrs when increasing or decreasing owner counts
			lux_sc_generate_nothing_constructor(ptr)	cell{ cell },			address{ address }					{ }
			//! [#] Structure is uninitialized            | >>> NOT CHECKED <<<
			//OK

			inline ptr( ) :								cell{ nullptr },		address{ nullptr }					{ }
			// [#] No init required
			//OK

			inline ptr(Cell_t* const vCell) :			cell{ vCell },		address{ (vCell) ? (type*)vCell->address : nullptr }	{ if(vCell) {cell->owners++; checka; } }
			// [#] No init required
			// [#] vCell is nullptr                      | k | It CAN be. If it's nullptr, address is also set to nullptr and the owner count not modified. This is the case of ram::ptr foo(nullptr);
			// [#] vCell is invalid                      | k | print error
			// [#] vCell is a casted tpe and not a vCell | k | compiler throws an error
			// [#] the new address is out of range       | k | print an error (only if it's not nullptr)
			//OK

			inline ptr(Cell_t* vCell, type* vAddress) :	constructExec(__cellAdrC, vCell, vAddress)  cell{ vCell },		address{ vAddress }					{ if(vCell){cell->owners++; checka; } }
			// [#] No init required
			// [#] vCell is nullptr, but vAddress no     | k | print an error
			// [#] vAddress is nullptr, but vCell no     | k | print an error
			// [#] vAddress and vCell are both nullptr   | k | ok. They can be. If they are, the owner count is not modified
			// [#] vAddress or vCell is invalid          | k | print an error
			// [#] vCell is a casted tpe and not a vCell | k | compiler throws an error
			// [#] the new address is out of range       | k | print an error (only if it's not nullptr)
			//OK

			inline ptr(ptr<type>& pPtr) : constructExec(__ptrC, pPtr)  cell{ pPtr.cell }, address{ pPtr.address }				{ if(pPtr){ cell->owners++; checka; } }
			// [#] No init required
			// [#] ptr's address is nullptr              | k | it can be. If it is, the owner count is not modified
			// [#] uninitialized ptr                     | k | print error
			// [#] the new address is out of range       | k | print an error (checka)
			//OK

			template<class pType>
			explicit inline ptr(ptr<pType>& pPtr) :		constructExec(__ptrC, pPtr)  cell{ pPtr.cell },		address{ (type*)pPtr.address }		{ if(pPtr){cell->owners++; checka;} }
			// [#] No init required
			// [#] ptr's address is nullptr              | k | it can be. If it is, the owner count is not modified
			// [#] uninitialized ptr                     | k | print error
			// [#] the new address is out of range       | k | print an error (only if it's not nullptr)
			//OK















			//TODO ASSIGNMENT OPERATOR BREAKS EVEERYTHING IF THE STRUCT IS NOT INITIALIZED
			//TODO don't use dummy cell. It can cause bugs
			//inline void operator=(Cell_t* const vCell){		if(cell) cell->owners--;	cell = vCell;		address = (type*)vCell->address;	cell->owners++; checka; }
			//inline void operator=(const ptr<type>& pPtr){	if(cell) cell->owners--;	cell = pPtr.cell;	address = pPtr.address;				cell->owners++; checka; }
			inline void operator=(Cell_t* const vCell){		ptr_validity(vCell, Cell_t, "Invalid pointer passed to assignment operator") if(cell) cell->owners--;	cell = vCell;		address = (type*)vCell->address;	if(cell) cell->owners++; checka; }
			// [#] cell  is nullptr        | k | it can be. If it is, don't change owner count
			// [#] vCell is nullptr        | k | it can be. If it is, don't change its owner count
			// [#] vCell is invalid        | k | print error
			//OK

			inline void operator=(const ptr<type>& pPtr){	luxDebug(__ptrCf(pPtr)); if(cell) cell->owners--;	cell = pPtr.cell;	address = pPtr.address;				if(cell) cell->owners++; checka; }
			// [#] No init required
			//pPtr is uninitialized        | k | print error
			//cell is nullptr              | k | it can be. If it is, don't change owner count
			//pPtr is nullptr              | k | it can be. If it is, don't change its owner count
			//OK

			inline bool operator==(ptr<type>	pPtr	) const { return pPtr.address		== address;		}
			// [#] No init required
			// [#] No init of pPtr required
			//OK

			inline bool operator==(const type*	vPtr) const { return (void*)address == (void*)vPtr; }
			// [#] No init required
			// [#] No init of vPtr required
			//OK

			inline bool operator!=(ptr<type>	pPtr	) const { return pPtr.address		!= address;		}
			// [#] No init required
			// [#] No init of pPtr required
			//OK

			inline bool operator!=(const type*	vPtr) const { return (void*)address != (void*)vPtr; }
			// [#] No init required
			// [#] No init of vPtr required
			//OK

			template<class pType> inline bool operator==(const ptr<pType>& pPtr) const { checkInit; return (void*)address == (void*)pPtr.address; }
			// [#] No init required
			// [#] No init of pPtr required
			//OK


			//TODO improve warnings and output object address or nanme
			inline type& operator*( ) const { checkInit; checkNullptr; checkSizeD; return *address; }
			// [#] Uninitialized structure | k | print error
			// [#] address is nullptr      | k | print error
			// [#] allocated count is 0     | k | print error
			//OK























			//TODO print warning if using a raw pointer //??
			//TODO or just use sparse pointers idk
			#define op_p(t) inline ptr<type>operator+(const t			v	) const { checkInit; checkNullptr; return ptr<type>{cell,	address	+ v};						}
			// [#] Uninitialized structure | k | print error
			// [#] address is nullptr      | k | print error
			//OK

			inline uint64					operator+(const type*		vPtr) const { checkInit; checkNullptr; param_error_2(vPtr == nullptr, vPtr, "Cannot use nullptr as a value in lux::ram::ptr arithmetics"); return (uint64)			address	+	(uint64)vPtr;			}
			// [#] Uninitialized structure | k | print error
			// [#] address is nullptr      | k | print error
			// [#] vPtr is nullptr         | k | print error
			// [#] vPtr is invalid         | k | no need to check it here. The result will be checked in the assignment functions
			//OK

			inline uint64					operator+(const ptr<type>&	vPtr) const { checkInit; checkNullptr; luxDebug(__ptrCf(vPtr)); return (uint64)			address	+	(uint64)vPtr.address;	}
			// [#] Uninitialized structure | k | print error
			// [#] address is nullptr      | k | print error
			// [#] pPtr is uninitialized   | k | print error
			//OK

			#define op_m(t) inline ptr<type>operator-(const t			v	) const { checkInit; checkNullptr; return ptr<type>{cell,	address	- v};						}
			// [#] Uninitialized structure | k | print error
			// [#] address is nullptr      | k | print error
			//OK

			inline uint64					operator-(const type*		vPtr) const { checkInit; checkNullptr; param_error_2(vPtr == nullptr, vPtr, "Cannot use nullptr as a value in lux::ram::ptr arithmetics"); return (uint64)			address - (uint64)vPtr;				}
			// [#] Uninitialized structure | k | print error
			// [#] address is nullptr      | k | print error
			// [#] vPtr is nullptr         | k | print error
			// [#] vPtr is invalid         | k | no need to check it here. The result will be checked in the assignment functions
			//OK

			inline uint64					operator-(const ptr<type>&	vPtr) const { checkInit; checkNullptr; luxDebug(__ptrCf(vPtr)); return (uint64)			address - (uint64)vPtr.address;		}
			// [#] Uninitialized structure | k | print error
			// [#] address is nullptr      | k | print error
			// [#] pPtr is uninitialized   | k | print error
			//OK


			op_p(uint64) op_p(uint32) op_p(uint16) op_p(uint8) op_p(int64) op_p(int32) op_p(int16) op_p(int8);
			op_m(uint64) op_m(uint32) op_m(uint16) op_m(uint8) op_m(int64) op_m(int32) op_m(int16) op_m(int8);
















			inline void			operator+=(uint64 v)	{ checkInit; checkNullptr; checkSize; address += v;	checkp; }
			// [#] Uninitialized structure    | k | print error
			// [#] address is nullptr         | k | print error
			// [#] address goes out of range  | k | print error
			// [#] count is 0                  | k | print error
			//OK

			inline void			operator-=(uint64 v)	{ checkInit; checkNullptr; checkSize; address -= v;	checkm; }
			// [#] Uninitialized structure    | k | print error
			// [#] address is nullptr         | k | print error
			// [#] address goes out of range  | k | print error
			// [#] count is 0                  | k | print error
			//OK

			inline void			operator++( )			{ checkInit; checkNullptr; checkSize; address++;	checkp; }
			// [#] Uninitialized structure    | k | print error
			// [#] address is nullptr         | k | print error
			// [#] address goes out of range  | k | print error
			// [#] count is 0                  | k | print error
			//OK

			inline void			operator--( )			{ checkInit; checkNullptr; checkSize; address--;	checkm; }
			// [#] Uninitialized structure    | k | print error
			// [#] address is nullptr         | k | print error
			// [#] address goes out of range  | k | print error
			// [#] count is 0                  | k | print error
			//OK




















			//TODO add negative index in containers


			~ptr( ){ if(address) if(!--cell->owners) cell->freeCell( ); }			//Decrease the cell's owner count when the pointer is destroyed
			// [#] No init required
			//OK

			inline operator type*( )	const { checkInit; /*checkCount; */lux_error(!address, "Cannot dereference a nullptr"); return address; }			//ram::ptr<type> to type* implicit conversion
			// [#] Uninitialized structure    | k | print error
			// [#] address is nullptr         | k | print error

			//TODO it can be. maybe
			//// [#] count is 0                  | k | print error
			// [#] count is 0                  | k | If count is 0, the pointer still has an address. It just contains no data and can overlap
			//OK

			inline operator bool( )		const { checkInit; return !!address; }			//ram::ptr<type> to bool implicit conversion (e.g. if(ptr) is the same as if(ptr != nullptr), like normal pointers)
			// [#] Uninitialized structure    | k | print error
			// [#] address is nullptr         | k | it can be
			//OK

			inline type& operator [](const uint64 vIndex)const { checkInit; checkSize; param_error_2((vIndex < 0 && vIndex > prior( )) || (vIndex >= 0 && vIndex > latter( )), vIndex, "Index is out of range"); return address[vIndex]; }
			// [#] Uninitialized structure    | k | print error
			// [#] address is nullptr         | k | it can be
			// [#] vIndex is negative         | k | print error
			// [#] vIndex is out of range     | k | print error
			// [#] count is 0                  | k | print error
			//OK





			inline type*	 begin( )	const { checkInit; checkNullptr; /*checkCount;*/ return (type*)cell->address;										} //Returns the first address of the allocated memory block as a normal pointer
			// [#] Uninitialized structure    | k | print error
			// [#] address or cell is nullptr | k | print error
			//// [#] count is 0                  | k | print error
			//OK

			inline ptr<type> beginp( )	const { checkInit; checkNullptr; /*checkCount;*/ return ptr<type>(cell);											} //Returns the first address of the allocated memory block as a lux::ram::ptr
			// [#] Uninitialized structure    | k | print error
			// [#] address or cell is nullptr | k | print error
			//// [#] count is 0                  | k | print error
			//OK

			inline type*	 end( )	const { checkInit; checkNullptr; /*checkCount;*/ return (type*)((int8*)cell->address + cell->cellSize);		} //Returns the address of the object past the last object in the memory block as a normal pointer. Don't dereference it
			// [#] Uninitialized structure    | k | print error
			// [#] address or cell is nullptr | k | print error
			//// [#] count is 0                  | k | print error
			//OK

			inline ptr<type> endp( )	const { checkInit; checkNullptr; /*checkCount;*/ return ptr<type>(cell, (type*)((int8*)cell->address + cell->cellSize));} //Returns the address of the object past the last object in the memory block as a lux::ram::ptr. Don't dereference it
			// [#] Uninitialized structure    | k | print error
			// [#] address or cell is nullptr | k | print error
			//// [#] count is 0                  | k | print error
			//OK

			inline type&	 last( )	const { checkInit; checkNullptr; checkSize; return ((type*)address)[count( ) - 1]; } //Returns a reference to the last element in the allocated memory block
			//inline type&	 last( )	const { checkInit; checkNullptr; checkCount; return *(((type*)((int8*)cell->address + cell->cellSize)) - 1); } //Returns a reference to the last element in the allocated memory block
			// [#] Uninitialized structure    | k | print error
			// [#] address or cell is nullptr | k | print error
			// [#] count is 0                  | k | print error
			//OK

			inline uint64 size( )	const { checkInit; checkNullptr; /*checkCount;*/ return cell->cellSize;												} //Returns the total size of the allocated memory
			// [#] Uninitialized structure    | k | print error
			// [#] address or cell is nullptr | k | print error
			//// [#] count is 0                  | k | print error
			//OK

			inline uint64 count( )	const { checkInit; checkNullptr; /*checkCount;*/ return cell->cellSize / sizeof(type);												} //Returns the number of elements in the allocated memory (use this only if you have allocated the memory with an allocArr function or you are sure that the size is a multiple of the type's size)
			// [#] Uninitialized structure    | k | print error
			// [#] address or cell is nullptr | k | print error
			//// [#] count is 0                  | k | print error
			//OK

			inline uint64 prior( )	const { checkInit; checkNullptr; /*checkCount;*/ return (uint64)address - (uint64)cell->address;						} //Returns the number of allocated bytes before the pointer
			// [#] Uninitialized structure    | k | print error
			// [#] address or cell is nullptr | k | print error
			//// [#] count is 0                  | k | print error
			//OK

			inline uint64 latter( )const { checkInit; checkNullptr; /*checkCount;*/ return (uint64)cell->address + cell->cellSize - (uint64)address;		} //Returns the number of allocated bytes after  the pointer
			// [#] Uninitialized structure    | k | print error
			// [#] address or cell is nullptr | k | print error
			//// [#] count is 0                  | k | print error
			//OK

		};
	}
}
#endif