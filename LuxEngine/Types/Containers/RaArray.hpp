#pragma once
#define LUX_H_RAARRAY
#include "LuxEngine/Math/Algebra/Algebra.hpp"
#include "LuxEngine/Types/Containers/ContainerBase.hpp"
#include "LuxEngine/Types/Pointer.hpp"
//TODO add shrink function to reorder the elements and use less memory possible
//TODO add self check on unusable elements








namespace lux {
	template<class type, class iter> class RaArray;

	namespace __pvt{
		template<class type, class iter, bool construct> struct raCtor_t{};
		template<class type, class iter> struct raCtor_t<type, iter, false>{
			protected:
			// alwaysInline void initRange(const iter& vFrom, const iter& vTo) const noexcept {}
		};
		template<class type, class iter> struct raCtor_t<type, iter, true>{
			protected:
			// inline void initRange(const iter vFrom, const iter vTo) const {
			// 	type* elm = ((lux::RaArray<type, iter>*)this)->begin();
			// 	for(iter i = vFrom; i <= vTo; ++i) {
			// 		new(elm + i) type();
			// 	}
			// }
			//FIXME ADD ADD FUNCTION
		};


		template<class type, class iter, bool destroy> struct raDtor_t{};
		template<class type, class iter> struct raDtor_t<type, iter, false>{
			protected:
			alwaysInline void destroy() const noexcept {}
		};
		template<class type, class iter> struct raDtor_t<type, iter, true>{
			protected:
			inline void destroy() const {
				using arrt = lux::RaArray<type, iter>;
				int i = 0;
				for(auto elm : *(arrt*)this) {
					if(((arrt*)this)->isValid(i++)) elm.~type();
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
	template<class type, class iter = uint32> struct RaArray :
	public __pvt::raCtor_t<type, iter, !std::is_base_of_v<ignoreCopy, type> && !std::is_trivial_v<type>>,
	public __pvt::raDtor_t<type, iter, !std::is_base_of_v<ignoreDtor, type> && !std::is_trivial_v<type>> {
		genInitCheck;


		struct Elm{
			type value;
			iter next;
		};


		struct Iterator{
			Elm* addr;

			alwaysInline Iterator operator++(int) noexcept { return Iterator{ (Elm*)(  addr++) }; }
			alwaysInline Iterator operator++(   ) noexcept { return Iterator{ (Elm*)(++addr  ) }; }
			alwaysInline Iterator operator--(int) noexcept { return Iterator{ (Elm*)(  addr--) }; }
			alwaysInline Iterator operator--(   ) noexcept { return Iterator{ (Elm*)(--addr  ) }; }

			alwaysInline void operator+=(const uint64 vVal) noexcept { addr += vVal; }
			alwaysInline void operator-=(const uint64 vVal) noexcept { addr += vVal; }
			alwaysInline void operator+=(const int64  vVal) noexcept { addr += vVal; }
			alwaysInline void operator-=(const int64  vVal) noexcept { addr += vVal; }

			alwaysInline Iterator operator+(const uint64 vVal) const noexcept { return { addr + vVal }; }
			alwaysInline Iterator operator-(const uint64 vVal) const noexcept { return { addr - vVal }; }


			alwaysInline type& operator[](const uint64 vIndex) const { return addr[vIndex].value; }
			alwaysInline type& operator*(                    ) const { return addr->value; }
			alwaysInline type* operator->(                   ) const noexcept { return (type*)addr; }

			alwaysInline bool operator==(type* vPtr)    const noexcept { return vPtr == (type*)addr; }
			alwaysInline bool operator!=(type* vPtr)    const noexcept { return vPtr != (type*)addr; }
			alwaysInline bool operator==(Iterator vPtr) const { return vPtr.addr == addr; }
			alwaysInline bool operator!=(Iterator vPtr) const { return vPtr.addr != addr; }

			alwaysInline iter index() const noexcept { return (data - addr) / sizeof(Elm); }
		};


	private:
		ram::ptr<Elm> data;

		iter head;		//First free element
		iter tail;		//Last free element
		iter count_;	//Number of allocated elements
		iter free_;		//Number of free elements in the array


	public:








		// Constructors -------------------------------------------------------------------------------------------------------- //








		/**
		 * @brief Creates an array without allocating memory to it.
		 *		The memory will be allocated when calling the add function
		 */
		inline RaArray() : data(nullptr),
			head{ (iter)-1 }, tail{ (iter)-1 }, count_{ 0 }, free_{ 0 } {
		}


		/**
		 * @brief Creates an array of size 0 and preallocates the memory for vCount elements
		 */
		inline RaArray(const iter vCount) :
			data(sizeof(Elm) * vCount),
			head{ (iter)-1 }, tail{ (iter)-1 }, count_{ 0 }, free_{ 0 } {
		}




		/**
		 * @brief Initializes the array by copy constructing each element from an std::initializer_list
		 */
		inline RaArray(const std::initializer_list<type> vElms) :
			RaArray(vElms.size()) {
			for(const type& elm : vElms) add(elm);
		}




		/**
		 * @brief Initializes the array by copy constructing each element from a lux::ContainerBase subclass
		 * @param pCont The container object to copy elements from.
		 *		It must have a compatible type and less elements than the maximum number of elements of the array you are initializing
		 */
		template<class eType, class iType> inline RaArray(const ContainerBase<eType, iType>& pCont) :
			RaArray(pCont.count()) {
			isInit(pCont);
			//!^ Just in case the engine didn't get segfault'd by the count() call
			for(iter i = 0; i < pCont.count(); ++i) add(pCont[i]);
		}


		/**
		 * @brief Initializes the array by copy constructing each element from a RaArray. Removed elements are preserved but not constructed.
		 * @param pCont The RaArray to copy elements from.
		 *		It must have a compatible type and less elements than the maximum number of elements of the array you are initializing
		 */
		template<class eType, class iType> inline RaArray(const RaArray<eType, iType>& pCont) :
			RaArray(pCont.count()) {
			isInit(pCont); //! Same here
			iter i = 0;
			for(auto e : pCont) {
				add(e);
				//FIXME improve performance. dont copy removed elements
				if(!pCont.isValid(i)) remove(i);
				i++;
			}
		}


		/**
		 * @brief Copy constructor. Elements are copied in a new memory allocation. Removed elements are preserved.
		 */
		inline RaArray(const RaArray<type, iter>& pCont) :
			RaArray(pCont.count()) {
			isInit(pCont); //! Same here
			iter i = 0;
			for(auto e : pCont) {
				add(e);
				//FIXME improve performance. dont copy removed elements
				if(!pCont.isValid(i)) remove(i);
				i++;
			}
		}


		/**
		 * @brief Move constructor //FIXME probably useless
		 */
		inline RaArray(RaArray<type, iter>&& pCont) : checkInitList(isInit(pCont))
			data{ pCont.data },
			head{ pCont.head }, tail{ pCont.tail }, count_{ pCont.count_ }, free_{ pCont.free_ } {
			pCont.count_ = 0;	//Prevent the destructor from destroying the elements
			// pCont.data = nullptr;
			//!^ pCont data is freed in its destructor
		}


		~RaArray() { if(!empty()) this->destroy(); }








		// Add, remove --------------------------------------------------------------------------------------------------------- //








		/**
		 * @brief Adds a new element to the end of the array and initializes it with the vElement value by calling its copy constructor
		 * @return Index of the new element
		 */
		iter append(const type& pData) {
			checkInit();
			data.reallocArr(count() + 1);

			new(&(data[count_].value)) type(pData);	//Initialize new element
			data[count_].next = -1;				//Set the tracker as valid
			return count_++;				//Update the number of elements and return the element index
		}




		/**
		 * @brief Copy constructs pData in the first free element of the array
 		 * @return Index of the new element
		 */
		iter add(const type& pData) {
			checkInit();
			if(head == (iter)-1) {				//If it has no free elements
				return append(pData);				//Append the new element
			}
			iter prevHead = head;				//Save head
			if(head == tail) {					//If it has only one free element
				data[prevHead].next = -1;				//Reset head and tail
				head = tail = -1;					//Reset tracker
			}
			else {								//If it has more than one
				head = data[prevHead].next;				//Update head
				data[prevHead].next = -1;				//Update tracker of the old head element
			}
			free_--;							//Update number of free elements
			new(&(data[prevHead].value)) type(pData);		//Initialize the new element
			return prevHead;						//Return the index of the new element
		}




		/**
		 * @brief Removes an element and calls its destructor
		 *		The destructor is not called on trivial types or lux::ignoreDtor subclasses
		 * @param vIndex Index of the element to remove
		 */
		void remove(const iter vIndex) {
			checkInit();
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			dbg::checkParam(!isValid(vIndex), "vIndex", "Cannot remove element at index %d. It was already deleted", vIndex);

			data[vIndex].value.~type();						//Destroy the element
			data[vIndex].next = 0;						//Set the index as free
			//!                 ^ 0 is used as a "not -1" value. -1 are valid elements
			if(head == (iter)-1) head = tail = vIndex;	//If it has no free elements, initialize head and tail.
			else {										//If it has free elements
				data[tail].next = vIndex;						//Set the new tail
				tail = vIndex;								//update the last free index
			}
			free_++;									//Update the number of free elements
		}




		/**
		 * @brief Resets the array to its initial state, freeing all the chunks and resizing it to 0
		 */
		inline void clear() {
			checkInit();
			this->destroy();
			head = tail = (iter)-1;
			count_ = free_ = 0;
			data.reallocArr(0); //FIXME FREE
		}








		// Assignment ---------------------------------------------------------------------------------------------------------- //








		/**
		 * @brief Initializes the array by copy constructing each element from a lux::ContainerBase subclass
		 * @param pCont The container object to copy elements from.
		 *		It must have a compatible type and less elements than the maximum number of elements of the array you are initializing
		 */
		template<class eType, class iType> inline auto& operator=(const ContainerBase<eType, iType>& pCont) {
			isInit(pCont);
			clear(); //FIXME
			data.reallocArr(pCont.count(), false);
			iter i = 0;
			for(auto e : pCont) {
				add(e);
				//FIXME improve performance. dont copy removed elements
				if(!pCont.isValid(i)) remove(i);
				i++;
			}
			return *this;
		}



	private:
		template<class eType, class iType> inline auto& copy(const RaArray<eType, iType>& pCont) {
			isInit(pCont); if(this == &pCont) return *this;
			clear(); //FIXME
			data.reallocArr(pCont.count(), false);
			iter i = 0;
			for(auto e : pCont) {
				add(e);
				//FIXME improve performance. dont copy removed elements
				if(!pCont.isValid(i)) remove(i);
				i++;
			}
			return *this;
		}


	public:
		/**
		 * @brief Initializes the array by copy constructing each element from a RaArray. Removed elements are preserved but not constructed.
		 * @param pCont The RaArray to copy elements from.
		 *		It must have a compatible type and less elements than the maximum number of elements of the array you are initializing
		 */
		template<class eType, class iType> alwaysInline auto& operator=(const RaArray<eType, iType>& pCont) {
			return copy(pCont);
		}


		/**
		 * @brief Copy assignment. Elements are copied in a new memory allocation. Removed elements are preserved.
		 */
		alwaysInline auto& operator=(const RaArray<type, iter>& pCont) {
			return copy(pCont);
		}




		/**
		 * @brief Move constructor //FIXME probably useless
		 */
		inline auto& operator=(RaArray<type, iter>&& pCont) {
			isInit(pCont);
			this->destroy();
			data = pCont.data;
			head = pCont.head; tail = pCont.tail; count_ = pCont.count_; free_ = pCont.free_;
			pCont.count_ = 0;	//Prevent the destructor from destroying the elements
			// pCont.data = nullptr;
			//!^ pCont data is freed in its destructor
			return *this;
		}








		// Elements state, get element ----------------------------------------------------------------------------------------- //








		/**
		 * @brief Returns the state of an element
		 * @param vIndex Index of the element
		 * @return True if the element is valid (non deleted), false if not
		 */
		alwaysInline bool isValid(const iter vIndex) const {
			checkInit();
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			return data[vIndex].next == (iter)-1;
		}




		/*alwaysInline*/neverInline type& operator[](const iter vIndex) const {
			checkInit();
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			dbg::checkParam(!isValid(vIndex), "vIndex", "Accessing deleted array element");
			return data[vIndex].value;
		}








		// Size ---------------------------------------------------------------------------------------------------------------- //








		//TODO a dd size
		alwaysInline iter     count() const { checkInit(); return count_;         } //Returns the number of elements in the map, including the free ones
		alwaysInline bool     empty() const { checkInit(); return !count();       } //Returns true if the array has 0 elements
		alwaysInline iter usedCount() const { checkInit(); return count_ - free_; } //Returns the number of used elements
		alwaysInline iter freeCount() const { checkInit(); return free_;          } //Returns the number of free elements
		alwaysInline Iterator begin() const { checkInit(); if(empty()) return { nullptr }; else return { data }; }
		alwaysInline Iterator   end() const { checkInit(); if(empty()) return { nullptr }; else return { data + count_ }; }
	};
}
