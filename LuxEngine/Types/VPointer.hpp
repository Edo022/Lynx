#pragma once
#define LUX_H_VPOINTER
#include "LuxEngine/Core/Memory/VRam/VCell_t.hpp"
#include "LuxEngine/Core/Render/Buffers.hpp"
#include "LuxEngine/Core/Devices.hpp"
#include "LuxEngine/System/SystemMacros.hpp"
#include "LuxEngine/Tests/StructureInit.hpp"
#include "LuxEngine/Types/Dummy.hpp"
#include "cstring"





//TODO USE PREALLOCATION INSTEAD OF CLASSES










namespace lux{
	enum bufferType{
		Storage,
		Uniform
	};
	enum bufferLocation{
		Ram,
		VRam
	};
}




namespace lux::vram{
	#define checkAllocSize(var, _class) luxDebug(if((_class) != VCellClass::CLASS_0 && (_class) != VCellClass::AUTO) {											\
		dbg::checkCond((var) > 0xFFFFffff, "Allocation size cannot exceed 0xFFFFFFFF bytes. The given size was %llu", (var));	\
		dbg::checkCond((uint32)(_class) < (var), "%lu-bytes class specified for %llu-bytes allocation. The cell class must be large enought to contain the bytes. %s", (uint32)(_class), (var), "Use lux::VCellClass::AUTO to automatically choose it");\
	});
	#define checkMapped() dbg::checkCond(!Super::mapped, "Unable to call this function on unmapped memory blocks")

	//ptr base class
	template<class type> struct Alloc_b {
		genInitCheck;

		uint8 location;
		uint8 buffType;
		Cell_t2* cell;						//A pointer to a lux::vram::Cell_t object that contains the cell informations
		type* mapped luxDebug(= nullptr);	//A pointer used to map the memory

		template<class type_> explicit alwaysInline operator Alloc_b<type_>&() const noexcept { return *(Alloc_b<type_>*)(this); }
	};




	template<class type, bufferLocation location, bufferType buffType> struct ptr : public Alloc_b<type> {
	private:
		using Super = Alloc_b<type>;

		//Memory allocation
		constexpr static void evaluateCellClass(const uint64 vSize, VCellClass& pClass) noexcept {
			if(pClass == VCellClass::AUTO) { [[likely]]
				     if(vSize <= (uint32)VCellClass::CLASS_A) [[likely]]   pClass = VCellClass::CLASS_A;
				else if(vSize <= (uint32)VCellClass::CLASS_B) [[likely]]   pClass = VCellClass::CLASS_B;
				else if(vSize <= (uint32)VCellClass::CLASS_C) [[likely]]   pClass = VCellClass::CLASS_C;
				else if(vSize <= (uint32)VCellClass::CLASS_D) [[likely]]   pClass = VCellClass::CLASS_D;
				else if(vSize <= (uint32)VCellClass::CLASS_Q) [[unlikely]] pClass = VCellClass::CLASS_Q;
				else if(vSize <= (uint32)VCellClass::CLASS_L) [[unlikely]] pClass = VCellClass::CLASS_L;
				else										 			   pClass = VCellClass::CLASS_0;
			}
		}

	public:
		void alloc_(const uint64 vSize, const VCellClass vClass);




		// Constructors ------------------------------------------------------------------------------------------------------------------//




		/**
		 * @brief Creates a nullptr ptr.
		 *		Initialize it with the .realloc function before accessing its memory
		 */
		alwaysInline ptr() { Super::location = location; Super::buffType = buffType; }
		alwaysInline ptr(const std::nullptr_t) : ptr() { }



		/**
		 * @brief Copy constructor. This function only copies the pointer structure. The 2 pointers will share the same memory
		 * @param pPtr The pointer to copy
		 */
		alwaysInline ptr(const ptr<type, location, buffType>& vAlloc) :
			ptr(vAlloc, Dummy{}) {
		}

		/**
		 * @brief Create a pointer by copying another pointer's address.This function only copies the pointer structure. The 2 pointers will share the same memory
		 * @param pPtr The pointer to copy
		 */
		template<class aType> explicit inline ptr(const ptr<aType, location, buffType>& vAlloc, const Dummy vDummy = Dummy{}){
			isInit(vAlloc);
			Super::location = location; Super::buffType = buffType;
			Alloc_b<type>::cell = vAlloc.cell;
		}




		/**
		 * @brief Move constructor
		 */
		inline ptr(ptr<type, location, buffType>&& vAlloc) {
			isInit(vAlloc);
			Super::location = location; Super::buffType = buffType;
			Super::cell = vAlloc.cell; //vAlloc.cell = &dummyCell;
			//!                          ^ Don't reset the vAlloc cell. It's required to decrement the owners count in vAlloc destructor
		}




		/**
		 * @brief Allocates a block of memory without initializing it
		 * @param vSize  Size of the block in bytes. It must be a positive integer and less than 0xFFFFFFFF
		 * @param vClass Class of the allocation. Default: AUTO
		 */
		inline ptr(const uint64 vSize, VCellClass vClass = VCellClass::AUTO) {
			Super::location = location; Super::buffType = buffType;
			evaluateCellClass(vSize, vClass); checkAllocSize(vSize, vClass);
			alloc_(vSize, vClass);
		}




		// Assignment --------------------------------------------------------------------------------------------------------------------//




		//Move assignment
		alwaysInline void operator=(ptr<type, location, buffType>&& vAlloc) {
			operator=((ptr<type, location, buffType>&)vAlloc);
		}


		//Copy assignment
		inline void operator=(const ptr<type, location, buffType>& vAlloc) {
			checkInit(); isInit(vAlloc);
			Super::cell = vAlloc.cell; //vAlloc.cell = nullptr;
			//!                          ^ Don't reset the vAlloc cell. It's required to decrement the owners count in vAlloc destructor
		}


		inline void operator=(const std::nullptr_t) {}




		// Get element -------------------------------------------------------------------------------------------------------------------//
		//TODO ADD SECURITY CHECKS FOR MAPPING




		alwaysInline type& operator[](const uint64 vIndex) const {
			checkInit(); checkMapped();
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			return ((type*)(Super::mapped))[vIndex];
		}
		alwaysInline type& operator*()  const { checkMapped(); checkInit(); return *((type*)(Super::mapped)); }
		alwaysInline type* operator->() const { checkMapped(); checkInit(); return   (type*)(Super::mapped);  }


		/**
		 * @brief Returns the first address of the allocated memory block
		 */
		alwaysInline type* begin() const {
			checkInit(); checkMapped();
            return (type*)Super::mapped;
		}

		/**
		 * @brief Returns the address of the object past the last object in the memory block
		 *		Dereferencing the pointer is undefined behaviour
		 */
		alwaysInline type* end() const {
			checkInit(); checkMapped();
			return (type*)((int8*)Super::mapped + count() * sizeof(type));
		}




		// Count and size ----------------------------------------------------------------------------------------------------------------//




		//Returns the size in BYTES of the allocate memory. use count to get the number of elements
		alwaysInline uint64 size()  const noexcept { return Super::cell->cellSize; }
		//Returns the number of complete elements in the allocated memory
		alwaysInline uint64 count() const noexcept { return Super::cell->cellSize / sizeof(type); }




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
		// 			((uint32)vClass && vSize <= (int64)vClass) || (!(uint32)vClass && vSize <= (vSize / incSize) * incSize)) { //FIXME USE DIFFERENT INCREMENT FOR GPU CELLS
		// 			//! ^ Not (vSize / incSize + 1)
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
		// 					uint64 size_ = (vSize / incSize + 1) * incSize;			//Calculate the new size and allocate the new memory
		// 					cell->address = win10(_aligned_malloc(size_, memOffset)) _linux(aligned_alloc(memOffset, size_));
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
            if(Super::cell->typeIndex != (uint16)-1) {											//For fixed  size cells,
                types[Super::cell->typeIndex].m.lock();
                types[Super::cell->typeIndex].cells.remove(Super::cell->localIndex);						//free the allocation object
                types[Super::cell->typeIndex].m.unlock();
				//FIXME FREE BUFFERS
            }
			else {
				vkFreeMemory(core::dvc::compute.LD, Super::cell->csc.memory);
				vkDestroyBuffer(core::dvc::compute.LD, Super::cell->csc.buffer);
			}

            cells_m.lock();
            cells.remove(Super::cell->cellIndex);												//Free the cell object
            cells_m.unlock();
		}


		alwaysInline bool operator==(ram::ptr<type> vPtr) { return vPtr.cell == Super::cell; }
		alwaysInline bool operator!=(ram::ptr<type> vPtr) { return vPtr.cell != Super::cell; }
		//! If they have the same cell, they also have he same address. No need to access it


		//FIXME SPECIALIZE RAM ALLOCATIONS
		//TODO manually flush data
		/**
		 * @brief Maps the memory block to a RAM pointer, allowing the CPU to access its data through operator[], operator->, operator*, begin() and end() functions.
		 *		The pointer MUST be unmapped by the same thread before the memory is freed.
		 *		This function will automatically invalidate the host cache and flush the data from the GPU
		 */
		void map(){
			dbg::checkCond(Super::mapped, "Memory block mapped twice");
			vkMapMemory(core::dvc::compute.LD, Super::cell->csc.memory, Super::cell->localOffset, Super::cell->cellSize, 0, (void**)&(Super::mapped));
		}

		//TODO manually flush data
		/**
		 * @brief Unmaps the memory block and flushes the data to the GPU.
		 *		This function can only be called on mapped blocks
		 */
		void unmap(){
			dbg::checkCond(!Super::mapped, "unmap() called on unmapped memory");
			vkUnmapMemory(core::dvc::compute.LD, Super::cell->csc.memory);
			luxDebug(Super::mapped = nullptr);
		}
	};







	//TODO CHECK MEMORY FULL
	template<class type, bufferLocation location, bufferType buffType> void lux::vram::ptr<type, location, buffType>::alloc_(const uint64 vSize, const VCellClass vClass) {
		using namespace lux::__pvt;
		//FIXME WRITE USER INTERFACE

		cells_m.lock();
		const auto cellIndex = cells.add(Cell_t2{});						//Save cell index
		cells_m.unlock();
		Super::cell = &cells[cellIndex];										//Update cell pointer
        uint16 typeIndex = (vClass == VCellClass::CLASS_0) ? (uint16)-1 : ((__pvt::classIndexFromEnum(vClass) << 2) | (location << 1) | buffType);
		*Super::cell = Cell_t2{													//Update cell data
			.typeIndex = typeIndex,						//Set cell type index
			.cellIndex  = cellIndex,										//Set cell index
			.cellSize = (uint32)vSize										//Set size specified in function call
		};


		if((uint32)vClass) {											//For fixed class cells
			auto& type_ = types[typeIndex];				//Cache buffer type
			type_.m.lock();
			const auto localIndex = type_.cells.add(true);					//Create a new allocation and save its index
			type_.m.unlock();
			Super::cell->localIndex = localIndex;									//Save local index in cell object
			Super::cell->localOffset = (vClass != VCellClass::CLASS_0) * localIndex * (uint32)vClass;

			const uint32 buffIndex = localIndex / type_.cellsPerBuff;		//Cache buffer index and allocate a new buffer, if necessary
			if(!type_.memory[buffIndex].memory) { //Vulkan structures, but they are set to nullptr and treated as pointers when not used
                //FIXME DONT DUPLICATE BUFFER CHECKS A
                lux::core::buffers::createBuffer(
                    &type_.memory[buffIndex].buffer,
                    ((buffType == bufferType::Uniform) ? VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT : VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT) | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                    __pvt::buffSize,
                    &type_.memory[buffIndex].memory,
                    (location == bufferLocation::Ram) ? (VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, //FIXME IDK
                    core::dvc::compute.LD
                );
            }
            Super::cell->csc.buffer = type_.memory[buffIndex].buffer;
            Super::cell->csc.memory = type_.memory[buffIndex].memory;
		}
		else {															//For custom size cells
			uint64 size = (vSize / __pvt::incSize + 1) * __pvt::incSize;			//Calculate the new size and allocate a new buffer
            //FIXME USE ARBITRARY RANGE FOR COMPATIBILITY
            dbg::checkParam(buffType == bufferType::Uniform && core::dvc::compute.PD.properties.limits.maxUniformBufferRange >= vSize, "vSize", "Allocation is too large to be a uniform buffer");
            //FIXME DONT DUPLICATE BUFFER CHECKS B
            lux::core::buffers::createBuffer(
                &Super::cell->csc.buffer,
                ((buffType == bufferType::Uniform) ? VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT : VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT) | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                size,
                &Super::cell->csc.memory,
                (location == bufferLocation::Ram) ? (VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, //FIXME IDK
				core::dvc::compute.LD
            );
			luxDebug(Super::cell->localIndex = 0;)
		}




		#undef checkAllocSize
	}
}