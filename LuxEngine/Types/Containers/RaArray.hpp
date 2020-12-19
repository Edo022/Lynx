#pragma once
#define LUX_H_RAARRAY
#include "LuxEngine/macros.hpp"
#include "LuxEngine/Math/Algebra/Algebra.hpp"








namespace lux {
	/**
	 * @brief "Random Access Array".
	 * 		Basically non-contiguous array of arrays.
	 *		New elements are written over previously removed ones, or concatenated if there are none.
	 *		The .isValid() function can be used to check if an element is valid or has been removed
	 * @tparam type Type of the array elements
	 * @tparam iter Type of the elements indices. Must be an integer type. Default: uint32
	 * @tparam chunkClass Class of each chunk in the array. Default: CellClass::CLASS_D (2KB)
	 */
	template<class type, class iter = uint32, CellClass chunkClass = CellClass::CLASS_D> class RaArray{
	private:
		genInitCheck;
		ram::ptr<ram::ptr<type, alloc>, alloc> chunks_;		//Elements
		ram::ptr<ram::ptr<iter, alloc>, alloc> tracker_;	//State of each element

		iter head;		//First free element
		iter tail;		//Last free element
		iter size_;		//Number of allocated elements
		iter free_;		//Number of free elements in the map

		#define chunks (index)  chunks_[(index) / (uint64)chunkClass][(index) % (uint64)chunkClass]	//Get a chunk   element using only one index instead of index in the chunk and chunk index
		#define tracker(index) tracker_[(index) / (uint64)chunkClass][(index) % (uint64)chunkClass]	//Get a tracker element using only one index instead of index in the chunk and chunk index

		
	public:




		// Constructors -------------------------------------------------------------------------------------------------------- //
		//FIXME ADD SIZE CONSTRUCTOR
		//FIXME USE SIZE CONSTRUCTOR IN CONTAINER CONSTRUCTOR TO PREVENT REALLOCATIONS




		/**
		 * @brief Creates an array with size 0 and no preallocated chunks
		 */
		inline RaArray( ) : head{ (iter)-1 }, tail{ (iter)-1 }, size_{ 0 }, free_{ 0 },
			chunks_ (0, ram::ptr<type, alloc>(), CellClass::AT_LEAST_CLASS_B),
			tracker_(0, ram::ptr<iter, alloc>(), CellClass::AT_LEAST_CLASS_B) {
		}		
		
		
		
		
		/**
		 * @brief Initializes the array by copying each element from a lux::ContainerBase subclass
		 * @param pCont The container object to copy elements from.
		 *		It must be a valid lux::ContainerBase subclass instance with a compatible type and 
		 *		less elements than the maximum number of elements of the array you are initializing
		 */
		template<class eType, class iType> inline RaArray(const ContainerBase<eType, iType>& pCont) : constructExec(isInit(pCont)) RaArray( ) {
			//TODO check sizes in constructexec
			for(auto i = pCont.begin(); i < pCont.end( ); ++i) add((type)(*pCont.begin( ))));
		}




		/**
		 * @brief Initializes the array by copying each element from a RaArray. Removed elements are preserved.
		 * @param pCont The RaArray to copy elements from. 
		 *		It must be a valid RaArray instance with a compatible type and 
		 *		less elements than the maximum number of elements of the array you are initializing
		 */
		template<class eType, class iType> inline RaArray(const RaArray<eType, iType>& pCont) : constructExec(isInit(pCont))
			head{ pCont.head }, tail{ pCont.tail }, size_{ pCont.size_ }, free_{ pCont.free_ },
			chunks_  (pCont.chunks_ .deepCopy()), 
			tracker_ (pCont.tracker_.deepCopy()){
			//TODO check sizes in constructexec
			for(int i = 0; i < pCont.chunks_.count(); ++i){ 
				chunks_[i] = pCont.chunks_[i].deepCopy();   //Deeper copy
				tracker_[i] = pCont.tracker_[i].deepCopy(); //UwU
			}
		}




		/**
		 * @brief Copy constructor. Elements are copied in a new memory allocation. Removed elements are preserved.
		 */
		inline RaArray(const RaArray<type, iter>& pCont) : constructExec(isInit(pCont))
			head{ pCont.head }, tail{ pCont.tail }, size_{ pCont.size_ }, free_{ pCont.free_ },
			chunks_ (pCont.chunks_ .deepCopy()), tracker_ (pCont.tracker_.deepCopy()){
			// chunks_ (pCont.chunks_ .size(), ram::ptr<type, alloc>(), CellClass::AT_LEAST_CLASS_B),
			// tracker_(pCont.tracker_.size(), ram::ptr<iter, alloc>(), CellClass::AT_LEAST_CLASS_B) {
			// for(iter i = 0; i < pCont.end( ) - pCont.begin( ); ++i) add((elmType) * (pCont.begin( ) + i));
			for(int i = 0; i < pCont.chunks_.count(); ++i){ 
				chunks_[i] = pCont.chunks_[i].deepCopy();
				tracker_[i] = pCont.tracker_[i].deepCopy();
			}
		}




		/**
		 * @brief Move constructor
		 */
		inline RaArray(RaArray<type, iter>&& pCont) : constructExec(isInit(pCont))
			head{ pCont.head }, tail{ pCont.tail }, size_{ pCont.size_ }, free_{ pCont.free_ },
			chunks_{pCont.chunks_}, tracker_{pCont.tracker_} {
			pCont.chunks_ = pCont.tracker_ = nullptr; //FIXME
		}




		// Add, remove --------------------------------------------------------------------------------------------------------- //
//TODO add shrink function to reorder the elements and use less memory possible



		//TODO REUSE CODE
		//TODO FIX DOCUMENTATION
		//Adds an element at the end of the map, allocating a new chunk if needed
		//Returns the ID of the element
		iter append(const type& vData) {
			checkInit();
			if(size_ + 1 > _chunkNum * chunkSize) {									//If the chunk is full
				chunks_ [_chunkNum].reallocArr(elmPerChunk, type());
				tracker_[_chunkNum].reallocArr(elmPerChunk, iter());
				_chunkNum++;															//Update the number of chunks
			}
			chunks (size_) = vData;												//Assign the data to the new element
			tracker(size_) = -1;												//Set the tracker as valid

			return size_++;														//Update the number of elements and return the ID
		}
		//Adds an element at the end of the map, allocating a new chunk if needed
		//Returns the ID of the element
		iter append() {
			checkInit();
			if(size_ + 1 > _chunkNum * chunkSize) {									//If the chunk is full
				chunks_ [_chunkNum].reallocArr(elmPerChunk, type());
				tracker_[_chunkNum].reallocArr(elmPerChunk, iter());
				_chunkNum++;															//Update the number of chunks
			}
			tracker(size_) = -1;												//Set the tracker as valid

			return size_++;														//Update the number of elements and return the ID
		}




		//TODO REUSE CODE
		//TODO FIX DOCUMENTATION
		//Adds an element at the firs free index of the map
		//Returns the ID of the element
		//TODO
		iter add(const type& vData) {
			checkInit();
			if(head == (iter)-1) return append(vData);			//If it has no free elements, append it
			else {
				iter head2 = head;
				if(head == tail) {							//If it has only one free element
					chunks(head) = vData;						//Replace it
					head = tail = tracker(head) = -1;		//And reset head and tail
				}
				else {											//If it has more than one
					chunks(head) = vData;						//Replace it
					head = tracker(head);					//Update head
					tracker(head2) = -1;						//Update the state of the first
				}
				freeSize_--;										//Update the number of free elements
				return head2;
			}
		}
		//Adds an element at the firs free index of the map
		//Returns the ID of the element
		auto add() {
			checkInit();
			struct __ret{ iter index; bool isNew; };
			if(head == (iter)-1) return __ret{ append(), true };	//If it has no free elements, append it
			else {
				iter head2 = head;
				if(head == tail) {								//If it has only one, free element
					head = tail = tracker(head) = -1;			//And reset head and tail
				}
				else {												//If it has more than one
					head = tracker(head);						//Update head
					tracker(head2) = -1;							//Update the state of the first
				}
				freeSize_--;										//Update the number of free elements
				return __ret{ head2, false };
			}
		}




		//Signs an index of the map as free, without deleting it
		//*   vIndex:  | the index to sign as free
		//*   freeElm: | whether to free the memory or not
		//*       Not freeing it saves performance but uses more memory
		//*       It will always be freed if the ramaining memory is too low
		void remove(const iter vIndex, const bool vFreeElm = false) {
			checkInit(); luxCheckParam(vIndex < 0, vIndex, "Index cannot be negative"); luxCheckParam(vIndex > count( ), vIndex, "Index is out of range");
			tracker(vIndex) = -1;								//Set the index as free
			if(vFreeElm) free(&chunks(vIndex));					//Free the element if necessary
			if(head == (iter)-1) head = tail = vIndex;			//If it has no free elements, initialize head and tail.
			else tail = tracker(tail) = vIndex;				//If it has free elements, set the new tail and update the last free index
			freeSize_++;											//Update the number of free elements
		}




		//Resets the array to its initial state, freeing all the chunks and resizing it to 0
		inline void clear( ) {
			checkInit();
			for(iter i = 0; i < _chunkNum; ++i) {
				chunks_ [i].free();
				tracker_[i].free();
			}
			chunks_ .free());
			tracker_.free());

			head = tail = (iter)-1;
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
			else if(tracker(vIndex) == (iter)-1) return 0;		//Used element //OK
			else return 1;											//Free element
		}


		//Returns true if the index is used, false if it's free or out of range (use the "state" function for more details)
		inline bool isValid(const iter vIndex) const {
			checkInit();
			if(vIndex >= size_ || vIndex < 0) return false;
			else return (tracker(vIndex) == (iter)-1);
		}




		// Get ----------------------------------------------------------------------------------------------------------------- //




		//Use the isValid() function to check if the element can be used or has been deleted
		inline type& operator[](const iter vIndex) const {
			luxCheckParam(vIndex < 0, vIndex, "Index cannot be negative"); luxCheckParam(vIndex >= count( ), vIndex, "Index is out of range");
			return chunks(vIndex);
		}

		//Returns a pointer to the first element of a chunk. The elements are guaranteed to be in contiguous order
		inline type* begin(const iter vChunkIndex) const {
			checkInit(); luxCheckParam(vChunkIndex < 0 || vChunkIndex >= _chunkNum, vChunkIndex, "Index is invalid or negative"); return &chunks_[vChunkIndex][0];
		}




		// Size ---------------------------------------------------------------------------------------------------------------- //




		//TODO add size
		inline iter count(     ) const { checkInit(); return size_;             } //Returns the number of elements in the map, including the free ones
		inline iter usedCount( ) const { checkInit(); return size_ - freeSize_; } //Returns the number of used elements
		inline iter freeCount( ) const { checkInit(); return freeSize_;         } //Returns the number of free elements
	};
}
#undef chunks
#undef tracker
