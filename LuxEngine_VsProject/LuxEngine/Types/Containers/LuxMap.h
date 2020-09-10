

#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/macros.h"
#include "LuxEngine/Types/Containers/LuxArray.h"
#include "LuxEngine/Math/Algebra/Algebra.h"
#include "LuxEngine/Types/Containers/LuxContainer.h"
#include "LuxEngine/Types/Nothing.h"
#include "LuxEngine/Core/ConsoleOutput.h"

#include <vector>



//Like an awway, but capable of containing billions of elements without losing performances
//The elements are not ordered, so each element has a unique ID. The ID is returned by the add functions
//Use the isValid() function to check if an element can be used or not
namespace lux {
	template<class type, class iter = uint64> class Map {
	private:
		iter chunkNum;		//Number of allocated chunks
	public:
		iter size_;			//Number of allocated elements. Use size() instead of this variable
		iter freeSize_;		//Number of free elements in the map. Use usedSize() or freeSize() instead of this variable
		type** data_;		//Map elements
		iter** tracker_;	//Map that stores the state of each element

	private:
		iter _head;			//First free element
		iter _tail;			//Last free element
		iter _chunkSize;	//Size of the chunks
		iter _maxSize;		//Maximum number of elements in the map

		#define __lp_Data(index) data_[(index) / _chunkSize][(index) % _chunkSize]			//Get a data    element using only one index instead of index in the chunk and chunk index
		#define __lp_Tracker(index) tracker_[(index) / _chunkSize][(index) % _chunkSize]	//Get a tracker element using only one index instead of index in the chunk and chunk index
	public:




		// Constructor --------------------------------------------------------------------------------------------------------- //



		inline Map(const Nothing) : chunkNum{ chunkNum },
			size_{ size_ }, freeSize_{ freeSize_ }, data_{ data_ }, tracker_{ tracker_ },
			_head{ _head }, _tail{ _tail }, _chunkSize{ _chunkSize }, _maxSize{ _maxSize } {
		}

		//Creates the map with the specified chunk and maximum size
		//The number of chunks depends on their size and the maximum size of the map (chunks = _maxSize / _chunkSize)
		//*   vChunkSize: | number of elements allocated when the map grows
		//*       Larger chunks improve performance but use more memory
		//*       Default at ~500KB (depends on the type)
		//*   vMaxSize:   | the maximum size the map can reach
		//*       It must be larger than vChunkSize
		//*       Default at 0xFF * vChunkSize. ~127MB (depends on the type)
		inline Map(const iter vChunkSize = fit(sizeof(type), 500000), const iter vMaxSize = fit(sizeof(type), 500000) * 0xFF) :
			_chunkSize(vChunkSize), _maxSize(vMaxSize), _head((iter)-1), _tail((iter)-1), chunkNum(0), size_(0), freeSize_(0) {
			luxDebug(if(vChunkSize > vMaxSize) param_error(vMaxSize, "The maximum size of a lux::Map must be larger or equal to the chunk size"));
			data_ = (type**)malloc(sizeof(type*) * (_maxSize / _chunkSize));	//Allocate data
			tracker_ = (iter**)malloc(sizeof(iter*) * (_maxSize / _chunkSize));	//Allocate tracker
		}


		//Initializes the array using a container object and converts each element to the array type. The input container must have a begin() and an end() function
		//*   in: a pointer to the container object
		template<class elmType>
		inline Map(const ContainerBase<elmType, iter>* in) {
			for(iter i = 0; i < in->end( ) - in->begin( ); ++i) add((elmType) * (in->begin( ) + i));
		}





		// Add, remove --------------------------------------------------------------------------------------------------------- //




		//Adds an element at the end of the map, allocating a new chunk if needed
		//Returns the ID of the element
		iter __vectorcall append(const type& vData) {
			if(size_ + 1 > chunkNum * _chunkSize) {									//If the chunk is full
				data_[chunkNum] = (type*)malloc(sizeof(type) * _chunkSize);				//Allocate a new data chunk
				tracker_[chunkNum] = scast<iter*>(malloc(sizeof(iter*) * _chunkSize));	//Allocate a new tracker chunk
				chunkNum++;																//Update the number of chunks
			}
			__lp_Data(size_) = vData;												//Assign the data to the new element
			__lp_Tracker(size_) = -1;												//Set the tracker as valid

			size_++;																//Update the number of elements
			return size_ - 1;														//Return the ID
		}




		//Adds an element at the firs free index of the map
		//Returns the ID of the element
		iter __vectorcall add(const type& vData) {
			if(_head == (iter)-1) return append(vData);			//If it has no free elements, append it
			else {
				iter head2 = _head;
				if(_head == _tail) {							//If it has only one free element
					__lp_Data(head2) = vData;						//Replace it
					_head = _tail = __lp_Tracker(head2) = -1;		//And reset _head and _tail
				}
				else {											//If it has more than one
					_head = __lp_Tracker(head2);					//Update _head
					__lp_Tracker(head2) = -1;						//Update the status of the first
					__lp_Data(head2) = vData;						//Replace it
				}
				freeSize_--;										//Update the number of free elements
				return head2;
			}
		}




		//Adds the elements of an std::vector to the map, placing each element in the first free index
		//Returns a static array containing the IDs of the elements, in the same order as they were in the input
		Array<iter> __vectorcall add(const std::vector<type>* pVector) {
			Array<iter> IDs;
			IDs.resize(pVector->size( ));												//Set the number of IDs
			for(int32 i = 0; i < pVector->size( ); i++) IDs[i] = add((*pVector)[i]);	//Add every element to the map and save its ID
			return IDs;
		}




		//Adds the elements of a LuxContainer to the map, skipping the invalid ones and placing the others in the first free index
		//Returns a static array containing the IDs of the elements, in the same order as they were in the input (invalid indices have -1 as ID)
		Array<iter> __vectorcall add(const Map<type, iter>* vContainer) {
			Array<iter> IDs;
			IDs.resize(vContainer->size( ));								//Set the number of IDs
			for(iter i = 0; i < vContainer->size( ); i++){					//For every element of the input map
				if(vContainer->isValid(i)) IDs[i] = add((*vContainer)[i]);		//If it's valid, add it to the map and save its ID
				else IDs[i] = -1;												//If not, save -1 as ID
			}
			return IDs;
		}




		//Signs an index of the map as free, without deleting it. Returns a char for performance reasons
		//*   vIndex:  | the index to signNz
		//*   freeElm: | whether to free the element or not. Keeping it in memory saves performances but increases memory usage
		//*   Returns  | true if the operation succeeded, false if the index is invalid or an error occurred
		bool __vectorcall remove(const iter vIndex, const bool vFreeElm = false) {
			if(!isValid(vIndex)) return false;						//Check for index validity
			else {													//If it's valid,
				__lp_Tracker(vIndex) = -1;								//Set the index as free
				if(vFreeElm) free(&__lp_Data(vIndex));					//Free the element if necessary
				if(_head == scast<iter>(-1)) _head = _tail = vIndex;	//If it has no free elements, initialize _head and _tail.
				else _tail = __lp_Tracker(_tail) = vIndex;				//If it has free elements, set the new _tail and update the last free index
				freeSize_++;												//Update the number of free elements
			}
			return 0;
		}




		//Sets the size of the map to 0, deleting all the elements and resetting it to the initial state
		inline void __vectorcall clear( ) {
			for(iter i = 0; i < chunkNum; ++i) free(data_[i]);
			free(data_);
			for(iter i = 0; i < chunkNum; ++i) free(tracker_[i]);
			free(tracker_);
			data_ = (type**)malloc(sizeof(type*) * (_maxSize / _chunkSize));			//Reallocate data
			tracker_ = scast<iter**>(malloc(sizeof(iter*) * (_maxSize / _chunkSize)));	//Reallocate tracker
			chunkNum = size_ = freeSize_ = 0;											//Reset number of chunk, number of elements, number of free elements
			_head = _tail = -1;															//Reset _head and _tail
		}




		// Status -------------------------------------------------------------------------------------------------------------- //




		//Returns 0 if the index is used, 1 if the index is free, -1 if the index is invalid or there is an error, -2 if the index is out of range
		inline signed char __vectorcall status(const iter vIndex) const {
			if(vIndex == (iter)-1)return -1;								//Invalid index
			else if(vIndex >= size_) return -2;								//Index out of range
			else if(__lp_Tracker(vIndex) == scast<iter>(-1)) return 0;		//Ok
			else if(__lp_Tracker(vIndex) >= 0)return 1;						//Invalid element
			else return -1;													//Unknown error
		}


		//Returns true if the index is used, false if it's free or invalid (use the status function for more details)
		inline bool __vectorcall isValid(const iter vIndex) const {
			if(vIndex > size_ - 1) return false;					//Return false if the index is out of range
			return (__lp_Tracker(vIndex) == scast<iter>(-1));		//Return true if the index is used, false if it's free
		}




		// Get ----------------------------------------------------------------------------------------------------------------- //




		//Use the isValid() function to check if the element can be used
		inline type& __vectorcall operator[](const iter vIndex) const { return __lp_Data(vIndex); }
		//Returns a pointer to the first element of a chunk. The elements are guaranteed to be in contiguous order
		inline type* __vectorcall data(const iter vChunkIndex) const { return &data_[vChunkIndex][0]; }

		//Returns a pointer to a new map that contains all the elements in the chunks, without the invalid ones
		//This operation can be really slow, try to avoid using it
		type* data( ) const {
			type* arr = (type*)malloc(sizeof(type) * usedSize( ));
			iter new_i = 0;
			for(iter i = 0; i < usedSize( ); ++i) {
				if(isValid(i)) {
					arr[new_i] = __lp_Data(i);
					++new_i;
				}
				else ++i;
			}
			return arr;
		}




		// Size ---------------------------------------------------------------------------------------------------------------- //




		//Returns the number of elements in the map, including the free ones
		inline iter const size( ) const { return size_; }
		//Returns the number of used elements
		inline iter __vectorcall usedSize( ) const { return size_ - freeSize_; }
		//Returns the number of free elements
		inline iter freeSize( ) const { return freeSize_; }
	};
}
#undef __lp_Data
#undef __lp_Tracker
