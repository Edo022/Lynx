#pragma once
#define LUX_H_POINTER
#include "LuxEngine/Core/Memory/Ram/Cell_t.hpp"
//TODO add option to disable specific warnings
//TODO add macro to disable specific warnings on specific object instances














// unspecialized pointer -----------------------------------------------------------------------------------------------------------------//
















namespace lux::ram{
	#define alloc 0
	#define addr  1
	// #define stack 0
	// #define heap  1


	/**
	 * @brief Like a normal pointer, but better.
	 * @tparam type The type of data the pointer points to (int, float, etc... )
	 * @tparam kind Can be "alloc" or "addr". alloc pointers allocate memory and can be freed.
	 * 		addr ones can only point to it.
	 * 		A block of memory becomes invalid when all the alloc pointers pointing to it go out of scope, or .free() is called.
	 * 		alloc pointers can only be initialized with an allocation or another alloc pointer.
	 * 		addr  pointers can only be initialized with a normal C pointer or an alloc/addr pointer.
	 */
	template<class type, uint32 kind = addr> struct ptr{
		//UwU
	};
	// namespace __pvt { template<class type> struct ptr_b; }
	// template<class type> struct ptr<type, alloc> : __pvt::ptr_base<type>;
	// template<class type> struct ptr<type, addr > : __pvt::ptr_base<type>;
















// Parent class --------------------------------------------------------------------------------------------------------------------------//
















	namespace __pvt{
		#define checkSize()     luxCheckCond(size( ) == 0, "This function cannot be called on pointers pointing to a 0-bytes memory block"    )
		#define checkSizeD()    luxCheckCond(size( ) == 0, "Cannot dereference a pointer pointing to a 0-byte memory allocation"              )




		template<class type> struct ptr_b{
			genInitCheck;
			type* address;			//The address the pointer points to


			template<class pType>
			inline bool operator==(const ptr_b<pType>& pPtr) const { checkInit(); return (void*)address == (void*)pPtr.address; }
			inline bool operator==(ptr_b<type> pPtr        ) const { checkInit(); return pPtr.address   == address;             }
			inline bool operator==(const type* vPtr        ) const { checkInit(); return (void*)address == (void*)vPtr;         }
			template<class pType>
			inline bool operator!=(const ptr_b<pType>& pPtr) const { checkInit(); return (void*)address != (void*)pPtr.address; }
			inline bool operator!=(ptr_b<type> pPtr        ) const { checkInit(); return pPtr.address   != address;             }
			inline bool operator!=(const type* vPtr        ) const { checkInit(); return (void*)address != (void*)vPtr;         }


			//Add, subtract
			template<class valType>
			inline ptr_b<type> operator+(const valType vVal) const { checkInit(); return ptr_b<type>{ .address = (type*)((uint64)address + vVal) };}
			inline uint64 operator+(const type*        vPtr) const { checkInit();               return (uint64)address + (uint64)vPtr;             }
			inline uint64 operator+(const ptr_b<type>& pPtr) const { checkInit(); isInit(pPtr); return (uint64)address + (uint64)pPtr.address;     }
			template<class valType>
			inline ptr_b<type> operator-(const valType vVal) const { checkInit(); return ptr_b<type>{ .address = (type*)((uint64)address - vVal) };}
			inline uint64 operator-(const type*        vPtr) const { checkInit();               return (uint64)address - (uint64)vPtr;             }
			inline uint64 operator-(const ptr_b<type>& pPtr) const { checkInit(); isInit(pPtr); return (uint64)address - (uint64)pPtr.address;     }


			//Implicit conversion
			inline operator type*( ) const { checkInit(); luxCheckCond(!this->address, "Cannot dereference a nullptr"); return this->address; }	//ram::ptr<type> to type* implicit conversion
			inline operator bool(  ) const { checkInit(); return !!this->address; }		//ram::ptr<type> to bool implicit conversion (e.g. if(ptr) is the same as if(ptr != nullptr), like normal pointers)
		};
	}
















// alloc pointer -------------------------------------------------------------------------------------------------------------------------//
//TODO ADD WARNING WHEN ACCESSING FREED MEMORY. SPECIFY HOW IT WAS FREED
//TODO improve warnings and output object address or nanme















	#define checkNullptr()  luxCheckCond(!this->address,     "function %s  have been called on a lux::ram::ptr with value nullptr", __FUNCTION__)
	#define checkNullptrD() luxCheckCond(!this->address,     "Cannot dereference a null pointer"                                                )

	#define checkp luxDebug(if(this->address >= end( ) + cell->cellSize) luxPrintWarning("A lux::ram::ptr has probably been increased too much and now points to an unallocated address. Reading or writing to this address is undefined behaviour and can cause runtime errors"))
	#define checkm luxDebug(if(this->address < begin( ))                 luxPrintWarning("A lux::ram::ptr has probably been decreased too much and now points to an unallocated address. Reading or writing to this address is undefined behaviour and can cause runtime errors"))

	#define __checka__err__ "The assigned address is out of the allocated block range. Reading or writing to this address is undefined behaviour and can cause runtime errors"
	#define checka luxDebug(																								\
		if(this->address && ((uint64)this->address < (uint64)cell->address)){															\
			if(cell->cellSize) luxCheckCond((uint64)this->address >= ((uint64)cell->address) + cell->cellSize, __checka__err__)	\
			else               luxCheckCond((uint64)this->address !=  (uint64)cell->address  + cell->cellSize, __checka__err__)	\
		}																													\
	);

	/**
	 * @brief alloc specialization of lux::ram::ptr.
	 *		This pointer can only be initialized with an allocation or another alloc pointer.
	 *		The owned memory block can only be changed by calling the .realloc function or with an assignment.
	 *		Adding or subtracting values do not change the memory block.
	 *		The memory becomes invalid when all the alloc pointers pointing to it go out of scope, or .free() is called.
	 * @tparam type The type of data the pointer points to (int, float, etc... )
	 */
	template<class type> struct ptr<type, alloc> : public __pvt::ptr_b<type> {
		genInitCheck;
		Cell_t* cell;			//A pointer to a lux::ram::Cell_t object that contains the cell informations




		inline ptr( ) : cell{  nullptr } { this->address = nullptr; }
		template<class pType> explicit
		inline ptr(ptr<pType, alloc>& pPtr) { isInit(pPtr); cell = pPtr.cell; this->address = (type*)pPtr.address; if(pPtr){ cell->owners++; checka; } }
		// inline ptr(ptr<type , alloc>& pPtr) { isInit(pPtr); cell = pPtr.cell; this->address =        pPtr.address; if(pPtr){ cell->owners++; checka; } }


		/**
		 * @brief Allocates a block of memory without initializing it.
		 * 		e.g. lux::ram::ptr<int32> p(302732);
		 * @param vSize  Size of the block in bytes
		 * @param vClass Class of the allocation
		 */
		inline ptr(const uint64 vSize, CellClass vClass = CellClass::AUTO){
			evaluateCellClass(vSize, vClass);
			alloc_(vSize, vClass);
			++cell->owners;
		}

		/**
		 * @brief Allocates a block of memory and initializes each element with the pValue value.
		 *		e.g.lux::ram::ptr<float> p(302732, 15.0f);
		 * @param vSize  Size of the block in bytes
		 * @param pValue Value each element is initialized with
		 * @param vClass Class of the allocation
		 */
		inline ptr(const uint64 vSize, const type& pValue, CellClass vClass = CellClass::AUTO) : ptr(vSize, vClass) {
			init_memory(cell->address, vSize, pValue);
			++cell->owners;
		}




		//Assignment and comparison
		// inline void operator=(Cell_t* const   vCell){
		// 	luxCheckRawPtr(vCell, Cell_t, "Invalid pointer passed to assignment operator")
		// 	if(cell) cell->owners--;	cell = vCell;		address = (type*)vCell->address;	if(cell) cell->owners++; checka; }
		inline void operator=(const ptr<type, alloc> pPtr){
			isInit(pPtr);
			if(cell) cell->owners--;
			cell = pPtr.cell; this->address = pPtr.address;
			if(cell) cell->owners++;
			checka;
			//TODO add dummy cell
		}
		//different types of pointers are converted with the explicit conversion operator


		//Add, subtract
		template<class valType> inline void operator+=(valType vVal){ checkInit(); this->address += vVal; checkp; }
		template<class valType> inline void operator-=(valType vVal){ checkInit(); this->address -= vVal; checkm; }
		inline void operator++( ){ checkInit(); this->address++; checkp; }
		inline void operator--( ){ checkInit(); this->address--; checkm; }


		//Get element
		inline type&     operator[](const uint64 vIndex)const { checkInit(); checkNullptrD(); checkSize(); luxCheckParam((vIndex < 0 && vIndex > prior( )) || (vIndex >= 0 && vIndex > latter( )), vIndex, "Index is out of range"); return this->address[vIndex]; }
		inline type&     operator*( ) const { checkInit(); checkNullptrD(); checkSizeD(); return *this->address; }
		inline type&	 last(      ) const { checkInit(); checkNullptr(); checkSize();  return ((type*)this->address)[count( ) - 1];	 } //Returns a reference to the last element in the allocated memory block
		inline type*	 begin(     ) const { checkInit(); checkNullptr(); return (type*)cell->address;							 } //Returns the first address of the allocated memory block as a normal pointer
		inline ptr<type> beginp(    ) const { checkInit(); checkNullptr(); return ptr<type>(cell);								 } //Returns the first address of the allocated memory block as a lux::ram::ptr
		inline type*	 end(       ) const { checkInit(); checkNullptr(); return (type*)((int8*)cell->address + cell->cellSize);} //Returns the address of the object past the last object in the memory block as a normal pointer. Don't dereference it
		inline ptr<type> endp(      ) const { checkInit(); checkNullptr(); return ptr<type>(cell, (type*)((int8*)cell->address + cell->cellSize));} //Returns the address of the object past the last object in the memory block as a lux::ram::ptr. Don't dereference it

		//Count
		inline uint64 size(         ) const { checkInit(); checkNullptr(); return cell->cellSize;								 } //Returns the total size of the allocated memory
		inline uint64 count(        ) const { checkInit(); checkNullptr(); return cell->cellSize / sizeof(type);				 } //Returns the number of elements in the allocated memory (use this only if you have allocated the memory with an allocArr function or you are sure that the size is a multiple of the type's size)
		inline uint64 prior(        ) const { checkInit(); checkNullptr(); return (uint64)this->address - (uint64)cell->address;		 } //Returns the number of allocated bytes before the pointer
		inline uint64 latter(       ) const { checkInit(); checkNullptr(); return (uint64)cell->address + cell->cellSize - (uint64)this->address;	} 	//Returns the number of allocated bytes after  the pointer

		//Destructor
		~ptr( ){ if(this->address) if(!--cell->owners) cell->free( ); }		//Decrease the cell's owner count when the pointer is destroyed






		//TODO CHECK MEMORY FULL IN alloc_
		//TODO fix all the documentation
		//TODO UNROLL LOOP in init_memory or use AVX2
	private:
		void evaluateCellClass(const uint64 vSize, CellClass& pClass);
		void alloc_(const uint64 vSize, const CellClass vClass);
		inline void init_memory(void* const vAddr, const uint64 vSize, const type& pValue){
			for(uint32 i = 0; i < vSize; i+=sizeof(type)) memcpy((char*)vAddr + i, &pValue, sizeof(type));
		}




	public:
		//Reallocates the pointer to a block of memory of vSize bytes without initializing it
		//e.g.
		//lux::ram::ptr<int32> p(4);	//Allocate 4B (1 int)
		//p.reallocBck(100);			//Reallocate to 100B (4 copied + 96 uninitialized) (25 ints)
		void realloc(const uint64 vSize, CellClass vCellClass = CellClass::AUTO);

		//Reallocates the pointer to a block of memory of vSize bytes and initializes each of the new elements with the pValue value
		//e.g.
		//lux::ram::ptr<float> p(4);	//Allocate 4B (1 float)
		//p.realloc(100, 0.1f);			//Reallocate to 100B (4 copied + 96 as floats with value 0.1f) (25 floats)
		inline void realloc(const uint64 vSize, const type& pValue, CellClass vCellClass = CellClass::AUTO){
			if(this->address){
				int64 d = vSize - size( );
				auto end_ = end();
				realloc(vSize, vCellClass);
				if(d > 0) init_memory(end_, d, pValue);
			}
			else {
				evaluateCellClass(vSize, vCellClass);
				alloc_(vSize, vCellClass);
				init_memory(this->address, size(), pValue);
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



		/**
		 * @brief Frees the memory block owned by the pointer.
		 *		The memory will become invalid for any other pointer currently using it
		 */
		void free(){ cell->free(); }
	};








	template<class type> void lux::ram::ptr<type, alloc>::evaluateCellClass(const uint64 vSize, CellClass& pClass){
		if(pClass != CellClass::AUTO && (uint32)pClass % LuxMemOffset == 1) {	//Check AT_LEAST values (normal class values + 1)
			if(vSize > ((uint32)pClass - 1)) pClass = CellClass::AUTO;				//If the class is too small, set it to AUTO
			else pClass = (CellClass)((uint64)pClass - 1);							//If it's large enough, assign the normal class value
		}
		if(pClass == CellClass::AUTO) { [[likely]]								//Choose cell class if it's AUTO
			if(vSize <= (uint32)CellClass::CLASS_A) [[likely]]	  pClass = CellClass::CLASS_A;
			else if(vSize <= (uint32)CellClass::CLASS_B) [[likely]]	  pClass = CellClass::CLASS_B;
			else if(vSize <= (uint32)CellClass::CLASS_C) [[likely]]	  pClass = CellClass::CLASS_C;
			else if(vSize <= (uint32)CellClass::CLASS_D) [[unlikely]] pClass = CellClass::CLASS_D;
			else if(vSize <= (uint32)CellClass::CLASS_Q) [[unlikely]] pClass = CellClass::CLASS_Q;
			else if(vSize <= (uint32)CellClass::CLASS_L) [[unlikely]] pClass = CellClass::CLASS_L;
			else													  pClass = CellClass::CLASS_0;
		}

		luxCheckParam(vSize > 0xFFFFffff, vSize, "Cell size cannot exceed 0xFFFFFFFF bytes. The given size was %llu", vSize);
		luxCheckParam((uint32)pClass < vSize, pClass,
			"Requested %lu-bytes class for %llu-bytes allocation. The cell class must be large enought to contain the cell. %s",
			(uint32)pClass, vSize, "Use lux::CellClass::AUTO to automatically choose it"
		);
	}








	//TODO CHECK MEMORY FULL
	template<class type> void lux::ram::ptr<type, alloc>::alloc_(const uint64 vSize, const CellClass vClass){
		uint32 typeIndex = lux::__pvt::classIndexFromEnum(vClass);							//Get buffer index from type and class
		Map_NMP_S<MemBuffer, uint32>& subBuffers = (lux::ram::buffers[typeIndex].buffers);	//Get list of buffers where to search for a free cell
		uint32 cellIndex;
		if((uint32)vClass){																	//If the cell is a fixed count cell
			uint64 cellNum = lux::__pvt::bufferSize / (uint32)vClass;							//Get the maximum number of cells in each buffer
			for(uint32 i = 0; i < subBuffers.size( ); i++){										//Search for a suitable buffer
				if(subBuffers.isValid(i) && (subBuffers[i].cells.usedSize( ) < cellNum)) {			//If a buffer is valid and it has a free cell
					cellIndex = subBuffers[i].cells.add(Cell_t{											//Create a new cell in the buffer
						.cellSize = vSize,
						.bufferType = &lux::ram::buffers[typeIndex]
					});
					cell = &subBuffers[i].cells[cellIndex];
					cell->buffer = &subBuffers[i];														//and set its buffer, index and address
					cell->cellIndex = cellIndex;
					this->address = (type*)(cell->address = (void*)((uint8*)(cell->buffer->memory) + getCellOffset(cell)));
					return;
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
			this->address = (type*)(cell->address = (void*)((uint8*)(cell->buffer = &buffer)->memory + getCellOffset(cell)));	//<^ Create a new cell in the new buffer. Set its address
			cell->cellIndex = (uint32)vClass ? cellIndex : 0;											//Set its index. 0 for custom count cells
		}
	}








template<class type> void lux::ram::ptr<type, alloc>::realloc(const uint64 vSize, CellClass vCellClass) {
    evaluateCellClass(vSize, vCellClass);

    if(!this->address) { [[unlikely]]				//If the pointer has not been allocated
        alloc_(vSize, vCellClass);					//Allocate it
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
                ram::ptr<type, alloc> ptr_(vSize, vCellClass);		//Allocate a new pointer
                memcpy(ptr_, this->address, size( ));				//Copy the old data
                free();												//Free the old cell
                *this = ptr_;										//Overwrite the cell itself. This is necessary in order to keep the pointers updated
            }
        }
        else [[unlikely]] return;							//If it has the same size, do nothing
    }
}
















// addr pointer --------------------------------------------------------------------------------------------------------------------------//
















	/**
	 * @brief addr specialization of lux::ram::ptr. This pointer
	 *		This pointer can only be initialized with a normal C pointer or an alloc/addr pointer.
	 * @tparam type The type of data the pointer points to (int, float, etc... )
	 */
	template<class type> struct ptr<type, addr> : public __pvt::ptr_b<type>{
		genInitCheck;
		///@param vPtr A lux::ram::ptr used to initialize the pointer
		template<class ptrType> inline ptr(__pvt::ptr_b<type> vPtr){
			isInit(vPtr);
			this->address = vPtr.address;
		}
		///@param vPtr A C pointer used to initialize the pointer
		template<class ptrType> inline ptr(ptrType* vPtr){
			luxCheckRawPtr(vPtr, ptrType, "invalid pointer passed to constructor");
			this->address = vPtr;
		}


		inline void operator=(__pvt::ptr_b<type> vPtr){
			isInit(vPtr);
			this->address = vPtr.address;
		}
		template<class ptrType> inline void operator=(ptrType* vPtr){
			luxCheckRawPtr(vPtr, ptrType, "invalid pointer passed to operator=");
			this->address = vPtr;
		}




		template<class valType> inline void operator+=(valType vVal){ checkInit(); this->address += vVal; }
		template<class valType> inline void operator-=(valType vVal){ checkInit(); this->address -= vVal; }
		inline void operator++( ){ checkInit(); this->address++; }
		inline void operator--( ){ checkInit(); this->address--; }




		inline type&     operator[](const uint64 vIndex) const { checkInit(); checkNullptrD(); return this->address[vIndex]; }
		inline type&     operator* (                   ) const { checkInit(); checkNullptrD(); return *this->address; }
	};
}