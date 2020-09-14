
#pragma once
#include "LuxArray.h"
#include "LuxEngine/macros.h"
#include "LuxEngine/Math/Algebra/Algebra.h"
#include "LuxEngine/Types/Nothing.h"
#include "LuxEngine/Types/Nothing_sc_p.h"



//TODO dont inherit from array
namespace lux {
	template<class type, class iter = uint32> struct DynArray : public Array<type, iter> {
		lux_sc_generate_debug_structure_body;
		iter chunkSize;
		lux_sc_generate_nothing_constructor(DynArray) chunkSize{ chunkSize } { this->Array<type, iter>::Array( ); }

		//Creates an array with no elements
		//*   vChunkSize | the number of new elements allocated when the array grows
		//*       Larger chunks improve performance but use more memory
		//*       Default at ~500KB (depends on the type)
		inline DynArray(const iter vChunkSize = fit(sizeof(type), 500000)) : chunkSize(vChunkSize) { this->Array<type, iter>::Array( ); }



		//Resizes the array without initializing the new elements
		//*   vNewSize | new size of the array
		//*   Returns  | the new size
		inline iter __vectorcall resize(const iter vNewSize) {
			lux_sc_F;
			Array<type, iter>::resize((vNewSize == 0) ? 0 : multipleOf(vNewSize, chunkSize));
			return this->__lp_size = vNewSize;
		}

		//Resizes the array and initializes the new elements with a value
		//Use resize(<newSize>) for better performance when initialization is not needed
		//*   vNewSize   | the new size of the array
		//*   vInitValue | the value to use to initialize the new elements
		//*   Returns    | the new size
		inline iter __vectorcall resize(const iter vNewSize, const type& vInitValue) {
			lux_sc_F;
			//TODO not secure
			Array<type, iter>::resize((vNewSize == 0) ? 0 : multipleOf(vNewSize, chunkSize), vInitValue);
			return this->__lp_size = vNewSize;
		}


		//Adds an element to the end of the array
		//*   vElement | the element to add
		//*   Returns  | the index of the element in the array
		inline iter __vectorcall add(const type& vElement) {
			lux_sc_F;
			this->resize(this->__lp_size + 1);
			this->__lp_data[this->__lp_size - 1] = vElement;
			return this->__lp_size - 1;
		}

		//Returns a reference to the last element of the array (easy way to array[array.size() - 1])
		inline type& __vectorcall last( ) { lux_sc_F; return (*this)[this->__lp_size - 1]; }
		//Returns the size of the array
		inline iter __vectorcall size( ) const override { lux_sc_F; return this->__lp_size; }
		//Returns true if the array is empty, false if not
		inline bool __vectorcall empty( ) const override { lux_sc_F; return this->__lp_size == 0; }

		inline type* __vectorcall begin( ) const override { lux_sc_F; return &this->__lp_data[0]; }
		inline type* __vectorcall end( ) const override { lux_sc_F; return &this->__lp_data[this->__lp_size - 1]; }
	};
}