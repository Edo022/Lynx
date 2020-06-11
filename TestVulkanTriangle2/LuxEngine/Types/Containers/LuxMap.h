

#pragma once
#include "LuxEngine/Types/Integers/Integers.h"
#include "LuxEngine/macros.h"
#include "LuxEngine/Types/Containers/LuxArray.h"
#include <vector>


#ifndef __LUXMAP
#define __LUXMAP




//Lux map
//A 64-bit index map capable of containing billions of elements without losing performances
//The elements are not ordered, so each element has a unique ID. The ID is returned by add functions
//Use the isValid() function to check if an element can be used or not
template<class type>
class LuxMap{
private:
	uint64 chunksDynNum;			//Allocated number of chunks
public:
	uint64 __lp_dynSize;			//Allocated number of elements. Please use size() instead of this variable
	uint64 __lp_freeNum;			//Number of free elements in the map. Please use usedSize() or freeSize() instead of this variable
	type** __lp_data;				//Map's elements
	uint64** __lp_tracker;			//Map that stores the state of each element

private:
	uint64 head;					//First free element
	uint64 tail;					//Last free element
	uint64 chunkSize;				//Size of the chunks
	uint64 maxSize;					//Maximum number of elements in the map

	#define __lp_Data(index) __lp_data[index / chunkSize][index % chunkSize]			//Get a data    element using only one index instead of index in the chunk and chunk index
	#define __lp_Tracker(index) __lp_tracker[index / chunkSize][index % chunkSize]		//Get a tracker element using only one index instead of index in the chunk and chunk index
public:




	// Constructor --------------------------------------------------------------------------------------------------------- //



	//Creates the map with the specified chunk and maximum size
	//The number of chunks depends on their size and the maximum size of the map (chunks = maxSize / chunkSize)
	//*   chunkSize: number of elements allocated when the map grows. Default at 0xFF (256). It must be <= maxSize
	//*   maxSize: the maximum size the map can reach. Default at 0xFFFF (65535). Max 0x FFFF FFFF FFFF FFFF
	LuxMap(uint64 _chunkSize = 0xFF, uint64 _maxSize = 0xFFFF) {
		chunkSize = _chunkSize;														//Set size of the chunks
		maxSize = _maxSize;															//Set maximum size of the map
		__lp_data = (type**)malloc(sizeof(type*) * (maxSize / chunkSize));			//Allocate data map
		__lp_tracker = (uint64**)malloc(sizeof(uint64*) * (maxSize / chunkSize));	//Allocate tracker map
		chunksDynNum = __lp_dynSize = __lp_freeNum = 0;								//Initialize number of chunks, elements and free elements							
		head = tail = -1;															//Initialize head and tail
	}




	// Add, remove --------------------------------------------------------------------------------------------------------- //




	//Adds an element at the end of the map, allocating a new chunk if needed
	//Returns the ID of the element
	uint64 append(type _data) {
		if (__lp_dynSize + 1 > chunksDynNum * chunkSize) {							//If the chunk is full
			__lp_data[chunksDynNum] = (type*)malloc(sizeof(type) * chunkSize);			//Allocate a new data chunk
			__lp_tracker[chunksDynNum] = (uint64*)malloc(sizeof(uint64) * chunkSize);	//Allocate a new tracker chunk
			chunksDynNum++;																//Update the number of chunks
		}
		__lp_Data(__lp_dynSize) = _data;											//Assign the data to the new element
		__lp_Tracker(__lp_dynSize) = -1;											//Set the tracker as valid

		__lp_dynSize++;																//Update the number of elements
		return __lp_dynSize - 1;													//Return the ID
	}




	//Adds an element at the firs free index of the map
	//Returns the ID of the element
	uint64 add(type _data) {
		uint64 head2 = head;
		if (head == -1) return append(_data);				//If it has no free elements, append it
		else {
			if (head == tail) {								//If it has only one free element
				__lp_Data(head2) = _data;						//Replace it
				head = tail = __lp_Tracker(head2) = -1;			//And reset head and tail
			}
			else {											//If it has more than one
				head = __lp_Tracker(head2);						//Update head
				__lp_Tracker(head2) = -1;						//Update the status of the first
				__lp_Data(head2) = _data;						//Replace it
			}
			__lp_freeNum--;									//Update the number of free elements
			return head2;
		}
	}




	//Adds an std::vector to the map, placing each element in the first free index
	//Returns a static map containing the IDs of the elements, in the same order as they were in the input
	LuxArray<uint64> add(std::vector<type> vec) {
		LuxArray<uint64> IDs;
		IDs.resize(vec.size());								//Set the number of IDs
		forEach(vec, i) IDs[i] = add(vec[i]);			//Add every element to the map and save its ID
		return IDs;											//Return the IDs
	}




	//Adds a LuxDynArray to the map, skipping all the invalid elements and placing the others in the first free index
	//Returns a static map containing the IDs of the elements, in the same order as they were in the input (invalid indices have -1 as ID)
	LuxArray<uint64> add(LuxMap<type> vec) {
		LuxArray<uint64> IDs;
		IDs.resize(vec.size());								//Set the number of IDs
		forEach(vec, i) {				//For every element of the input map
			if (vec.isValid(i)) IDs[i] = add(vec[i]);		//If it's valid, add it to the map and save its ID
			else IDs[i] = -1;								//If not, save -1 as ID
		}
		return IDs;											//Return the IDs
	}




	//Signs an index of the map as free, without deleting it. Returns a char for performance reasons
	//*   index: the index to sign
	//*   freeElm: whether to free the element or keep it in memory. Keeping it saves performances but increases memory usage
	//*   Returns 0 if the operation succeed, status(index) if not
	signed char remove(uint64 index, bool freeElm = false) {
		signed char r = status(index);

		if (r != 0) return r;								//If the index is not valid, return the error code
		else {												//If it is,
			__lp_Tracker(index) = -1;							//Set the index as free
			if (freeElm) free(&__lp_Data(index));				//Free the element if necessary
			if (head == (uint64)-1) head = tail = index;		//If it has no free elements, initialize head and tail.
			else tail = __lp_Tracker(tail) = index;				//If it has free elements, set the new tail and update the last free index
			__lp_freeNum++;										//Update the number of free elements
		}
		return 0;
	}




	//Sets the size of the map to 0, deleting all the elements and resetting it to the initial state
	void clear() {
		for (int32 i = 0; i < chunksDynNum; i++) free(__lp_data[i]); free(__lp_data);		//Free data map
		for (int32 i = 0; i < chunksDynNum; i++) free(__lp_tracker[i]); free(__lp_tracker);	//Free tracker map
		__lp_data = (type**)malloc(sizeof(type*) * (maxSize / chunkSize));					//Allocate data map
		__lp_tracker = (uint64**)malloc(sizeof(uint64*) * (maxSize / chunkSize));			//Allocate tracker map
		chunksDynNum = __lp_dynSize = __lp_freeNum = 0;										//Reset number of chunk, number of elements, number of free elements
		head = tail = -1;																	//Reset head and tail
	}




	// Status -------------------------------------------------------------------------------------------------------------- //




	//Returns 0 if the index is used, 1 if the index is free, -1 if the index is invalid or there is an error, -2 if the index is out of range
	inline signed char status(uint64 index) {
		if (index == (int64)-1)return -1;						//Invalid index
		else if (index >= __lp_dynSize) return -2;				//Index out of range
		else if (__lp_Tracker(index) == (uint64)-1) return 0;	//Ok
		else if (__lp_Tracker(index) >= 0)return 1;				//Invalid element
		else return -1;											//Unknown error
	}


	//Returns true if the index is used, 1 if it's free or invalid (use the 'status' function for more details)
	inline bool isValid(uint64 index) { return (__lp_dynSize > index && __lp_Tracker(index) == (uint64)-1); }




	// Get ----------------------------------------------------------------------------------------------------------------- //




	//Use the isValid() function to check if the element can be used
	inline type& operator [](uint64 index) { return __lp_Data(index); }
	//Returns a pointer to the first element of a chunk. The elements are guaranteed to be in contiguous order
	inline type* data(uint64 chunkIndex) { return &__lp_data[chunkIndex][0]; }

	//Returns a pointer to a new map that contains all the elements in the chunks, without the invalid ones
	//This operation can be really slow, try to avoid using it
	type* data() {
		type* arr = (type*)malloc(sizeof(type) * usedSize());
		int32 new_i = 0;
		for (int32 i = 0; i < usedSize(); i++) {
			if (isValid(i)) {
				arr[new_i] = __lp_Data(i);
				new_i++;
			}
			else i++;
		}
		return arr;
	}




	// Size ---------------------------------------------------------------------------------------------------------------- //




	//Returns the number of elements in the map, including the free ones
	inline uint64 size() { return __lp_dynSize; }
	//Returns the number of used elements
	inline uint64 usedSize() { return __lp_dynSize - __lp_freeNum; }
	//Returns the number of free elements
	inline uint64 freeSize() { return __lp_freeNum; }
};


#endif // __LUXMAP








