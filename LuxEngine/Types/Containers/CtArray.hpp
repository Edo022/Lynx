#pragma once
#define LUX_H_CTARRAY
#include "LuxEngine/macros.hpp"
#include "LuxEngine/Types/Integers/Integers.hpp"
#include "LuxEngine/Types/Containers/ContainerBase.hpp"
#include "LuxEngine/Tests/StructureInit.hpp"




//TODO dont use initializer list
//TODO MAKE IT COMPLETELY COMPILE TIME
namespace lux {
	//A static array that knows its count
	//This array does NOT use the memory pool. All the data is allocated when the variable gets initialized
	// template <class type, uint64 count_> class CtArray : public ContainerBase<type, uint64> {
	template <class type, uint64 count_> class CtArray {
		genInitCheck;
	private:
		type* data_;	//Elements of the array
	public:




		// Constructors -----------------------------------------------------------------------------------------------------------------------------//




		//Creates an array with no elements
		inline CtArray( ) :/* size_{ 0 },*/ data_{ (type*)malloc(sizeof(type) * count_) } {
			type* _type = new type( );
			for(uint32 i = 0; i < count_; i++) memcpy(&data_[i], _type, sizeof(type));
			delete(_type);
		}




		//TODO remove
		#define __lp_lux_static_array_init data_ = (type*)malloc(sizeof(type) * count_)
		//Initializes the array using a list of elements, automatically converting it to the right type
		template<class inType> inline CtArray(const std::initializer_list<inType>& pElements) {
			__lp_lux_static_array_init;
			for(uint64 i = 0; i < pElements.end( ) - pElements.begin( ); ++i) data_[i] = (inType) * (pElements.begin( ) + i);
		}

		//TODO remove
		//Initializes the array using a list of elements of the same type
		inline CtArray(const std::initializer_list<type>& pElements) {
			luxCheckParam(pElements.size( ) > count_, pElements, "%d-elements CtArray initialized with %d-elements container.\nA compile time array cannot be initialized with larger containers", count_, pElements.size( ));
			__lp_lux_static_array_init;
			memcpy(begin( ), pElements.begin( ), ((pElements.size( ) * sizeof(type))));
		}


		luxDebug(template<class elmType, class elmIter> bool __isInit(const ContainerBase<elmType, elmIter>& pArray){ isInit(pArray); return false; })
		//Initializes the array using a container object and converts each element to the array type. The input container must have a begin() and an end() function
		//*   pArray: a pointer to the container object
		template<class elmType, class elmIter> inline CtArray(const ContainerBase<elmType, elmIter>& pArray) : checkInitList(__isInit, pArray) /*size_{ pArray.size( ) }, */data_{ (type*)malloc(sizeof(type) * pArray.size( )) } {
			for(uint64 i = 0; i < pArray.end( ) - pArray.begin( ); ++i) data_[i] = (elmType) * (pArray.begin( ) + i);
		}
		#undef __lp_lux_static_array_init




		// Add, remove --------------------------------------------------------------------------------------------------------- //




		template<class cIter> inline void operator=(const ContainerBase<type, cIter>& pContainer) {
			isInit(pContainer);
			luxCheckParam(pContainer.count( ) > count_, pContainer, "%d-elements CtArray initialized with %d-elements container.\nA compile time array cannot be initialized with larger containers", count_, pContainer.size( ));
			data_ = (type*)malloc(pContainer.size( ));
			memcpy(data_, pContainer.begin( ), pContainer.size( ));
		}




		// Get ----------------------------------------------------------------------------------------------------------------- //




		inline uint64 count( ) const override { checkInit(); return count_;				   }
		inline uint64 size(  ) const override { checkInit(); return count_ * sizeof(type); }
		inline bool	  empty( ) const override { checkInit(); return !count_;			   }
		inline type*  begin( ) const override { checkInit(); return data_;		 	       }
		inline type*  end(   ) const override { checkInit(); return &data_[count_];	       }

		inline type&	operator[](const uint64 vIndex) const {
			checkInit();
			checkCond(count() == 0,              "This function cannot be called on containers with size 0");
			luxCheckParam(vIndex < 0, vIndex,       "The index of an array cannot be negative yet");
			luxCheckParam(vIndex >= count_, vIndex, "Index is out of range");
			return data_[vIndex];
		}
	};
}
