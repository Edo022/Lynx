#pragma once
#define LUX_H_RAARRAY
#include "LuxEngine/Math/Algebra/Algebra.hpp"
#include "LuxEngine/Types/Containers/ContainerBase.hpp"
#include "LuxEngine/Types/Pointer.hpp"








namespace lux {
		namespace __pvt{
		template<class type, class iter, bool construct> struct raCtor_t{};
		template<class type, class iter> struct raCtor_t<type, iter, false>{
			alwaysInline void initRange(const iter& vFrom, const iter& vTo) const noexcept {}
		};
		template<class type, class iter> struct raCtor_t<type, iter, true>{
			inline void initRange(const iter vFrom, const iter vTo) const {
				type* elm = ((lux::RaArray<type, iter>*)this)->data;
				for(iter i = vFrom; i <= vTo; ++i) {
					new(elm + i) type();
				}
			}
		};


		template<class type, class iter, bool destroy> struct raDtor_t{};
		template<class type, class iter> struct raDtor_t<type, iter, false>{
			alwaysInline void destroy() const noexcept {}
			inline void destroyRange(const iter vFrom, const iter vTo) const noexcept {}
		};
		template<class type, class iter> struct raDtor_t<type, iter, true>{
			inline void destroy() const {
				type* end = ((lux::RaArray<type, iter>*)this)->end();
				for(type* elm = ((lux::RaArray<type, iter>*)this)->data; elm != end; ++elm) {
					elm->~type();
				}
			}
			inline void destroyRange(const iter vFrom, const iter vTo) const {
				type* elm = ((lux::RaArray<type, iter>*)this)->data;
				for(iter i = vFrom; i <= vTo; ++i) {
					elm[i].~type();
				}
			}
		};
	}








	/**
	 * @brief A dynamic array with non contiguous elements.
	 *		New elements are written over previously removed ones, or concatenated if there are none.
	 *		The .isValid() function can be used to check if an element is valid or has been removed
	 * @tparam type Type of the elements
	 * @tparam iter Type of the index. The type of any index or count relative to this object depend on this
	 */
	template<class type, class iter = uint32> class RaArray :
	public __pvt::raCtor_t<type, iter, !std::is_base_of_v<ignoreCtor, type> && !std::is_trivial_v<type>>,
	public __pvt::raDtor_t<type, iter, !std::is_base_of_v<ignoreDtor, type> && !std::is_trivial_v<type>> {
		genInitCheck;
	private:
		ram::Alloc<type> data;	//Elements
		ram::Alloc<iter> lnkd;	//State of each element

		iter head;		//First free element
		iter tail;		//Last free element
		iter count_;		//Number of allocated elements
		iter free_;		//Number of free elements in the array

		inline void destroy() {
			for(iter i = 0; i < count(); ++i) if(isValid(i)) data[i].~type();
		}


	public:




		// Constructors -------------------------------------------------------------------------------------------------------- //




		/**
		 * @brief Creates an array without allocating memory to it.
		 *		The memory will be allocated when calling the add function
		 */
		inline RaArray( ) : data(nullptr), lnkd(nullptr),
			head{ (iter)-1 }, tail{ (iter)-1 }, count_{ 0 }, free_{ 0 } {
		}


		/**
		 * @brief Creates an array of size 0 and preallocates the memory for vCount elements
		 */
		inline RaArray(const iter vCount) :
			data(sizeof(type) * pCont.count()),
			lnkd(sizeof(iter) * pCont.count()),
			head{ (iter)-1 }, tail{ (iter)-1 }, count_{ 0 }, free_{ 0 } {
		}



		/**
		 * @brief Initializes the array by copy constructing each element from a lux::ContainerBase subclass
		 * @param pCont The container object to copy elements from.
		 *		It must have a compatible type and less elements than the maximum number of elements of the array you are initializing
		 */
		template<class eType, class iType> inline RaArray(const ContainerBase<eType, iType>& pCont) : checkInitList(isInit(pCont)) RaArray() {
			for(auto i = pCont.begin(); i < pCont.end( ); ++i) add((type)(*i));
		}




		/**
		 * @brief Initializes the array by copy constructing each element from a RaArray. Removed elements are preserved but not constructed.
		 * @param pCont The RaArray to copy elements from.
		 *		It must have a compatible type and less elements than the maximum number of elements of the array you are initializing
		 */
		template<class eType, class iType> inline RaArray(const RaArray<eType, iType>& pCont) : checkInitList(isInit(pCont)),
			RaArray(pCont.count()){
			for(int i = 0; i < pCont.count(); ++i) add(pCont[i]);
		}




		/**
		 * @brief Copy constructor. Elements are copied in a new memory allocation. Removed elements are preserved.
		 */
		inline RaArray(const RaArray<type, iter>& pCont) : checkInitList(isInit(pCont))
			RaArray(pCont.count()) {
			for(iter i = 0; i < pCont.count(); ++i) add(pCont[i]);
		}




		/**
		 * @brief Move constructor
		 */
		inline RaArray(RaArray<type, iter>&& pCont) : checkInitList(isInit(pCont))
			head{ pCont.head }, tail{ pCont.tail }, count_{ pCont.count_ }, free_{ pCont.free_ },
			data{ pCont.data }, lnkd{ pCont.lnkd } {
			// pCont.data = pCont.lnkd = nullptr; //FIXME
			//!^ pCont data and lnkd are freed in its destructor
		}




		// Add, remove --------------------------------------------------------------------------------------------------------- //
//TODO add shrink function to reorder the elements and use less memory possible

//TODO add self check on unusable elements


		/**
		 * @brief Adds a new element to the end of the array and initializes it with the vElement value by calling its copy constructor
		 * @return Index of the new element
		 */
		iter append(const type& pData) {
			checkInit();
			// if(count_ + 1 > data.count()/* * (uint64)chunkClass*/) {					//If the chunk is full
				// chunks_ [chunks_.count()].reallocArr((uint64)chunkClass, type());		//Create a new one
				// tracker_[chunks_.count()].reallocArr((uint64)chunkClass, iter());
				data.reallocArr(count() + 1);		//Create a new one
				lnkd.reallocArr(count() + 1);

			// }
			new(&data[count_]) type(pData);//Initialize new element

			lnkd[count_] = -1;	//Set the tracker as valid
			return count_++;			//Update the number of elements and return the ID
		}
		//BUG RAARRAY DOES NOT INITIALIZE NEW ELEMENTS


		// /**
		//  * @brief Adds an element at the end of the array.
		//  * @param pData Value the new element will be initialized with
		//  * @return Index of the new element
		//  */
		// inline iter append(const type& pData) {
		// 	checkInit();
		// 	append();
		// 	data[count_ - 1] = pData;
		// 	return count_;
		// }



//TODO
		/**
		 * @brief Adds an element at the first free index of the array without initializing it
 		 * @return Index of the new element
		 */
		iter add(const type& pData) {
			checkInit();
			if(head == (iter)-1) {				//If it has no free elements, append it
				// int ret = append();					//Append the new element
				// // new(&data[ret]) type();				//Initialize it
				// return ret;							//Return its index (initialization in append function)
				return append(pData);
			}
			iter prevHead = head;
			if(head == tail) {					//If it has only one free element
				lnkd[prevHead] = -1;//Reset head and tail
				head = tail = -1;		//Reset tracker
			}
			else {								//If it has more than one
				head = lnkd[prevHead];					//Update head
				lnkd[prevHead] = -1;					//Update tracker of the old head element
			}
			free_--;							//Update number of free elements
			new(&data[prevHead]) type(pData);		//Initialize the new element
			return prevHead;						//Return the index of the new element
		}


// 		/**
// 		 * @brief Adds an element at the first free index of the array
// 		 * @param vData Value the new element will be initialized with
// 		 * @return Index of the new element
// 		 */
// 		inline iter add(const type& vData) {
// 			checkInit();
// 			iter i = add(); //BUG add specific function for allocations that needs to be initialized
// 			// data[i] = vData;
// 			new(&data[i]) type(vData);
// 			// data[i] = type(vData);
// 			return i;
// 		}//BUG not initializing the element leaves it in an uninitialized state that is illegal for some operator= functions

// //BUG CHECK IF AGGREGATE INITIALIZATION CALLS COPY CONSTRUCTOR OF NON UNINIT COPYABLE STRUCTS


		/**
		 * @brief Removed an element without freeing it
		 * @param vIndex Index of the element to remove
		 */
		void remove(const iter vIndex) {
			checkInit();
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			dbg::checkParam(!isValid(vIndex), "vIndex", "Cannot remove element at index %d. It was already deleted", vIndex);

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
		inline void clear() {
			checkInit();
			// for(iter i = 0; i < data.count(); ++i) {
			// 	chunks_ [i].free();
			// 	tracker_[i].free();
			// }
			// chunks_ .free(); //BUG dont free
			// tracker_.free();

			destroy();
			head = tail = (iter)-1;
			count_ = free_ = 0;
			data.reallocArr(0);
			lnkd.reallocArr(0);
		}




		// Elements state ------------------------------------------------------------------------------------------------------ //




		// //Returns 0 if the index is used, 1 if the index is free, -1 if the index is invalid, -2 if the index is out of range
		// inline signed char state(const iter vIndex) const {
		// 	checkInit();
		// 	if(vIndex < 0) return -1;								//Invalid index
		// 	else if(vIndex >= count_) return -2;						//Index out of range
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
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			return lnkd[vIndex] == (iter)-1;
		}




		// Get ----------------------------------------------------------------------------------------------------------------- //


//FIXME add all constructors and assignment operators

		inline type& operator[](const iter vIndex) const noexcept {
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			return data[vIndex];
		}

		// //Returns a pointer to the first element of a chunk. The elements are guaranteed to be in contiguous order
		// /**
		//  * @param vChunkIndex
		//  * @return type*
		//  */
		// inline type* begin(const iter vChunkIndex) const {
		// 	checkInit(); dbg::checkParam(vChunkIndex < 0 || vChunkIndex >= _chunkNum, vChunkIndex, "Index is invalid or negative"); return &chunks_[vChunkIndex][0];
		// }




		// Size ---------------------------------------------------------------------------------------------------------------- //




		//TODO add size
		inline iter count(     ) const noexcept { checkInit(); return count_;         } //Returns the number of elements in the map, including the free ones
		inline iter usedCount( ) const noexcept { checkInit(); return count_ - free_; } //Returns the number of used elements
		inline iter freeCount( ) const noexcept { checkInit(); return free_;         } //Returns the number of free elements
	};
}
// #undef chunks
// #undef tracker
