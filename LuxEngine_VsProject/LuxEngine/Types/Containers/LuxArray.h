#pragma once
#include "LuxEngine/macros.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxContainer.h"
#include "LuxEngine/Types/Nothing.h"
#include "LuxEngine/Types/Nothing_sc_p.h"


//TODO dont use initializer list
//TODO MAKE IT COMPLETELY COMPILE TIME
namespace lux {
	//A static array that knows its size. It can be resized, but it doesn't have add or remove functions
	template <class type, class iter = uint32> class Array : public ContainerBase<type, iter> {
	private:
		lux_sc_generate_debug_structure_body_func_only;
		type* data_;	//Elements of the array
		iter size_;		//Size of the array
	public:




		// Constructors -----------------------------------------------------------------------------------------------------------------------------//




		lux_sc_generate_nothing_constructor(Array) data_{ data_ }, size_{ size_ } { }
		//! [#] Structure is uninitialized            | >>> NOT CHECKED <<<


		//Creates an array with no elements
		inline Array( ) : size_{ 0 }, data_{ (type*)malloc(sizeof(type)) } { }


		//Sets the size of the array and allocates it, without inizializing the elements
		inline Array(const iter vInitSize) : size_{ vInitSize }, data_{ (type*)malloc(sizeof(type) * vInitSize) } {
			type* _type = new type( );
			for(uint32 i = 0; i < vInitSize; i++) memcpy(&data_[i], _type, sizeof(type));
			delete(_type);
		}


		//TODO remove
		#define __lp_lux_static_array_init(_size) size_ = _size; data_ = (type*)malloc(sizeof(type) * size_)
		//Initializes the array using a list of elements, automatically converting it to the right type
		template<class inType> inline Array(const std::initializer_list<inType>& pElements) {
			__lp_lux_static_array_init((iter)pElements.size( ));
			for(uint64 i = 0; i < pElements.end( ) - pElements.begin( ); ++i) data_[i] = (inType) * (pElements.begin( ) + i);
		}

		//TODO remove
		//Initializes the array using a list of elements of the same type
		inline Array(const std::initializer_list<type>& pElements) {
			__lp_lux_static_array_init(scast<iter>(pElements.size( )));
			memcpy(begin( ), pElements.begin( ), ((pElements.size( ) * sizeof(type))));
		}


		luxDebug(template<class elmType> bool __isInit(const ContainerBase<elmType, iter>& pArray){ isInit(pArray); return false; })
		//Initializes the array using a container object and converts each element to the array type. The input container must have a begin() and an end() function
		//*   pArray: a pointer to the container object
		template<class elmType> inline Array(const ContainerBase<elmType, iter>& pArray) : constructExec(__isInit, pArray) size_{ pArray.size( ) }, data_{ (type*)malloc(sizeof(type) * pArray.size( )) } {
			for(uint64 i = 0; i < pArray.end( ) - pArray.begin( ); ++i) data_[i] = (elmType) * (pArray.begin( ) + i);
		}
		#undef __lp_lux_static_array_init




		// Add, remove --------------------------------------------------------------------------------------------------------- //




		//Resizes the array without initializing the new elements
		//*   vNewSize: the new size of the array
		//*   Returns the new size. (allocUB)-1 if the size is invalid
		inline iter __vectorcall resize(const iter vNewSize) {
			checkInit; param_error_2(vNewSize < 0, vNewSize, "The size of an array cannot be negative");
			type* __lp_data_r = (type*)realloc(data_, sizeof(type) * vNewSize);
			if(__lp_data_r != nullptr)data_ = __lp_data_r;
			else printError("Nullptr returned by malloc", true, -1);
			return size_ = vNewSize;
		}


		inline void __vectorcall clear( ){
			free(data_);
			this->Array::Array( );
		}


		template<class cIter> inline void operator=(const ContainerBase<type, cIter>& pContainer) {
			isInit(pContainer);
			data_ = (type*)malloc(pContainer.bytes( ));
			memcpy(data_, pContainer.begin( ), pContainer.bytes( ));
			size_ = pContainer.size( );
		}




		// Get ----------------------------------------------------------------------------------------------------------------- //




		inline iter		__vectorcall size( )	const override { checkInit; return size_;					}
		inline uint64	__vectorcall bytes( )	const override { checkInit; return size_ * sizeof(type);	}
		inline bool		__vectorcall empty( )	const override { checkInit; return !size_;					}
		inline type*	__vectorcall begin( )	const override { checkInit; return data_;					}
		inline type*	__vectorcall end( )		const override { checkInit; return &data_[size_ - 1];		}

		inline type&	__vectorcall operator[](const iter vIndex) const {
			checkInit; checkSize; param_error_2(vIndex < 0, vIndex, "The index of an array cannot be negative yet"); param_error_2(vIndex >= size( ), vIndex, "Index is out of range");
			return data_[vIndex];
		}
	};
}
#undef __lp_lux_static_array_init


