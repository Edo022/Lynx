#pragma once
#define LUX_H_POINTER
#include "LuxEngine/Core/Memory/Ram/Cell_t.hpp"
#include "LuxEngine/System/SystemMacros.hpp"
#include "cstring"
//TODO add option to disable specific warnings
//TODO add macro to disable specific warnings on specific object instances














// unspecialized pointer -----------------------------------------------------------------------------------------------------------------//
















namespace lux::ram{
	#define alloc 0
	#define addr  1


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
			constexpr inline bool operator==(const ptr_b<pType>& pPtr) noexcept const { checkInit(); return (void*)address == (void*)pPtr.address; }
			constexpr inline bool operator==(ptr_b<type> pPtr        ) noexcept const { checkInit(); return pPtr.address   == address;             }
			constexpr inline bool operator==(const type* vPtr        ) noexcept const { checkInit(); return (void*)address == (void*)vPtr;         }
			template<class pType>
			constexpr inline bool operator!=(const ptr_b<pType>& pPtr) noexcept const { checkInit(); return (void*)address != (void*)pPtr.address; }
			constexpr inline bool operator!=(ptr_b<type> pPtr        ) noexcept const { checkInit(); return pPtr.address   != address;             }
			constexpr inline bool operator!=(const type* vPtr        ) noexcept const { checkInit(); return (void*)address != (void*)vPtr;         }




			//TODO check if it actually uses the conversion when calling operators
			//Implicit conversion
			inline operator type*( ) const { checkInit(); return this->address;   }		//ram::ptr<type> to type* implicit conversion
			inline operator bool(  ) const { checkInit(); return !!this->address; }		//ram::ptr<type> to bool  implicit conversion (e.g. if(ptr) is the same as if(ptr != nullptr), like normal pointers)
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
	#define checka luxDebug(																									\
		if(this->address && ((uint64)this->address < (uint64)cell->address)){													\
			if(cell->cellSize) luxCheckCond((uint64)this->address >= ((uint64)cell->address) + cell->cellSize, __checka__err__)	\
			else               luxCheckCond((uint64)this->address !=  (uint64)cell->address  + cell->cellSize, __checka__err__)	\
		}																														\
	);

	#define checkAllocSize(var, _class) luxDebug(if(_class != lux::CellClass::AUTO){											\
		luxCheckParam(var > 0xFFFFffff, var, "Allocation size cannot exceed 0xFFFFFFFF bytes. The given size was %llu", var);	\
		luxCheckParam((uint32)_class < var, _class,																				\
			"%lu-bytes class specified for %llu-bytes allocation. The cell class must be large enought to contain the bytes. %s"\
			, (uint32)_class, var, "Use lux::CellClass::AUTO to automatically choose it"										\
		)});


	/**
	 * @brief alloc specialization of lux::ram::ptr.
	 *		This pointer can only be initialized with an allocation or another alloc pointer.
	 *		The owned memory block can only be changed by calling the .realloc function or with an assignment.
	 *		Adding or subtracting values do not change the memory block.
	 *		The memory becomes invalid when all the alloc pointers pointing to it go out of scope, or .free() is called.
	 * @tparam type The type of data the pointer points to (int, float, etc... )
	 */
	template<class type> struct ptr<type, alloc> : public __pvt::ptr_b<type> {
	private:
		genInitCheck;
		inline ptr(Cell_t* _cell, type* address_) : cell{_cell  } { this->address = address_; }
		//THOSE FUNCTIONS SET CELL AND ADDRESS TO NULLPTR, REGARDLESS OF THE VALUE OF ADDRESS_
		inline ptr(type* address_) : cell{nullptr} { this->address = nullptr; }
		inline void operator=(type* address_) { cell = this->address = nullptr; }

		//Memory allocation
		void evaluateCellClass(const uint64 vSize, CellClass& pClass);
		void alloc_(const uint64 vSize, const CellClass vClass);
		inline void init_memory(void* const vAddr, const uint64 vSize, const type& pValue){
			for(uint32 i = 0; i < vSize; i+=sizeof(type)) memcpy((char*)vAddr + i, &pValue, sizeof(type));
		}




		// Constructors ------------------------------------------------------------------------------------------------------------------//




	public:
		Cell_t* cell; //A pointer to a lux::ram::Cell_t object that contains the cell informations




		/**
		 * @brief Creates a nullptr ptr.
		 *		Initialize it with the .realloc function before accessing its memory
		 */
		inline ptr( ) : cell{  nullptr } { this->address = nullptr; }




		/**
		 * @brief Copy constructor. This function will only copy the pointer address and cell, not the data it points to
		 * @param pPtr The pointer to copy. It must be a valid alloc lux::ram::ptr instance
		 */
		inline ptr(const ptr<type, alloc>& pPtr) : constructExec(isInit(pPtr)),
			cell{ pPtr.cell } {
			this->address = pPtr.address;
			if(pPtr){ cell->owners++; checka; }
		}
		/**
		 * @brief Create a pointer by copying another pointer's address and cell
		 * @param pPtr The pointer to copy. It must be a valid alloc lux::ram::ptr instance of a compatible type
		 */
		template<class pType> explicit inline ptr(const ptr<pType, alloc> pPtr) : constructExec(isInit(pPtr)),
			cell{ pPtr.cell } {
			this->address = (type*)pPtr.address;
			if(pPtr){ cell->owners++; checka; }
		}




		/**
		 * @brief Move constructor
		 */
		inline ptr(ptr<type, alloc>&& pPtr) : constructExec(isInit(pPtr)),
			cell{ pPtr.cell }{				pPtr.cell    = nullptr;
			this->address = pPtr.address;	pPtr.address = nullptr;
			checka;
		}




		/**
		 * @brief Allocates a block of memory without initializing it.
		 * 		e.g. lux::ram::ptr<int32> p(302732);
		 * @param vSize  Size of the block in bytes. It must be positive and less than 0xFFFFFFFF
		 * @param vClass Class of the allocation. It must be a valid lux::CellClass value. Default: AUTO
		 */
		inline ptr(const uint64 vSize, CellClass vClass = CellClass::AUTO){
			checkAllocSize(vSize, vClass);
			evaluateCellClass(vSize, vClass);
			alloc_(vSize, vClass); //BUG HERE
			++cell->owners;
		}
		
		/**
		 * @brief Allocates a block of memory and initializes each element with the pValue value.
		 *		e.g.lux::ram::ptr<float> p(302732, 15.0f);
		 * @param vSize  Size of the block in bytes. It must be positive and less than 0xFFFFFFFF
		 * @param pValue Value each element will be initialized with
		 * @param vClass Class of the allocation. It must be a valid lux::CellClass value. Default: AUTO
		 */
		inline ptr(const uint64 vSize, const type& pValue, CellClass vClass = CellClass::AUTO) : ptr(vSize, vClass) {
			// checkAllocSize(vSize, vClass); //Checked in the first constructor
			init_memory(cell->address, vSize, pValue);
			// ++cell->owners; //Set in the first constructor
		}




		// Assignment --------------------------------------------------------------------------------------------------------------------//
		//TODO add dummy cell




		//Move assignment
		inline void operator=(ptr<type, alloc>&& pPtr){
			isInit(pPtr);
			cell = pPtr.cell;				pPtr.cell = nullptr;
			this->address = pPtr.address;	pPtr.address = nullptr;
			checka;
		}
		//Copy assignment
		inline void operator=(const ptr<type, alloc>& pPtr){
			isInit(pPtr);
			if(cell) cell->owners--;
			cell = pPtr.cell; this->address = pPtr.address;
			if(cell) cell->owners++;
			checka;
		}
		//Assignment
		template<class ptrType> explicit inline void operator=(const ptr<ptrType, alloc>& pPtr){
			isInit(pPtr);
			if(cell) cell->owners--;
			cell = pPtr.cell; this->address = (type*)pPtr.address;
			if(cell) cell->owners++;
			checka;
		}

		//different types of pointers are converted with the explicit conversion operator




		// Add, subtract -----------------------------------------------------------------------------------------------------------------//




		inline ptr<type, alloc> operator++(int) noexcept { checkInit(); luxDebug(this->address++; checkp; this->address--) return ptr<type, alloc>(cell, this->address++); }
		inline ptr<type, alloc> operator--(int) noexcept { checkInit(); luxDebug(this->address--; checkm; this->address++) return ptr<type, alloc>(cell, this->address--); }
		inline ptr<type, alloc> operator++(   ) noexcept { checkInit(); luxDebug(this->address++; checkp; this->address--) return ptr<type, alloc>(cell, ++this->address); }
		inline ptr<type, alloc> operator--(   ) noexcept { checkInit(); luxDebug(this->address--; checkm; this->address++) return ptr<type, alloc>(cell, --this->address); }
		
		template<class pType> inline void operator+=(const pType* vPtr) noexcept { checkInit(); this->addres += vPtr; checkp; }
		template<class vType> inline void operator+=(const vType  vVal) noexcept { checkInit(); this->addres += vVal; checkp; }
		template<class pType> inline void operator-=(const pType* vPtr) noexcept { checkInit(); this->addres -= vPtr; checkm; }
		template<class vType> inline void operator-=(const vType  vVal) noexcept { checkInit(); this->addres -= vVal; checkm; }
		
		template<class pType> inline ptr<type, alloc> operator+(const __pvt::ptr_b<pType> pPtr) noexcept const { checkInit(); return ptr<type, alloc>(cell, this->address + pPtr.address); }
		template<class pType> inline ptr<type, alloc> operator+(const pType*              vPtr) noexcept const { checkInit(); return ptr<type, alloc>(cell, this->address + vPtr);         }
		template<class vType> inline ptr<type, alloc> operator+(const vType               vVal) noexcept const { checkInit(); return ptr<type, alloc>(cell, this->address + vVal);         }
		template<class pType> inline ptr<type, alloc> operator-(const __pvt::ptr_b<pType> pPtr) noexcept const { checkInit(); return ptr<type, alloc>(cell, this->address - pPtr.address); }
		template<class pType> inline ptr<type, alloc> operator-(const pType*              vPtr) noexcept const { checkInit(); return ptr<type, alloc>(cell, this->address - vPtr);         }
		template<class vType> inline ptr<type, alloc> operator-(const vType               vVal) noexcept const { checkInit(); return ptr<type, alloc>(cell, this->address - vVal);         }
//FIXME rename address addr





		// Get element -------------------------------------------------------------------------------------------------------------------//




		inline type&     operator[](const uint64 vIndex) const { 
			checkInit(); checkNullptrD(); checkSize(); 
			luxCheckParam((vIndex < 0 && vIndex > prior( )) || (vIndex >= 0 && vIndex > latter( )), vIndex, "Index is out of range"); 
			return this->address[vIndex]; 
		}
		inline type&     operator*( ) const { checkInit(); checkNullptrD(); checkSizeD(); return *this->address; }
		inline type&	 last(      ) const { checkInit(); checkNullptr(); checkSize();  return ((type*)this->address)[count( ) - 1];	 } //Returns a reference to the last element in the allocated memory block
		inline type*	 begin(     ) const { checkInit(); checkNullptr(); return (type*)cell->address;							 } //Returns the first address of the allocated memory block as a normal pointer
		inline ptr<type> beginp(    ) const { checkInit(); checkNullptr(); return ptr<type>(cell);								 } //Returns the first address of the allocated memory block as a lux::ram::ptr
		inline type*	 end(       ) const { checkInit(); checkNullptr(); return (type*)((int8*)cell->address + cell->cellSize);} //Returns the address of the object past the last object in the memory block as a normal pointer. Don't dereference it
		inline ptr<type> endp(      ) const { checkInit(); checkNullptr(); return ptr<type>(cell, (type*)((int8*)cell->address + cell->cellSize));} //Returns the address of the object past the last object in the memory block as a lux::ram::ptr. Don't dereference it

//BUG READ. FIX ALL CONSTRUCTORS. WRITE SPECIFIC MEMORY ARRAY STRUCT


		// Count and size ----------------------------------------------------------------------------------------------------------------//




		inline uint64 size(         ) noexcept const { checkInit(); checkNullptr(); return cell->cellSize;								 } //Returns the total size of the allocated memory
		inline uint64 count(        ) noexcept const { checkInit(); checkNullptr(); return cell->cellSize / sizeof(type);				 } //Returns the number of elements in the allocated memory (use this only if you have allocated the memory with an allocArr function or you are sure that the size is a multiple of the type's size)
		inline uint64 prior(        ) noexcept const { checkInit(); checkNullptr(); return (uint64)this->address - (uint64)cell->address;		 } //Returns the number of allocated bytes before the pointer
		inline uint64 latter(       ) noexcept const { checkInit(); checkNullptr(); return (uint64)cell->address + cell->cellSize - (uint64)this->address;	} 	//Returns the number of allocated bytes after  the pointer




		// Copy --------------------------------------------------------------------------------------------------------------------------//




		/**
		 * @brief Returns an alloc ram::ptr of the same type, pointing to a new memory block that contains a copy of the pointer data.
		 *		If the pointer is null, the new pointer will also be null and have no allocated block
		 */
		inline ptr<type, alloc> deepCopy() const {
			if(this->address) {
				ptr<type, alloc> _ptr(cell->cellSize);
				ram::cpy(this->address, _ptr, size());
				return _ptr;
			}
			else return ptr<type, alloc>(nullptr, nullptr);
		}



		// Destructor --------------------------------------------------------------------------------------------------------------------//




		inline ~ptr( ) noexcept {
			if(this->address) {
				if(!--cell->owners) cell->free( );
			}
		}		
		//BUG THIS. THIS IS THE BUG. LOCAL VARIABLES CALL THE DESTRUCTOR BEFORE GETTING RETURNED




		//TODO CHECK MEMORY FULL IN alloc_
		// Reallocation ------------------------------------------------------------------------------------------------------------------//




		/**
		 * @brief Reallocates the pointer to a block of memory of vSize bytes without initializing it
		 * @param vSize  Size of the block in bytes. It must be positive and less than 0xFFFFFFFF
		 * @param vClass Class of the allocation. It must be a valid lux::CellClass value. Default: AUTO
		 */
		void realloc(const uint64 vSize, CellClass vClass = CellClass::AUTO);


		/**
		 * @brief Reallocates the pointer to a block of memory of vSize bytes and initializes each of the new elements with the pValue value
		 * @param vSize  Size of the block in bytes. It must be positive and less than 0xFFFFFFFF
		 * @param pValue Value each element will be initialized with
		 * @param vClass Class of the allocation. It must be a valid lux::CellClass value. Default: AUTO
		 */
		inline void realloc(const uint64 vSize, const type& pValue, CellClass vClass = CellClass::AUTO){
			checkInit(); checkAllocSize(vSize, vClass);
			if(this->address){
				int64 d = vSize - size( );
				auto end_ = end();
				realloc(vSize, vClass);
				if(d > 0) init_memory(end_, d, pValue);
			}
			else {
				evaluateCellClass(vSize, vClass);
				alloc_(vSize, vClass);
				init_memory(this->address, size(), pValue);
			}
		}


		/**
		 * @brief Reallocates the pointer to a block of memory containing vCount elements without initializing them
		 * @param vCount Number of elements
		 * @param vClass Class of the allocation. It must be a valid lux::CellClass value. Default: AUTO
		 */
		void reallocArr(const uint64 vCount, const CellClass vClass = CellClass::AUTO){
			checkInit(); checkAllocSize(vSize, vClass);
			realloc(sizeof(type) * vCount, vClass);
		}


		/**
		 * @brief Reallocates the pointer to a block of memory containing vCount elements initializing them with the pValue value
		 * @param vCount Number of elements
		 * @param pValue Value each element will be initialized with
		 * @param vClass Class of the allocation. It must be a valid lux::CellClass value. Default: AUTO
		 */
		inline void reallocArr(const uint64 vCount, const type& pValue, CellClass vClass = CellClass::AUTO){
			checkInit(); checkAllocSize(vSize, vClass);
			realloc(sizeof(type) * vCount, pValue, vClass);
		}




		// Free memory -------------------------------------------------------------------------------------------------------------------//




		/**
		 * @brief Frees the memory block owned by the pointer.
		 *		The memory will become invalid and unaccessible for any other pointer currently using it
		 */
		inline void free(){ cell->free(); }
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
			else if(vSize <= (uint32)CellClass::CLASS_D) [[likely]]   pClass = CellClass::CLASS_D;
			else if(vSize <= (uint32)CellClass::CLASS_Q) [[unlikely]] pClass = CellClass::CLASS_Q;
			else if(vSize <= (uint32)CellClass::CLASS_L) [[unlikely]] pClass = CellClass::CLASS_L;
			else										 [[unlikely]] pClass = CellClass::CLASS_0;
		} //TODO use direct access array
	}







	//TODO CHECK MEMORY FULL
	template<class type> void lux::ram::ptr<type, alloc>::alloc_(const uint64 vSize, const CellClass vClass){
		uint32 typeIndex = lux::__pvt::classIndexFromEnum(vClass);							//Get buffer index from type and class
		__nmp_RaArray<MemBuffer, uint32, 32>& subBuffers = lux::ram::buffers[typeIndex].buffers;	//Get list of buffers where to search for a free cell
		uint32 cellIndex;
		if((uint32)vClass){																	//If the cell is a fixed count cell
			uint64 cellNum = lux::__pvt::bufferSize / (uint32)vClass;							//Get the maximum number of cells of each buffer
			for(uint32 i = 0; i < subBuffers.count( ); i++){										//Search for a suitable buffer
//BUG OK ^
//BUG DONT CHECK FOR EVERY CELL. JUST ADD A NEW ONE. THE ARRAY WILL DO EVERYTHING
				if(subBuffers.isValid(i) && (subBuffers[i].cells.usedCount( ) < cellNum)) {			//If a buffer is valid and it has a free cell
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
		}{																					//If there are no free buffers or the cell is a custom count cell
			uint32 cellsNum = (uint32)vClass ? lux::__pvt::bufferSize / (uint32)vClass : 1;		//Create 1 cell in it for custom count cells, or the maximum number of cells for fixed count cells
			auto[bufferIndex, init] = subBuffers.add();											//Save the buffer index
			MemBuffer& buffer = subBuffers[bufferIndex]; buffer.bufferIndex = bufferIndex;		//Save the buffer object

			if(init) {	//Initialize the buffer if it's a new element
				//TODO CLEANUP COMMENTS
				//FIXME USE A NORMAL RaArray THAT ALLOCATES WITH MALLOC
				//! The map requires the chunk count and the max count. bufferSize is the count in bytes of the whole buffer, not the number of cells. The number of cells is (bufferSize / (uint32)vClass)
				//.cells = (uint32)vClass ? Map_NMP_S<Cell_t, uint32>(max(/*384*/24576, cellsNum), cellsNum) : Map_NMP_S<Cell_t, uint32>(1, 1),
				buffer.cells = (uint32)vClass ? __nmp_RaArray<Cell_t, uint32, 32>() : __nmp_RaArray<Cell_t, uint32, 32>(), //FIXME USE CASTED POINTER INSTEAD OF NMP_RAARRAY

				buffer.memory = //Allocate new memory if the buffer has not already been allocated
					win10(_aligned_malloc((uint32)vClass ? lux::__pvt::bufferSize : vSize, LuxMemOffset))
					linux( aligned_alloc( LuxMemOffset, (uint32)vClass ? lux::__pvt::bufferSize : vSize))
				;

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








	template<class type> void lux::ram::ptr<type, alloc>::realloc(const uint64 vSize, CellClass vClass) {
		checkInit(); checkAllocSize(vSize, vClass);
		evaluateCellClass(vSize, vClass);

		if(!this->address) { [[unlikely]]				//If the pointer has not been allocated
			alloc_(vSize, vClass);					//Allocate it
			return;
		}
		else { 													//If it's allocated
			int64 d = vSize - size( );							//Calculate the difference in size between the current size and the new size
			if(d < 0) [[unlikely]] cell->cellSize = vSize;		//If the new size is smaller, change the cellSize variable and return
			else if(d > 0) { [[likely]]							//If it's larger
				if(vSize <= (int64)vClass) { [[likely]]			//But not larger than the maximum cell size
					cell->cellSize = vSize;								//Change the cellSize variable
				}
				else {												//If it's also larger than the cell
					ram::ptr<type, alloc> ptr_(vSize, vClass);		//Allocate a new pointer
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
		template<class ptrType> constexpr inline ptr(__pvt::ptr_b<type> vPtr){
			isInit(vPtr);
			this->address = vPtr.address;
		}


		///@param vPtr A C pointer used to initialize the pointer
		template<class ptrType> constexpr inline ptr(ptrType* vPtr){
			luxCheckRawPtr(vPtr, ptrType, "invalid pointer passed to constructor");
			this->address = vPtr;
		}



		constexpr inline void operator=(__pvt::ptr_b<type> vPtr){
			isInit(vPtr);
			this->address = vPtr.address;
		}
		
		template<class ptrType> constexpr inline void operator=(ptrType* vPtr){
			luxCheckRawPtr(vPtr, ptrType, "invalid pointer passed to operator=");
			this->address = vPtr;
		}




		constexpr inline type* operator++(int) noexcept { checkInit(); return   this->address++; }
		constexpr inline type* operator++(   ) noexcept { checkInit(); return ++this->address;   }
		constexpr inline type* operator--(int) noexcept { checkInit(); return   this->address--; }
		constexpr inline type* operator--(   ) noexcept { checkInit(); return --this->address;   }
		
		template<class pType> constexpr inline void operator+=(const pType* vPtr) noexcept { checkInit(); this->addres += vPtr; }
		template<class vType> constexpr inline void operator+=(const vType  vVal) noexcept { checkInit(); this->addres += vVal; }
		template<class pType> constexpr inline void operator-=(const pType* vPtr) noexcept { checkInit(); this->addres += vPtr; }
		template<class vType> constexpr inline void operator-=(const vType  vVal) noexcept { checkInit(); this->addres += vVal; }
		
		template<class pType> constexpr inline type* operator+(const __pvt::ptr_b<pType> pPtr) noexcept const { checkInit(); return this->address + pPtr.address; }
		template<class pType> constexpr inline type* operator+(const pType*              vPtr) noexcept const { checkInit(); return this->address + vPtr;         }
		template<class vType> constexpr inline type* operator+(const vType               vVal) noexcept const { checkInit(); return this->address + vVal;         }
		template<class pType> constexpr inline type* operator-(const __pvt::ptr_b<pType> pPtr) noexcept const { checkInit(); return this->address - pPtr.address; }
		template<class pType> constexpr inline type* operator-(const pType*              vPtr) noexcept const { checkInit(); return this->address - vPtr;         }
		template<class vType> constexpr inline type* operator-(const vType               vVal) noexcept const { checkInit(); return this->address - vVal;         }




		inline type& operator[](const uint64 vIndex) const { checkInit(); checkNullptrD(); return this->address[vIndex]; }
		inline type& operator* (                   ) const { checkInit(); checkNullptrD(); return *this->address; }
	};
}