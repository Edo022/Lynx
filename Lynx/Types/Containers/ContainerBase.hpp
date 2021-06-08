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
	template <class tType, class tIdxt> struct ContainerBase;
	//Any type that inherits from this struct will not be default constructed by lnx containers
	struct ignoreCtor{};
	//Any type that inherits from this struct will not be copy constructed by lnx containers
	struct ignoreCopy{};
	// //Any type that inherits from this struct will not be move constructed by lnx containers
	// struct ignoreMove{};
	//Any type that inherits from this struct will not be destroyed by lnx containers
	struct ignoreDtor{};




	namespace __pvt{
		template<class tType, class tIdxt, bool construct> struct cbCtor_t{};
		template<class tType, class tIdxt> struct cbCtor_t<tType, tIdxt, false>{
			protected:
			alwaysInline void initRange(const tIdxt& vFrom, const tIdxt& vTo) const noexcept {}
		};
		template<class tType, class tIdxt> struct cbCtor_t<tType, tIdxt, true>{
			protected:
			inline void initRange(const tIdxt vFrom, const tIdxt vTo) const {
				tType* elm = ((const lnx::ContainerBase<tType, tIdxt>*)this)->begin();
				for(tIdxt i = vFrom; i <= vTo; ++i) {
					new(elm + i) tType();
				}
			}
		};


		// template<class type, class tIdxt, bool construct> struct cbCtor_t{};
		// template<class type, class tIdxt> struct cbCtor_t<type, tIdxt, false>{
		// 	alwaysInline void initRange(const tIdxt& vFrom, const tIdxt& vTo) const noexcept {}
		// };
		// template<class type, class tIdxt> struct cbCtor_t<type, tIdxt, true>{
		// 	inline void initRange(const tIdxt vFrom, const tIdxt vTo) const {
		// 		type* elm = ((lnx::ContainerBase<type, tIdxt>*)this)->begin();
		// 		for(tIdxt i = vFrom; i <= vTo; ++i) {
		// 			new(elm + i) type();
		// 		}
		// 	}
		// };


		template<class tType, class tIdxt, bool destroy> struct cbDtor_t{};
		template<class tType, class tIdxt> struct cbDtor_t<tType, tIdxt, false>{
			protected:
			alwaysInline void destroy() const noexcept {}
			inline void destroyRange(const tIdxt vFrom, const tIdxt vTo) const noexcept {}
		};
		template<class tType, class tIdxt> struct cbDtor_t<tType, tIdxt, true>{
			protected:
			inline void destroy() const {
				tType* end = ((const lnx::ContainerBase<tType, tIdxt>*)this)->end();
				for(tType* elm = ((const lnx::ContainerBase<tType, tIdxt>*)this)->begin(); elm != end; ++elm) {
					elm->~tType();
				}
			}
			inline void destroyRange(const tIdxt vFrom, const tIdxt vTo) const {
				tType* elm = ((const lnx::ContainerBase<tType, tIdxt>*)this)->begin();
				for(tIdxt i = vFrom; i <= vTo; ++i) {
					elm[i].~tType();
				}
			}
		};



		template<class tType, class tIdxt> struct cbFwd_t:
		public __pvt::cbCtor_t<tType, tIdxt, !std::is_base_of_v<ignoreCopy, tType> && !std::is_trivial_v<tType>>,
		public __pvt::cbDtor_t<tType, tIdxt, !std::is_base_of_v<ignoreDtor, tType> && !std::is_trivial_v<tType>> {};
	}








	template <class tType, class tIdxt> struct ContainerBase :
	public __pvt::cbFwd_t<tType, tIdxt>{
		static_assert(!std::is_void_v<tType>, "Container declared as array of void");
		static_assert(
			has_int_conversion_operator_v<tIdxt> || std::is_integral_v<tIdxt> || std::is_enum_v<tIdxt>,
			"tIdxt template parameter must be convertible to an integer or have integral or enum type"
		);
		static_assert(std::is_trivial_v<tIdxt>, "tIdxt template parameter must be a trivial type");

		genInitCheck;
		ram::ptr<tType> data;	//Elements of the array
		_dbg(tType* viewer;) //FIXME make this actually usable
		#define updateView() _dbg(if(data && data.size()) viewer = data.begin(); else viewer = nullptr)


		// Inititalize and destroy elements ---------------------------------------------------------------------------------------------------------//




	protected:
		//Resizes the array and calls the default constructor on each of the new elements
		inline void resize(const tIdxt vSize) {
			checkInit(); dbg::checkParam(vSize < 0, "vSize", "The size of a container cannot be negative");
			auto oldCount = count();
			data.reallocArr(vSize);
			updateView();
			     if(oldCount < count()) lnx::__pvt::cbFwd_t<tType, tIdxt>::initRange(oldCount, count() - 1);
			else if(oldCount > count()) lnx::__pvt::cbFwd_t<tType, tIdxt>::destroyRange(count(), oldCount - 1);
		}

		//Concatenates a container and initializes the new elements by calling their copy constructor
		template<class cType, class cIter> inline void cat(const ContainerBase<cType, cIter>& pCont) {
			auto oldCount = count();
			data.reallocArr(oldCount + pCont.count());
			updateView();
			for(tIdxt i = 0; i < pCont.count(); ++i) new(&data[oldCount + i]) tType((cType)pCont[(cIter)i]);
		}
		//Concatenates a single element and initializes it by calling its copy constructor
		inline void cat1(const tType& vElm) {
			data.reallocArr(count() + 1);
			new(&data[count() - 1]) tType(vElm);
		}




		// Constructors -----------------------------------------------------------------------------------------------------------------------------//




		//Unallocated
		alwaysInline ContainerBase() : data{ nullptr } { updateView(); }


		//Count constructor
		inline ContainerBase(const tIdxt vCount) :
			checkInitList(dbg::checkParam(vCount < 0, "vCount", "Count cannot be negative"))
			data{ sizeof(tType) * vCount } {
			updateView();
			lnx::__pvt::cbFwd_t<tType, tIdxt>::initRange(0, count() - 1);
		}


		inline ContainerBase(const ContainerBase<tType, tIdxt>&  pCont) = delete;	//Delete default copy constructor
		inline ContainerBase(      ContainerBase<tType, tIdxt>&& pCont) = delete;	//Delete default move constructor
		//Copy constructor
		template<class cType, class cIter> inline ContainerBase(const ContainerBase<cType, cIter>& pCont, const Dummy vDummy) :
			checkInitList(
				isInit(pCont); dbg::checkParam(sizeof(cIter) > sizeof(tIdxt), "pCont",
				"The iterator of a container must be large enough to contain all the elements.\
				Max iterator index is %d, but pCont has %d elements", pow(2, sizeof(tIdxt) * 8 - 1), pCont.count())
			)
			data{ pCont.size() } {						//Allocate new elements
			updateView();
			for(tIdxt i = 0; i < pCont.count(); ++i) {
				new(&data[i]) tType(pCont[(cIter)i]);	//Assign new elements
			}
		}


		alwaysInline ContainerBase(const std::initializer_list<tType> vElms) :
			data{ sizeof(tType) * vElms.size() } {
			updateView();
			tIdxt i = 0;
			for(const tType& elm : vElms) new(&data[i++]) tType(elm);
		}


	protected: //TODO move to type specialization
		alwaysInline ~ContainerBase() {
			if(data) lnx::__pvt::cbFwd_t<tType, tIdxt>::destroy(); //Destroy elemens if the array was not moved
			// data.free();
			//! ^ Not an error. data will be freed in its destructor
		}




		// Move and assignment ----------------------------------------------------------------------------------------------------------------------//




		alwaysInline void move(ContainerBase<tType, tIdxt>& pCont) {
			data = pCont.data; pCont.data = nullptr;
			updateView();
		}


		//Destroys each element and re-initializes them with the pCont elements by calling their copy constructor
		template<class cType, class cIter> inline void copy(const ContainerBase<cType, cIter>& pCont) {
			if(this == &pCont) return;
			lnx::__pvt::cbFwd_t<tType, tIdxt>::destroy();									//Destroy old elements
			data.reallocArr(pCont.count(), false);
			updateView();
			for(tIdxt i = 0; i < pCont.count(); ++i) {
				new(&data[i]) tType(pCont[(cIter)i]);	//Assign new elements
			}
		}




		// Get size and elements --------------------------------------------------------------------------------------------------------------------//




	public:
		alwaysInline auto  begin() const { return data.begin();       };	//Returns a pointer to the first element of the container
		alwaysInline auto    end() const { return data.end();         };	//Returns a pointer to the element after the last element of the container
		alwaysInline tIdxt  count() const { return (tIdxt)data.count(); };	//Returns the number of elements in the container
		alwaysInline uint64 size() const { return data.size();        };	//Returns the size in bytes of the contianer
		alwaysInline bool  empty() const { return !count();           };	//Returns true if the container has size 0, false otherwise

		alwaysInline auto& operator[](const tIdxt vIndex) const { return data[vIndex]; }
	};
}