#pragma once
////#define LNX_H_POINTER
#include "Lynx/Core/Memory/Ram/Cell_t.hpp"
#include "Lynx/System/SystemMacros.hpp"
#include "Lynx/Tests/StructureInit.hpp"
#include "Lynx/Types/Dummy.hpp"
#include "cstring"
//FIXME add callble and automatically called __rearrange__ function to pack sparse cells in few buffers and free space


//TODO FIX DOCUMENTATION





namespace lnx::ram{
	/**
	 * @brief A pointer that keeps track of how many objects are using it and automatically frees the memory block once it becomes unreachable
	 * @tparam tType The type of data pointed by the pointer (int, float, etc...)
	 */
	template<class tType> struct ptr {
	private:
		alwaysInline void checkSize()  const { _dbg(
			lnx::dbg::checkCond(size() == 0, "This function cannot be called on 0-byte memory allocations");
		)}
		alwaysInline void checkSizeD() const { _dbg(
			lnx::dbg::checkCond(size() == 0, "Cannot dereference a 0-byte memory allocation");
		)}


		alwaysInline void checkAlloc() const { _dbg(
			lnx::dbg::checkCond(state == __pvt::CellState::eFreed, "Unable to call this function on invalid allocations: The memory block have been manually freed");
		)}
		#define isAlloc(a) dbg::checkParam((a).state == __pvt::CellState::eFreed, #a,\
			"Use of invalid allocation: The memory block have been manually freed")
		;


		alwaysInline void checkNullptr()  const { _dbg(
			lnx::dbg::checkCond(state == __pvt::CellState::eNullptr, "Unable to call this function on unallocated memory blocks");
		)}
		alwaysInline void checkNullptrD() const { _dbg(
			lnx::dbg::checkCond(state == __pvt::CellState::eNullptr, "Cannot dereference an unallocated memory block");
		)}


		static alwaysInline void checkAllocSize(uint64 size_, CellClass _class) { _dbg(
			if(_class != CellClass::e0 && _class != CellClass::eAuto) {
				dbg::checkCond(size_ > 0xFFFFffff, "Allocation size cannot exceed 0xFFFFFFFF bytes. The given size was %llu", size_);
				dbg::checkCond((uint32)_class < size_, "%lu-bytes class specified for %llu-bytes allocation. The cell class must be large enought to contain the bytes. %s", (uint32)_class, size_, "Use lnx::CellClass::AUTO to automatically choose it");
			}
		)};


		constexpr static void evaluateCellClass(const uint64 vSize, CellClass& pClass) noexcept {
			if(pClass == CellClass::eAuto) { [[likely]]
				     if(vSize <= (uint32)CellClass::eA) [[likely]]	  pClass = CellClass::eA;
				else if(vSize <= (uint32)CellClass::eB) [[likely]]	  pClass = CellClass::eB;
				else if(vSize <= (uint32)CellClass::eC) [[likely]]	  pClass = CellClass::eC;
				else if(vSize <= (uint32)CellClass::eD) [[likely]]   pClass = CellClass::eD;
				else if(vSize <= (uint32)CellClass::eQ) [[unlikely]] pClass = CellClass::eQ;
				else if(vSize <= (uint32)CellClass::eL) [[unlikely]] pClass = CellClass::eL;
				else										 			  pClass = CellClass::e0;
			} //TODO use direct access array
		}


		/**
		 * @brief Adds this pointer in the owners list of the memory block
		 * Complexity: O(1)
		 */
		void pushOwner() { _dbg(
			if(!cell->address) return;									//Return if the cell is nullptr
			if(!cell->firstOwner) {										//If this is the first owner of the cell
				cell->firstOwner = cell->lastOwner = (ptr<Dummy>*)this;		//Set this as both the first and last owners
				prevOwner = nextOwner = nullptr;							//Set this prev and next owners to nullptr
			} else {													//If this is not the first owner
				prevOwner = cell->lastOwner;								//Set the cell's last owner as this prev
				nextOwner = nullptr;										//Set this next to nullptr
				cell->lastOwner->nextOwner = (ptr<Dummy>*)this;				//Update the cell's last owner's next to this
				cell->lastOwner = (ptr<Dummy>*)this;						//Update the cell's last owner to this
			}
		)}

		/**
		 * @brief Removes this pointer from the owners list of the memory block
		 * Complexity: O(1)
		 */
		void popOwner() { _dbg(
			if(!cell->address) return;									//Return if the cell is nullptr
			if(!prevOwner && !nextOwner) {								//If this was the only owner
				cell->firstOwner = cell->lastOwner = nullptr;				//Set both the first and last owner of the cell to nullptr
			} else {													//If the cell had other owners
				if(prevOwner) prevOwner->nextOwner = nextOwner;				//If this was NOT the first owner, update the previus owner's next
				else cell->firstOwner = (ptr<Dummy>*)nextOwner;				//If this was the first owner,     update the cell's first owner
				if(nextOwner) nextOwner->prevOwner = prevOwner;				//If this was NOT the last owner,  update the next owner's previous
				else cell->lastOwner = (ptr<Dummy>*)prevOwner;				//If this was the last owner,      update the cell's last owner
				prevOwner = nextOwner = nullptr;							//Set both this next and prev owners to nullptr. This is not necessary but helps debugging
			}
		)}




	public:
		genInitCheck;
		Cell_t* cell; 							//A pointer to a lnx::ram::Cell_t object that contains the cell informations
		_dbg(mutable __pvt::CellState state;)	//[State of the pointer]
		_dbg(mutable ptr<Dummy>* prevOwner;)	//The pointer that acquired the memory before this object
		_dbg(mutable ptr<Dummy>* nextOwner;)	//The pointer that acquired the memory after this object








		// Constructors ------------------------------------------------------------------------------------------------------------------//








		/**
		 * @brief Creates a nullptr ptr
		 *     The pointer will need to be initialized with the .realloc function before accessing its data
		 * Complexity: O(1)
		 */
		alwaysInline ptr() : cell{ &dummyCell } {
			_dbg(prevOwner = nextOwner = nullptr;)
			_dbg(state = __pvt::CellState::eNullptr);
		}
		alwaysInline ptr(const std::nullptr_t) : ptr() {}




		/**
		 * @brief Copy constructor
		 *     This function only copies the pointer structure
		 *     The 2 pointers will share the same memory block
		 * Complexity: O(1)
		 * @param pPtr The pointer to copy
		 */
		alwaysInline ptr(const ptr<tType>& pPtr) :
			ptr(pPtr, Dummy{}) {
		}


		/**
		 * @brief Creates a pointer by copying another pointer's address
		 *     This function only copies the pointer structure
		 *     The 2 pointers will share the same memory block
		 * Complexity: O(1)
		 * @param pPtr The pointer to copy
		 */
		template<class aType> explicit inline ptr(const ptr<aType>& pPtr, const Dummy vDummy = Dummy{}) :
			checkInitList(isInit(pPtr); isAlloc(pPtr))
			cell{ pPtr.cell } {
			cell->owners++;
			pushOwner();
			_dbg(state = pPtr.state);
		}




		/**
		 * @brief Move constructor
		 * Complexity: O(1)
		 */
		inline ptr(ptr<tType>&& pPtr) : checkInitList(isInit(pPtr); isAlloc(pPtr))
			cell{ pPtr.cell } { //pPtr.cell = &dummyCell;
			//!                     ^ Don't reset the pPtr cell. It's required to decrement the owners count in pPtr destructor
			++cell->owners;
			//! ^ This is not an error. The cell's owners will get decremented when pPtr is destroyed,
			//! so the move constructor has to increment it to make it stay the same without useless checks
			pushOwner();
			_dbg(state = pPtr.state);
		}




		/**
		 * @brief Allocates a memory block of vSize bytes without initializing it
		 * Complexity:
		 *     O(1)    [Memory block fits in preallocated cells]
		 *     Unknown [Memory block is too large || There are no available cells] [Depends on system resources]
		 * @param vSize  Size of the block in bytes. It must be less than 0xFFFFFFFF
		 * @param vClass Class of the allocation. Default: AUTO
		 */
		inline ptr(const uint64 vSize, CellClass vClass = CellClass::eAuto) {
			evaluateCellClass(vSize, vClass); checkAllocSize(vSize, vClass);
			alloc_(vSize, vClass);
			// ++cell->owners; //! no. owners already set in alloc_
			pushOwner();
			_dbg(state = __pvt::CellState::eAlloc);
		}








		// Assignment --------------------------------------------------------------------------------------------------------------------//








		/**
		 * @brief Move assignment
		 * Complexity: O(1)
		 */
		alwaysInline void operator=(ptr<tType>&& vAlloc) {
			operator=((ptr<tType>&)vAlloc);
		}


		/**
		 * @brief Copy assignment
		 * Complexity: O(1)
		 */
		inline void operator=(const ptr<tType>& vAlloc) {
			checkInit(); isInit(vAlloc);
			if(!--cell->owners) free();
			popOwner();

			cell = vAlloc.cell; //vAlloc.cell = nullptr;
			//! ^ Don't reset the vAlloc cell. It's required to decrement the owners count in vAlloc destructor
			++cell->owners;
			//! ^ Same as move constructor. This is not an error. The cell's owners will get decremented when vAlloc is destroyed
			pushOwner();
			_dbg(state = vAlloc.state;)
		}
		//different types of pointers are converted with the explicit conversion operator


		/**
		 * @brief Sets the pointer to nullptr
		 *     Frees the memory block owned by the pointer, if one exists
		 * Complexity: O(1)
		 */
		inline void operator=(const std::nullptr_t) {
			if(!--cell->owners) free();
			popOwner();

			cell = &ram::dummyCell;
		}








		// Add, subtract -----------------------------------------------------------------------------------------------------------------//








		alwaysInline uint64 operator+(const auto* vPtr) const { checkInit(); return (uint64)cell->address + vPtr ; }
		alwaysInline tType* operator+(const auto  vVal) const { checkInit(); return (tType*)cell->address + vVal ; }
		alwaysInline uint64 operator-(const auto* vPtr) const { checkInit(); return (uint64)cell->address - vPtr ; }
		alwaysInline tType* operator-(const auto  vVal) const { checkInit(); return (tType*)cell->address - vVal ; }








		// Get element -------------------------------------------------------------------------------------------------------------------//








		/**
		 * @brief Returns a reference to an element
		 * Complexity: O(1)
		 * @param vIndex The index of the element
		 * @return A rvalue reference to the vIndex-th element
		 */
		alwaysInline tType& operator[](const uint64 vIndex) const {
			checkInit(); checkNullptrD(); checkSize();
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			return ((tType*)(cell->address))[vIndex];
		}


		/**
		 * @brief Returns a reference to the first element
		 * Complexity: O(1)
		 * @return a rvalue-reference to the first element
		 */
		alwaysInline tType& operator* () const { checkInit(); checkNullptrD(); checkSizeD(); return *((tType*)(cell->address)); }


		/**
		 * @brief Allows the pointer to be used as a normal C pointer
		 */
		alwaysInline tType* operator->() const { checkInit(); checkNullptrD(); return (tType*)(cell->address); }


		/**
		 * @brief Returns the first address of the allocated memory block
		 * Complexity: O(1)
		 * @return The first address of the allocated memory block
		 */
		alwaysInline tType* begin() const {
			checkInit(); checkNullptr();  checkSize();  return (tType*)cell->address;
		}


		/**
		 * @brief Returns the address of the object past the last object in the memory block
		 *     Dereferencing the pointer is undefined behaviour
		 * Complexity: O(1)
		 * @return The address of the object past the last object in the memory block
		 */
		alwaysInline tType* end() const {
			checkInit(); checkNullptr();  checkSize();
			return (tType*)((int8*)cell->address + count() * sizeof(tType));
		}








		// Count and size ----------------------------------------------------------------------------------------------------------------//








		/**
		 * @brief Returns the size in bytes of the memory block
		 *     Use count() to get the number of elements
		 * Complexity: O(1)
		 * @return The size in bytes of the memory block
		 */
		alwaysInline uint64 size() const noexcept {
			return cell->cellSize;
		}


		/**
		 * @brief Returns the number of complete elements in the memory block
		 * Complexity: O(1)
		 * @return The number of complete elements in the memory block
		 */
		alwaysInline uint64 count() const noexcept { return cell->cellSize / sizeof(tType); }








		// Destructor --------------------------------------------------------------------------------------------------------------------//








		/**
		 * @brief Destroys the pointer object and frees the memory block owned by it
		 *     Objects allocated in the memory block are not destroyed
		 * Complexity: O(1)
		 */
		inline ~ptr() noexcept {
			if(cell->address) {
				if(!--cell->owners) {
					if(cell->typeIndex != (uint16)-1) {						//For fixed  size cells,
						g_types()[cell->typeIndex].m.lock();
						g_types()[cell->typeIndex].cells.remove(cell->localIndex);	//free the allocation object
						g_types()[cell->typeIndex].m.unlock();
					}
					else std::free(cell->address);							//For custom size cells, free the entire buffer

					g_cells_m().lock();
					g_cells().remove(cell->cellIndex);							//Free the cell object
					g_cells_m().unlock();
				}
				popOwner();
			}
		}








		// Reallocation ------------------------------------------------------------------------------------------------------------------//








	private:
		/**
		 * @brief Base function for memory allocation
		 *     Allocates a memory block of vSize bytes
		 *     This function does not initialize memory nor update the owner list or check the parameters correctness
		 * Complexity:
		 *     O(1)    [Memory block fits in preallocated cells]
		 *     Unknown [Memory block is too large || There are no available cells] [Depends on system resources]
		 * @param vSize  Size of the block in bytes. It must be less than 0xFFFFFFFF
		 * @param vClass Class of the allocation. Default: AUTO
		 */
		void alloc_(const uint64 vSize, const CellClass vClass) {
			using namespace lnx::__pvt;
			g_cells_m().lock();
			const auto cellIndex = g_cells().add(Cell_t{});						//Save cell index
			g_cells_m().unlock();
			cell = &g_cells()[cellIndex];										//Update cell pointer
			*cell = Cell_t{													//Update cell data
				.typeIndex = __pvt::classIndexFromEnum(vClass),					//Set cell type index
				.owners = 1,													//Set 1 owner: this pointer
				//! ^ This is not an error. Allocations are not shared when passing a nullptr to operator=
				//!   This means that reallocating a pointer after having assigned it will only reassign the one you are calling the functio on
				.cellIndex  = cellIndex,										//Set cell index
				.cellSize = (uint32)vSize,										//Set size specified in function call
			};
			_dbg(state = __pvt::CellState::eAlloc);						//Add cell state info if in debug mode


			if((uint32)vClass) {											//For fixed class cells
				auto& type_ = g_types()[__pvt::classIndexFromEnum(vClass)];			//Cache buffer type
				type_.m.lock();
				const auto localIndex = type_.cells.add(true);					//Create a new allocation and save its index
				type_.m.unlock();
				cell->localIndex = localIndex;									//Save local index in cell object

				const uint32 buffIndex = localIndex / type_.cellsPerBuff;		//Cache buffer index and allocate a new buffer, if necessary
				if(!type_.memory[buffIndex]) type_.memory[buffIndex] = _wds(_aligned_malloc(bufferSize, memOffset)) _lnx(aligned_alloc(__pvt::memOffset, __pvt::buffSize));
				//															 	 Save allocation address in cell object
				cell->address = (char*)type_.memory[buffIndex] + (uint64)type_.cellClass * localIndex;
			}
			else {															//For custom size cells
				uint64 size = (vSize / __pvt::incSize + 1) * __pvt::incSize;	//Calculate the new size and allocate a new buffer
				cell->address = _wds(_aligned_malloc(size, __pvt::memOffset)) _lnx(aligned_alloc(__pvt::memOffset, size));
				_dbg(cell->localIndex = 0;)
			}
			_dbg(cell->firstOwner = cell->lastOwner = nullptr);
		}
	public:








		/**
		 * @brief Reallocates the pointer to a block of memory of vSize bytes without initializing it
		 * Complexity:
		 *     O(n)    [Memory block fits in preallocated cells && vCopyOldData = true]
		 *     O(1)    [Memory block fits in preallocated cells && vCopyOldData = false]
		 *     Unknown [Memory block is too large || There are no available cells] [Depends on system resources]
		 *     Where n = this->count()
		 * @param vSize  Size of the block in bytes. It must be positive and less than 0xFFFFFFFF
		 * @param vCopyOldData If true, copies the old data when the memory block is changed
		 * @param vClass Class of the allocation. It must be a valid lnx::CellClass value. Default: AUTO
		 */
		void realloc(const uint64 vSize, const bool vCopyOldData = true, CellClass vClass = CellClass::eAuto) {
			//FIXME FREE CUSTOM SIZE BUFFERS
			using namespace lnx::__pvt;
			checkInit(); checkAllocSize(vSize, vClass);
			evaluateCellClass(vSize, vClass);


			if(!cell->address) { [[unlikely]]									//If the memory is unallocated
				alloc_(vSize, vClass);												//Allocate it
				//!^ Ok. Owners count is set back to 1 if the address was nullptr. nullptr allocations are not shared
				pushOwner();
			}
			else { [[likely]]													//If it's allocated
				if(																	//And the new size is smaller or equal to the maximum cell size
					((uint32)vClass && vSize <= (int64)vClass) || (!(uint32)vClass && vSize <= (vSize / __pvt::incSize) * __pvt::incSize)) {
					//!                                                                        ^ Not (vSize / incSize + 1)
					[[unlikely]] cell->cellSize = vSize;								//change the cellSize variable and return //FIXME move to fixed size cell
				}
				else { [[likely]]												//If it's larger than the maximum cell size //TODO check realloc and free returns
					if(cell->typeIndex != (uint16)-1) {								//If the cell is a fixed size cell
						tType* oldAddr = (tType*)cell->address;							//Save the old address
						g_types()[cell->typeIndex].m.lock();
						g_types()[cell->typeIndex].cells.remove(cell->localIndex);			//Remove old allocation
						g_types()[cell->typeIndex].m.unlock();
						//! ^ this doesn't remove or invalidate the cell object but only the buffer's cell tracker. This is to allow other pointers to use the same cell even after a reallocation
						if((uint32)vClass) {											//Fixed size --> fixed
							auto& type_ = g_types()[cell->typeIndex];							//Cache buffer type
							type_.m.lock();
							cell->localIndex = type_.cells.add(true);						//Create a new allocation and save its index. Then set the new address
							type_.m.unlock();
							cell->address = (char*)type_.memory[cell->localIndex / type_.cellsPerBuff] + (uint64)type_.cellClass * cell->localIndex;
						}
						else {															//Fixed size --> custom
							uint64 size_ = (vSize / __pvt::incSize + 1) * __pvt::incSize;	//Calculate the new size and allocate the new memory
							cell->address = _wds(_aligned_malloc(size_, __pvt::memOffset)) _lnx(aligned_alloc(__pvt::memOffset, size_));
						}
						if(vCopyOldData) memcpy(cell->address, oldAddr, cell->cellSize);//Copy old data in the new memory
						//! ^ The cell still has the same size as before, so it's ok to use it to copy the old data
						cell->typeIndex = __pvt::classIndexFromEnum(vClass);			//Set the new type index
					}
					else {															//Custom size --> custom
						//FIXME use normal malloc if the data doesnt need to be copied
						cell->address = std::realloc(cell->address, vSize);				//Just reallocate the pointer
					}
					cell->cellSize = vSize;											//Set the new cell size
					//! Custom size --> fixed is managed in smaller size case
				}
			}
		}








		/**
		 * @brief Reallocates the pointer to a block of memory containing vCount elements without initializing them
		 * Complexity:
		 *     O(1)    [Memory block fits in preallocated cells && vCopyOldData = false]
		 *     O(n)    [Memory block fits in preallocated cells && vCopyOldData = true]
		 *     Unknown [Memory block is too large || There are no available cells] [Depends on system resources]
		 *     Where n = this->count()
		 * @param vCount Number of elements
		 * @param vCopyOldData If true, copies the old data when the memory block is changed
		 * @param vClass Class of the allocation. It must be a valid lnx::CellClass value. Default: AUTO
		 */
		alwaysInline void reallocArr(const uint64 vCount, const bool vCopyOldData = true, const CellClass vClass = CellClass::eAuto) {
			checkInit(); checkAllocSize(sizeof(tType) * vCount, vClass);
			realloc(sizeof(tType) * vCount, vCopyOldData, vClass);
		}








		// Free memory -------------------------------------------------------------------------------------------------------------------//








		//TODO add free function to erase memory contents
		/**
		 * @brief Frees the memory block owned by the pointer
		 *		The memory will become invalid and unaccessible for any other pointer currently using it
		 * Complexity:
		 *     O(1) [Release mode]
		 *     O(n) [  Debug mode]
		 *     Where n = number of objects using the memory block
		 */
		inline void free() {
			checkAlloc();
			if(cell->address) {
				this->realloc(0);
				//! owners is not updated. Freeing an allocation does't destroy the pointer
				#ifdef LNX_DBG
					//! [Call from destructor] No need to set the correct state of the owners, as there are none (they're all out of scope)
					for(auto i = cell->firstOwner; i != nullptr; i = i->nextOwner) {
						i->state = __pvt::CellState::eFreed;
					}
				#endif
			}
		}




		// Comparison and conversion operators -------------------------------------------------------------------------------------------//




		template<class type_> explicit alwaysInline operator type_*() const { checkInit(); return (type_*)cell->address; }
		alwaysInline operator tType*() const { checkInit(); return (tType*)cell->address; }
		alwaysInline operator bool()  const { checkInit(); return !!cell->address;      }

		alwaysInline bool operator==(ram::ptr<tType> vPtr) { return vPtr.cell == cell; }
		alwaysInline bool operator!=(ram::ptr<tType> vPtr) { return vPtr.cell != cell; }
		//! If they have the same cell, they also have he same address. No need to access it


		#undef isAlloc
	};
}