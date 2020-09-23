#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/macros.h"
#include "LuxEngine/Math/Algebra/Algebra.h"
#include "LuxEngine/Types/Containers/LuxContainer.h"
#include "LuxEngine/Types/Nothing.h"
#include "LuxEngine/Core/ConsoleOutput.h"
#include "LuxEngine/Memory/Ram/Memory.h"
#include "LuxEngine/Types/Nothing_sc_p.h"



//TODO always initialize data with class B



namespace lux {
	//An array with non-contiguous data capable of containing millions of elements without affecting performance
	//New elements are written over previously deleted elements, or concatenated if there are none
	//Useful if you need to constantly add or remove elements
	//Use the isValid() function to check if an element is valid or has been removed
	//template<class type, class iter = uint64, uint64 chunkSize = 50000 /*50 KB*/, uint64 elmPerChunk = chunkSize / sizeof(ram::ptr<int>)> class Map {
	template<class type, class iter = uint64, uint64 elmPerChunk = max(50100 / sizeof(type), 4), uint64 chunkSize = sizeof(type)* elmPerChunk> class Map {
	//TODO DO SOMETHING IF THE ELEMENT SIZE IS MORE THAN THE CHUNK SIZE
	public:
		//lux_sc_generate_debug_structure_body_func_only;
		lux_sc_generate_debug_structure_body;
		ram::ptr<ram::ptr<type>> chunks_;		//Elements
		ram::ptr<ram::ptr<iter>> tracker_;	//State of each element

		iter head_;			//First free element
		iter tail_;			//Last free element
		iter size_;			//Number of allocated elements. Use size() instead of this variable
		iter freeSize_;		//Number of free elements in the map. Use usedSize() or freeSize() instead of this variable

	private:
		iter _chunkNum;		//Number of allocated chunks
		//iter _chunkSize;	//Size of the chunks
		//iter _maxSize;		//Maximum number of elements in the map

		#define __lp_Data(index) chunks_[(index) / chunkSize][(index) % chunkSize]			//Get a data    element using only one index instead of index in the chunk and chunk index
		#define __lp_Tracker(index) tracker_[(index) / chunkSize][(index) % chunkSize]	//Get a tracker element using only one index instead of index in the chunk and chunk index
	public:




		// Constructor --------------------------------------------------------------------------------------------------------- //

		//TODO remove Cell_t* constructor from lux ptrs

		//inline Map(const Nothing) : luxDebug2(lux_sc_N,)
		lux_sc_generate_nothing_constructor(Map) _chunkNum{ _chunkNum },
			size_{ size_ }, freeSize_{ freeSize_ }, chunks_{ chunks_ }, tracker_{ tracker_ },
			head_{ head_ }, tail_{ tail_ } {
		}
		//! [#] Structure is uninitialized            | >>> NOT CHECKED <<<
		//OK

		//Creates the map with the specified chunk and maximum size
		//The number of chunks depends on their size and the maximum size of the map (chunks = _maxSize / _chunkSize)
		//*   vChunkSize: | number of elements allocated when the map grows
		//*       Larger chunks improve performance but use more memory
		//*       Default at ~500KB (depends on the type)
		//*   vMaxSize:   | the maximum size the map can reach
		//*       It must be larger than vChunkSize
		//*       Default at 0xFF * vChunkSize. ~127MB (depends on the type)
		//inline Map(const iter vChunkSize = fit(sizeof(type), 500000), const iter vMaxSize = fit(sizeof(type), 500000) * 0xFF) :
		inline Map( ) : head_{ (iter)-1 }, tail_{ (iter)-1 }, _chunkNum{ 0 }, size_{ 0 }, freeSize_{ 0 },
			//TODO substitute with AllocDA
			//luxDebug(if(vChunkSize > vMaxSize) param_error(vMaxSize, "The maximum size of a lux::Map must be larger or equal to the chunk size"));
			//TODO NOT SECURE
			chunks_{  ram::AllocDA<ram::ptr<type>>(sizeof(ram::ptr<type>), (uint64)CellClass::CLASS_B) },
			tracker_{ ram::AllocDA<ram::ptr<iter>>(sizeof(ram::ptr<iter>), (uint64)CellClass::CLASS_B) } {
		}
		// [#] No init required
		//OK

			//data_ = ram::alloc(sizeof(type*) * (_maxSize / _chunkSize));	//Allocate data
			//tracker_ = ram::alloc(sizeof(iter*) * (_maxSize / _chunkSize));	//Allocate tracker
			//memset(data_.address, 0, data_.size( ));
			//memset(tracker_.address, 0, tracker_.size( ));
			//data_[0] = ram::ptr<type>{ };
			//TODO ALWAYS MANUALLY CALL THE CONSTRUCTOR OF A PTR FROM AN ALLOCATED BLOCK MEMORY
			//TODO ^ nop
			//TODO ASSIGNMENT OPERATOR BREAKS EVEERYTHING IF THE STRUCT IS NOT INITIALIZED
			//TODO ^ nop
			//data_[0].ptr<type>::ptr( );


			//data_ = (type**)malloc(sizeof(type*) * (_maxSize / _chunkSize));	//Allocate data
			//tracker_ = (iter**)malloc(sizeof(iter*) * (_maxSize / _chunkSize));	//Allocate tracker
		//}


		//Initializes the array using a container object and converts each element to the array type. The input container must have a begin() and an end() function
		//*   in: a pointer to the container object
		template<class elmType>
		inline Map(const ContainerBase<elmType, iter>& pContainer) : Map( ) {
			isInit(pContainer);
			for(iter i = 0; i < pContainer.end( ) - pContainer.begin( ); ++i) add((elmType) * (pContainer.begin( ) + i));
		}
		// [#] No init required
		// [#] pContainer is uninitialized   | k | print error
		//OK



		// Add, remove --------------------------------------------------------------------------------------------------------- //



		//Adds an element at the end of the map, allocating a new chunk if needed
		//Returns the ID of the element
		iter __vectorcall append(const type& vData) {
			checkInit;
			if(size_ + 1 > _chunkNum * chunkSize) {								//If the chunk is full
				//data_[_chunkNum] = (type*)malloc(sizeof(type) * _chunkSize);			//Allocate a new data chunk
				//tracker_[_chunkNum] = (iter*)(malloc(sizeof(iter*) * _chunkSize));		//Allocate a new tracker chunk
				//TODO ???
				//chunks_[_chunkNum].ptr<type>::ptr( );
				//tracker_[_chunkNum].ptr<iter>::ptr( );
				chunks_[_chunkNum] = ram::AllocDA<type>(sizeof(type), elmPerChunk);
				tracker_[_chunkNum] = ram::AllocDA<iter>(sizeof(iter*), elmPerChunk);
				_chunkNum++;															//Update the number of chunks
			}
			__lp_Data(size_) = vData;												//Assign the data to the new element
			__lp_Tracker(size_) = -1;												//Set the tracker as valid

			size_++;																//Update the number of elements
			return size_ - 1;														//Return the ID
		}
		// [#] Structure is uninitialized  | k | print error
		//OK




		//Adds an element at the firs free index of the map
		//Returns the ID of the element
		iter __vectorcall add(const type& vData) {
			checkInit;
			if(head_ == (iter)-1) return append(vData);			//If it has no free elements, append it
			else {
				iter head2 = head_;
				if(head_ == tail_) {							//If it has only one free element
					__lp_Data(head2) = vData;						//Replace it
					head_ = tail_ = __lp_Tracker(head2) = -1;		//And reset head_ and tail_
				}
				else {											//If it has more than one
					head_ = __lp_Tracker(head2);					//Update head_
					__lp_Tracker(head2) = -1;						//Update the state of the first
					__lp_Data(head2) = vData;						//Replace it
				}
				freeSize_--;										//Update the number of free elements
				return head2;
			}
		}
		// [#] Structure is uninitialized  | k | print error
		//OK




		//Signs an index of the map as free, without deleting it. Returns a char for performance reasons
		//*   vIndex:  | the index to signNz
		//*   freeElm: | whether to free the element or not. Keeping it in memory saves performances but increases memory usage
		//*   Returns  | true if the operation succeeded, false if the index is invalid or an error occurred
		bool __vectorcall remove(const iter vIndex, const bool vFreeElm = false) {
			checkInit;
			//if(!isValid(vIndex)) return false;						//Check for index validity
			param_error_2(vIndex < 0, vIndex, "Index cannot be negative");
			param_error_2(vIndex > size( ), vIndex, "Index is out of range");

			//else {													//If it's valid,
				__lp_Tracker(vIndex) = -1;								//Set the index as free
				if(vFreeElm) free(&__lp_Data(vIndex));					//Free the element if necessary
				if(head_ == (iter)-1) head_ = tail_ = vIndex;			//If it has no free elements, initialize head_ and tail_.
				else tail_ = __lp_Tracker(tail_) = vIndex;				//If it has free elements, set the new tail_ and update the last free index
				freeSize_++;											//Update the number of free elements
			//}
			return 0;
		}
		// [#] Structure is uninitialized  | k | print error
		// [#] vIndex is negative          | k | print error
		// [#] vIndex is out of range      | k | print error
		// [#] vIndex is invalid           | k | print error
		//OK




		//Sets the size of the map to 0, deleting all the elements and resetting it to the initial state
		inline void __vectorcall clear( ) {
			checkInit;
			for(iter i = 0; i < _chunkNum; ++i) {
			//for(iter i = 0; i < _chunkNum; ++i) {
				ram::free(chunks_[i]);
				ram::free(tracker_[i]);
			}
			ram::free(tracker_);
			ram::free(chunks_);
			//for(iter i = 0; i < _chunkNum; ++i) free(tracker_[i]);	free(tracker_);		//Free tracker
			//chunks_    = ram::alloc(sizeof(type*) * (_maxSize / _chunkSize));			//Reallocate data
			//tracker_ = ram::alloc(sizeof(iter*) * (_maxSize / _chunkSize));			//Reallocate tracker
			////memset(data_.address, 0, data_.size( ));
			////memset(tracker_.address, 0, tracker_.size( ));

			////data_    = (type**)malloc(sizeof(type*) * (_maxSize / _chunkSize));			//Reallocate data
			////tracker_ = (iter**)malloc(sizeof(iter*) * (_maxSize / _chunkSize));			//Reallocate tracker
			//_chunkNum = size_ = freeSize_ = 0;											//Reset number of chunk, elements and free elements
			//head_ = tail_ = -1;															//Reset head_ and tail_
			this->Map::Map( );
		}
		// [#] Structure is uninitialized  | k | print error
		//OK



		// Status -------------------------------------------------------------------------------------------------------------- //




		//Returns 0 if the index is used, 1 if the index is free, -1 if the index is invalid, -2 if the index is out of range
		inline signed char __vectorcall state(const iter vIndex) const {
			checkInit;
			if(vIndex < 0) return -1;										//Invalid index
			else if(vIndex >= size_) return -2;								//Index out of range
			else if(__lp_Tracker(vIndex) == (iter)-1) return 0;				//Used element //OK
			//else if(__lp_Tracker(vIndex) >= 0) return 1;
			else return 1;					//Free element
		}
		// [#] Structure is uninitialized  | k | print error
		//OK

		//Returns true if the index is used, false if it's free or invalid (use the state function for more details)
		inline bool __vectorcall isValid(const iter vIndex) const {
			checkInit;
			if(vIndex >= size_ || vIndex < 0) return false;				//Return false if the index is out of range
			else return (__lp_Tracker(vIndex) == (iter)-1);			//Return true if the index is used, false if it's free
		}
		// [#] Structure is uninitialized  | k | print error
		//OK



		// Get ----------------------------------------------------------------------------------------------------------------- //




		//Use the isValid() function to check if the element can be used
		inline type& __vectorcall operator[](const iter vIndex) const {
			//checkInit; param_error_2(!isValid(vIndex), vIndex, "Index is invalid or negative");
			param_error_2(vIndex < 0, vIndex, "Index cannot be negative");
			param_error_2(vIndex > size( ), vIndex, "Index is out of range");
			return __lp_Data(vIndex);
		}
		// [#] Structure is uninitialized  | k | print error
		// [#] vIndex is negative          | k | print error
		// [#] vIndex is out of range      | k | print error
		// [#] vIndex is invalid           | k | print error
		//OK

		//Returns a pointer to the first element of a chunk. The elements are guaranteed to be in contiguous order
		inline type* __vectorcall begin(const iter vChunkIndex) const { checkInit; param_error_2(vChunkIndex < 0 || vChunkIndex >= _chunkNum, vChunkIndex, "Index is invalid or negative"); return &chunks_[vChunkIndex][0]; }
		// [#] Structure is uninitialized  | k | print error
		// [#] vChunkIndex is negative     | k | print error
		// [#] vChunkIndex is out of range | k | print error
		// [#] vChunkIndex is invalid      | k | print error
		//OK


		////DEPRECATED FUNCTION
		////TODO remove
		////Returns a pointer to a new map that contains all the elements in the chunks, without the invalid ones
		////This operation can be really slow, try to avoid using it
		//type* data( ) const {
		//	type* arr = (type*)malloc(sizeof(type) * usedSize( ));
		//	iter new_i = 0;
		//	for(iter i = 0; i < usedSize( ); ++i) {
		//		if(isValid(i)) {
		//			arr[new_i] = __lp_Data(i);
		//			++new_i;
		//		}
		//		else ++i;
		//	}
		//	return arr;
		//}




		// Size ---------------------------------------------------------------------------------------------------------------- //




		//Returns the number of elements in the map, including the free ones
		inline iter __vectorcall size( ) const { checkInit; return size_; }
		// [#] Structure is uninitialized  | k | print error
		//OK

		//Returns the number of used elements
		inline iter __vectorcall usedSize( ) const { checkInit; return size_ - freeSize_; }
		// [#] Structure is uninitialized  | k | print error
		//OK

		//Returns the number of free elements
		inline iter __vectorcall freeSize( ) const { checkInit; return freeSize_; }
		// [#] Structure is uninitialized  | k | print error
		//OK

	};
}
#undef __lp_Data
#undef __lp_Tracker











































//
//
//#pragma once
//#include "LuxEngine/Types/Integers/Integers.h"
//#include "LuxEngine/macros.h"
//#include "LuxEngine/Math/Algebra/Algebra.h"
//#include "LuxEngine/Types/Containers/LuxContainer.h"
//#include "LuxEngine/Types/Nothing.h"
//#include "LuxEngine/Core/ConsoleOutput.h"
//#include "LuxEngine/Memory/Ram/Memory.h"
//
//
//
//
//namespace lux {
//	//Like an array, but capable of containing billions of elements without losing performances
//	//The elements are not ordered, so each element has a unique ID. The ID is returned by the add functions
//	//Use the isValid() function to check if an element can be used or not
//	template<class type, class iter = uint64> class Map {
//	public:
//		//type** data_;		//Elements
//		ram::ptr<ram::ptr<type>> data_;		//Elements
//		//iter** tracker_;	//State of each element
//		ram::ptr<ram::ptr<iter>> tracker_;	//State of each element
//		iter head_;			//First free element
//		iter tail_;			//Last free element
//		iter size_;			//Number of allocated elements. Use size() instead of this variable
//		iter freeSize_;		//Number of free elements in the map. Use usedSize() or freeSize() instead of this variable
//
//	private:
//		iter _chunkNum;		//Number of allocated chunks
//		iter _chunkSize;	//Size of the chunks
//		iter _maxSize;		//Maximum number of elements in the map
//
//		#define __lp_Data(index) data_[(index) / _chunkSize][(index) % _chunkSize]			//Get a data    element using only one index instead of index in the chunk and chunk index
//		#define __lp_Tracker(index) tracker_[(index) / _chunkSize][(index) % _chunkSize]	//Get a tracker element using only one index instead of index in the chunk and chunk index
//	public:
//
//
//
//
//		// Constructor --------------------------------------------------------------------------------------------------------- //
//
//
//
//		inline Map(const Nothing) : _chunkNum{ _chunkNum },
//			size_{ size_ }, freeSize_{ freeSize_ }, data_{ data_ }, tracker_{ tracker_ },
//			head_{ head_ }, tail_{ tail_ }, _chunkSize{ _chunkSize }, _maxSize{ _maxSize } {
//		}
//
//		//Creates the map with the specified chunk and maximum size
//		//The number of chunks depends on their size and the maximum size of the map (chunks = _maxSize / _chunkSize)
//		//*   vChunkSize: | number of elements allocated when the map grows
//		//*       Larger chunks improve performance but use more memory
//		//*       Default at ~500KB (depends on the type)
//		//*   vMaxSize:   | the maximum size the map can reach
//		//*       It must be larger than vChunkSize
//		//*       Default at 0xFF * vChunkSize. ~127MB (depends on the type)
//		inline Map(const iter vChunkSize = fit(sizeof(type), 500000), const iter vMaxSize = fit(sizeof(type), 500000) * 0xFF) :
//			_chunkSize(vChunkSize), _maxSize(vMaxSize), head_((iter)-1), tail_((iter)-1), _chunkNum(0), size_(0), freeSize_(0) {
//			luxDebug(if(vChunkSize > vMaxSize) param_error(vMaxSize, "The maximum size of a lux::Map must be larger or equal to the chunk size"));
//			//data_ = (type**)malloc(sizeof(type*) * (_maxSize / _chunkSize));	//Allocate data
//			data_ = ram::alloc(sizeof(ram::ptr<type>) * (_maxSize / _chunkSize));	//Allocate data
//			//tracker_ = (iter**)malloc(sizeof(iter*) * (_maxSize / _chunkSize));	//Allocate tracker
//			tracker_ = ram::alloc(sizeof(ram::ptr<iter>) * (_maxSize / _chunkSize));	//Allocate tracker
//		}
//
//
//		//Initializes the array using a container object and converts each element to the array type. The input container must have a begin() and an end() function
//		//*   in: a pointer to the container object
//		template<class elmType>
//		inline Map(const ContainerBase<elmType, iter>* in) {
//			for(iter i = 0; i < in->end( ) - in->begin( ); ++i) add((elmType) * (in->begin( ) + i));
//		}
//
//
//
//
//
//		// Add, remove --------------------------------------------------------------------------------------------------------- //
//
//
//
//
//		//Adds an element at the end of the map, allocating a new chunk if needed
//		//Returns the ID of the element
//		iter __vectorcall append(const type& vData) {
//			if(size_ + 1 > _chunkNum * _chunkSize) {								//If the chunk is full
//				//data_[_chunkNum] = (type*)malloc(sizeof(type) * _chunkSize);			//Allocate a new data chunk
//				//TODO data_'s address is allocated, but not data_'s address's ptr
//				//TODO cell{ } -> address[ cell{U}, cell{U} ]
//				//TODO just use a huge buffer per chunk
//
//				//TODO ptrs with no cell are sparse pointers and do not generate any warning
//				//TODO -or- sparse pointers generate warning searching through all the allocated cells and finding where they are
//				data_[_chunkNum] = ram::alloc(sizeof(type) * _chunkSize);			//Allocate a new data chunk
//				//tracker_[_chunkNum] = (iter*)(malloc(sizeof(iter*) * _chunkSize));		//Allocate a new tracker chunk
//				tracker_[_chunkNum] = ram::alloc(sizeof(iter) * _chunkSize);		//Allocate a new tracker chunk
//				_chunkNum++;															//Update the number of chunks
//			}
//			__lp_Data(size_) = vData;												//Assign the data to the new element
//			__lp_Tracker(size_) = -1;												//Set the tracker as valid
//
//			size_++;																//Update the number of elements
//			return size_ - 1;														//Return the ID
//		}
//
//
//
//
//		//Adds an element at the firs free index of the map
//		//Returns the ID of the element
//		iter __vectorcall add(const type& vData) {
//			if(head_ == (iter)-1) return append(vData);			//If it has no free elements, append it
//			else {
//				iter head2 = head_;
//				if(head_ == tail_) {							//If it has only one free element
//					__lp_Data(head2) = vData;						//Replace it
//					head_ = tail_ = __lp_Tracker(head2) = -1;		//And reset head_ and tail_
//				}
//				else {											//If it has more than one
//					head_ = __lp_Tracker(head2);					//Update head_
//					__lp_Tracker(head2) = -1;						//Update the state of the first
//					__lp_Data(head2) = vData;						//Replace it
//				}
//				freeSize_--;										//Update the number of free elements
//				return head2;
//			}
//		}
//
//
//
//
//		//Signs an index of the map as free, without deleting it. Returns a char for performance reasons
//		//*   vIndex:  | the index to signNz
//		//*   freeElm: | whether to free the element or not. Keeping it in memory saves performances but increases memory usage
//		//*   Returns  | true if the operation succeeded, false if the index is invalid or an error occurred
//		bool __vectorcall remove(const iter vIndex, const bool vFreeElm = false) {
//			if(!isValid(vIndex)) return false;						//Check for index validity
//			else {													//If it's valid,
//				__lp_Tracker(vIndex) = -1;								//Set the index as free
//				if(vFreeElm) free(&__lp_Data(vIndex));					//Free the element if necessary
//				if(head_ == (iter)-1) head_ = tail_ = vIndex;			//If it has no free elements, initialize head_ and tail_.
//				else tail_ = __lp_Tracker(tail_) = vIndex;				//If it has free elements, set the new tail_ and update the last free index
//				freeSize_++;											//Update the number of free elements
//			}
//			return 0;
//		}
//
//
//
//
//		//Sets the size of the map to 0, deleting all the elements and resetting it to the initial state
//		inline void __vectorcall clear( ) {
//			for(iter i = 0; i < _chunkNum; ++i) free(data_[i]);		free(data_);		//Free data
//			for(iter i = 0; i < _chunkNum; ++i) free(tracker_[i]);	free(tracker_);		//Free tracker
//			//data_ = (type**)malloc(sizeof(type*) * (_maxSize / _chunkSize));			//Reallocate data
//			data_ = ram::alloc(sizeof(ram::ptr<type>) * (_maxSize / _chunkSize));			//Reallocate data
//			//tracker_ = (iter**)malloc(sizeof(iter*) * (_maxSize / _chunkSize));			//Reallocate tracker
//			tracker_ = ram::alloc(sizeof(ram::ptr<iter>) * (_maxSize / _chunkSize));			//Reallocate tracker
//			_chunkNum = size_ = freeSize_ = 0;											//Reset number of chunk, elements and free elements
//			head_ = tail_ = -1;															//Reset head_ and tail_
//		}
//
//
//
//
//		// Status -------------------------------------------------------------------------------------------------------------- //
//
//
//
//
//		//Returns 0 if the index is used, 1 if the index is free, -1 if the index is invalid or there is an error, -2 if the index is out of range
//		inline signed char __vectorcall state(const iter vIndex) const {
//			if(vIndex == (iter)-1)return -1;								//Invalid index
//			else if(vIndex >= size_) return -2;								//Index out of range
//			else if(__lp_Tracker(vIndex) == (iter)-1) return 0;				//Ok
//			else if(__lp_Tracker(vIndex) >= 0) return 1;					//Invalid element
//			else return -1;													//Unknown error
//		}
//
//
//		//Returns true if the index is used, false if it's free or invalid (use the state function for more details)
//		inline bool __vectorcall isValid(const iter vIndex) const {
//			if(vIndex > size_ - 1) return false;				//Return false if the index is out of range
//			return (__lp_Tracker(vIndex) == (iter)-1);			//Return true if the index is used, false if it's free
//		}
//
//
//
//
//		// Get ----------------------------------------------------------------------------------------------------------------- //
//
//
//
//
//		//Use the isValid() function to check if the element can be used
//		inline type& __vectorcall operator[](const iter vIndex) const { return __lp_Data(vIndex); }
//		//Returns a pointer to the first element of a chunk. The elements are guaranteed to be in contiguous order
//		inline type* __vectorcall begin(const iter vChunkIndex) const { return &data_[vChunkIndex][0]; }
//
//		//DEPRECATED FUNCTION
//		//TODO remove
//		//Returns a pointer to a new map that contains all the elements in the chunks, without the invalid ones
//		//This operation can be really slow, try to avoid using it
//		type* data( ) const {
//			type* arr = (type*)malloc(sizeof(type) * usedSize( ));
//			iter new_i = 0;
//			for(iter i = 0; i < usedSize( ); ++i) {
//				if(isValid(i)) {
//					arr[new_i] = __lp_Data(i);
//					++new_i;
//				}
//				else ++i;
//			}
//			return arr;
//		}
//
//
//
//
//		// Size ---------------------------------------------------------------------------------------------------------------- //
//
//
//
//
//		//Returns the number of elements in the map, including the free ones
//		inline iter __vectorcall size( ) const { return size_; }
//		//Returns the number of used elements
//		inline iter __vectorcall usedSize( ) const { return size_ - freeSize_; }
//		//Returns the number of free elements
//		inline iter __vectorcall freeSize( ) const { return freeSize_; }
//	};
//}
//#undef __lp_Data
//#undef __lp_Tracker
