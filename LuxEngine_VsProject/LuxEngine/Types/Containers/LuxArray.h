#pragma once
#include "LuxEngine/macros.h"
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/Types/Containers/LuxContainer.h"
#include "LuxEngine/Types/Nothing.h"
#include "LuxEngine/Types/Nothing_sc_p.h"



//TODO use faster memcpy
//TODO dont use initializer list
//A static array that knows its size. It can be resized, but it doesn't have add or remove functions
namespace lux {
	template <class type, class iter = uint32> class Array : public ContainerBase<type, iter> {
	public:
		lux_sc_generate_debug_structure_body;
		type* __lp_data{ nullptr };	//Elements of the array
		iter __lp_size{ 0 };		//Size of the array
		#define __lp_lux_static_array_init(_size) __lp_size = _size; __lp_data = (type*)malloc(sizeof(type) * __lp_size)

		//Debug only
		luxDebug(void checkSize(const iter vInitSize){
			if(vInitSize == scast<iter>(-1) && vInitSize > 0){
				Failure printf("Error:");
				Failure printf("Something went wrong .-. You were trying to allocate %u bytes of memory", scast<iter>(vInitSize));
				Failure printf("This error will not be reported in release mode. Make sure to fix it");
				system("pause"); exit(-1);
			}
		});



		// Constructors -----------------------------------------------------------------------------------------------------------------------------//



		//Creates an array with no elements
		//inline Array(const Nothing) { *this = *this; }
		lux_sc_generate_nothing_constructor(Array) __lp_data{ __lp_data }, __lp_size{ __lp_size } { }
		inline Array( ) {
			__lp_size = 0;
			//__lp_data = (type*)malloc(sizeof(type));
			__lp_data = nullptr;
		}
		//Sets the size of the array and allocates it, without inizializing the elements
		inline Array(const iter vInitSize) {
			luxDebug(checkSize(vInitSize));
			__lp_lux_static_array_init((vInitSize >= 0) ? vInitSize : 0);
		}

		//TODO use lux EverythingArray
		//Initializes the array using a list of elements, automatically converting it to the right type
		template<class inType> inline Array(const std::initializer_list<inType>& pElements) {
			__lp_lux_static_array_init((iter)pElements.size( ));
			for(uint64 i = 0; i < pElements.end( ) - pElements.begin( ); ++i) __lp_data[i] = (inType) * (pElements.begin( ) + i);
		}
		//Initializes the array using a list of elements of the same type
		inline Array(const std::initializer_list<type>& pElements) {
			__lp_lux_static_array_init(scast<iter>(pElements.size( )));
			memcpy(begin( ), pElements.begin( ), ((pElements.size( ) * sizeof(type))));
		}


		//Initializes the array using a container object and converts each element to the array type. The input container must have a begin() and an end() function
		//*   pArray: a pointer to the container object
		template<class elmType> inline Array(const ContainerBase<elmType, iter>& pArray) {
			__lp_lux_static_array_init(pArray.end( ) - pArray.begin( ));
			for(uint64 i = 0; i < pArray.end( ) - pArray.begin( ); ++i) __lp_data[i] = (elmType) * (pArray.begin( ) + i);
		}
		#undef __lp_lux_static_array_init




		// Get, set, begin, end ---------------------------------------------------------------------------------------------------------------------//




		inline iter __vectorcall size( ) const override { lux_sc_F; return __lp_size; }
		inline bool __vectorcall empty( ) const override { lux_sc_F; return __lp_size == 0; }

		inline type& __vectorcall operator[](const iter vIndex) const { return __lp_data[vIndex]; }
		inline type* __vectorcall begin( ) const override { lux_sc_F; return __lp_data; }
		inline type* __vectorcall end( ) const override { lux_sc_F; return &__lp_data[__lp_size - 1]; }




		// Resize -----------------------------------------------------------------------------------------------------------------------------------//




		//Resizes the array without initializing the new elements
		//*   vNewSize: the new size of the array
		//*   Returns the new size. (alloc)-1 if the size is invalid
		inline iter __vectorcall resize(const iter vNewSize) {
			lux_sc_F;
			luxDebug(checkSize(vNewSize));
			if(vNewSize < 0) return -1;
			else if(vNewSize == 0) {
				free(__lp_data);//TODO dont free with global memory pool
				__lp_data = nullptr;
				//__lp_data = (type*)realloc(__lp_data, sizeof(type));
				return __lp_size = 0;
			}

			type* __lp_data_r = (type*)realloc(__lp_data, sizeof(type) * vNewSize);
			if(__lp_data_r != nullptr)__lp_data = __lp_data_r;
			return __lp_size = vNewSize;
		}

		//Resizes the array and initializes the new elements with a value
		//Use resize(<newSize>) for better performance when initialization is not needed
		//*   vNewSize: the new size of the array
		//*   vInitValue: the value to use to initialize the new elements
		//*   Returns the new size. (alloc)-1 if the size is invalid
		inline iter __vectorcall resize(const iter vNewSize, const type& vInitValue) {
			lux_sc_F;
			luxDebug(checkSize(vNewSize));
			//TODO not secure
			if(vNewSize < 0) return -1;
			iter oldSize = __lp_size;
			resize(vNewSize);
			//TODO use intrinsic function copy with automatic loop unwrap
			if(vNewSize > oldSize) for(iter i = oldSize; i < vNewSize; ++i) __lp_data[i] = vInitValue;
			return vNewSize;
		}
	};
}
#undef __lp_lux_static_array_init

















//#if (!defined LUX_H_ARRAY) || (!defined LUX_H_ARRAY_NMP)
//#include "LuxEngine/macros.h"
//#include "LuxEngine/Types/Integers/Integers.h"
//#include "LuxEngine/Types/Containers/LuxContainer.h"
//#include "LuxEngine/Types/Nothing.h"
//
//
////#undef LUX_NO_MEMORY_POOL
////TODO move to memory
//#ifdef LUX_NO_MEMORY_POOL	//define this to use a lux::ArrayNMP instead of a lux::Array. You have to manually undefine it after including the headers
//#	ifndef LUX_H_ARRAY
//#		define LUX_H_ARRAY
//#		define mp_lux_ram_ptr(type)							(type)*
//#		define mp_lux_ram_alloc(size, type, _class)			(type*)malloc				(	(size)								)
//#		define mp_lux_ram_realloc(ptr, size, type, _class)	(ptr) = (type*)realloc		(	(ptr),		(size)					)
//#		define mp_lux_ram_cpy(src, dst, num)				memcpy						(	(dst),		(src),		(num)		)
//#		define mp_lux_ram_free(ptr)							free						(	(ptr)								)
//#		define Array ArrayNMP				//Change class name
//#	endif
//#else
//#	ifndef LUX_H_ARRAY_NMP
//#		define LUX_H_ARRAY_NMP
//#		include "LuxEngine/Memory/Ram/Memory.h"
////#		define mp_lux_ram_ptr(__type)								lux::ram::ptr<float>
//#		define mp_lux_ram_ptr(__type)								lux::ram::ptr<__type>
//#		define mp_lux_ram_alloc(__size, __type, __class)			lux::ram::alloc		(	(__size),	(__class)				)
//#		define mp_lux_ram_realloc(__ptr, __size, __type, __class)	lux::ram::realloc	(	(__ptr),	(__size),	(__class)	)
//#		define mp_lux_ram_cpy(__src, __dst, __num)					lux::ram::cpy		(	(__src),	(__dst),	(__num)		)
//#		define mp_lux_ram_free(__ptr)								lux::ram::free		(	(__ptr)								)
//#		undef Array
//#	endif
//#endif
//
//
//
//
////TODO JUST USE LUX NO MP IN MEMORY AND CHANGE THE FUNCTION DECLARATIONS
////TODO ALSO USE THE DOUBLE SIDE IFDEF FOR EACH STRUCTURE
//
//
////TODO use faster memcpy
////TODO dont use initializer list
////A static array that knows its size. It can be resized, but it doesn't have add or remove functions
//namespace lux {
//	template <class type, class iter = uint32> class Array : public ContainerBase<type, iter> {
//	public:
//		//type* __lp_data{ nullptr };	//Elements of the array
//		mp_lux_ram_ptr(type) __lp_data{ nullptr };	//Elements of the array
//		iter __lp_size{ 0 };		//Size of the array
//		//#define __lp_lux_static_array_init(_size) __lp_size = _size; __lp_data = (type*)malloc(sizeof(type) * __lp_size)
//		#define __lp_lux_static_array_init(_size) __lp_size = _size; __lp_data = mp_lux_ram_alloc(sizeof(type)*__lp_size, type, lux::CellClass::AUTO)
//
//		//Debug only
//		luxDebug(void checkSize(const iter vInitSize){
//			if(vInitSize == scast<iter>(-1) && vInitSize > 0){
//				Failure printf("Error:");
//				Failure printf("Something went wrong .-. You were trying to allocate %u bytes of memory", scast<iter>(vInitSize));
//				Failure printf("This error will not be reported in release mode. Make sure to fix it");
//				system("pause"); exit(-1);
//			}
//		});
//
//
//
//		// Constructors -----------------------------------------------------------------------------------------------------------------------------//
//
//
//
//		//Creates an array with no elements
//		//inline Array(const Nothing) { *this = *this; }
//		inline Array(const Nothing) : __lp_data{ __lp_data }, __lp_size{ __lp_size } { }
//		inline Array( ) {
//			__lp_size = 0;
//			//__lp_data = (type*)malloc(sizeof(type));
//			__lp_data = nullptr;
//		}
//
//		//Sets the size of the array and allocates it, without inizializing the elements
//		inline Array(const iter vInitSize) {
//			luxDebug(checkSize(vInitSize));
//			__lp_lux_static_array_init((vInitSize >= 0) ? vInitSize : 0);
//		}
//
//		//TODO use lux EverythingArray
//		//Initializes the array using a list of elements, automatically converting it to the right type
//		template<class inType> inline Array(const std::initializer_list<inType>& pElements) {
//			__lp_lux_static_array_init((iter)pElements.size( ));
//			for(uint64 i = 0; i < pElements.end( ) - pElements.begin( ); ++i) __lp_data[i] = (inType) * (pElements.begin( ) + i);
//		}
//
//		//Initializes the array using a list of elements of the same type
//		inline Array(const std::initializer_list<type>& pElements) {
//			__lp_lux_static_array_init(scast<iter>(pElements.size( )));
//			//memcpy(begin( ), pElements.begin( ), ((pElements.size( ) * sizeof(type))));
//			mp_lux_ram_cpy(pElements.begin( ), begin( ), ((pElements.size( ) * sizeof(type))));
//		}
//
//
//		//Initializes the array using a container object and converts each element to the array type. The input container must have a begin() and an end() function
//		//*   pArray: a pointer to the container object
//		template<class elmType> inline Array(const ContainerBase<elmType, iter>& pArray) {
//			__lp_lux_static_array_init(pArray.end( ) - pArray.begin( ));
//			for(uint64 i = 0; i < pArray.end( ) - pArray.begin( ); ++i) __lp_data[i] = (elmType) * (pArray.begin( ) + i);
//		}
//		#undef __lp_lux_static_array_init
//
//
//
//
//		// Get, set, begin, end ---------------------------------------------------------------------------------------------------------------------//
//
//
//
//
//		inline iter __vectorcall size( ) const override { return __lp_size; }
//		inline bool __vectorcall empty( ) const override { return __lp_size == 0; }
//
//		inline type& __vectorcall operator[](const iter vIndex) const { return __lp_data[vIndex]; }
//		inline type* __vectorcall begin( ) const override { return __lp_data; }
//		inline type* __vectorcall end( ) const override { return &__lp_data[__lp_size - 1]; }
//
//
//
//
//		// Resize -----------------------------------------------------------------------------------------------------------------------------------//
//
//
//
//
//		//Resizes the array without initializing the new elements
//		//*   vNewSize: the new size of the array
//		//*   Returns the new size. (alloc)-1 if the size is invalid
//		inline iter __vectorcall resize(const iter vNewSize) {
//			luxDebug(checkSize(vNewSize));
//			if(vNewSize < 0) return -1;
//			else if(vNewSize == 0) {
//				//free(__lp_data);//TODO dont free with global memory pool
//				mp_lux_ram_free(__lp_data);//TODO dont free with global memory pool
//				__lp_data = nullptr;
//				//__lp_data = (type*)realloc(__lp_data, sizeof(type));
//				return __lp_size = 0;
//			}
//			//TODO fix
//			//TODO add implicit bool conversion in lux ram ptr struct
//			//type* __lp_data_r = (type*)realloc(__lp_data, sizeof(type) * vNewSize);
//			//if(__lp_data_r != nullptr)__lp_data = __lp_data_r;
//			mp_lux_ram_realloc(__lp_data, sizeof(type)*vNewSize, type, lux::CellClass::AUTO);
//
//			return __lp_size = vNewSize;
//		}
//
//
//		//Resizes the array and initializes the new elements with a value
//		//Use resize(<newSize>) for better performance when initialization is not needed
//		//*   vNewSize: the new size of the array
//		//*   vInitValue: the value to use to initialize the new elements
//		//*   Returns the new size. (alloc)-1 if the size is invalid
//		inline iter __vectorcall resize(const iter vNewSize, const type& vInitValue) {
//			luxDebug(checkSize(vNewSize));
//			//TODO not secure
//			if(vNewSize < 0) return -1;
//			iter oldSize = __lp_size;
//			resize(vNewSize);
//			//TODO use intrinsic function copy with automatic loop unwrap
//			if(vNewSize > oldSize) for(iter i = oldSize; i < vNewSize; ++i) __lp_data[i] = vInitValue;
//			return vNewSize;
//		}
//	};
//}
//#undef __lp_lux_static_array_init
//#endif