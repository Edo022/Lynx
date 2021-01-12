#pragma once
#define LUX_H_RAARRAY
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
		genInitCheck;
	private:
		ram::Alloc<type> data;	//Elements
		ram::Alloc<iter> lnkd;	//State of each element

		iter head;		//First free element
		iter tail;		//Last free element
		iter size_;		//Number of allocated elements
		iter free_;		//Number of free elements in the map

		inline void destroy(){
			for(iter i = 0; i < count(); ++i) if(isValid(i)) data[i].~type();
		}


	public:




		// Constructors -------------------------------------------------------------------------------------------------------- //
		//FIXME ADD SIZE CONSTRUCTOR
		//FIXME USE SIZE CONSTRUCTOR IN CONTAINER CONSTRUCTOR TO PREVENT REALLOCATIONS




		/**
		 * @brief Creates an array with size 0 and no preallocated chunks
		 */
		inline RaArray( ) : head{ (iter)-1 }, tail{ (iter)-1 }, size_{ 0 }, free_{ 0 },
			data(nullptr), lnkd(nullptr) {
		}




		/**
		 * @brief Initializes the array by copying each element from a lux::ContainerBase subclass
		 * @param pCont The container object to copy elements from.
		 *		It must be a valid lux::ContainerBase subclass instance with a compatible type and
		 *		less elements than the maximum number of elements of the array you are initializing
		 */
		template<class eType, class iType> inline RaArray(const ContainerBase<eType, iType>& pCont) : /*checkInitList(isInit(pCont))*/ RaArray( ) {
			//TODO check sizes in checkInitList
			//FIXME preallocate all the elements before looping them
			for(auto i = pCont.begin(); i < pCont.end( ); ++i) add((type)(*i));
		}




		/**
		 * @brief Initializes the array by copying each element from a RaArray. Removed elements are preserved.
		 * @param pCont The RaArray to copy elements from.
		 *		It must be a valid RaArray instance with a compatible type and
		 *		less elements than the maximum number of elements of the array you are initializing
		 */
		template<class eType, class iType> inline RaArray(const RaArray<eType, iType>& pCont) : //checkInitList(isInit(pCont))
			//head{ pCont.head }, tail{ pCont.tail }, size_{ pCont.size_ }, free_{ pCont.free_ } {
			RaArray() {
			// data(pCont.data.deepCopy()),
			// lnkd(pCont.lnkd.deepCopy()){
			for(int i = 0; i < pCont.count(); ++i) add(pCont[i]);
			//TODO check sizes in checkInitList
			// for(int i = 0; i < pCont.chunks_.count(); ++i){
			// 	chunks_[i] = pCont.chunks_[i].deepCopy();   //Deeper copy
			// 	tracker_[i] = pCont.tracker_[i].deepCopy(); //UwU
			// }
		}




		/**
		 * @brief Copy constructor. Elements are copied in a new memory allocation. Removed elements are preserved.
		 */
		inline RaArray(const RaArray<type, iter>& pCont) : //checkInitList(isInit(pCont))
			//head{ pCont.head }, tail{ pCont.tail }, size_{ pCont.size_ }, free_{ pCont.free_ },
			//data(pCont.data.deepCopy()), lnkd(pCont.lnkd.deepCopy()){
			RaArray(){
			for(int i = 0; i < pCont.count(); ++i) add(pCont[i]);
		}




		/**
		 * @brief Move constructor
		 */
		inline RaArray(RaArray<type, iter>&& pCont) : checkInitList(isInit(pCont))
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
				data.reallocArr(count() + 1);		//Create a new one
				lnkd.reallocArr(count() + 1);

			// }
			new(&data[size_]) type();//Initialize new element

			lnkd[size_] = -1;	//Set the tracker as valid
			return size_++;			//Update the number of elements and return the ID
		}
		//BUG RAARRAY DOES NOT INITIALIZE NEW ELEMENTS


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
		iter add() {
			checkInit();
			if(head == (iter)-1) {				//If it has no free elements, append it
				// int ret = append();					//Append the new element
				// // new(&data[ret]) type();				//Initialize it
				// return ret;							//Return its index (initialization in append function)
				return append();
			}
			iter prevHead = head;
			if(head == tail) {					//If it has only one free element
				lnkd[head] = -1;
				head = tail = -1;		//Reset head, tail and tracker
			}
			else {								//If it has more than one
				head = lnkd[head];					//Update head
				lnkd[prevHead] = -1;					//Update tracker of the old head element
			}
			free_--;							//Update number of free elements
			new(&data[prevHead]) type();		//Initialize the new element
			return prevHead;						//Return the index of the new element
		}


		/**
		 * @brief Adds an element at the first free index of the array
		 * @param vData Value the new element will be initialized with
		 * @return Index of the new element
		 */
		inline iter add(const type& vData) {
			checkInit();
			iter i = add(); //BUG add specific function for allocations that needs to be initialized
			// data[i] = vData;
			new(&data[i]) type(vData);
			// data[i] = type(vData);
			return i;
		}//BUG not initializing the element leaves it in an uninitialized state that is illegal for some operator= functions

//BUG CHECK IF AGGREGATE INITIALIZATION CALLS COPY CONSTRUCTOR OF NON UNINIT COPYABLE STRUCTS


		/**
		 * @brief Removed an element without freeing it
		 * @param vIndex Index of the element to remove
		 */
		void remove(const iter vIndex) {
			checkInit();
			luxCheckParam(vIndex < 0, vIndex, "Index cannot be negative");
			luxCheckParam(vIndex > count( ), vIndex, "Index is out of range");
			luxCheckParam(!isValid(vIndex), vIndex, "Cannot remove element at index %d. It was already deleted", vIndex);

			data[vIndex].~type();							//Destroy the element
			lnkd[vIndex] = -1;								//Set the index as free
			if(head == (iter)-1) head = tail = vIndex;			//If it has no free elements, initialize head and tail.
			else {
				lnkd[tail] = vIndex;
				tail = vIndex;					//If it has free elements, set the new tail and update the last free index
			}
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

			destroy();
			head = tail = (iter)-1;
			size_ = free_ = 0;
			data.reallocArr(0);
			lnkd.reallocArr(0);
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


//FIXME add all constructors and assignment operators

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
