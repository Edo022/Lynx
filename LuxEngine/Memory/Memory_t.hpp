#pragma once
#define LUX_H_MEMORY_T
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Memory/LuxMap_NMP_S.hpp"
#include "LuxEngine/Core/ConsoleOutput.hpp"
#include "LuxEngine/Tests/StructureInit.hpp"
#include "LuxEngine/System/SystemMacros.hpp"


#include <string.h>

//TODO
	#define LuxMemOffset 32








namespace lux{
	//Bytes allocated to each cell
	//Buffer classes and addresses are 32-byte aligned to allow the use of AVX2 and match the GPU minimum offsets //FIXME align on 32 bytes
	enum class CellClass : uint32 {
		CLASS_A = 32U,					//32 bytes. The minimum size of a cell
		CLASS_B = CLASS_A * 16,			//16x CLASS_A. 512 B per cell (~0.5KB)
		CLASS_C = CLASS_A * 32,			//32x CLASS_A. 1024 B per cell (~1KB)
		CLASS_D = CLASS_A * 64,			//64x CLASS_A. 2048 B per cell (~2KB)
		CLASS_Q = CLASS_D * 64,			//64x CLASS_D. 131072 B per cell (~130KB)
		CLASS_L = CLASS_Q * 64,			//64x CLASS_Q. 8388608 B per cell (~8.4MB)
		CLASS_0 = 0,					//Dedicated buffer for cells larger than CellClass::CLASS_L
		AUTO = (uint32)-1,				//Choose a class large enough to contain the cell

		AT_LEAST_CLASS_A = 1 + CLASS_A,	//Like AUTO, but the class is guaranteed to be at least CLASS_A
		AT_LEAST_CLASS_B = 1 + CLASS_B,	//Like AUTO, but the class is guaranteed to be at least CLASS_B
		AT_LEAST_CLASS_C = 1 + CLASS_C,	//Like AUTO, but the class is guaranteed to be at least CLASS_C
		AT_LEAST_CLASS_D = 1 + CLASS_D,	//Like AUTO, but the class is guaranteed to be at least CLASS_D
		AT_LEAST_CLASS_Q = 1 + CLASS_Q,	//Like AUTO, but the class is guaranteed to be at least CLASS_Q
		AT_LEAST_CLASS_L = 1 + CLASS_L,	//Like AUTO, but the class is guaranteed to be at least CLASS_L
	};


	namespace __pvt{
		//Indices corresponding to lux::CellClass values
		//Engine iternal use
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
		//Size of each buffer. 50331648 B (~50MB)
		static const uint32 bufferSize = (uint32)CellClass::CLASS_L * 6;


		//Returns the CellClassIndex value of a CellClass
		static constexpr inline uint32 classIndexFromEnum(const CellClass vClass){
			switch(vClass){
				#define _case(n) case CellClass::CLASS_##n: return (uint32)CellClassIndex::INDEX_##n;
				_case(A) _case(B) _case(C) _case(D) _case(Q) _case(L) _case(0)
				default: luxPrintError("Invalid cell class %d", (uint32)vClass) return (uint32)-1;
			}
		}
		//Returns the CellClass value of a CellClassIndex
		static constexpr CellClass classes[] = { CellClass::CLASS_A, CellClass::CLASS_B, CellClass::CLASS_C, CellClass::CLASS_D, CellClass::CLASS_Q, CellClass::CLASS_L };
		static constexpr inline CellClass classEnumFromIndex(const CellClassIndex vIndex){
			return classes[(uint64)vIndex];
		}
	}








	namespace ram{
		struct Cell_t;
		struct MemBuffer {
			void* 						memory = nullptr;	//Address of the buffer
			Map_NMP_S<Cell_t, uint32> 	cells;				//Cells in the buffer
			uint32 						bufferIndex;		//Index of the buffer in the MemBufferType buffers array
		};
		struct MemBufferType {
			CellClass 					cellClass;			//Class of the cells
			Map_NMP_S<MemBuffer, uint32> buffers;			//Buffers containing the cells
		};
		struct Cell_t {
			uint32 						owners;				//Number of lux::ram::ptr instances that point to an address of the cell
			uint64 						cellSize;			//Size of the cell in bytes
			void* 						address;			//Address of the cell. The same as you would get with malloc
			MemBufferType* 				bufferType;			//Type of buffer allocation
			MemBuffer* 					buffer;				//Index of the buffer where the cell is allocated
			uint32 						cellIndex;			//Index of the cell in the buffer
			void freeCell();
		};
		static inline uint32 getCellOffset(const Cell_t* pCell){ return (uint32)pCell->bufferType->cellClass * pCell->cellIndex; }






		extern MemBufferType* 	buffers;	//Allocated buffers
		extern uint32 			allocated;	//TODO remove

		//Like a normal pointer, but better
		//Allocate memory to the pointer with lux::ram::allocBck
		//This pointer will automatically be freed once it is not longer used by any thread
		//You can also manually free it with the lux::mem:free function
		//    Accessing the memory of a freed pointer is ub
		template<class type/*, class ptrType = ram::*/> struct ptr{
			genInitCheck;
			Cell_t* cell;			//A pointer to a lux::ram::Cell_t object that contains the cell informations
			type* address;			//The address the pointer points to



			//FIXME REMOVE POINTER VALIDITY CHECKS
			//#######################################################################################################//

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




			#define checkNullptr luxCheckCond(!address, "function %s  have been called on a lux::ram::ptr with value nullptr", __FUNCTION__)
			#define checkSizeD   luxCheckCond(size( ) == 0, "Cannot dereference a pointer pointing to a 0-byte memory allocation")


			#define checkp luxDebug(if(address >= end( ) + cell->cellSize) luxPrintWarning("A lux::ram::ptr has probably been increased too much and now points to an unallocated address. Reading or writing to this address is undefined behaviour and can cause runtime errors"))
			#define checkm luxDebug(if(address < begin( ))                 luxPrintWarning("A lux::ram::ptr has probably been decreased too much and now points to an unallocated address. Reading or writing to this address is undefined behaviour and can cause runtime errors"))

			#define __checka__err__ "The assigned address is out of the allocated block range. Reading or writing to this address is undefined behaviour and can cause runtime errors"
			#define checka luxDebug(																								\
				if(address && ((uint64)address < (uint64)cell->address)){															\
					if(cell->cellSize) luxCheckCond((uint64)address >= ((uint64)cell->address) + cell->cellSize, __checka__err__)	\
					else               luxCheckCond((uint64)address !=  (uint64)cell->address  + cell->cellSize, __checka__err__)	\
				}																													\
			);




			//TODO check nullptr in every function. only in debug mode
			//TODO use dummy cell to improve performance and dont check for nullptrs when increasing or decreasing owner counts
			inline ptr( ) :							                cell{  nullptr }, address{  nullptr          } { }
			inline ptr(Cell_t* vCell, type* vAddr) :                cell{  vCell   }, address{  vAddr            } { if(vCell){ cell->owners++; checka; } }
			inline ptr(ptr<type>& pPtr)				{ isInit(pPtr); cell = pPtr.cell; address = pPtr.address;        if(pPtr ){ cell->owners++; checka; } }
			template<class pType>
			explicit inline ptr(ptr<pType>& pPtr) 	{ isInit(pPtr); cell = pPtr.cell; address = (type*)pPtr.address; if(pPtr ){ cell->owners++; checka; } }


			//Allocates a block of memory without initializing it
			//e.g.
			//lux::ram::ptr<int32> p(302732);
			inline ptr(const uint64 vSize, CellClass vClass = CellClass::AUTO){
				evaluateCellClass(vSize, vClass);
				alloc_call(vSize, vClass);
				++cell->owners;
			}

			//Allocates a block of memory and initializes each element with the pValue value
			//e.g.
			//lux::ram::ptr<float> p(302732, 15.0f);
			inline ptr(const uint64 vSize, const type& pValue, CellClass vClass = CellClass::AUTO) : ptr(vSize, vClass) {
				init_memory(cell->address, vSize, pValue);
				++cell->owners;
			}




			//TODO improve warnings and output object address or nanme
			//Assignment and comparison
			inline void operator=(Cell_t* const   vCell){
				luxCheckRawPtr(vCell, Cell_t, "Invalid pointer passed to assignment operator")
				if(cell) cell->owners--;	cell = vCell;		address = (type*)vCell->address;	if(cell) cell->owners++; checka; }
			inline void operator=(const ptr<type>& pPtr){ isInit(pPtr);
				if(cell) cell->owners--;	cell = pPtr.cell;	address = pPtr.address;				if(cell) cell->owners++; checka;
			}
			inline bool operator==(ptr<type>	   pPtr) const { return pPtr.address   == address;     }
			inline bool operator==(const type*	   vPtr) const { return (void*)address == (void*)vPtr; }
			inline bool operator!=(ptr<type>	   pPtr) const { return pPtr.address   != address;     }
			inline bool operator!=(const type*     vPtr) const { return (void*)address != (void*)vPtr; }
			template<class pType> inline bool operator==(const ptr<pType>& pPtr) const { checkInit; return (void*)address == (void*)pPtr.address; }
			template<class pType> inline bool operator!=(const ptr<pType>& pPtr) const { checkInit; return (void*)address != (void*)pPtr.address; }




			//Add, subtract
			#define as_nullptrCheck luxCheckParam(!vPtr, vPtr, "Cannot use nullptr as a value in lux::ram::ptr arithmetics")
			#define op_p(t) inline ptr<type>operator+(const t			v	) const { checkInit; checkNullptr; return ptr<type>{cell,	address	+ v}; }
			#define op_m(t) inline ptr<type>operator-(const t			v	) const { checkInit; checkNullptr; return ptr<type>{cell,	address	- v}; }
			inline uint64					operator+(const type*		vPtr) const { checkInit; checkNullptr; as_nullptrCheck; return (uint64)address + (uint64)vPtr;         }
			inline uint64					operator+(const ptr<type>&	pPtr) const { checkInit; checkNullptr; isInit(pPtr)     return (uint64)address + (uint64)pPtr.address; }
			inline uint64					operator-(const type*		vPtr) const { checkInit; checkNullptr; as_nullptrCheck; return (uint64)address - (uint64)vPtr;         }
			inline uint64					operator-(const ptr<type>&	pPtr) const { checkInit; checkNullptr; isInit(pPtr)     return (uint64)address - (uint64)pPtr.address; }

			op_p(uint64) op_p(uint32) op_p(uint16) op_p(uint8) op_p(int64) op_p(int32) op_p(int16) op_p(int8);
			op_m(uint64) op_m(uint32) op_m(uint16) op_m(uint8) op_m(int64) op_m(int32) op_m(int16) op_m(int8);


			inline void operator+=(uint64 v) { checkInit; checkNullptr; checkSize; address += v; checkp; }
			inline void operator-=(uint64 v) { checkInit; checkNullptr; checkSize; address -= v; checkm; }
			inline void operator++( )        { checkInit; checkNullptr; checkSize; address++;    checkp; }
			inline void operator--( )        { checkInit; checkNullptr; checkSize; address--;    checkm; }




			//TODO add negative index in containers
			//Implicit conversion
			~ptr( ){ if(address) if(!--cell->owners) cell->freeCell( ); }		//Decrease the cell's owner count when the pointer is destroyed
			inline operator type*( ) const { checkInit; luxCheckCond(!address, "Cannot dereference a nullptr"); return address; }	//ram::ptr<type> to type* implicit conversion
			inline operator bool(  ) const { checkInit; return !!address; }		//ram::ptr<type> to bool implicit conversion (e.g. if(ptr) is the same as if(ptr != nullptr), like normal pointers)




			//Get element
			inline type&     operator[](const uint64 vIndex)const { checkInit; checkSize; luxCheckParam((vIndex < 0 && vIndex > prior( )) || (vIndex >= 0 && vIndex > latter( )), vIndex, "Index is out of range"); return address[vIndex]; }
			inline type&     operator*( ) const { checkInit; checkNullptr; checkSizeD; return *address; }
			inline type&	 last(      ) const { checkInit; checkNullptr; checkSize; return ((type*)address)[count( ) - 1];	 } //Returns a reference to the last element in the allocated memory block
			inline type*	 begin(     ) const { checkInit; checkNullptr; return (type*)cell->address;							 } //Returns the first address of the allocated memory block as a normal pointer
			inline ptr<type> beginp(    ) const { checkInit; checkNullptr; return ptr<type>(cell);								 } //Returns the first address of the allocated memory block as a lux::ram::ptr
			inline type*	 end(       ) const { checkInit; checkNullptr; return (type*)((int8*)cell->address + cell->cellSize);} //Returns the address of the object past the last object in the memory block as a normal pointer. Don't dereference it
			inline ptr<type> endp(      ) const { checkInit; checkNullptr; return ptr<type>(cell, (type*)((int8*)cell->address + cell->cellSize));} //Returns the address of the object past the last object in the memory block as a lux::ram::ptr. Don't dereference it

			//Count
			inline uint64 size(         ) const { checkInit; checkNullptr; return cell->cellSize;								 } //Returns the total size of the allocated memory
			inline uint64 count(        ) const { checkInit; checkNullptr; return cell->cellSize / sizeof(type);				 } //Returns the number of elements in the allocated memory (use this only if you have allocated the memory with an allocArr function or you are sure that the size is a multiple of the type's size)
			inline uint64 prior(        ) const { checkInit; checkNullptr; return (uint64)address - (uint64)cell->address;		 } //Returns the number of allocated bytes before the pointer
			inline uint64 latter(       ) const { checkInit; checkNullptr; return (uint64)cell->address + cell->cellSize - (uint64)address;	} 	//Returns the number of allocated bytes after  the pointer








		private:
			void evaluateCellClass(const uint64 vSize, CellClass& pClass){
				if(pClass != CellClass::AUTO && (uint32)pClass % LuxMemOffset == 1) {	//Check AT_LEAST values (normal class values + 1)
					if(vSize > ((uint32)pClass - 1)) pClass = CellClass::AUTO;				//If the class is too small, set it to AUTO
					else pClass = (CellClass)((uint64)pClass - 1);							//If it's large enough, assign the normal class value
				}
				if(pClass == CellClass::AUTO) { [[likely]]								//Choose cell class if it's AUTO
					/**/ if(vSize <= (uint32)CellClass::CLASS_A) [[likely]]		pClass = CellClass::CLASS_A;
					else if(vSize <= (uint32)CellClass::CLASS_B) [[likely]]		pClass = CellClass::CLASS_B;
					else if(vSize <= (uint32)CellClass::CLASS_C) [[likely]]		pClass = CellClass::CLASS_C;
					else if(vSize <= (uint32)CellClass::CLASS_D) [[unlikely]]	pClass = CellClass::CLASS_D;
					else if(vSize <= (uint32)CellClass::CLASS_Q) [[unlikely]]	pClass = CellClass::CLASS_Q;
					else if(vSize <= (uint32)CellClass::CLASS_L) [[unlikely]]	pClass = CellClass::CLASS_L;
					else														pClass = CellClass::CLASS_0;
				}

				luxCheckParam(vSize > 0xFFFFffff, vSize, "Cell size cannot exceed 0xFFFFFFFF bytes. The given size was %llu", vSize);
				luxCheckParam((uint32)pClass < vSize, pClass,
					"Requested %lu-bytes class for %llu-bytes allocation. The cell class must be large enought to contain the cell. %s",
					(uint32)pClass, vSize, "Use lux::CellClass::AUTO to automatically choose it"
				);
			}


			//FIXME MERGE WITH alloc(...)
			//TODO CHECK MEMORY FULL
			//This function allocates a memory cell or a pointer into a buffer
			//*   vSize                 | count of the cell
			//*   vClass                | class of the cell. This is the maximum count the cell can reach before it needs to be copied
			//*   vForceDedicatedBuffer | if true, the memory will be allocated in a new buffer instead of a fixed count cell
			//*   Returns               | the allocated Cell object
			//e.g. lux::ram::ptr<int> foo = lux::ram::allocBck(100, lux::CellClass::AUTO);
			//e.g. same as int* foo = (int*)malloc(100);
			void alloc_internal(const uint64 vSize, const CellClass vClass){
				uint32 typeIndex = lux::__pvt::classIndexFromEnum(vClass);												//Get buffer index from type and class
				Map_NMP_S<MemBuffer, uint32>& subBuffers = (lux::ram::buffers[typeIndex].buffers);					//Get list of buffers where to search for a free cell
				uint32 cellIndex;
				if((uint32)vClass){																			//If the cell is a fixed count cell
					uint64 cellNum = lux::__pvt::bufferSize / (uint32)vClass;												//Get the maximum number of cells in each buffer
					for(uint32 i = 0; i < subBuffers.size( ); i++){												//Search for a suitable buffer
						if(subBuffers.isValid(i) && (subBuffers[i].cells.usedSize( ) < cellNum)) {					//If a buffer is valid and it has a free cell
							cellIndex = subBuffers[i].cells.add(Cell_t{
								.cellSize = vSize,
								.bufferType = &lux::ram::buffers[typeIndex]
							});
							cell = &subBuffers[i].cells[cellIndex];	 //<^ Create a new cell in the buffer and set its buffer, index and address
							cell->buffer = &subBuffers[i];
							cell->cellIndex = cellIndex;
							address = (type*)(cell->address = (void*)((uint8*)(cell->buffer->memory) + getCellOffset(cell)));
						}
					}
				}{																							//If there are no free buffers or the cell is a custom count cell
					uint32 bufferIndex, cellsNum = (uint32)vClass ? lux::__pvt::bufferSize / (uint32)vClass : 1;
					bufferIndex = subBuffers.add(MemBuffer{														//Create a new buffer and save the buffer index
						//FIXME USE A NORMAL RaArray THAT ALLOCATES WITH MALLOC
						//! The map requires the chunk count and the max count. bufferSize is the count in bytes of the whole buffer, not the number of cells. The number of cells is (bufferSize / (uint32)vClass)
						//.cells = (uint32)vClass ? Map_NMP_S<Cell_t, uint32>(max(/*384*/24576, cellsNum), cellsNum) : Map_NMP_S<Cell_t, uint32>(1, 1),
						.cells = (uint32)vClass ? Map_NMP_S<Cell_t, uint32>(min(/*384*/24576, cellsNum), cellsNum) : Map_NMP_S<Cell_t, uint32>(1, 1),
					});																							//^ Create in it 1 cell for custom count cells, or the maximum number of cells for fixed count cells
					MemBuffer& buffer = subBuffers[bufferIndex]; buffer.bufferIndex = bufferIndex;				//Set the buffer index of the created buffer
					//TODO set right like
					if(!buffer.memory) {
						buffer.memory = //Allocate new memory if the buffer has not already been allocated
							win10(_aligned_malloc((uint32)vClass ? lux::__pvt::bufferSize : vSize, LuxMemOffset);)
							linux( aligned_alloc( LuxMemOffset, (uint32)vClass ? lux::__pvt::bufferSize : vSize);)

						//TODO remove
						lux::ram::allocated += (uint32)vClass ? lux::__pvt::bufferSize : vSize;
						Main printf("allocated MBs: %d", allocated / 1000000);
					}


					cell = &buffer.cells[cellIndex = buffer.cells.add(Cell_t{
						.cellSize = vSize,
						.bufferType = &buffers[typeIndex]
					})];
					address = (type*)(cell->address = (void*)((uint8*)(cell->buffer = &buffer)->memory + getCellOffset(cell)));	//<^ Create a new cell in the new buffer. Set its address
					cell->cellIndex = (uint32)vClass ? cellIndex : 0;											//Set its index. 0 for custom count cells
				}
			}



			inline void alloc_call(const uint64 vSize, CellClass vClass = CellClass::AUTO){
				if(!vSize) {
					alloc_internal(1, vClass);
					cell->cellSize = 0;
				}
				else alloc_internal(vSize, vClass);
			}



			//TODO UNROLL LOOP
			inline void init_memory(void* const vAddr, const uint64 vSize, const type& pValue){
				for(uint32 i = 0; i < vSize; i+=sizeof(type)) memcpy((char*)vAddr + i, &pValue, sizeof(type));
			}




		public:

			//Reallocates the pointer to a block of memory of vSize bytes without initializing it
			//e.g.
			//lux::ram::ptr<int32> p(4);	//Allocate 4B (1 int)
			//p.reallocBck(100);			//Reallocate to 100B (4 copied + 96 uninitialized) (25 ints)
			inline void realloc(const uint64 vSize, CellClass vCellClass = CellClass::AUTO) {
				evaluateCellClass(vSize, vCellClass);

				if(!address) { [[unlikely]]						//If the pointer has not been allocated
					alloc_call(vSize, vCellClass);					//Allocate it
					return;
				}
				else { 													//If it's allocated
					int64 d = vSize - size( );							//Calculate the difference in size between the current size and the new size
					if(d < 0) [[unlikely]] cell->cellSize = vSize;		//If the new size is smaller, change the cellSize variable and return
					else if(d > 0) { [[likely]]							//If it's larger
						if(vSize <= (int64)vCellClass) { [[likely]]			//But not larger than the maximum cell size
							cell->cellSize = vSize;								//Change the cellSize variable
						}
						else {												//If it's also larger than the cell
							ram::ptr<type> ptr_(vSize, vCellClass);				//Allocate a new pointer
							memcpy(ptr_, address, size( ));						//Copy the old data
							free();												//Free the old cell
							*this = ptr_;										//Overwrite the cell itself. This is necessary in order to keep the pointers updated
						}
					}
					else [[unlikely]] return;							//If it has the same size, do nothing
				}

			}


			//Reallocates the pointer to a block of memory of vSize bytes and initializes each of the new elements with the pValue value
			//e.g.
			//lux::ram::ptr<float> p(4);	//Allocate 4B (1 float)
			//p.realloc(100, 0.1f);			//Reallocate to 100B (4 copied + 96 as floats with value 0.1f) (25 floats)
			inline void realloc(const uint64 vSize, const type& pValue, CellClass vCellClass = CellClass::AUTO){
				if(address){
					int64 d = vSize - size( );
					auto end_ = end();
					realloc(vSize, vCellClass);
					if(d > 0) init_memory(end_, d, pValue);
				}
				else {
					evaluateCellClass(vSize, vCellClass);
					alloc_call(vSize, vCellClass);
					init_memory(address, size(), pValue);
				}
			}




			//Reallocates the pointer to a block of memory containing vCount elements without initializing them
			//e.g.
			//lux::ram::ptr<float> p(100);	//Allocate 100B (25 floats)
			//p.reallocArr(100);			//Reallocate to 100 floats (400B) (25 copied + 75 uninitialized)
			void reallocArr(const uint64 vCount, const CellClass vCellClass = CellClass::AUTO){
				realloc(sizeof(type) * vCount, vCellClass);
			}


			//Reallocates the pointer to a block of memory containing vCount elements initializing them with the pValue value
			//e.g.
			//lux::ram::ptr<float> p(100);	//Allocate 100B (25 floats)
			//p.reallocArr(100, 0.1f);		//Reallocate to 100 floats (400B) (25 copied + 75 with value 0.1)
			inline void reallocArr(const uint64 vCount, const type& pValue, CellClass vCellClass = CellClass::AUTO){
				realloc(sizeof(type) * vCount, pValue, vCellClass);
			}






			void free(){
			//TODO destroy buffers from asyncrhonous garbage collector
				cell->buffer->cells.remove(cell->cellIndex);
				if(cell->buffer->cells.usedSize( ) == 0) {
					cell->bufferType->buffers.remove(cell->buffer->bufferIndex);
					//TODO free only if there is not enough memory
				}
			}

		};
	}
}