#pragma once
#define LUX_H_CONTAINER_BASE
#include "LuxEngine/Types/Nothing.hpp"
#include "LuxEngine/Tests/StructureInit.hpp"




/*                                                        Lux Containers
.
.
.
.
.                        | Name           | data types | types num | data order  | count | get, set | memory alloc | data location
.    --------------------.----------------.------------.-----------.-------------.-------.----------.--------------.-------------.
.                        | CtArray        | any        | 1         | continuous  | ct    | rt       | default      | heap/stack  |  Compile time array
.    lux::ContainerBase  | RtArray        | any        | 1         | continuous  | rt    | rt       | gmp          | heap/stack  |  Runtime array
.    subclasses          | RaArray        | any        | 1         | random      | rt    | rt       | gmp          | heap/stack  |  Random access array
.                        | Stirng         | char8      | 1         | continuous  | rt    | rt       | gmp          | heap/stack  |  just String
.    --------------------|----------------|------------|-----------|-------------|-------|----------|--------------|-------------|
.    special containers  | HdCtArray      | any        | no limit  | just don't  | ct    | ct/rt    | default      | stack       |  Heterogeneous data compile time array
.                        | LuxMap_NMP_S   | any        | 1         | random      | cy    | rt       | default      | heap        |  old version of RaArray used by the memory pool
.    --------------------'----------------'------------'-----------'-------------'-------'----------'--------------'-------------'
.    ct  = Compile time
.    rt  = Runtime
.    gmp = Global memory pool
.
.
.
.
.    | Name           | get/set element     | get size/count                           | add/remove elements   | count | get, set | memory alloc
.    .----------------.---------------------.---------------------------------------- -.-----------------------.-------.----------.--------------.
.    | CtArray        | operator[]          | size(), count()                          | -                     | ct    | rt       | default      |
.    | RtArray        | operator[], last()  | size(), empty(), count()                 | add(), resize()       | rt    | rt       | gmp          |
.    | RaArray        | operator[]          | size(), usedSize(), freeSize(), empty(), | add()                 | rt    | rt       | gmp          |
.    |                |                     | count(), usedCount(), freeCount()        |                       | rt    | rt       | gmp          |
.    | Stirng         | operator[]          | size(), count()                          | operator+, operator+= | rt    | rt       | gmp          |
.    |----------------|---------------------|------------------------------------------|-----------------------|-------|----------|--------------|
.    | HdCtArray      | get<>(), operator[] | count(),                                 | -                     | ct    | ct/rt    | default      |
.    | LuxMap_NMP_S   | operator[]          | count()                                  | add()                 | rt    | rt       | default      |
.    '----------------'---------------------'------------------------------------------'-----------------------'-------'----------'--------------'
.
*/




namespace lux {
	template <class type, class iter> class ContainerBase {
	public:
		genInitCheck;

		virtual inline type*	begin( ) const = 0;		//Returns a pointer to the first element of the container
		virtual inline type*	end(   ) const = 0;		//Returns a pointer to the element after the last element of the container
		virtual inline iter		count( ) const = 0;		//Returns the number of elements in the container
		virtual inline uint64	size(  ) const = 0;		//Returns the size in bytes of the data contained in the container
		virtual inline bool		empty( ) const = 0;		//Returns true if the container has size 0, false otherwise
	};
}