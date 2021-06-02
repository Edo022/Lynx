#pragma once
#define LNX_H_CONTAINER_BASE
#include "Lynx/Types/Integers/Integers.hpp"
#include "Lynx/Debug/Debug.hpp"
#include "Lynx/Tests/StructureInit.hpp"

#include "Lynx/Types/Pointer.hpp"
#include "Lynx/Core/Memory/Ram/Ram.hpp"

#include <new>
#include <initializer_list>
#include <type_traits>
#include <cmath>

//FIXME USE DIFFERENT MEMORY FOR CONST VALUES

/*                                                        Lynx Containers
.
.
.
.
.                        | Name           | data types | types num | data order   | count | get, set | memory alloc | data location
.    --------------------.----------------.------------.-----------.--------------.-------.----------.--------------.-------------.
.                        | CtArray        | any        | 1         | contiguous   | ct    | rt       | default      | heap/stack  |  Compile time array
.    lnx::ContainerBase  | RtArray        | any        | 1         | contiguous   | rt    | rt       | gmp          | heap/stack  |  Runtime array
.    subclasses          | RaArray        | any        | 1         | random       | rt    | rt       | gmp          | heap/stack  |  Random access array
.                        | Stirng         | char8      | 1         | contiguous   | rt    | rt       | gmp          | heap/stack  |  just String
.    --------------------|----------------|------------|-----------|--------------|-------|----------|--------------|-------------|
.    special containers  | HcArray      | any        | no limit  | imp specific | ct    | ct/rt    | default      | stack       |  Heterogeneous data compile time array
.                        | LynxMap_NMP_S   | any        | 1         | random       | cy    | rt       | default      | heap        |  deprecated version of RaArray used by the memory pool
.    --------------------'----------------'------------'-----------'--------------'-------'----------'--------------'-------------'
.    ct  = Compile time
.    rt  = Runtime
.    gmp = Global memory pool
.
.
.
.
.    | Name           | get/set element     | get size           | get count               | add/remove elements   | count | get, set | memory alloc
.    .----------------.---------------------.--------------------.-------------------------.-----------------------.-------.----------.--------------.
.    | CtArray        | operator[]          | size()             | count()                 | -                     | ct    | rt       | default      |
.    | RtArray        | operator[], last()  | size()             | count(), empty()        | add(), resize()       | rt    | rt       | gmp          |
.    | RaArray        | operator[]          | size(), usedSize() | count(), empty()        | add()                 | rt    | rt       | gmp/default  |
.    |                |                     | freeSize()         | usedCount(), freeCount()|                       |       |          |              |
.    | Stirng         | operator[]          | size()             | count()                 | operator+, operator+= | rt    | rt       | gmp          |
.    |----------------|---------------------|--------------------|-------------------------|-----------------------|-------|----------|--------------|
.    | HcArray      | get<>(), operator[] | -                  | count()                 | -                     | ct    | ct/rt    | default      |
.    | LynxMap_NMP_S   | operator[]          | -                  | count()                 | add()                 | rt    | rt       | default      |
.    '----------------'---------------------'--------------------'-------------------------'-----------------------'-------'----------'--------------'
.
*/








namespace lnx {
	template <class type, class iter> struct ContainerBase;
	//Any type that inherits from this struct will not be default constructed by lnx containers
	struct ignoreCtor{};
	//Any type that inherits from this struct will not be copy constructed by lnx containers
	struct ignoreCopy{};
	// //Any type that inherits from this struct will not be move constructed by lnx containers
	// struct ignoreMove{};
	//Any type that inherits from this struct will not be destroyed by lnx containers
	struct ignoreDtor{};




	namespace __pvt{
		template<class type, class iter, bool construct> struct cbCtor_t{};
		template<class type, class iter> struct cbCtor_t<type, iter, false>{
			protected:
			alwaysInline void initRange(const iter& vFrom, const iter& vTo) const noexcept {}
		};
		template<class type, class iter> struct cbCtor_t<type, iter, true>{
			protected:
			inline void initRange(const iter vFrom, const iter vTo) const {
				type* elm = ((const lnx::ContainerBase<type, iter>*)this)->begin();
				for(iter i = vFrom; i <= vTo; ++i) {
					new(elm + i) type();
				}
			}
		};


		// template<class type, class iter, bool construct> struct cbCtor_t{};
		// template<class type, class iter> struct cbCtor_t<type, iter, false>{
		// 	alwaysInline void initRange(const iter& vFrom, const iter& vTo) const noexcept {}
		// };
		// template<class type, class iter> struct cbCtor_t<type, iter, true>{
		// 	inline void initRange(const iter vFrom, const iter vTo) const {
		// 		type* elm = ((lnx::ContainerBase<type, iter>*)this)->begin();
		// 		for(iter i = vFrom; i <= vTo; ++i) {
		// 			new(elm + i) type();
		// 		}
		// 	}
		// };


		template<class type, class iter, bool destroy> struct cbDtor_t{};
		template<class type, class iter> struct cbDtor_t<type, iter, false>{
			protected:
			alwaysInline void destroy() const noexcept {}
			inline void destroyRange(const iter vFrom, const iter vTo) const noexcept {}
		};
		template<class type, class iter> struct cbDtor_t<type, iter, true>{
			protected:
			inline void destroy() const {
				type* end = ((const lnx::ContainerBase<type, iter>*)this)->end();
				for(type* elm = ((const lnx::ContainerBase<type, iter>*)this)->begin(); elm != end; ++elm) {
					elm->~type();
				}
			}
			inline void destroyRange(const iter vFrom, const iter vTo) const {
				type* elm = ((const lnx::ContainerBase<type, iter>*)this)->begin();
				for(iter i = vFrom; i <= vTo; ++i) {
					elm[i].~type();
				}
			}
		};



		template<class type, class iter> struct cbFwd_t:
		public __pvt::cbCtor_t<type, iter, !std::is_base_of_v<ignoreCopy, type> && !std::is_trivial_v<type>>,
		public __pvt::cbDtor_t<type, iter, !std::is_base_of_v<ignoreDtor, type> && !std::is_trivial_v<type>> {};
	}








	template <class type, class iter> struct ContainerBase :
	public __pvt::cbFwd_t<type, iter>{
		static_assert(!std::is_void_v<type>, "lnx::ContainerBase declared as array of void");
		static_assert(
			has_int_conversion_operator_v<iter> || std::is_integral_v<iter>,
			"iter template parameter must have integral or unscoped enum type"
		);
	public:
		genInitCheck;
		ram::ptr<type> data;	//Elements of the array
		_dbg(type* viewer;) //FIXME make this actually usable
		#define updateView() _dbg(if(data && data.size()) viewer = data.begin(); else viewer = nullptr)


		// Inititalize and destroy elements ---------------------------------------------------------------------------------------------------------//




	protected:
		//Resizes the array and calls the default constructor on each of the new elements
		inline void resize(const iter vSize) {
			checkInit(); dbg::checkParam(vSize < 0, "vSize", "The size of a container cannot be negative");
			auto oldCount = count();
			data.reallocArr(vSize);
			updateView();
			     if(oldCount < count()) lnx::__pvt::cbFwd_t<type, iter>::initRange(oldCount, count() - 1);
			else if(oldCount > count()) lnx::__pvt::cbFwd_t<type, iter>::destroyRange(count(), oldCount - 1);
		}

		//Concatenates a container and initializes the new elements by calling their copy constructor
		template<class cType, class cIter> inline void cat(const ContainerBase<cType, cIter>& pCont) {
			auto oldCount = count();
			data.reallocArr(oldCount + pCont.count());
			updateView();
			for(iter i = 0; i < pCont.count(); ++i) new(&data[oldCount + i]) type((cType)pCont[(cIter)i]);
		}
		//Concatenates a single element and initializes it by calling its copy constructor
		inline void cat1(const type& vElm) {
			data.reallocArr(count() + 1);
			new(&data[count() - 1]) type(vElm);
		}




		// Constructors -----------------------------------------------------------------------------------------------------------------------------//




		//Unallocated
		alwaysInline ContainerBase() : data{ nullptr } { updateView(); }


		//Count constructor
		inline ContainerBase(const iter vCount) :
			checkInitList(dbg::checkParam(vCount < 0, "vCount", "Count cannot be negative"))
			data{ sizeof(type) * vCount } {
			updateView();
			lnx::__pvt::cbFwd_t<type, iter>::initRange(0, count() - 1);
		}


		inline ContainerBase(const ContainerBase<type, iter>&  pCont) = delete;	//Delete default copy constructor
		inline ContainerBase(      ContainerBase<type, iter>&& pCont) = delete;	//Delete default move constructor
		//Copy constructor
		template<class cType, class cIter> inline ContainerBase(const ContainerBase<cType, cIter>& pCont, const Dummy vDummy) :
			checkInitList(
				isInit(pCont); dbg::checkParam(sizeof(cIter) > sizeof(iter), "pCont",
				"The iterator of a container must be large enough to contain all the elements.\
				Max iterator index is %d, but pCont has %d elements", pow(2, sizeof(iter) * 8 - 1), pCont.count())
			)
			data{ pCont.size() } {						//Allocate new elements
			updateView();
			for(iter i = 0; i < pCont.count(); ++i) {
				new(&data[i]) type(pCont[(cIter)i]);	//Assign new elements
			}
		}


		alwaysInline ContainerBase(const std::initializer_list<type> vElms) :
			data{ sizeof(type) * vElms.size() } {
			updateView();
			iter i = 0;
			for(const type& elm : vElms) new(&data[i++]) type(elm);
		}


	protected: //TODO move to type specialization
		alwaysInline ~ContainerBase() {
			if(data) lnx::__pvt::cbFwd_t<type, iter>::destroy(); //Destroy elemens if the array was not moved
			// data.free();
			//! ^ Not an error. data will be freed in its destructor
		}




		// Move and assignment ----------------------------------------------------------------------------------------------------------------------//




		alwaysInline void move(ContainerBase<type, iter>& pCont) {
			data = pCont.data; pCont.data = nullptr;
			updateView();
		}


		//Destroys each element and re-initializes them with the pCont elements by calling their copy constructor
		template<class cType, class cIter> inline void copy(const ContainerBase<cType, cIter>& pCont) {
			if(this == &pCont) return;
			lnx::__pvt::cbFwd_t<type, iter>::destroy();									//Destroy old elements
			data.reallocArr(pCont.count(), false);
			updateView();
			for(iter i = 0; i < pCont.count(); ++i) {
				new(&data[i]) type(pCont[(cIter)i]);	//Assign new elements
			}
		}




		// Get size and elements --------------------------------------------------------------------------------------------------------------------//




	public:
		alwaysInline auto  begin() const { return data.begin();       };	//Returns a pointer to the first element of the container
		alwaysInline auto    end() const { return data.end();         };	//Returns a pointer to the element after the last element of the container
		alwaysInline iter  count() const { return (iter)data.count(); };	//Returns the number of elements in the container
		alwaysInline uint64 size() const { return data.size();        };	//Returns the size in bytes of the contianer
		alwaysInline bool  empty() const { return !count();           };	//Returns true if the container has size 0, false otherwise

		alwaysInline auto& operator[](const iter vIndex) const { return data[vIndex]; }
	};
}