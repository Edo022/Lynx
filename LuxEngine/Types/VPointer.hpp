#pragma once
#define LUX_H_POINTER
#include "LuxEngine/Core/Memory/VRam/VCell_t.hpp"
#include "LuxEngine/System/SystemMacros.hpp"
#include "LuxEngine/Tests/StructureInit.hpp"
#include "LuxEngine/Tests/CondChecks.hpp"
#include "cstring"
















namespace lux::rem{
	#define checkSize()     luxCheckCond(size( ) == 0, "This function cannot be called on 0-byte memory allocations")
	#define checkSizeD()    luxCheckCond(size( ) == 0, "Cannot dereference a 0-byte memory allocation"              )

	#define checkAlloc() \
		luxCheckCond(state == lux::__pvt::CellState::FREED,      "Unable to call this function on an invalid allocation: The memory block have been manually freed")\
		luxCheckCond(state == lux::__pvt::CellState::OUTOFSCOPE, "Unable to call this function on an invalid allocation: All the Alloc instances owning the memory went out of scope and were destroyed")
	#define isAlloc(a)\
		luxCheckCond(a.state == lux::__pvt::CellState::FREED,      "Invalid allocation: The memory block have been manually freed")\
		luxCheckCond(a.state == lux::__pvt::CellState::OUTOFSCOPE, "Invalid allocation: All the Alloc instances owning the memory went out of scope and were destroyed")
	#define checkNullptr()  luxCheckCond(!cell, "Unable to call this function on an unallocated memory block")
	#define checkNullptrD() luxCheckCond(!cell->address, "Cannot dereference an unallocated memory block")


	#define checkAllocSize(var, _class) luxDebug(if(_class != lux::CellClass::AUTO){											\
		luxCheckParam(var > 0xFFFFffff, var, "Allocation size cannot exceed 0xFFFFFFFF bytes. The given size was %llu", var);	\
		luxCheckParam((uint32)_class < var, _class, "%lu-bytes class specified for %llu-bytes allocation. The cell class must be large enought to contain the bytes. %s", (uint32)_class, var, "Use lux::CellClass::AUTO to automatically choose it"					\
	)});




	/** //TODO FIX
	 * @brief alloc specialization of lux::ram::ptr.
	 *		This pointer can only be initialized with an allocation or another alloc pointer.
	 *		The owned memory block can only be changed by calling the .realloc function or with an assignment.
	 *		Adding or subtracting values do not change the memory block.
	 *		The memory becomes invalid when all the alloc pointers pointing to it go out of scope, or .free() is called.
	 * @tparam type The type of data the pointer points to (int, float, etc... )
	 */
	template<class type> struct VAlloc {
	private:
		genInitCheck;

		void alloc_(const uint64 vSize, const CellClass vClass);
		// void realloc_(const uint64 vSize, const CellClass vClass);
		// inline void init_memory(void* const vAddr, const uint64 vSize, const type& pValue){
		// 	for(uint32 i = 0; i < vSize; i+=sizeof(type)) memcpy((char*)vAddr + i, &pValue, sizeof(type));
		// }




		// Constructors ------------------------------------------------------------------------------------------------------------------//




	public:
		VCell_t* vcell; //A pointer to a lux::ram::Cell_t object that contains the cell informations //TODO cache address with option
		// luxDebug(lux::__pvt::CellState state;)
		// luxDebug(Alloc<uint32>* nextOwner;)



		/**
		 * @brief Creates a nullptr ptr.
		 *		Initializes it with the .realloc function before accessing its memory
		 */
		// inline Alloc( ) : cell{ &dummyCell } { luxDebug(state = lux::__pvt::CellState::ALLOC;) }
		inline Alloc( ) : cell{ nullptr } { }




		/**
		 * @brief Copy constructor. This function will only copy the pointer address and cell, not the data it points to
		 * @param pPtr The pointer to copy. It must be a valid alloc lux::ram::ptr instance
		 */
		inline Alloc(const rem::Alloc<type>& vAlloc) : constructExec(isInit(vAlloc)/*; isAlloc(vAlloc)*/)
			cell{ vAlloc.cell } {
			// cell->owners++;
		}
		/**
		 * @brief Create a pointer by copying another pointer's address and cell
		 * @param pPtr The pointer to copy. It must be a valid alloc lux::ram::ptr instance of a compatible type
		 */
		template<class aType> explicit inline Alloc(const rem::Alloc<aType> vAlloc) : constructExec(isInit(vAlloc)/*; isAlloc(vAlloc)*/)
			cell{ vAlloc.cell } {
			// cell->owners++;
		}




		/**
		 * @brief Move constructor
		 */
		inline Alloc(rem::Alloc<type>&& vAlloc) : constructExec(isInit(vAlloc)/*; isAlloc(vAlloc)*/)
			cell{ vAlloc.cell } { vAlloc.cell = nullptr;
		}




		/**
		 * @brief Allocates a block of memory without initializing it.
		 * 		e.g. lux::ram::ptr<int32> p(302732);
		 * @param vSize  Size of the block in bytes. It must be positive and less than 0xFFFFFFFF
		 * @param vClass Class of the allocation. It must be a valid lux::CellClass value. Default: AUTO
		 */
		inline Alloc(const uint64 vSize, CellClass vClass = CellClass::AUTO){ //TODO use  uint32s
			checkAllocSize(vSize, vClass);
			evaluateCellClass(vSize, vClass);
			alloc_(vSize, vClass);
			// ++cell->owners;
			// luxDebug(state = lux::__pvt::CellState::ALLOC;)
		}

		// /**
		//  * @brief Allocates a block of memory and initializes each element with the pValue value.
		//  *		e.g.lux::ram::ptr<float> p(302732, 15.0f);
		//  * @param vSize  Size of the block in bytes. It must be positive and less than 0xFFFFFFFF
		//  * @param pValue Value each element will be initialized with
		//  * @param vClass Class of the allocation. It must be a valid lux::CellClass value. Default: AUTO
		//  */
		// inline Alloc(const uint64 vSize, const type& pValue, CellClass vClass = CellClass::AUTO) : Alloc(vSize, vClass) {
		// 	init_memory(cell->address, vSize, pValue);
		// 	luxDebug(state = lux::__pvt::CellState::ALLOC;)
		// }




		// Assignment --------------------------------------------------------------------------------------------------------------------//




		//Move assignment
		inline void operator=(Alloc<type>&& vAlloc){
			isInit(vAlloc); //isAlloc(vAlloc);
			cell = vAlloc.cell;				vAlloc.cell = nullptr;
			// luxDebug(state = vAlloc.state;)
		}
		//Copy assignment
		inline void operator=(const Alloc<type>& vAlloc){
			isInit(vAlloc); //isAlloc(vAlloc);
			// cell->owners--;
			cell = vAlloc.cell;
			// cell->owners++;
			// luxDebug(state = vAlloc.state;)
		}
		//Assignment
		template<class aType> inline void operator=(const Alloc<aType> vAlloc){
			isInit(vAlloc); //isAlloc(vAlloc);
			// cell->owners--;
			cell = vAlloc.cell;
			// cell->owners++;
			// luxDebug(state = vAlloc.state;)
		}

		//different types of pointers are converted with the explicit conversion operator




		// Add, subtract -----------------------------------------------------------------------------------------------------------------//




		// template<class pType> inline uint64 operator+(const pType* vPtr) const noexcept { checkInit(); return (uint64)cell->address + vPtr ; }
		// template<class vType> inline type*  operator+(const vType  vVal) const noexcept { checkInit(); return (type* )cell->address + vVal ; }
		// template<class pType> inline uint64 operator-(const pType* vPtr) const noexcept { checkInit(); return (uint64)cell->address - vPtr ; }
		// template<class vType> inline type*  operator-(const vType  vVal) const noexcept { checkInit(); return (type* )cell->address - vVal ; }
//FIXME rename address addr




		// Get element -------------------------------------------------------------------------------------------------------------------//




		// inline type&     operator[](const uint64 vIndex) const {
		// 	checkInit(); checkNullptrD(); checkSize();
		// 	luxCheckParam((vIndex < 0 || vIndex >= count()), vIndex, "Index is out of range");
		// 	return ((type*)(cell->address))[vIndex];
		// }
		// inline type& operator*( ) const { checkInit(); checkNullptrD(); checkSizeD(); return *this->address; }
		// inline type* begin(     ) const { checkInit(); checkNullptr();  checkSize();  return (type*)cell->address;							 } //Returns the first address of the allocated memory block as a lux::ram::ptr
		// inline type* end(       ) const { checkInit(); checkNullptr();  checkSize();  return (type*)((int8*)cell->address + cell->cellSize); } //Returns the address of the object past the last object in the memory block as a lux::ram::ptr. Don't dereference it




		// Count and size ----------------------------------------------------------------------------------------------------------------//




		inline uint64 size()  const noexcept { checkInit(); checkNullptr(); return cell->cellSize;                } //Returns the total size of the allocated memory
		inline uint64 count() const noexcept { checkInit(); checkNullptr(); return cell->cellSize / sizeof(type); } //Returns the number of elements in the allocated memory (use this only if you have allocated the memory with an allocArr function or you are sure that the size is a multiple of the type's size)




		// Copy --------------------------------------------------------------------------------------------------------------------------//




		// /**
		//  * @brief Returns an alloc ram::ptr of the same type, pointing to a new memory block that contains a copy of the pointer data.
		//  *		If the pointer is null, the new pointer will also be null and have no allocated block
		//  */
		// inline Alloc<type> deepCopy() const {
		// 	checkInit(); checkAlloc();
		// 	if(cell->address) {
		// 		Alloc<type> alloc(cell->cellSize);
		// 		memcpy(alloc, cell->address, size());
		// 		return alloc;
		// 	}
		// 	else return Alloc<type>();
		// }




		// Destructor --------------------------------------------------------------------------------------------------------------------//




		inline ~Alloc( ) noexcept {
			// if(cell->address) {
				// if(!--cell->owners) {
					// free();
					// luxDebug(state = lux::__pvt::CellState::OUTOFSCOPE;)
				// }
			// }
		}




		// Reallocation ------------------------------------------------------------------------------------------------------------------//




		// /**
		//  * @brief Reallocates the pointer to a block of memory of vSize bytes without initializing it
		//  * @param vSize  Size of the block in bytes. It must be positive and less than 0xFFFFFFFF
		//  * @param vClass Class of the allocation. It must be a valid lux::CellClass value. Default: AUTO
		//  */
		// //FIXME add callble and automatically called __rearrange__ function to pack sparse cells in few buffers and free space
		// //FIXME or create specific array type that puts new cells in the first free element with the smallest index
		// //FIXME add __behaviour__ template parameter to specify those things
		// void realloc(const uint64 vSize, CellClass vClass = CellClass::AUTO){
		// 	checkInit(); checkAllocSize(vSize, vClass);
		// 	evaluateCellClass(vSize, vClass);

		// 	if(!cell->address) { [[unlikely]]									//If the memory is unallocated
		// 		alloc_(vSize, vClass);												//Allocate it
		// 	}
		// 	else { 																//If it's allocated
		// 		if(																	//And the new size is smaller than the maximum cell size
		// 			((uint32)vClass && vSize <= (int64)vClass) || (!(uint32)vClass && vSize <= vSize / LuxIncSize * (LuxIncSize + 1)) ) {
		// 			[[likely]] cell->cellSize = vSize;									//change the cellSize variable and return //FIXME move to fixed size cell
		// 		}
		// 		else{																//If it's larger than the maximum cell size //TODO check realloc and free returns
		// 			if(cell->typeIndex){
		// 				type* oldAddr = (type*)cell->address;							//Save the old address
		// 				types[cell->typeIndex].cells.remove(cell->localIndex);			//Remove old allocation
		// 				if((uint32)vClass){												//Fixed size --> fixed
		// 					cell->typeIndex = lux::__pvt::classIndexFromEnum(vClass);		//Set the new type index
		// 					auto& type_ = types[cell->typeIndex];							//Cache buffer type

		// 					cell->localIndex = type_.cells.add(true);						//Create a new allocation and save its index
		// 					const uint32 buffIndex = cell->localIndex / type_.cellsPerBuff;	//Cache buffer index and set the new address //FIXME
		// 					cell->address = (char*)type_.memory[buffIndex] + (uint64)type_.cellClass * cell->localIndex;
		// 				}
		// 				else{															//Fixed size --> custom
		// 					cell->typeIndex = 0;											//Set the new type index

		// 					uint64 size_ = vSize / LuxIncSize * (LuxIncSize + 1);		//Calculate the new size
		// 					cell->address = win10(_aligned_malloc(size_, LuxMemOffset)) linux(aligned_alloc(LuxMemOffset, size_));
		// 				}
		// 				memcpy(cell->address, oldAddr, cell->cellSize);					//Copy old data in new allocation
		// 				cell->cellSize = vSize;											//Set the new cell size
		// 			}
		// 			else{																//Custom size --> custom
		// 				cell->address = std::realloc(cell->address, vSize);					//Just reallocate the pointer
		// 			}
		// 			//Custom size --> fixed is managed in smaller size case
		// 		}
		// 	}
		// }




		// /**
		//  * @brief Reallocates the pointer to a block of memory of vSize bytes and initializes each of the new elements with the pValue value
		//  * @param vSize  Size of the block in bytes. It must be positive and less than 0xFFFFFFFF
		//  * @param pValue Value each element will be initialized with
		//  * @param vClass Class of the allocation. It must be a valid lux::CellClass value. Default: AUTO
		//  */
		// void realloc(const uint64 vSize, const type& pValue, CellClass vClass = CellClass::AUTO){
		// 	checkInit(); checkAllocSize(vSize, vClass);
		// 	if(cell->address/* luxDebug(|| cell->state != lux::__pvt::CellState::ALLOC)*/){
		// 		int64 d = vSize - size( );
		// 		auto end_ = end();
		// 		realloc(vSize, vClass);
		// 		if(d > 0) init_memory(end_, d, pValue);
		// 	}
		// 	else {
		// 		evaluateCellClass(vSize, vClass);
		// 		alloc_(vSize, vClass);
		// 		init_memory(cell->address, size(), pValue);
		// 	}
		// }




		// /**
		//  * @brief Reallocates the pointer to a block of memory containing vCount elements without initializing them
		//  * @param vCount Number of elements
		//  * @param vClass Class of the allocation. It must be a valid lux::CellClass value. Default: AUTO
		//  */
		// inline void reallocArr(const uint64 vCount, const CellClass vClass = CellClass::AUTO){
		// 	checkInit(); checkAllocSize(sizeof(type) * vCount, vClass);
		// 	realloc(sizeof(type) * vCount, vClass);
		// }


		// /**
		//  * @brief Reallocates the pointer to a block of memory containing vCount elements initializing them with the pValue value
		//  * @param vCount Number of elements
		//  * @param pValue Value each element will be initialized with
		//  * @param vClass Class of the allocation. It must be a valid lux::CellClass value. Default: AUTO
		//  */
		// inline void reallocArr(const uint64 vCount, const type& pValue, CellClass vClass = CellClass::AUTO){
		// 	checkInit(); checkAllocSize(sizeof(type) * vCount, vClass);
		// 	realloc(sizeof(type) * vCount, pValue, vClass);
		// }




		//TODO add function to change the cell without modifying it.
		//TODO or write it in the documentation
		// Free memory -------------------------------------------------------------------------------------------------------------------//




		/**
		 * @brief Frees the memory block owned by the pointer.
		 *		The memory will become invalid and unaccessible for any other pointer currently using it
		 */
		//TODO add free function to erase memory contents
		inline void free(){
			checkNullptr();
			if((uint32)cell->typeIndex) types[cell->cellIndex].cells.remove(cell->localIndex);	//For fixed  size cells, free the allocation object
			else {
				// std::free(cell->address);														//For custom size cells, free the entire buffer
				vkDestroyBuffer(vcell->)
			}
			rem::vcells.remove(cell->cellIndex);														//And then free the cell object
		}


		// inline operator type*( ) const { checkInit(); return (type*)cell->address; }	//ram::ptr<type> to type* implicit conversion
		inline operator bool(  ) const { checkInit(); return !!cell->address;      }	//ram::ptr<type> to bool  implicit conversion (e.g. if(ptr) is the same as if(ptr != nullptr), like normal pointers)
	};








	template<class type> void lux::rem::VAlloc<type>::alloc_(const uint64 vSize, const CellClass vClass){
		using namespace lux::__pvt;

		const uint32 cellIndex = cells.add(Cell_t{});
		vcell = &vcells[cellIndex];
		cell->cellIndex  = cellIndex;
		cell->owners = 1;								//Set 1 owner: this pointer
		cell->cellSize = vSize;							//Set size specified in function call
		cell->typeIndex = classIndexFromEnum(vClass);	//Set cell type index
		luxDebug(state = CellState::ALLOC);				//Add cell state info if in debug mode

		if((uint32)vClass){											//For fixed class cells
			auto& type_ = types[classIndexFromEnum(vClass)];			//Cache buffer type
			const uint32 localIndex = type_.cells.add(true);			//Create a new allocation and save its index
			cell->localIndex = localIndex;								//Save local index in cell object

			const uint32 buffIndex = localIndex / type_.cellsPerBuff;	//Cache buffer index and allocate a new buffer, if necessary
			if(!type_.memory[buffIndex]) type_.memory[buffIndex] = win10(_aligned_malloc(bufferSize, LuxMemOffset)) linux(aligned_alloc(LuxMemOffset, bufferSize));
			//															  Save allocation address in cell object
			cell->address = (char*)type_.memory[buffIndex] + (uint64)type_.cellClass * localIndex;
		}
		else{														//For custom size cells
			uint64 size = vSize / LuxIncSize * (LuxIncSize + 1);		//Calculate the new size and allocate a new buffer
			cell->address = win10(_aligned_malloc(size, LuxMemOffset)) linux(aligned_alloc(LuxMemOffset, size));
			luxDebug(cell->localIndex = 0;)
		}
	}
}