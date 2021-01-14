// //###############################################################################################################################################//
// //NO PRAGMA ONCE
// //DO NOT INCLUDE THIS FILE. USE RaArray.hpp and __nmp_RaArray.hpp INSTEAD
// //#pragma once
// //###############################################################################################################################################//

// #include "LuxEngine/macros.hpp"
// #include "LuxEngine/Math/Algebra/Algebra.hpp"

// #ifdef __lux_no_gmp
// #	include "LuxEngine/Tests/StructureInit.hpp"
// #	include "cstring"
// #	define gmp(...)
// #	define def(...) __VA_ARGS__
// #	define __RaArrayName __nmp_RaArray
// #else
// #	include "LuxEngine/Types/Containers/ContainerBase.hpp"
// #	define gmp(...) __VA_ARGS__
// #	define def(...)
// #	define __RaArrayName RaArray
// #endif






// //FIXME OK
// //TODO add optional negative index support
// namespace lux {
// 	//
// 	//
// 	//Uses the global memory pool
// 	//New elements are written over previously deleted elements, or concatenated if there are none
// 	//Useful if you need to constantly add or remove elements
// 	//Use the isValid() function to check if an element is valid or has been removed
// 	//TODO remove 3rd and 4th template parameters if theyre useless


// #ifdef __lux_no_gmp
// 	/**
// 	 * @brief Specific version of RaArray used by the memory pool
// 	 */
// 	template<class type, class iter = uint64, CellClass chunkClass = CellClass::CLASS_D> class __nmp_RaArray{
// #else
// 	/**
// 	 * @brief "Random Access Array".
// 	 * 		An array with non-contiguous data capable of containing millions of elements without affecting performance.
// 	 *		Uses the global memory pool.
// 	 *		New elements are written over previously removed ones, or concatenated if there are none.
// 	 *		Use the isValid() function to check if an element is valid or has been removed
// 	 * @tparam type Type of the array elements
// 	 * @tparam iter Type of the elements indices. Default: uint64
// 	 * @tparam chunkClass Class of each chunk in the array. Default: CellClass::CLASS_D (2KB)
// 	 */
// 	template<class type, class iter = uint64, CellClass chunkClass = CellClass::CLASS_D> class RaArray{
// #endif
// 	private:
// 		genInitCheck;
// 		gmp(ram::ptr<ram::ptr<type, alloc>, alloc>)def(type**) chunks_;		//Elements
// 		gmp(ram::ptr<ram::ptr<iter, alloc>, alloc>)def(iter**) tracker_;	//State of each element

// 		iter head;		//First free element
// 		iter tail;		//Last free element
// 		iter used_;		//Number of allocated elements
// 		iter free_;		//Number of free elements in the map

// 		iter _chunkNum;		//Number of allocated chunks

// 		#define chunks (index)  chunks_[(index) / (uint64)chunkClass][(index) % (uint64)chunkClass]	//Get a chunk   element using only one index instead of index in the chunk and chunk index
// 		#define tracker(index) tracker_[(index) / (uint64)chunkClass][(index) % (uint64)chunkClass]	//Get a tracker element using only one index instead of index in the chunk and chunk index
// 	public:




// 		// Constructors -------------------------------------------------------------------------------------------------------- //




// 		inline __RaArrayName( ) : head{ (iter)-1 }, tail{ (iter)-1 }, used_{ 0 }, free_{ 0 },
// 			gmp(	//Allocate 0 pointers in a B class
// 				chunks_ (0, ram::ptr<type, alloc>(), CellClass::AT_LEAST_CLASS_B),
// 				tracker_(0, ram::ptr<iter, alloc>(), CellClass::AT_LEAST_CLASS_B)
// 			)def(	//Allocate
// 				chunks_ { (type**)malloc(sizeof(type*) * (uint64)CellClass::CLASS_B) },
// 				tracker_{ (iter**)malloc(sizeof(iter*) * (uint64)CellClass::CLASS_B) }
// 			){
// 			def(for(int i = 0; i < (uint64)CellClass::CLASS_B; ++i){ chunks_[i] = nullptr; tracker_[i] = nullptr; })
// 		}


// 		/**
// 		 * @brief Initializes the array by copying each element from a lux::ContainerBase subclass
// 		 * @param pContainer The container object to copy elements from
// 		 *		The pContainer iterator must be of equal or smaller type than the one of the array you are initializing
// 		 */
// 		gmp(template<class elmType> inline __RaArrayName(const ContainerBase<elmType, iter>& pContainer) : __RaArrayName( ) {
// 			isInit(pContainer);
// 			for(iter i = 0; i < pContainer.end( ) - pContainer.begin( ); ++i) add((elmType) * (pContainer.begin( ) + i));
// 		})




// 		// Add, remove --------------------------------------------------------------------------------------------------------- //




// 		//TODO REUSE CODE
// 		//TODO FIX DOCUMENTATION
// 		//Adds an element at the end of the map, allocating a new chunk if needed
// 		//Returns the ID of the element
// 		iter append(const type& vData) {
// 			checkInit();
// 			if(size_ + 1 > _chunkNum * chunkSize) {									//If the chunk is full
// 				chunks_ [_chunkNum] gmp(.reallocArr(elmPerChunk, type())) def(= (type*)std::realloc(chunks_ [_chunkNum], elmPerChunk * sizeof(type)));
// 				tracker_[_chunkNum] gmp(.reallocArr(elmPerChunk, iter())) def(= (iter*)std::realloc(tracker_[_chunkNum], elmPerChunk * sizeof(iter)));
// 				def(for(int i = 0; i < elmPerChunk; ++i){
// 					type __type_ = type();  iter __iter_ = iter();
// 					memcpy(&chunks_ [_chunkNum][i], &__type_, sizeof(type));
// 					memcpy(&tracker_[_chunkNum][i], &__iter_, sizeof(iter));
// 				})
// 				_chunkNum++;															//Update the number of chunks
// 			}
// 			__lp_Data(size_) = vData;												//Assign the data to the new element
// 			__lp_Tracker(size_) = -1;												//Set the tracker as valid

// 			return size_++;														//Update the number of elements and return the ID
// 		}
// 		//Adds an element at the end of the map, allocating a new chunk if needed
// 		//Returns the ID of the element
// 		iter append() {
// 			checkInit();
// 			if(size_ + 1 > _chunkNum * chunkSize) {									//If the chunk is full
// 				chunks_ [_chunkNum] gmp(.reallocArr(elmPerChunk, type())) def(= (type*)std::realloc(chunks_ [_chunkNum], elmPerChunk * sizeof(type)));
// 				tracker_[_chunkNum] gmp(.reallocArr(elmPerChunk, iter())) def(= (iter*)std::realloc(tracker_[_chunkNum], elmPerChunk * sizeof(iter)));
// 				def(for(int i = 0; i < elmPerChunk; ++i){
// 					type __type_ = type();  iter __iter_ = iter();
// 					memcpy(&chunks_ [_chunkNum][i], &__type_, sizeof(type));
// 					memcpy(&tracker_[_chunkNum][i], &__iter_, sizeof(iter));
// 				})
// 				_chunkNum++;															//Update the number of chunks
// 			}
// 			__lp_Tracker(size_) = -1;												//Set the tracker as valid

// 			return size_++;														//Update the number of elements and return the ID
// 		}




// 		//TODO REUSE CODE
// 		//TODO FIX DOCUMENTATION
// 		//Adds an element at the firs free index of the map
// 		//Returns the ID of the element
// 		//TODO
// 		iter add(const type& vData) {
// 			checkInit();
// 			if(head_ == (iter)-1) return append(vData);			//If it has no free elements, append it
// 			else {
// 				iter head2 = head_;
// 				if(head_ == tail_) {							//If it has only one free element
// 					__lp_Data(head_) = vData;						//Replace it
// 					head_ = tail_ = __lp_Tracker(head_) = -1;		//And reset head_ and tail_
// 				}
// 				else {											//If it has more than one
// 					__lp_Data(head_) = vData;						//Replace it
// 					head_ = __lp_Tracker(head_);					//Update head_
// 					__lp_Tracker(head2) = -1;						//Update the state of the first
// 				}
// 				freeSize_--;										//Update the number of free elements
// 				if(head_ == 23){ //BUG 23 gets removed even if it shouldn't
// 					int h32424 = 434;
// 				}
// 				return head2;
// 			}
// 		}
// 		//Adds an element at the firs free index of the map
// 		//Returns the ID of the element
// 		auto add() {
// 			checkInit();
// 			struct __ret{ iter index; bool isNew; };
// 			if(head_ == (iter)-1) return __ret{ append(), true };	//If it has no free elements, append it
// 			else {
// 				iter head2 = head_;
// 				if(head_ == tail_) {								//If it has only one, free element
// 					head_ = tail_ = __lp_Tracker(head_) = -1;			//And reset head_ and tail_
// 				}
// 				else {												//If it has more than one
// 					head_ = __lp_Tracker(head_);						//Update head_
// 					__lp_Tracker(head2) = -1;							//Update the state of the first
// 				}
// 				freeSize_--;										//Update the number of free elements
// 				return __ret{ head2, false };
// 			}
// 		}




// 		//Signs an index of the map as free, without deleting it
// 		//*   vIndex:  | the index to sign as free
// 		//*   freeElm: | whether to free the memory or not
// 		//*       Not freeing it saves performance but uses more memory
// 		//*       It will always be freed if the ramaining memory is too low
// 		void remove(const iter vIndex, const bool vFreeElm = false) {
// 			checkInit(); dbg::checkParam(vIndex < 0, vIndex, "Index cannot be negative"); dbg::checkParam(vIndex > count( ), vIndex, "Index is out of range");
// 			__lp_Tracker(vIndex) = -1;								//Set the index as free
// 			if(vFreeElm) free(&__lp_Data(vIndex));					//Free the element if necessary
// 			if(head_ == (iter)-1) head_ = tail_ = vIndex;			//If it has no free elements, initialize head_ and tail_.
// 			else tail_ = __lp_Tracker(tail_) = vIndex;				//If it has free elements, set the new tail_ and update the last free index
// 			freeSize_++;											//Update the number of free elements
// 		}




// 		//Resets the array to its initial state, freeing all the chunks and resizing it to 0
// 		inline void clear( ) {
// 			checkInit();
// 			for(iter i = 0; i < _chunkNum; ++i) {
// 				gmp(chunks_ [i].free()) def(free(chunks_ ));
// 				gmp(tracker_[i].free()) def(free(tracker_));
// 			}
// 			gmp(chunks_ .free()) def(free(chunks_ )); def(chunks_  = nullptr;)
// 			gmp(tracker_.free()) def(free(tracker_)); def(tracker_ = nullptr;)

// 			head_ = tail_ = (iter)-1;
// 			_chunkNum = size_ = freeSize_ = 0;
// 			chunks_  gmp(.reallocArr((uint64)CellClass::CLASS_B, ram::ptr<type, alloc>())) def(= (type**)std::realloc(chunks_,  (uint64)CellClass::CLASS_B * sizeof(type*)));
// 			tracker_ gmp(.reallocArr((uint64)CellClass::CLASS_B, ram::ptr<iter, alloc>())) def(= (iter**)std::realloc(tracker_, (uint64)CellClass::CLASS_B * sizeof(iter*)));
// 			def(for(int i = 0; i < (uint64)CellClass::CLASS_B; ++i){ chunks_[i] = nullptr; tracker_[i] = nullptr; })
// 		}




// 		// Elements state ------------------------------------------------------------------------------------------------------ //




// 		//Returns 0 if the index is used, 1 if the index is free, -1 if the index is invalid, -2 if the index is out of range
// 		inline signed char state(const iter vIndex) const {
// 			checkInit();
// 			if(vIndex < 0) return -1;								//Invalid index
// 			else if(vIndex >= size_) return -2;						//Index out of range
// 			else if(__lp_Tracker(vIndex) == (iter)-1) return 0;		//Used element //OK
// 			else return 1;											//Free element
// 		}


// 		//Returns true if the index is used, false if it's free or out of range (use the "state" function for more details)
// 		inline bool isValid(const iter vIndex) const {
// 			checkInit();
// 			if(vIndex >= size_ || vIndex < 0) return false;
// 			else return (__lp_Tracker(vIndex) == (iter)-1);
// 		}




// 		// Get ----------------------------------------------------------------------------------------------------------------- //




// 		//Use the isValid() function to check if the element can be used or has been deleted
// 		inline type& operator[](const iter vIndex) const {
// 			dbg::checkParam(vIndex < 0, vIndex, "Index cannot be negative"); dbg::checkParam(vIndex >= count( ), vIndex, "Index is out of range");
// 			return __lp_Data(vIndex);
// 		}

// 		//Returns a pointer to the first element of a chunk. The elements are guaranteed to be in contiguous order
// 		inline type* begin(const iter vChunkIndex) const {
// 			checkInit(); dbg::checkParam(vChunkIndex < 0 || vChunkIndex >= _chunkNum, vChunkIndex, "Index is invalid or negative"); return &chunks_[vChunkIndex][0];
// 		}




// 		// Size ---------------------------------------------------------------------------------------------------------------- //




// 		//TODO add size
// 		inline iter count(     ) const { checkInit(); return size_;             } //Returns the number of elements in the map, including the free ones
// 		inline iter usedCount( ) const { checkInit(); return size_ - freeSize_; } //Returns the number of used elements
// 		inline iter freeCount( ) const { checkInit(); return freeSize_;         } //Returns the number of free elements
// 	};
// }
// #undef __lp_Data
// #undef __lp_Tracker
// #undef gmp
// #undef dft
// #undef __RaArrayName