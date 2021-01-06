#pragma once
#define LUX_H_CONTAINER_BASE
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Core/ConsoleOutput.hpp"
#include "LuxEngine/Tests/StructureInit.hpp"

//#ifndef __lux_no_gmp //RaArray specific
#	include "LuxEngine/Types/Pointer.hpp"
#	include "LuxEngine/Core/Memory/Ram/Ram.hpp"
//#endif

#include <new>



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
		//FIXME move data array to base class to avoid the use of virtual functions. theyre slow af
		genInitCheck;
		ram::Alloc<type> data;	//Elements of the array
		template<class type_, class... types, iter index> inline void init(type_&& vElm, types&&... vElms){
			init<type_, index>(vElm);
			init<type_, types..., index + 1>(vElm, vElms...);
		}
		template<class type_, iter index> inline void init(type_&& vElm){ data[index] = (type)vElm; }



		inline ContainerBase() : data((int)0) {}
		inline ContainerBase(const iter vCount) :
			constructExec(luxCheckParam(vCount < 0, vCount, "Count cannot be negative"))
			data(sizeof(type) * vCount) {
			for(int i = 0; i < vCount; i++) new(&data[i]) type();
		}
		template<class... types> inline ContainerBase(types&&... vElms) : ContainerBase(sizeof...(vElms)) { init<types..., 0>(vElms...); }
		inline ~ContainerBase(){
			for(int i = 0; i < count(); i++) data[i].~type();
		}

		inline auto begin( ) const { return ram::ptr<type>{ data.begin() }; };	//Returns a pointer to the first element of the container
		inline auto end(   ) const { return ram::ptr<type>{ data.end()   }; };	//Returns a pointer to the element after the last element of the container
		inline iter	count( ) const { return (iter)data.count(); 			};	//Returns the number of elements in the container
		inline bool	empty( ) const { return !count(); 						};	//Returns true if the container has size 0, false otherwise
		// inline uint64 size(  ) const { return };		//Returns the size in bytes of the data contained in the container

		// inline auto& operator=(const ContainerBase<type, iter>& pCont){

		// 	return *this;
		// }
	};
}