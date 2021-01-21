#pragma once
#define LUX_H_CONTAINER_BASE
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Debug/Debug.hpp"
#include "LuxEngine/Tests/StructureInit.hpp"

//#ifndef __lux_no_gmp //RaArray specific
#	include "LuxEngine/Types/Pointer.hpp"
#	include "LuxEngine/Core/Memory/Ram/Ram.hpp"
//#endif

#include <new>
#include <initializer_list>
#include <cmath>

//FIXME USE DIFFERENT MEMORY FOR CONST VALUES

/*                                                        Lux Containers
.
.
.
.
.                        | Name           | data types | types num | data order   | count | get, set | memory alloc | data location
.    --------------------.----------------.------------.-----------.--------------.-------.----------.--------------.-------------.
.                        | CtArray        | any        | 1         | contiguous   | ct    | rt       | default      | heap/stack  |  Compile time array
.    lux::ContainerBase  | RtArray        | any        | 1         | contiguous   | rt    | rt       | gmp          | heap/stack  |  Runtime array
.    subclasses          | RaArray        | any        | 1         | random       | rt    | rt       | gmp          | heap/stack  |  Random access array
.                        | Stirng         | char8      | 1         | contiguous   | rt    | rt       | gmp          | heap/stack  |  just String
.    --------------------|----------------|------------|-----------|--------------|-------|----------|--------------|-------------|
.    special containers  | HdCtArray      | any        | no limit  | imp specific | ct    | ct/rt    | default      | stack       |  Heterogeneous data compile time array
.                        | LuxMap_NMP_S   | any        | 1         | random       | cy    | rt       | default      | heap        |  deprecated version of RaArray used by the memory pool
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
.    | HdCtArray      | get<>(), operator[] | -                  | count()                 | -                     | ct    | ct/rt    | default      |
.    | LuxMap_NMP_S   | operator[]          | -                  | count()                 | add()                 | rt    | rt       | default      |
.    '----------------'---------------------'--------------------'-------------------------'-----------------------'-------'----------'--------------'
.
*/








namespace lux {
	template <class type, class iter> class ContainerBase {
	public:
		genInitCheck;
		ram::Alloc<type> data;	//Elements of the array




		// Inititalize and destroy elements ---------------------------------------------------------------------------------------------------------//








	protected:
		inline void initRange(iter vFrom, iter vTo) { for(iter i = vFrom; i < vTo + 1; ++i) new(&data[i]) type(); }
		inline void destroy(                      ) { for(iter i = 0;     i < count(); ++i) data[i].~type();      }

		//Resizes the array and calls the default constructor on each of the new elements
		inline void resize(const iter vSize) {
			checkInit(); dbg::checkParam(vSize < 0, "vSize", "The size of a container cannot be negative");
			auto oldCount = count();
			data.reallocArr(vSize);
			initRange(oldCount, count() - 1);
		}

		//Concatenates a container and initializes the new elements by calling their copy constructor
		template<class cType, class cIter> inline void cat(const ContainerBase<cType, cIter>& pCont) {
			auto oldCount = count();
			data.reallocArr(oldCount + pCont.count());
			for(iter i = 0; i < pCont.count(); ++i) new(&data[oldCount + i]) type((cType)pCont[(cIter)i]);
		}
		//Concatenates a single element and initializes it by calling its copy constructor
		inline void cat1(const type& vElm) {
			data.reallocArr(count() + 1);
			new(&data[count() - 1]) type(vElm);
		}








		// Constructors -----------------------------------------------------------------------------------------------------------------------------//








		//Unallocated
		alwaysInline ContainerBase() : data{ nullptr } {}


		//Count constructor
		inline ContainerBase(const iter vCount) :
			checkInitList(dbg::checkParam(vCount < 0, "vCount", "Count cannot be negative"))
			data{ sizeof(type) * vCount } {
			initRange(0, count() - 1);
		}


		inline ContainerBase(const ContainerBase<type, iter>&  pCont) = delete;	//Delete default copy constructor
		inline ContainerBase(      ContainerBase<type, iter>&& pCont) = delete;	//Delete default move constructor
		//Copy constructor
		template<class cType, class cIter> inline ContainerBase(const ContainerBase<cType, cIter>& pCont, Dummy vDummy) :
			checkInitList(
				isInit(pCont); dbg::checkParam(sizeof(cIter) > sizeof(iter), "pCont",
				"The iterator of a container must be large enough to contain all the elements.\
				Max iterator index is %d, but pCont has %d elements", pow(2, sizeof(iter) * 8 - 1), pCont.count())
			)
			data{ pCont.size() } {						//Allocate new elements
			for(iter i = 0; i < pCont.count(); ++i) {
				new(&data[i]) type((type)pCont[(cIter)i]);	//Assign new elements
			}
		}


		alwaysInline ContainerBase(const std::initializer_list<type>& vElms) :
			ContainerBase(vElms.size()) {
			iter i = 0;
			for(auto elm : vElms) new(&data[i++]) type(elm);
		}


	public:
		alwaysInline ~ContainerBase() {
			if(data) {		//Free data if the array was not moved
				destroy();
				data.free();
			}
		}








		// Move and assignment ----------------------------------------------------------------------------------------------------------------------//








	protected:
		alwaysInline void move(ContainerBase<type, iter>& pCont) {
			data = pCont.data; pCont.data = nullptr;
		}


		alwaysInline void moveAssignment(ContainerBase<type, iter>& pCont) {
			data = pCont.data; pCont.data = nullptr;
		}


		//Destroys each element and re-initializes them with the pCont elements by calling their copy constructor
		template<class cType, class cIter> inline void copy(const ContainerBase<cType, cIter>& pCont) {
			destroy();									//Destroy old elements
			data.reallocArr(pCont.count(), false);
			for(iter i = 0; i < pCont.count(); ++i) {
				new(&data[i]) type((type)pCont[(cIter)i]);	//Assign new elements
			}
		}








		// Get size and elements --------------------------------------------------------------------------------------------------------------------//








	public:
		alwaysInline auto  begin() const { return ram::ptr<type>{ data.begin() }; };	//Returns a pointer to the first element of the container
		alwaysInline auto    end() const { return ram::ptr<type>{ data.end(  ) }; };	//Returns a pointer to the element after the last element of the container
		alwaysInline iter  count() const { return (iter)data.count(); 			  };	//Returns the number of elements in the container
		alwaysInline uint64 size() const { return data.size();		              };	//Returns the size in bytes of the contianer
		alwaysInline bool  empty() const { return !count(); 					  };	//Returns true if the container has size 0, false otherwise

		alwaysInline auto& operator[](iter vIndex) const { return data[vIndex]; }
	};
}