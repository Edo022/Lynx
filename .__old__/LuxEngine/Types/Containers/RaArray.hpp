#pragma once
#define LUX_H_RAARRAY
#include "LuxEngine/macros.hpp"
#include "LuxEngine/Math/Algebra/Algebra.hpp"
#include "LuxEngine/Types/Containers/ContainerBase.hpp"





//TODO add optional negative index support
namespace lux {
	//"Random Access Array"
	//An array with non-contiguous data capable of containing millions of elements without affecting performance
	//Uses the global memory pool
	//New elements are written over previously deleted elements, or concatenated if there are none
	//Useful if you need to constantly add or remove elements
	//Use the isValid() function to check if an element is valid or has been removed
	template<class type, class iter = uint64, uint64 elmPerChunk = max(50000/*50KB*/ / sizeof(type), 4), uint64 chunkSize = sizeof(type)* elmPerChunk> class RaArray {
	private:
		genInitCheck;
		ram::ptr<ram::ptr<type, alloc>, alloc> chunks_;		//Elements
		ram::ptr<ram::ptr<iter, alloc>, alloc> tracker_;	//State of each element

		iter head_;			//First free element
		iter tail_;			//Last free element
		iter size_;			//Number of allocated elements. Use count() instead of this variable
		iter freeSize_;		//Number of free elements in the map. Use usedCount() or freeCount() instead of this variable

		iter _chunkNum;		//Number of allocated chunks

		#define __lp_Data(index) chunks_[(index) / chunkSize][(index) % chunkSize]		//Get a data    element using only one index instead of index in the chunk and chunk index
		#define __lp_Tracker(index) tracker_[(index) / chunkSize][(index) % chunkSize]	//Get a tracker element using only one index instead of index in the chunk and chunk index
	public:




		// Constructors -------------------------------------------------------------------------------------------------------- //




		inline RaArray( ) : head_{ (iter)-1 }, tail_{ (iter)-1 }, _chunkNum{ 0 }, size_{ 0 }, freeSize_{ 0 },
			chunks_ (sizeof(ram::ptr<type>) * (uint64)CellClass::CLASS_B, ram::ptr<type, alloc>()),
			tracker_(sizeof(ram::ptr<iter>) * (uint64)CellClass::CLASS_B, ram::ptr<iter, alloc>()) {
		}


		//Initializes the array using a container object of a compatible type
		//*   pContainer | The container object to copy elements from
		//*       The pContainer iterator must be of equal or smaller type than the one of the object you are initializing
		template<class elmType> inline RaArray(const ContainerBase<elmType, iter>& pContainer) : RaArray( ) {
			isInit(pContainer);
			for(iter i = 0; i < pContainer.end( ) - pContainer.begin( ); ++i) add((elmType) * (pContainer.begin( ) + i));
		}




		// Add, remove --------------------------------------------------------------------------------------------------------- //




		//Adds an element at the end of the map, allocating a new chunk if needed
		//Returns the ID of the element
		iter append(const type& vData) {
			checkInit();
			if(size_ + 1 > _chunkNum * chunkSize) {									//If the chunk is full
				chunks_ [_chunkNum].reallocArr(elmPerChunk, type());
				tracker_[_chunkNum].reallocArr(elmPerChunk, iter());
				_chunkNum++;															//Update the number of chunks
			}
			__lp_Data(size_) = vData;												//Assign the data to the new element
			__lp_Tracker(size_) = -1;												//Set the tracker as valid

			size_++;																//Update the number of elements
			return size_ - 1;														//Return the ID
		}




		//Adds an element at the firs free index of the map
		//Returns the ID of the element
		iter add(const type& vData) {
			checkInit();
			if(head_ == (iter)-1) return append(vData);			//If it has no free elements, append it
			else {
				iter head2 = head_;
				if(head_ == tail_) {							//If it has only one free element
					__lp_Data(head2) = vData;						//Replace it
					head_ = tail_ = __lp_Tracker(head2) = -1;		//And reset head_ and tail_
				}
				else {											//If it has more than one
					head_ = __lp_Tracker(head2);					//Update head_
					__lp_Tracker(head2) = -1;						//Update the state of the first
					__lp_Data(head2) = vData;						//Replace it
				}
				freeSize_--;										//Update the number of free elements
				return head2;
			}
		}




		//Signs an index of the map as free, without deleting it
		//*   vIndex:  | the index to sign as free
		//*   freeElm: | whether to free the memory or not
		//*       Not freeing it saves performance but uses more memory
		//*       It will always be freed if the ramaining memory is too low
		void remove(const iter vIndex, const bool vFreeElm = false) {
			checkInit(); luxCheckParam(vIndex < 0, vIndex, "Index cannot be negative"); luxCheckParam(vIndex > count( ), vIndex, "Index is out of range");
			__lp_Tracker(vIndex) = -1;								//Set the index as free
			if(vFreeElm) free(&__lp_Data(vIndex));					//Free the element if necessary
			if(head_ == (iter)-1) head_ = tail_ = vIndex;			//If it has no free elements, initialize head_ and tail_.
			else tail_ = __lp_Tracker(tail_) = vIndex;				//If it has free elements, set the new tail_ and update the last free index
			freeSize_++;											//Update the number of free elements
		}




		//Resets the array to its initial state, freeing all the chunks and resizing it to 0
		inline void clear( ) {
			checkInit();
			for(iter i = 0; i < _chunkNum; ++i) {
				chunks_ [i].free();
				tracker_[i].free();
			}
			chunks_ .free();
			tracker_.free();

			// this->Map::Map( );
			//TODO fix
			//TODO constructor. remove
			head_ = tail_ = (iter)-1;
			_chunkNum = size_ = freeSize_ = 0;
			chunks_ .reallocArr((uint64)CellClass::CLASS_B, ram::ptr<type, alloc>());
			tracker_.reallocArr((uint64)CellClass::CLASS_B, ram::ptr<iter, alloc>());
		}




		// Elements state ------------------------------------------------------------------------------------------------------ //




		//Returns 0 if the index is used, 1 if the index is free, -1 if the index is invalid, -2 if the index is out of range
		inline signed char state(const iter vIndex) const {
			checkInit();
			if(vIndex < 0) return -1;								//Invalid index
			else if(vIndex >= size_) return -2;						//Index out of range
			else if(__lp_Tracker(vIndex) == (iter)-1) return 0;		//Used element //OK
			else return 1;											//Free element
		}


		//Returns true if the index is used, false if it's free or out of range (use the "state" function for more details)
		inline bool isValid(const iter vIndex) const {
			checkInit();
			if(vIndex >= size_ || vIndex < 0) return false;
			else return (__lp_Tracker(vIndex) == (iter)-1);
		}




		// Get ----------------------------------------------------------------------------------------------------------------- //




		//Use the isValid() function to check if the element can be used or has been deleted
		inline type& operator[](const iter vIndex) const {
			luxCheckParam(vIndex < 0, vIndex, "Index cannot be negative"); luxCheckParam(vIndex >= count( ), vIndex, "Index is out of range");
			return __lp_Data(vIndex);
		}

		//Returns a pointer to the first element of a chunk. The elements are guaranteed to be in contiguous order
		inline type* begin(const iter vChunkIndex) const {
			checkInit(); luxCheckParam(vChunkIndex < 0 || vChunkIndex >= _chunkNum, vChunkIndex, "Index is invalid or negative"); return &chunks_[vChunkIndex][0];
		}


//TODO add function to send multiple functions to the thread pool to allow them to be execute simultaneously
//TODO while preventing their threads to go in deadlock if synchronized

		// Size ---------------------------------------------------------------------------------------------------------------- //




		//TODO add size
		inline iter count(     ) const { checkInit(); return size_;             } //Returns the number of elements in the map, including the free ones
		inline iter usedCount( ) const { checkInit(); return size_ - freeSize_; } //Returns the number of used elements
		inline iter freeCount( ) const { checkInit(); return freeSize_;         } //Returns the number of free elements
	};
}
#undef __lp_Data
#undef __lp_Tracker
