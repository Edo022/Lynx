#pragma once
#define LUX_H_POINTER
#include "LuxEngine/Core/Memory/Ram/Cell_t.hpp"
#include "LuxEngine/System/SystemMacros.hpp"
#include "LuxEngine/Tests/StructureInit.hpp"
#include "LuxEngine/Tests/CondChecks.hpp"
#include "LuxEngine/Types/Dummy.hpp"
#include "cstring"
//FIXME add callble and automatically called __rearrange__ function to pack sparse cells in few buffers and free space
//FIXME or create specific array type that puts new cells in the first free element with the smallest index
//FIXME add __behaviour__ template parameter to specify those things














// alloc pointer -------------------------------------------------------------------------------------------------------------------------//
















namespace lux::ram{
	#define checkSize()     lux::dbg::checkCond(size( ) == 0, "This function cannot be called on 0-byte memory allocations")
	#define checkSizeD()    lux::dbg::checkCond(size( ) == 0, "Cannot dereference a 0-byte memory allocation"              )


	#define checkAlloc()  lux::dbg::checkCond(state == lux::__pvt::CellState::FREED,   \
		"Unable to call this function on invalid allocations: The memory block have been manually freed")
	#define isAlloc(a) dbg::checkParam(a.state == lux::__pvt::CellState::FREED, #a,\
		"Use of invalid allocation: The memory block have been manually freed")


	#define checkNullptr()  lux::dbg::checkCond(state == lux::__pvt::CellState::NULLPTR,\
		"Unable to call this function on an unallocated memory block")
	#define checkNullptrD() lux::dbg::checkCond(state == lux::__pvt::CellState::NULLPTR,\
		"Cannot dereference an unallocated memory block")


	#define checkAllocSize(var, _class) luxDebug(if(_class != lux::CellClass::CLASS_0 && _class != lux::CellClass::AUTO) {											\
		dbg::checkParam(var > 0xFFFFffff, "var", "Allocation size cannot exceed 0xFFFFFFFF bytes. The given size was %llu", var);	\
		dbg::checkParam((uint32)_class < var, "_class", "%lu-bytes class specified for %llu-bytes allocation. The cell class must be large enought to contain the bytes. %s", (uint32)_class, var, "Use lux::CellClass::AUTO to automatically choose it");\
	});




	/**
	 * @brief alloc specialization of lux::ram::ptr.
	 *		This pointer can only be initialized with an allocation or another alloc pointer.
	 *		The owned memory block can only be changed by calling the .realloc function or with an assignment.
	 *		Adding or subtracting values do not change the memory block.
	 *		The memory becomes invalid when all the alloc pointers pointing to it go out of scope, or .free() is called.
	 * @tparam type The type of data pointed by the pointer (int, float, etc... )
	 */
	template<class type> struct Alloc {
		genInitCheck;
	private:

		//Memory allocation
		constexpr static void evaluateCellClass(const uint64 vSize, CellClass& pClass) noexcept {
			if(pClass != CellClass::AUTO && (uint32)pClass % LuxMemOffset == 1) {	//Check AT_LEAST values (normal class values + 1)
				if(vSize > ((uint32)pClass)) pClass = CellClass::AUTO;					//If the class is too small, set it to AUTO
				else pClass = (CellClass)((uint64)pClass - 1);							//If it's large enough, assign the normal class value
			}
			if(pClass == CellClass::AUTO) { [[likely]]								//Choose cell class if it's AUTO
				     if(vSize <= (uint32)CellClass::CLASS_A) [[likely]]	  pClass = CellClass::CLASS_A;
				else if(vSize <= (uint32)CellClass::CLASS_B) [[likely]]	  pClass = CellClass::CLASS_B;
				else if(vSize <= (uint32)CellClass::CLASS_C) [[likely]]	  pClass = CellClass::CLASS_C;
				else if(vSize <= (uint32)CellClass::CLASS_D) [[likely]]   pClass = CellClass::CLASS_D;
				else if(vSize <= (uint32)CellClass::CLASS_Q) [[unlikely]] pClass = CellClass::CLASS_Q;
				else if(vSize <= (uint32)CellClass::CLASS_L) [[unlikely]] pClass = CellClass::CLASS_L;
				else										 			  pClass = CellClass::CLASS_0;
			} //TODO use direct access array
		}

		void alloc_(const uint64 vSize, const CellClass vClass);
		void realloc_(const uint64 vSize, const CellClass vClass);

		#ifdef LUX_DEBUG
			#define pushOwner(vAlloc) luxDebug(__pushOwner(vAlloc))
			void __pushOwner() {
				if(!cell->address) return;									//Return if the cell is nullptr
				if(!cell->firstOwner) {										//If this is the first owner of the cell
					cell->firstOwner = cell->lastOwner = (Alloc<Dummy>*)this;	//Set this as both the first and last owners
					prevOwner = nextOwner = nullptr;							//Set this prev and next owners to nullptr
				} else{														//If this is not the first owner
					prevOwner = cell->lastOwner;								//Set the cell's last owner as this prev
					nextOwner = nullptr;										//Set this next to nullptr
					cell->lastOwner->nextOwner = (Alloc<Dummy>*)this;			//Update the cell's last owner's next to this
					cell->lastOwner = (Alloc<Dummy>*)this;						//Update the cell's last owner to this
				}
			}
			#define popOwner() luxDebug(__popOwner())
			void __popOwner() {
				if(!cell->address) return;									//Return if the cell is nullptr
				if(!prevOwner && !nextOwner) {								//If this was the only owner
					cell->firstOwner = cell->lastOwner = nullptr;				//Set both the first and last owner of the cell to nullptr
				} else{														//If the cell had other owners
					if(prevOwner) prevOwner->nextOwner = nextOwner;				//If this was NOT the first owner, update the previus owner's next
					else cell->firstOwner = (Alloc<Dummy>*)nextOwner;			//If this was the first owner, update the cell's first owner
					if(nextOwner) nextOwner->prevOwner = prevOwner;				//If this was NOT the last owner, update the next owner's previous
					else cell->lastOwner = (Alloc<Dummy>*)prevOwner;			//If this was the last owner, update the cell's last owner
					prevOwner = nextOwner = nullptr;							//Set both this next and prev owners to nullptr. This is not necessary but helps debugging
				}
			}
		#endif




		// Constructors ------------------------------------------------------------------------------------------------------------------//




	public:
		Cell_t* cell; //A pointer to a lux::ram::Cell_t object that contains the cell informations //TODO add option to cache address
		luxDebug(mutable lux::__pvt::CellState state;)
		luxDebug(mutable Alloc<Dummy>* prevOwner;)
		luxDebug(mutable Alloc<Dummy>* nextOwner;)



		/**
		 * @brief Creates a nullptr ptr.
		 *		Initialize it with the .realloc function before accessing its memory
		 */
		alwaysInline Alloc( ) : cell{ &dummyCell } {
			luxDebug(prevOwner = nextOwner = nullptr;)
			luxDebug(state = lux::__pvt::CellState::NULLPTR);
		}
		alwaysInline Alloc(const std::nullptr_t null) : Alloc() {}



		/**
		 * @brief Copy constructor. This function only copies the pointer structure. The 2 pointers will share the same memory
		 * @param pPtr The pointer to copy
		 */
		alwaysInline Alloc(const Alloc<type>& vAlloc) :
			Alloc(vAlloc, {}) {
		}

		/**
		 * @brief Create a pointer by copying another pointer's address.This function only copies the pointer structure. The 2 pointers will share the same memory
		 * @param pPtr The pointer to copy
		 */
		template<class aType> explicit inline Alloc(const Alloc<aType>& vAlloc, const Dummy vDummy = Dummy{}) :
			checkInitList(isInit(vAlloc); isAlloc(vAlloc))
			cell{ vAlloc.cell } {
			cell->owners++;
			pushOwner();
			luxDebug(state = vAlloc.state);
		}




		/**
		 * @brief Move constructor
		 */
		inline Alloc(Alloc<type>&& vAlloc) : checkInitList(isInit(vAlloc); isAlloc(vAlloc))
			cell{ vAlloc.cell } { //vAlloc.cell = &dummyCell;
			//! ^ Don't reset the vAlloc cell. It's required to decrement the owners count in vAlloc destructor
			++cell->owners;
			//! ^ This is not an error. The cell's owners will get decremented when vAlloc is destroyed,
			//! so the move constructor has to increment it to make it stay the same without useless checks
			pushOwner();
			luxDebug(state = vAlloc.state);
		}




		/**
		 * @brief Allocates a block of memory without initializing it
		 * @param vSize  Size of the block in bytes. It must be a positive integer and less than 0xFFFFFFFF
		 * @param vClass Class of the allocation. Default: AUTO
		 */
		inline Alloc(const uint64 vSize, CellClass vClass = CellClass::AUTO) {
			evaluateCellClass(vSize, vClass); checkAllocSize(vSize, vClass);
			alloc_(vSize, vClass);
			// ++cell->owners; //!no. owners already set in alloc_
			pushOwner();
			luxDebug(state = lux::__pvt::CellState::ALLOC);
		}




		// Assignment --------------------------------------------------------------------------------------------------------------------//




		//Move assignment
		alwaysInline void operator=(Alloc<type>&& vAlloc) {
			operator=((Alloc<type>&)vAlloc);
		}


		//Copy assignment
		inline void operator=(const Alloc<type>& vAlloc) {
			checkInit(); isInit(vAlloc);
			if(!--cell->owners) free();
			popOwner();

			cell = vAlloc.cell; //vAlloc.cell = nullptr;
			//! ^ Don't reset the vAlloc cell. It's required to decrement the owners count in vAlloc destructor
			++cell->owners;
			//! ^ Same as move constructor. This is not an error. The cell's owners will get decremented when vAlloc is destroyed
			pushOwner();
			luxDebug(state = vAlloc.state;)
		}
		//different types of pointers are converted with the explicit conversion operator


		inline void operator=(const std::nullptr_t null) {
			if(!--cell->owners) free();
			popOwner();

			cell = &ram::dummyCell;
		}




		// Add, subtract -----------------------------------------------------------------------------------------------------------------//




		template<class pType> inline uint64 operator+(const pType* vPtr) const noexcept { checkInit(); return (uint64)cell->address + vPtr ; }
		template<class vType> inline type*  operator+(const vType  vVal) const noexcept { checkInit(); return (type* )cell->address + vVal ; }
		template<class pType> inline uint64 operator-(const pType* vPtr) const noexcept { checkInit(); return (uint64)cell->address - vPtr ; }
		template<class vType> inline type*  operator-(const vType  vVal) const noexcept { checkInit(); return (type* )cell->address - vVal ; }




		// Get element -------------------------------------------------------------------------------------------------------------------//




		inline type& operator[](const uint64 vIndex) const {
			checkInit(); checkNullptrD(); checkSize();
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			return ((type*)(cell->address))[vIndex];
		}
		inline type& operator*(  ) const { checkInit(); checkNullptrD(); checkSizeD(); return *this->address; }
		inline type* operator->( ) const { checkInit(); checkNullptrD(); return cell->address; }


		/**
		 * @brief Returns the first address of the allocated memory block as a lux::ram::ptr //FIXME
		 */
		inline type* begin() const {
			checkInit(); checkNullptr();  checkSize();  return (type*)cell->address;
		}

		/**
		 * @brief Returns the address of the object past the last object in the memory block as a lux::ram::ptr. //FIXME
		 *		Dereferencing the pointer is undefined behaviour
		 */
		inline type* end() const {
			checkInit(); checkNullptr();  checkSize();
			return (type*)((int8*)cell->address + count() * sizeof(type));
		}




		// Count and size ----------------------------------------------------------------------------------------------------------------//




		//Returns the size in BYTES of the allocate memory. use count to get the number of elements
		inline uint64 size()  const noexcept { return cell->cellSize; }
		//Returns the number of complete elements in the allocated memory
		inline uint64 count() const noexcept { return cell->cellSize / sizeof(type); }




		// Destructor --------------------------------------------------------------------------------------------------------------------//




		inline ~Alloc( ) noexcept {
			if(cell->address) {
				if(!--cell->owners) free();
				popOwner();
			}
		}




		// Reallocation ------------------------------------------------------------------------------------------------------------------//




		/**
		 * @brief Reallocates the pointer to a block of memory of vSize bytes without initializing it
		 * @param vSize  Size of the block in bytes. It must be positive and less than 0xFFFFFFFF
		 * @param vCopyOldData If true, copies the old data when the memory block is changed
		 * @param vClass Class of the allocation. It must be a valid lux::CellClass value. Default: AUTO
		 */
		void realloc(const uint64 vSize, const bool vCopyOldData = true, CellClass vClass = CellClass::AUTO) {
			using namespace lux::__pvt;
			checkInit(); checkAllocSize(vSize, vClass);
			evaluateCellClass(vSize, vClass);


			if(!cell->address) { [[unlikely]]									//If the memory is unallocated
				alloc_(vSize, vClass);												//Allocate it
				//!^ Ok. Owners count is set back to 1 if the address was nullptr. nullptr allocations are not shared
				pushOwner();
			}
			else { 																//If it's allocated
				if(																	//And the new size is smaller than the maximum cell size
					((uint32)vClass && vSize <= (int64)vClass) || (!(uint32)vClass && vSize <= vSize / LuxIncSize * (LuxIncSize + 1)) ) {
					[[likely]] cell->cellSize = vSize;									//change the cellSize variable and return //FIXME move to fixed size cell
				}
				else{															//If it's larger than the maximum cell size //TODO check realloc and free returns
					if(cell->typeIndex != (uint16)-1) {								//If the cell is a fixed size cell
						type* oldAddr = (type*)cell->address;							//Save the old address
						types[cell->typeIndex].cells.remove(cell->localIndex);			//Remove old allocation
						//! ^ this doesn't remove or invalidate the cell object but only the buffer's cell tracker. This is to allow other pointers to use the same cell even after a reallocation
						if((uint32)vClass) {											//Fixed size --> fixed
							auto& type_ = types[cell->typeIndex];							//Cache buffer type

							cell->localIndex = type_.cells.add(true);						//Create a new allocation and save its index. Then set the new address
							cell->address = (char*)type_.memory[cell->localIndex / type_.cellsPerBuff] + (uint64)type_.cellClass * cell->localIndex;
						}
						else{															//Fixed size --> custom
							uint64 size_ = vSize / LuxIncSize * (LuxIncSize + 1);			//Calculate the new size and allocate the new memory
							cell->address = win10(_aligned_malloc(size_, LuxMemOffset)) _linux(aligned_alloc(LuxMemOffset, size_));
						}
						if(vCopyOldData) memcpy(cell->address, oldAddr, cell->cellSize);//Copy old data in the new memory
						//! ^ The cell still has the same size as before, so it's ok to use it to copy the old data
						cell->typeIndex = classIndexFromEnum(vClass);					//Set the new type index
					}
					else{																//Custom size --> custom
						//FIXME use normal malloc if the data doesnt need to be copied
						cell->address = std::realloc(cell->address, vSize);					//Just reallocate the pointer
					}
					cell->cellSize = vSize;											//Set the new cell size
					//! Custom size --> fixed is managed in smaller size case
				}
			}
		}




		/**
		 * @brief Reallocates the pointer to a block of memory containing vCount elements without initializing them
		 * @param vCount Number of elements
		 * @param vCopyOldData If true, copies the old data when the memory block is changed
		 * @param vClass Class of the allocation. It must be a valid lux::CellClass value. Default: AUTO
		 */
		inline void reallocArr(const uint64 vCount, const bool vCopyOldData = true, const CellClass vClass = CellClass::AUTO) {
			checkInit(); checkAllocSize(sizeof(type) * vCount, vClass);
			realloc(sizeof(type) * vCount, vCopyOldData, vClass);
		}




		// Free memory -------------------------------------------------------------------------------------------------------------------//




		//TODO add free function to erase memory contents
		/**
		 * @brief Frees the memory block owned by the pointer.
		 *		The memory will become invalid and unaccessible for any other pointer currently using it
		 */
		inline void free() {
			checkAlloc();
			if(cell->address) {
				if(cell->typeIndex != (uint16)-1)											//For fixed  size cells,
					types[cell->typeIndex].cells.remove(cell->localIndex);						//free the allocation object
				else std::free(cell->address);												//For custom size cells, free the entire buffer
				cells.remove(cell->cellIndex);												//Free the cell object
				//! owners is not updated. Freeing an allocation does't destroy the pointer
				#ifdef LUX_DEBUG
					//! [Call from destructor] No need to set the correct state of the owners, as there are none (they're all out of scope)
					for(auto i = cell->firstOwner; i != nullptr; i = i->nextOwner) i->state = lux::__pvt::CellState::FREED;
				#endif

			}
		}


		inline operator type*( ) const { checkInit(); return (type*)cell->address; }	//ram::ptr<type> to type* implicit conversion
		inline operator bool(  ) const { checkInit(); return !!cell->address;      }	//ram::ptr<type> to bool  implicit conversion (e.g. if(ptr) is the same as if(ptr != nullptr), like normal pointers)

		inline bool operator==(ram::Alloc<type> vPtr) { return vPtr.cell == cell; }
		inline bool operator!=(ram::Alloc<type> vPtr) { return vPtr.cell != cell; }
		//! If they have the same cell, they also have he same address. No need to access it
	};








	//TODO CHECK MEMORY FULL
	template<class type> void lux::ram::Alloc<type>::alloc_(const uint64 vSize, const CellClass vClass) {
		using namespace lux::__pvt;

		const auto cellIndex = cells.add(Cell_t{});						//Save cell index
		cell = &cells[cellIndex];										//Update cell pointer
		*cell = Cell_t{													//Update cell data
			.typeIndex = classIndexFromEnum(vClass),						//Set cell type index
			.owners = 1,													//Set 1 owner: this pointer
			//! ^ This is not an error. Allocations are not shared when passing a nullptr to operator=
			//!   This means that reallocating a pointer after having assigned it will only reassign the one you are calling the functio on
			.cellIndex  = cellIndex,										//Set cell index
			.cellSize = (uint32)vSize,										//Set size specified in function call
		};
		luxDebug(state = CellState::ALLOC);								//Add cell state info if in debug mode


		if((uint32)vClass) {											//For fixed class cells
			auto& type_ = types[classIndexFromEnum(vClass)];				//Cache buffer type
			const auto localIndex = type_.cells.add(true);					//Create a new allocation and save its index
			cell->localIndex = localIndex;									//Save local index in cell object

			const uint32 buffIndex = localIndex / type_.cellsPerBuff;		//Cache buffer index and allocate a new buffer, if necessary
			if(!type_.memory[buffIndex]) type_.memory[buffIndex] = win10(_aligned_malloc(bufferSize, LuxMemOffset)) _linux(aligned_alloc(LuxMemOffset, bufferSize));
			//															 	 Save allocation address in cell object
			cell->address = (char*)type_.memory[buffIndex] + (uint64)type_.cellClass * localIndex;
		}
		else{															//For custom size cells
			uint64 size = vSize / LuxIncSize * (LuxIncSize + 1);			//Calculate the new size and allocate a new buffer
			cell->address = win10(_aligned_malloc(size, LuxMemOffset)) _linux(aligned_alloc(LuxMemOffset, size));
			luxDebug(cell->localIndex = 0;)
		}
		luxDebug(cell->firstOwner = cell->lastOwner = nullptr);
	}
















// addr pointer --------------------------------------------------------------------------------------------------------------------------//
















	/**
	 * @brief Like a C pointer, but safer
	 * @tparam Type The type of data pointer by the pointer (int, float, etc... )
	 */
	template<class type> struct ptr{
		type* address;
		genInitCheck; //TODO add debug checks for cells


		constexpr inline ptr(nullptr_t) : address{ nullptr } {}
		constexpr inline ptr() : ptr(nullptr) {};

		//Copy C pointers
		template<class pType>
		constexpr explicit inline ptr(  pType* vPtr) { this->address = (type*)vPtr; }
		constexpr          inline ptr(  type * vPtr) { this->address =        vPtr; }
		constexpr inline void operator=(type * vPtr) { this->address =        vPtr; }

		//Copy Lux pointers
		template<class pType>
		constexpr explicit inline ptr(  ptr<pType>* vPtr) { this->address = (type*)vPtr.address; }
		constexpr          inline ptr(  ptr<type> * vPtr) { this->address =        vPtr.address; }
		constexpr inline void operator=(ptr<type> * vPtr) { this->address =        vPtr.address; }






		constexpr inline type* operator++(int) noexcept { checkInit(); return   address++; }
		constexpr inline type* operator++(   ) noexcept { checkInit(); return ++address;   }
		constexpr inline type* operator--(int) noexcept { checkInit(); return   address--; }
		constexpr inline type* operator--(   ) noexcept { checkInit(); return --address;   }

		template<class vType> constexpr inline void operator+=(const vType  vVal) noexcept { checkInit(); address += vVal; }
		template<class vType> constexpr inline void operator-=(const vType  vVal) noexcept { checkInit(); address += vVal; }

		template<class pType> constexpr inline uint64 operator+(const pType* vPtr) const noexcept { checkInit(); return address + vPtr; }
		template<class vType> constexpr inline type*  operator+(const vType  vVal) const noexcept { checkInit(); return address + vVal; }
		template<class pType> constexpr inline uint64 operator-(const pType* vPtr) const noexcept { checkInit(); return address - vPtr; }
		template<class vType> constexpr inline type*  operator-(const vType  vVal) const noexcept { checkInit(); return address - vVal; }



		#undef checkNullptrD
		#define checkNullptrD() lux::dbg::checkCond(address == nullptr, "Cannot dereference a nullptr pointer")
		inline type& operator[](const uint64 vIndex) const { checkInit(); checkNullptrD(); return address[vIndex]; }
		inline type& operator*(                   ) const { checkInit(); checkNullptrD(); dbg::checkRawPtr(address, "Invalid pointer"); return *address; }
		inline type* operator->(                   ) const { checkInit(); checkNullptrD(); dbg::checkRawPtr(address, "Invalid pointer"); return  address; }
		inline operator type*( ) const { checkInit(); return (type*)address; }	//ram::ptr<type> to type* implicit conversion
		inline operator bool(  ) const { checkInit(); return !!address;      }	//ram::ptr<type> to bool  implicit conversion (e.g. if(ptr) is the same as if(ptr != nullptr), like normal pointers)

		inline bool operator==(ram::ptr<type> vPtr) { return vPtr.address == address; }
		inline bool operator!=(ram::ptr<type> vPtr) { return vPtr.address != address; }
	};
}