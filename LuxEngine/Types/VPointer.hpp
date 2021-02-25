#pragma once
#define LUX_H_VPOINTER
#include "LuxEngine/Core/Memory/Gpu/VCell_t.hpp"
#include "LuxEngine/Core/Render/Buffers.hpp"
#include "LuxEngine/System/SystemMacros.hpp"
#include "LuxEngine/Tests/StructureInit.hpp"
#include "LuxEngine/Tests/CondChecks.hpp"
#include "LuxEngine/Types/Dummy.hpp"
#include "cstring"










// alloc pointer -------------------------------------------------------------------------------------------------------------------------//
















namespace lux::rem{
	#define checkAllocSize(var, _class) luxDebug(if(_class != lux::VCellClass::CLASS_0 && _class != lux::VCellClass::AUTO) {											\
		dbg::checkParam(var > 0xFFFFffff, "var", "Allocation size cannot exceed 0xFFFFFFFF bytes. The given size was %llu", var);	\
		dbg::checkParam((uint32)_class < var, "_class", "%lu-bytes class specified for %llu-bytes allocation. The cell class must be large enought to contain the bytes. %s", (uint32)_class, var, "Use lux::VCellClass::AUTO to automatically choose it");\
	});


enum bufferType{
    Storage,
    Uniform
};
enum allocLocation{
    Ram,
    VRam
};

	template<class type, allocLocation location, bufferType buffType> struct Alloc {
		genInitCheck;
	private:

		//Memory allocation
		constexpr static void evaluateCellClass(const uint64 vSize, VCellClass& pClass) noexcept {
			if(pClass != VCellClass::AUTO && (uint32)pClass % LuxMemOffset == 1) {	//Check AT_LEAST values (normal class values + 1)
				if(vSize > ((uint32)pClass)) pClass = VCellClass::AUTO;					//If the class is too small, set it to AUTO
				else pClass = (VCellClass)((uint64)pClass - 1);							//If it's large enough, assign the normal class value
			}
			if(pClass == VCellClass::AUTO) { [[likely]]								//Choose cell class if it's AUTO
				     if(vSize <= (uint32)VCellClass::CLASS_A) [[likely]]   pClass = VCellClass::CLASS_A;
				else if(vSize <= (uint32)VCellClass::CLASS_B) [[likely]]   pClass = VCellClass::CLASS_B;
				else if(vSize <= (uint32)VCellClass::CLASS_C) [[likely]]   pClass = VCellClass::CLASS_C;
				else if(vSize <= (uint32)VCellClass::CLASS_D) [[likely]]   pClass = VCellClass::CLASS_D;
				else if(vSize <= (uint32)VCellClass::CLASS_Q) [[unlikely]] pClass = VCellClass::CLASS_Q;
				else if(vSize <= (uint32)VCellClass::CLASS_L) [[unlikely]] pClass = VCellClass::CLASS_L;
				else										 			   pClass = VCellClass::CLASS_0;
			} //TODO use direct access array
		}

		void alloc_(const uint64 vSize, const VCellClass vClass);
		void realloc_(const uint64 vSize, const VCellClass vClass);




		// Constructors ------------------------------------------------------------------------------------------------------------------//




	public:
		Cell_t2* cell; //A pointer to a lux::rem::Cell_t object that contains the cell informations



		/**
		 * @brief Creates a nullptr ptr.
		 *		Initialize it with the .realloc function before accessing its memory
		 */
		alwaysInline Alloc( ) : cell{ /*&dummyCell*/ } {
		}
		alwaysInline Alloc(const std::nullptr_t) : Alloc() {}



		/**
		 * @brief Copy constructor. This function only copies the pointer structure. The 2 pointers will share the same memory
		 * @param pPtr The pointer to copy
		 */
		alwaysInline Alloc(const Alloc<type>& vAlloc) :
			Alloc(vAlloc, Dummy{}) {
		}

		/**
		 * @brief Create a pointer by copying another pointer's address.This function only copies the pointer structure. The 2 pointers will share the same memory
		 * @param pPtr The pointer to copy
		 */
		template<class aType> explicit inline Alloc(const Alloc<aType>& vAlloc, const Dummy vDummy = Dummy{}) :
			checkInitList(isInit(vAlloc); isAlloc(vAlloc))
			cell{ vAlloc.cell } {
			// cell->owners++;
		}




		/**
		 * @brief Move constructor
		 */
		inline Alloc(Alloc<type>&& vAlloc) : checkInitList(isInit(vAlloc); isAlloc(vAlloc))
			cell{ vAlloc.cell } { //vAlloc.cell = &dummyCell;
			//! ^ Don't reset the vAlloc cell. It's required to decrement the owners count in vAlloc destructor
			// ++cell->owners;
			//! ^ This is not an error. The cell's owners will get decremented when vAlloc is destroyed,
			//! so the move constructor has to increment it to make it stay the same without useless checks
		}




		/**
		 * @brief Allocates a block of memory without initializing it
		 * @param vSize  Size of the block in bytes. It must be a positive integer and less than 0xFFFFFFFF
		 * @param vClass Class of the allocation. Default: AUTO
		 */
		inline Alloc(const uint64 vSize, VCellClass vClass = VCellClass::AUTO) {
			evaluateCellClass(vSize, vClass); checkAllocSize(vSize, vClass);
			alloc_(vSize, vClass);
			// ++cell->owners; //! no. owners already set in alloc_
		}




		// Assignment --------------------------------------------------------------------------------------------------------------------//




		//Move assignment
		alwaysInline void operator=(Alloc<type>&& vAlloc) {
			operator=((Alloc<type>&)vAlloc);
		}


		//Copy assignment
		inline void operator=(const Alloc<type>& vAlloc) {
			checkInit(); isInit(vAlloc);
			// if(!--cell->owners) free();

			cell = vAlloc.cell; //vAlloc.cell = nullptr;
			//! ^ Don't reset the vAlloc cell. It's required to decrement the owners count in vAlloc destructor
			// ++cell->owners;
			//! ^ Same as move constructor. This is not an error. The cell's owners will get decremented when vAlloc is destroyed
		}
		//different types of pointers are converted with the explicit conversion operator


		inline void operator=(const std::nullptr_t) {
			// if(!--cell->owners) free();

			// cell = &ram::dummyCell;
		}




		// Add, subtract -----------------------------------------------------------------------------------------------------------------//




		// template<class pType> alwaysInline uint64 operator+(const pType* vPtr) const { checkInit(); return (uint64)cell->address + vPtr ; }
		// template<class vType> alwaysInline type*  operator+(const vType  vVal) const { checkInit(); return (type* )cell->address + vVal ; }
		// template<class pType> alwaysInline uint64 operator-(const pType* vPtr) const { checkInit(); return (uint64)cell->address - vPtr ; }
		// template<class vType> alwaysInline type*  operator-(const vType  vVal) const { checkInit(); return (type* )cell->address - vVal ; }




		// Get element -------------------------------------------------------------------------------------------------------------------//




		// alwaysInline type& operator[](const uint64 vIndex) const {
		// 	checkInit(); //checkNullptrD(); checkSize();
		// 	dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
		// 	return ((type*)(cell->address))[vIndex]; //FIXME use mapped pointer
		// }
		// alwaysInline type& operator*(  ) const { checkInit(); checkNullptrD(); checkSizeD(); return *((type*)(cell->address)); }
		// alwaysInline type* operator->( ) const { checkInit(); checkNullptrD(); return (type*)(cell->address); }


		// /**
		//  * @brief Returns the first address of the allocated memory block as a lux::ram::ptr //FIXME
		//  */
		// alwaysInline type* begin() const {
		// 	checkInit(); //checkNullptr();  checkSize();
        //     return (type*)cell->address;  //FIXME use mapped pointer
		// }

		// /**
		//  * @brief Returns the address of the object past the last object in the memory block as a lux::ram::ptr. //FIXME
		//  *		Dereferencing the pointer is undefined behaviour
		//  */
		// alwaysInline type* end() const {
		// 	checkInit(); //checkNullptr();  checkSize();
		// 	return (type*)((int8*)cell->address + count() * sizeof(type));  //FIXME use mapped pointer
		// }




		// Count and size ----------------------------------------------------------------------------------------------------------------//




		//Returns the size in BYTES of the allocate memory. use count to get the number of elements
		alwaysInline uint64 size()  const noexcept { return cell->cellSize; }
		//Returns the number of complete elements in the allocated memory
		alwaysInline uint64 count() const noexcept { return cell->cellSize / sizeof(type); }




		// Destructor --------------------------------------------------------------------------------------------------------------------//



        //FIXME idk
		// inline ~Alloc( ) noexcept {
		// 	// if(cell->address) {
		// 		// if(!--cell->owners) {
		// 			if(cell->typeIndex != (uint16)-1) {											//For fixed  size cells,
		// 				types[cell->typeIndex].m.lock();
		// 				types[cell->typeIndex].cells.remove(cell->localIndex);						//free the allocation object
		// 				types[cell->typeIndex].m.unlock();
		// 			}
		// 			else std::free(cell->address);												//For custom size cells, free the entire buffer

		// 			cells_m.lock();
		// 			cells.remove(cell->cellIndex);												//Free the cell object
		// 			cells_m.unlock();
		// 		// }
		// 	// }
		// }




		// Reallocation ------------------------------------------------------------------------------------------------------------------//




		// /**
		//  * @brief Reallocates the pointer to a block of memory of vSize bytes without initializing it
		//  * @param vSize  Size of the block in bytes. It must be positive and less than 0xFFFFFFFF
		//  * @param vCopyOldData If true, copies the old data when the memory block is changed
		//  * @param vClass Class of the allocation. It must be a valid lux::VCellClass value. Default: AUTO
		//  */
		// void realloc(const uint64 vSize, const bool vCopyOldData = true, VCellClass vClass = VCellClass::AUTO) {
		// 	using namespace lux::__pvt;
		// 	checkInit(); checkAllocSize(vSize, vClass);
		// 	evaluateCellClass(vSize, vClass);


		// 	// if(!cell->address) { [[unlikely]]									//If the memory is unallocated
		// 		// alloc_(vSize, vClass);												//Allocate it
		// 		// //!^ Ok. Owners count is set back to 1 if the address was nullptr. nullptr allocations are not shared
		// 	// }
		// 	// else { [[likely]]														//If it's allocated
		// 		if(																	//And the new size is smaller or equal to the maximum cell size
		// 			((uint32)vClass && vSize <= (int64)vClass) || (!(uint32)vClass && vSize <= (vSize / LuxIncSize) * LuxIncSize)) { //FIXME USE DIFFERENT INCREMENT FOR GPU CELLS
		// 			//! ^ Not (vSize / LuxIncSize + 1)
		// 			[[unlikely]] cell->cellSize = vSize;								//change the cellSize variable and return //FIXME move to fixed size cell
		// 		}
		// 		else { [[likely]]												//If it's larger than the maximum cell size //TODO check realloc and free returns
		// 			if(cell->typeIndex != (uint16)-1) {								//If the cell is a fixed size cell
		// 				type* oldAddr = (type*)cell->address;							//Save the old address
		// 				types[cell->typeIndex].m.lock();
		// 				types[cell->typeIndex].cells.remove(cell->localIndex);			//Remove old allocation
		// 				types[cell->typeIndex].m.unlock();
		// 				//! ^ this doesn't remove or invalidate the cell object but only the buffer's cell tracker. This is to allow other pointers to use the same cell even after a reallocation
		// 				if((uint32)vClass) {											//Fixed size --> fixed
		// 					auto& type_ = types[cell->typeIndex];							//Cache buffer type
		// 					type_.m.lock();
		// 					cell->localIndex = type_.cells.add(true);						//Create a new allocation and save its index. Then set the new address
		// 					type_.m.unlock();
		// 					cell->address = (char*)type_.memory[cell->localIndex / type_.cellsPerBuff] + (uint64)type_.VCellClass * cell->localIndex;
		// 				}
		// 				else {															//Fixed size --> custom
		// 					uint64 size_ = (vSize / LuxIncSize + 1) * LuxIncSize;			//Calculate the new size and allocate the new memory
		// 					cell->address = win10(_aligned_malloc(size_, LuxMemOffset)) _linux(aligned_alloc(LuxMemOffset, size_));
		// 				}
		// 				// if(vCopyOldData) memcpy(cell->address, oldAddr, cell->cellSize);//Copy old data in the new memory //FIXME COPY
		// 				//! ^ The cell still has the same size as before, so it's ok to use it to copy the old data
		// 				cell->typeIndex = classIndexFromEnum(vClass);					//Set the new type index
		// 			}
		// 			else {																//Custom size --> custom
		// 				//FIXME use normal malloc if the data doesnt need to be copied
		// 				cell->address = std::realloc(cell->address, vSize);					//Just reallocate the pointer
		// 			}
		// 			cell->cellSize = vSize;											//Set the new cell size
		// 			//! Custom size --> fixed is managed in smaller size case
		// 		}
		// 	// }
		// }




		// /**
		//  * @brief Reallocates the pointer to a block of memory containing vCount elements without initializing them
		//  * @param vCount Number of elements
		//  * @param vCopyOldData If true, copies the old data when the memory block is changed
		//  * @param vClass Class of the allocation. It must be a valid lux::VCellClass value. Default: AUTO
		//  */
		// alwaysInline void reallocArr(const uint64 vCount, const bool vCopyOldData = true, const VCellClass vClass = VCellClass::AUTO) {
		// 	checkInit(); checkAllocSize(sizeof(type) * vCount, vClass);
		// 	realloc(sizeof(type) * vCount, vCopyOldData, vClass);
		// }




		// Free memory -------------------------------------------------------------------------------------------------------------------//




		//TODO add free function to erase memory contents
		/**
		 * @brief Frees the memory block owned by the pointer.
		 *		The memory will become invalid and unaccessible for any other pointer currently using it
		 */
		inline void free() {
			// // checkAlloc();
			// // if(cell->address) {
			// 	// if(cell->typeIndex != (uint16)-1)											//For fixed  size cells,
			// 		// types[cell->typeIndex].cells.remove(cell->localIndex);						//free the allocation object
			// 	// else std::free(cell->address);												//For custom size cells, free the entire buffer
			// 	// cells.remove(cell->cellIndex);												//Free the cell object
			// 	this->realloc(0);
			// 	//! owners is not updated. Freeing an allocation does't destroy the pointer
			// // }
            if(cell->typeIndex != (uint16)-1) {											//For fixed  size cells,
                types[cell->typeIndex].m.lock();
                types[cell->typeIndex].cells.remove(cell->localIndex);						//free the allocation object
                types[cell->typeIndex].m.unlock();
            }
            // else std::free(cell->address);												//For custom size cells, free the entire buffer
            //FIXME FREE BUFFERS

            cells_m.lock();
            cells.remove(cell->cellIndex);												//Free the cell object
            cells_m.unlock();
		}


		// alwaysInline operator type*( ) const { checkInit(); return (type*)cell->address; }	//ram::ptr<type> to type* implicit conversion
		// alwaysInline operator bool(  ) const { checkInit(); return !!cell->address;      }	//ram::ptr<type> to bool  implicit conversion ("if(ptr)" is the same as "if(ptr != nullptr)")

		alwaysInline bool operator==(ram::Alloc<type> vPtr) { return vPtr.cell == cell; }
		alwaysInline bool operator!=(ram::Alloc<type> vPtr) { return vPtr.cell != cell; }
		//! If they have the same cell, they also have he same address. No need to access it
	};







	//TODO CHECK MEMORY FULL
	template<class type, allocLocation location, bufferType buffType> void lux::rem::Alloc<type, location, buffType>::alloc_(const uint64 vSize, const VCellClass vClass) {
		using namespace lux::__pvt;
		cells_m.lock();
		const auto cellIndex = cells.add(Cell_t{});						//Save cell index
		cells_m.unlock();
		cell = &cells[cellIndex];										//Update cell pointer
        uint16 typeIndex = (classIndexFromEnum(vClass) << 2) | (location << 1) | buffType;
		*cell = Cell_t{													//Update cell data
			.typeIndex = typeIndex,						//Set cell type index
			// .owners = 1,													//Set 1 owner: this pointer
			//! ^ This is not an error. Allocations are not shared when passing a nullptr to operator=
			//!   This means that reallocating a pointer after having assigned it will only reassign the one you are calling the functio on
			.cellIndex  = cellIndex,										//Set cell index
			.cellSize = (uint32)vSize,										//Set size specified in function call
		};


		if((uint32)vClass) {											//For fixed class cells
			auto& type_ = types[typeIndex];				//Cache buffer type
			type_.m.lock();
			const auto localIndex = type_.cells.add(true);					//Create a new allocation and save its index
			type_.m.unlock();
			cell->localIndex = localIndex;									//Save local index in cell object

			const uint32 buffIndex = localIndex / type_.cellsPerBuff;		//Cache buffer index and allocate a new buffer, if necessary
			// if(!type_.memory[buffIndex]) type_.memory[buffIndex] = win10(_aligned_malloc(bufferSize, LuxMemOffset)) _linux(aligned_alloc(LuxMemOffset, bufferSize));
			if(!type_.memory[buffIndex]) {
                // type_.memory[buffIndex] = win10(_aligned_malloc(bufferSize, LuxMemOffset)) _linux(aligned_alloc(LuxMemOffset, bufferSize));
                //FIXME DONT DUPLICATE BUFFER CHECKS A
                lux::core::buffers::createBuffer(
                    type_.buffer,
                    ((buffType == bufferType::Uniform) ? VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT : VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT) | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                    bufferSize,
                    type_.memory,
                    (location == allocLocation::Ram) ? (VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, //FIXME IDK
                    core::dvc::compute.LD
                )
            }
			//															 	 Save allocation address in cell object
			// cell->address = (char*)type_.memory[buffIndex] + (uint64)type_.VCellClass * localIndex;
            cell->csc.buffer = type_.buffer; //FIXME one of those is probably useless
            cell->csc.memory = type_.memory; //FIXME one of those is probably useless
            //FIXME cell offset? It could be calculated from the cell index tho
		}
		else {															//For custom size cells
			uint64 size = (vSize / LuxIncSize + 1) * LuxIncSize;			//Calculate the new size and allocate a new buffer
			// cell->address = win10(_aligned_malloc(size, LuxMemOffset)) _linux(aligned_alloc(LuxMemOffset, size));
            //FIXME CHOOSE STORAGE TYPE
            //FIXME CHOOSE READ/WRITE FLAHS
            //FIXME USE ARBITRARY RANGE FOR COMPATIBILITY
            checkParam(buffType == bufferType::Uniform && core::dvc::compute.PD.properties.limits.maxUniformBufferRange >= vSize, "vSize", "Allocation is too large to be a uniform buffer");
            //FIXME DONT DUPLICATE BUFFER CHECKS B
            lux::core::buffers::createBuffer(
                cell->csc.buffer,
                // ((((uint32)vAllocType & 0b1) && (core::dvc::compute.PD.properties.limits.maxUniformBufferRange >= vSize)) ? VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT : VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT) | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                ((buffType == bufferType::Uniform) ? VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT : VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT) | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                vSize,
                cell->csc.memory,
                (location == allocLocation::Ram) ? (VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, //FIXME IDK
				core::dvc::compute.LD
            )
			luxDebug(cell->localIndex = 0;)
		}
		// luxDebug(cell->firstOwner = cell->lastOwner = nullptr);
	}
}