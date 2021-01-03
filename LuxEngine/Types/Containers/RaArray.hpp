// #pragma once
// #define LUX_H_CellMng_t
// #include "LuxEngine/Math/Algebra/Algebra.hpp"
// #include "LuxEngine/Types/Containers/ContainerBase.hpp"
// #include "LuxEngine/Types/Pointer.hpp"








// namespace lux {
// 	/**
// 	 * @brief "Random Access Array".
// 	 * 		Basically non-contiguous array of arrays.
// 	 *		New elements are written over previously removed ones, or concatenated if there are none.
// 	 *		The .isValid() function can be used to check if an element is valid or has been removed
// 	 * @tparam type Type of the array elements
// 	 * @tparam iter Type of the elements indices. Must be an integer type. Default: uint32
// 	 * @tparam chunkClass Class of each chunk in the array. Default: CellClass::CLASS_D (2KB)
// 	 */
// 	template<class type, class iter = uint32, CellClass chunkClass = CellClass::CLASS_D> class RaArray{
// 	private:
// 		genInitCheck;
// 		ram::Alloc<ram::Alloc<type>> chunks_;		//Elements
// 		ram::Alloc<ram::Alloc<iter>> tracker_;	//State of each element

// 		iter head;		//First free element
// 		iter tail;		//Last free element
// 		iter size_;		//Number of allocated elements
// 		iter free_;		//Number of free elements in the map

// 		constexpr inline type& chunks (iter index) const { return  chunks_[(index) / (uint64)chunkClass][(index) % (uint64)chunkClass]; }	//Get a chunk   element using only one index instead of index in the chunk and chunk index
// 		constexpr inline iter& tracker(iter index) const { return tracker_[(index) / (uint64)chunkClass][(index) % (uint64)chunkClass]; }	//Get a tracker element using only one index instead of index in the chunk and chunk index


// 	public:




// 		// Constructors -------------------------------------------------------------------------------------------------------- //
// 		//FIXME ADD SIZE CONSTRUCTOR
// 		//FIXME USE SIZE CONSTRUCTOR IN CONTAINER CONSTRUCTOR TO PREVENT REALLOCATIONS




// 		/**
// 		 * @brief Creates an array with size 0 and no preallocated chunks
// 		 */
// 		inline RaArray( ) : head{ (iter)-1 }, tail{ (iter)-1 }, size_{ 0 }, free_{ 0 },
// 			chunks_ (0, CellClass::AT_LEAST_CLASS_B),
// 			tracker_(0, CellClass::AT_LEAST_CLASS_B) {
// 		}




// 		/**
// 		 * @brief Initializes the array by copying each element from a lux::ContainerBase subclass
// 		 * @param pCont The container object to copy elements from.
// 		 *		It must be a valid lux::ContainerBase subclass instance with a compatible type and
// 		 *		less elements than the maximum number of elements of the array you are initializing
// 		 */
// 		template<class eType, class iType> inline RaArray(const ContainerBase<eType, iType>& pCont) : /*constructExec(isInit(pCont))*/ RaArray( ) {
// 			//TODO check sizes in constructexec
// 			for(auto i = pCont.begin(); i < pCont.end( ); ++i) add((type)(*pCont.begin( )));
// 		}




// 		/**
// 		 * @brief Initializes the array by copying each element from a RaArray. Removed elements are preserved.
// 		 * @param pCont The RaArray to copy elements from.
// 		 *		It must be a valid RaArray instance with a compatible type and
// 		 *		less elements than the maximum number of elements of the array you are initializing
// 		 */
// 		template<class eType, class iType> inline RaArray(const RaArray<eType, iType>& pCont) : constructExec(isInit(pCont))
// 			head{ pCont.head }, tail{ pCont.tail }, size_{ pCont.size_ }, free_{ pCont.free_ },
// 			chunks_  (pCont.chunks_ .deepCopy()),
// 			tracker_ (pCont.tracker_.deepCopy()){
// 			//TODO check sizes in constructexec
// 			for(int i = 0; i < pCont.chunks_.count(); ++i){
// 				chunks_[i] = pCont.chunks_[i].deepCopy();   //Deeper copy
// 				tracker_[i] = pCont.tracker_[i].deepCopy(); //UwU
// 			}
// 		}




// 		/**
// 		 * @brief Copy constructor. Elements are copied in a new memory allocation. Removed elements are preserved.
// 		 */
// 		inline RaArray(const RaArray<type, iter>& pCont) : constructExec(isInit(pCont))
// 			head{ pCont.head }, tail{ pCont.tail }, size_{ pCont.size_ }, free_{ pCont.free_ },
// 			chunks_ (pCont.chunks_ .deepCopy()), tracker_ (pCont.tracker_.deepCopy()){
// 			// chunks_ (pCont.chunks_ .size(), ram::ptr<type, alloc>(), CellClass::AT_LEAST_CLASS_B),
// 			// tracker_(pCont.tracker_.size(), ram::ptr<iter, alloc>(), CellClass::AT_LEAST_CLASS_B) {
// 			// for(iter i = 0; i < pCont.end( ) - pCont.begin( ); ++i) add((elmType) * (pCont.begin( ) + i));
// 			for(int i = 0; i < pCont.chunks_.count(); ++i){
// 				chunks_[i] = pCont.chunks_[i].deepCopy();
// 				tracker_[i] = pCont.tracker_[i].deepCopy();
// 			}
// 		}




// 		/**
// 		 * @brief Move constructor
// 		 */
// 		inline RaArray(RaArray<type, iter>&& pCont) : constructExec(isInit(pCont))
// 			head{ pCont.head }, tail{ pCont.tail }, size_{ pCont.size_ }, free_{ pCont.free_ },
// 			chunks_{pCont.chunks_}, tracker_{pCont.tracker_} {
// 			pCont.chunks_ = pCont.tracker_ = nullptr; //FIXME
// 		}




// 		// Add, remove --------------------------------------------------------------------------------------------------------- //
// //TODO add shrink function to reorder the elements and use less memory possible




// 		/**
// 		 * @brief Adds a new element at the end of the array, without intializing it
// 		 * @return Index of the new element
// 		 */
// 		iter append() {
// 			checkInit();
// 			if(size_ + 1 > chunks_.count() * (uint64)chunkClass) {					//If the chunk is full
// 			//BUG use contiguous array
// 				// chunks_ [chunks_.count()].reallocArr((uint64)chunkClass, type());		//Create a new one
// 				// tracker_[chunks_.count()].reallocArr((uint64)chunkClass, iter());
// 				chunks_ .reallocArr((uint64)chunkClass, type());		//Create a new one
// 				tracker_.reallocArr((uint64)chunkClass, iter());
// 			}
// 			tracker(size_) = -1;	//Set the tracker as valid
// 			return size_++;			//Update the number of elements and return the ID
// 		}


// 		/**
// 		 * @brief Adds an element at the end of the array.
// 		 * @param pData Value the new element will be initialized with
// 		 * @return Index of the new element
// 		 */
// 		inline iter append(const type& pData) {
// 			checkInit();
// 			append();
// 			chunks(size_ - 1) = pData;
// 			return size_;
// 		}




// 		/**
// 		 * @brief Adds an element at the first free index of the array without initializing it
//  		 * @return Index of the new element
// 		 */
// 		auto add() {
// 			checkInit();
// 			if(head == (iter)-1) return append();		//If it has no free elements, append it
// 			iter head2 = head;
// 			if(head == tail) {							//If it has only one free element
// 				head = tail = tracker(head) = -1;			//Reset head, tail and tracker
// 			}
// 			else {										//If it has more than one
// 				head = tracker(head);						//Update head
// 				tracker(head2) = -1;						//Update tracker of the old head element
// 			}
// 			free_--;									//Update number of free elements
// 			return head2;
// 		}


// 		/**
// 		 * @brief Adds an element at the first free index of the array
// 		 * @param vData Value the new element will be initialized with
// 		 * @return Index of the new element
// 		 */
// 		inline iter add(const type& vData) {
// 			checkInit();
// 			auto i = add();
// 			chunks(i) = vData;
// 			return i;
// 		}




// 		/**
// 		 * @brief Removed an element without freeing it
// 		 * @param vIndex Index of the element to remove
// 		 */
// 		void remove(const iter vIndex) {
// 			checkInit(); luxCheckParam(vIndex < 0, vIndex, "Index cannot be negative"); luxCheckParam(vIndex > count( ), vIndex, "Index is out of range");
// 			tracker(vIndex) = -1;								//Set the index as free
// 			if(head == (iter)-1) head = tail = vIndex;			//If it has no free elements, initialize head and tail.
// 			else tail = tracker(tail) = vIndex;					//If it has free elements, set the new tail and update the last free index
// 			free_++;											//Update the number of free elements
// 		}




// 		/**
// 		 * @brief Resets the array to its initial state, freeing all the chunks and resizing it to 0
// 		 */
// 		inline void clear( ) {
// 			checkInit();
// 			for(iter i = 0; i < chunks_.count(); ++i) {
// 				chunks_ [i].free();
// 				tracker_[i].free();
// 			}
// 			// chunks_ .free(); //BUG dont free
// 			// tracker_.free();

// 			head = tail = (iter)-1;
// 			size_ = free_ = 0;
// 			chunks_ .reallocArr(0, CellClass::AT_LEAST_CLASS_B);
// 			tracker_.reallocArr(0, CellClass::AT_LEAST_CLASS_B);
// 		}




// 		// Elements state ------------------------------------------------------------------------------------------------------ //




// 		// //Returns 0 if the index is used, 1 if the index is free, -1 if the index is invalid, -2 if the index is out of range
// 		// inline signed char state(const iter vIndex) const {
// 		// 	checkInit();
// 		// 	if(vIndex < 0) return -1;								//Invalid index
// 		// 	else if(vIndex >= size_) return -2;						//Index out of range
// 		// 	else if(tracker(vIndex) == (iter)-1) return 0;		//Used element //OK
// 		// 	else return 1;											//Free element
// 		// }


// 		/**
// 		 * @brief Returns the state of an element
// 		 * @param vIndex Index of the element
// 		 * @return True if the element is valid (non deleted), false if not
// 		 */
// 		inline bool isValid(const iter vIndex) const noexcept {
// 			checkInit();
// 			luxCheckParam(vIndex < 0, vIndex, "Index cannot be negative"); luxCheckParam(vIndex >= count( ), vIndex, "Index is out of range");
// 			return (tracker(vIndex) == (iter)-1);
// 		}




// 		// Get ----------------------------------------------------------------------------------------------------------------- //




// 		inline type& operator[](const iter vIndex) const noexcept {
// 			luxCheckParam(vIndex < 0, vIndex, "Index cannot be negative"); luxCheckParam(vIndex >= count( ), vIndex, "Index is out of range");
// 			return chunks(vIndex);
// 		}

// 		// //Returns a pointer to the first element of a chunk. The elements are guaranteed to be in contiguous order
// 		// /**
// 		//  * @param vChunkIndex
// 		//  * @return type*
// 		//  */
// 		// inline type* begin(const iter vChunkIndex) const {
// 		// 	checkInit(); luxCheckParam(vChunkIndex < 0 || vChunkIndex >= _chunkNum, vChunkIndex, "Index is invalid or negative"); return &chunks_[vChunkIndex][0];
// 		// }




// 		// Size ---------------------------------------------------------------------------------------------------------------- //




// 		//TODO add size
// 		inline iter count(     ) const noexcept { checkInit(); return size_;         } //Returns the number of elements in the map, including the free ones
// 		inline iter usedCount( ) const noexcept { checkInit(); return size_ - free_; } //Returns the number of used elements
// 		inline iter freeCount( ) const noexcept { checkInit(); return free_;         } //Returns the number of free elements
// 	};
// }
// #undef chunks
// #undef tracker
































#pragma once
#define LUX_H_CellMng_t
#include "LuxEngine/Math/Algebra/Algebra.hpp"
#include "LuxEngine/Types/Containers/ContainerBase.hpp"
#include "LuxEngine/Types/Pointer.hpp"








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
	template<class type, class iter = uint32> class RaArray{
	private:
		genInitCheck;
		ram::Alloc<type> data;	//Elements
		ram::Alloc<iter> lnkd;	//State of each element

		iter head;		//First free element
		iter tail;		//Last free element
		iter size_;		//Number of allocated elements
		iter free_;		//Number of free elements in the map


	public:




		// Constructors -------------------------------------------------------------------------------------------------------- //
		//FIXME ADD SIZE CONSTRUCTOR
		//FIXME USE SIZE CONSTRUCTOR IN CONTAINER CONSTRUCTOR TO PREVENT REALLOCATIONS




		/**
		 * @brief Creates an array with size 0 and no preallocated chunks
		 */
		inline RaArray( ) : head{ (iter)-1 }, tail{ (iter)-1 }, size_{ 0 }, free_{ 0 },
			data(0, CellClass::AT_LEAST_CLASS_B),
			lnkd(0, CellClass::AT_LEAST_CLASS_B) {
		}




		/**
		 * @brief Initializes the array by copying each element from a lux::ContainerBase subclass
		 * @param pCont The container object to copy elements from.
		 *		It must be a valid lux::ContainerBase subclass instance with a compatible type and
		 *		less elements than the maximum number of elements of the array you are initializing
		 */
		template<class eType, class iType> inline RaArray(const ContainerBase<eType, iType>& pCont) : /*constructExec(isInit(pCont))*/ RaArray( ) {
			//TODO check sizes in constructexec
			for(auto i = pCont.begin(); i < pCont.end( ); ++i) add((type)(*i));
		}




		/**
		 * @brief Initializes the array by copying each element from a RaArray. Removed elements are preserved.
		 * @param pCont The RaArray to copy elements from.
		 *		It must be a valid RaArray instance with a compatible type and
		 *		less elements than the maximum number of elements of the array you are initializing
		 */
		template<class eType, class iType> inline RaArray(const RaArray<eType, iType>& pCont) : constructExec(isInit(pCont))
			head{ pCont.head }, tail{ pCont.tail }, size_{ pCont.size_ }, free_{ pCont.free_ },
			data(pCont.data.deepCopy()),
			lnkd(pCont.lnkd.deepCopy()){
			//TODO check sizes in constructexec
			// for(int i = 0; i < pCont.chunks_.count(); ++i){
			// 	chunks_[i] = pCont.chunks_[i].deepCopy();   //Deeper copy
			// 	tracker_[i] = pCont.tracker_[i].deepCopy(); //UwU
			// }
		}




		/**
		 * @brief Copy constructor. Elements are copied in a new memory allocation. Removed elements are preserved.
		 */
		inline RaArray(const RaArray<type, iter>& pCont) : constructExec(isInit(pCont))
			head{ pCont.head }, tail{ pCont.tail }, size_{ pCont.size_ }, free_{ pCont.free_ },
			data(pCont.data.deepCopy()), lnkd(pCont.lnkd.deepCopy()){
			// // chunks_ (pCont.chunks_ .size(), ram::ptr<type, alloc>(), CellClass::AT_LEAST_CLASS_B),
			// // tracker_(pCont.tracker_.size(), ram::ptr<iter, alloc>(), CellClass::AT_LEAST_CLASS_B) {
			// // for(iter i = 0; i < pCont.end( ) - pCont.begin( ); ++i) add((elmType) * (pCont.begin( ) + i));
			// for(int i = 0; i < pCont.chunks_.count(); ++i){
			// 	chunks_[i] = pCont.chunks_[i].deepCopy();
			// 	tracker_[i] = pCont.tracker_[i].deepCopy();
			// }
		}




		/**
		 * @brief Move constructor
		 */
		inline RaArray(RaArray<type, iter>&& pCont) : constructExec(isInit(pCont))
			head{ pCont.head }, tail{ pCont.tail }, size_{ pCont.size_ }, free_{ pCont.free_ },
			data{ pCont.data }, lnkd{ pCont.lnkd } {
			pCont.data = pCont.lnkd = nullptr; //FIXME
		}




		// Add, remove --------------------------------------------------------------------------------------------------------- //
//TODO add shrink function to reorder the elements and use less memory possible

//TODO add self check on unusable elements


		/**
		 * @brief Adds a new element at the end of the array, without intializing it
		 * @return Index of the new element
		 */
		iter append() {
			checkInit();
			// if(size_ + 1 > data.count()/* * (uint64)chunkClass*/) {					//If the chunk is full
				// chunks_ [chunks_.count()].reallocArr((uint64)chunkClass, type());		//Create a new one
				// tracker_[chunks_.count()].reallocArr((uint64)chunkClass, iter());
				data.reallocArr(count() + 1, type());		//Create a new one
				lnkd.reallocArr(count() + 1, iter());
			// }
			lnkd[size_] = -1;	//Set the tracker as valid
			return size_++;			//Update the number of elements and return the ID
		}


		/**
		 * @brief Adds an element at the end of the array.
		 * @param pData Value the new element will be initialized with
		 * @return Index of the new element
		 */
		inline iter append(const type& pData) {
			checkInit();
			append();
			data[size_ - 1] = pData;
			return size_;
		}



//TODO
		/**
		 * @brief Adds an element at the first free index of the array without initializing it
 		 * @return Index of the new element
		 */
		auto add() {
			checkInit();
			if(head == (iter)-1) return append();		//If it has no free elements, append it
			iter head2 = head;
			if(head == tail) {							//If it has only one free element
				head = tail = lnkd[head] = -1;			//Reset head, tail and tracker
			}
			else {										//If it has more than one
				head = lnkd[head];						//Update head
				lnkd[head2] = -1;						//Update tracker of the old head element
			}
			free_--;									//Update number of free elements
			return head2;
		}


		/**
		 * @brief Adds an element at the first free index of the array
		 * @param vData Value the new element will be initialized with
		 * @return Index of the new element
		 */
		inline iter add(const type& vData) {
			checkInit();
			auto i = add();
			data[i] = vData;
			return i;
		}




		/**
		 * @brief Removed an element without freeing it
		 * @param vIndex Index of the element to remove
		 */
		void remove(const iter vIndex) {
			checkInit(); luxCheckParam(vIndex < 0, vIndex, "Index cannot be negative"); luxCheckParam(vIndex > count( ), vIndex, "Index is out of range");
			lnkd[vIndex] = -1;								//Set the index as free
			if(head == (iter)-1) head = tail = vIndex;			//If it has no free elements, initialize head and tail.
			else tail = lnkd[tail] = vIndex;					//If it has free elements, set the new tail and update the last free index
			free_++;											//Update the number of free elements
		}




		/**
		 * @brief Resets the array to its initial state, freeing all the chunks and resizing it to 0
		 */
		inline void clear( ) {
			checkInit();
			// for(iter i = 0; i < data.count(); ++i) {
			// 	chunks_ [i].free();
			// 	tracker_[i].free();
			// }
			// chunks_ .free(); //BUG dont free
			// tracker_.free();

			head = tail = (iter)-1;
			size_ = free_ = 0;
			data.reallocArr(0, CellClass::AT_LEAST_CLASS_B);
			lnkd.reallocArr(0, CellClass::AT_LEAST_CLASS_B);
		}




		// Elements state ------------------------------------------------------------------------------------------------------ //




		// //Returns 0 if the index is used, 1 if the index is free, -1 if the index is invalid, -2 if the index is out of range
		// inline signed char state(const iter vIndex) const {
		// 	checkInit();
		// 	if(vIndex < 0) return -1;								//Invalid index
		// 	else if(vIndex >= size_) return -2;						//Index out of range
		// 	else if(tracker(vIndex) == (iter)-1) return 0;		//Used element //OK
		// 	else return 1;											//Free element
		// }


		/**
		 * @brief Returns the state of an element
		 * @param vIndex Index of the element
		 * @return True if the element is valid (non deleted), false if not
		 */
		inline bool isValid(const iter vIndex) const noexcept {
			checkInit();
			luxCheckParam(vIndex < 0, vIndex, "Index cannot be negative");
			luxCheckParam(vIndex >= count( ), vIndex, "Index is out of range");
			return (lnkd[vIndex] == (iter)-1);
		}




		// Get ----------------------------------------------------------------------------------------------------------------- //




		inline type& operator[](const iter vIndex) const noexcept {
			luxCheckParam(vIndex < 0, vIndex, "Index cannot be negative");
			luxCheckParam(vIndex >= count( ), vIndex, "Index is out of range");
			return data[vIndex];
		}

		// //Returns a pointer to the first element of a chunk. The elements are guaranteed to be in contiguous order
		// /**
		//  * @param vChunkIndex
		//  * @return type*
		//  */
		// inline type* begin(const iter vChunkIndex) const {
		// 	checkInit(); luxCheckParam(vChunkIndex < 0 || vChunkIndex >= _chunkNum, vChunkIndex, "Index is invalid or negative"); return &chunks_[vChunkIndex][0];
		// }




		// Size ---------------------------------------------------------------------------------------------------------------- //




		//TODO add size
		inline iter count(     ) const noexcept { checkInit(); return size_;         } //Returns the number of elements in the map, including the free ones
		inline iter usedCount( ) const noexcept { checkInit(); return size_ - free_; } //Returns the number of used elements
		inline iter freeCount( ) const noexcept { checkInit(); return free_;         } //Returns the number of free elements
	};
}
#undef chunks
#undef tracker
