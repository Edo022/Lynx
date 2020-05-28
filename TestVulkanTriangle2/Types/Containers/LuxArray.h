
//Copyright 2020 Edoardo Manente
//Version 1.0



#pragma once
#include <vector>
#include "Types/Integers/Integers.h"

#ifndef __LUXARRAY
#define __LUXARRAY





//Chained Random Access array
//A 64-bit index array capable of containing billions of elements without losing performances
//The elements are not ordered, so you have to use their ID to get them
//If you can't use a value for invalid indices, use the 'isValid' function
template<class vecType>
class LuxArray {
private:
	uint64 dynSize, chunksDynNum;	//Allocated number of elements and chunks
	uint64 head, tail;				//First and last free element
	uint64 freeNum;					//Number of free elements in the array
	uint64 chunkSize;				//Size of the chunks
	uint64 maxSize;					//Maximum number of elements in the array
	vecType** data_;				//Array elements
	uint64** tracker;				//Array that stores the state of each element

	#define Data(index) data_[index / chunkSize][index % chunkSize]			//Get a data    element using only one index instead of index in the chunk and chunk index
	#define Tracker(index) tracker[index / chunkSize][index % chunkSize]	//Get a tracker element using only one index instead of index in the chunk and chunk index




public:

// Constructor --------------------------------------------------------------------------------------------------------- //


	//size		initial size of the array. Max 0x 7FFF FFFF FFFF FFFE (wtf ~ 9.2 quintillion)
	//chunkSize 	number of elements allocated when the array grows. Default at 0x FFFF (65535)
	//maxSize		the maximum size the array can have. Default at 0x FFFF FFFF (~ 4.3 billion)
	//The number of chunks depends on their size and the maximum size of the array (chunks = maxSize / chunkSize)
	LuxArray(/*uint64 _size = 0,*/ uint64 _chunkSize = 0xFFFF, uint64 _maxSize = 0xFFFFffff) {
		//resize(_size);
		chunkSize = _chunkSize;													//Set size of the chunks
		maxSize = _maxSize;														//Set maximum size of the array
		data_ = (vecType**)malloc(sizeof(vecType*) * (maxSize / chunkSize));	//Allocate data array
		tracker = (uint64**)malloc(sizeof(uint64*) * (maxSize / chunkSize));	//Allocate tracker array
		chunksDynNum = dynSize = freeNum = 0;									//Initialize number of chunks, number of elements and number of free elements							
		head = tail = -1;														//Initialize head and tail
	}


// Add, remove --------------------------------------------------------------------------------------------------------- //


	//Adds an element at the end of the array, allocating a new chunk if needed
	//Returns the ID of the element
	uint64 append(vecType _data) {
		if (dynSize + 1 > chunksDynNum * chunkSize) {						//If the chunk is full
			data_[chunksDynNum] = (vecType*)malloc(sizeof(vecType) * chunkSize);		//Allocate a new data chunk
			tracker[chunksDynNum] = (uint64*)malloc(sizeof(uint64) * chunkSize);		//Allocate a new tracker chunk
			chunksDynNum++;													//Update the number of chunks
		}
		Data(dynSize) = _data;												//Assign the data to the new element
		Tracker(dynSize) = -1;												//Set the tracker as valid

		dynSize++;															//Update the number of elements
		return dynSize - 1;	
	}




	//Adds an element at the firs free index of the array
	//Returns the ID of the element
	uint64 add(vecType _data) {
		uint64 head2 = head;
		if (head == -1) return append(_data);			//If it has no free elements, append it
		else {
			if (head == tail) {								//If it has only one free element
				Data(head2) = _data;							//Replace it
				head = tail = Tracker(head2) = -1;				//And reset head and tail
			}
			else {											//If it has more than one
				head = Tracker(head2);							//Update head
				Tracker(head2) = -1;							//Update the status of the first
				Data(head2) = _data;							//Replace it
			}
			freeNum--;										//Update the number of free elements
			return head2;
		}
	}




	//Adds an std::array to the array, placing each element in the first free index
	//Returns a array containing the IDs of the elements, in the same order as they were in the input
	std::vector<uint64> add(std::vector<vecType> vec) {
		std::vector<uint64> IDs;
		IDs.resize(vec.size());								//Set the number of IDs
		for (auto i : vec) {								//For every element of the std vector
			IDs.push_back(add(i));								//Add it to the array and save its ID
		}
		return IDs;
	}




	//Adds an RAarray to the array, skipping all the invalid elements and placing the others in the first free index
	//Returns a array containing the IDs of the elements, in the same order as they were in the input (invalid indices have -1 as ID)
	std::vector<uint64> add(LuxArray<vecType> vec) {
		std::vector<uint64> IDs;
		IDs.resize(vec.size());								//Set the number of IDs
		for (uint64 i; i < vec.size(); i++) {				//For every element of the input array
			if (vec.isValid(i)) IDs.push_back(add(vec[i]));		//If it's valid, add it to the array and save its ID
			else IDs.push_back(-1);								//If not, save -1 as ID
		}
		return IDs;
	}




	//Signs an index of the array as free, without deleting it
	//*   index: the index to sign
	//*   freeElm: whether to free the element or keep it in memory. Keeping it saves performances but increases memory usage
	//*   Returns 0 if the operation succeed. If not, returns status(index)
	signed short remove(uint64 index, bool freeElm = false) {
		signed short r = status(index);

		if (r != 0) return r;							//If the index is not valid, return the error code
		else {
			Tracker(index) = (int64)-1;
			if (freeElm) free(&Data(index));				//Free the element if necessary
			if (head == (int64)-1) head = tail = index;		//If it has no free elements, initialize head and tail.
			else tail = Tracker(tail) = index;				//If it has free elements, set the new tail and update the last free index
			freeNum++;										//Update the number of free elements
		}
		return 0;
	}




	//Sets the size of the array to 0, deleting all the elements and resetting it to the initial state
	void clear() {
		free(data_);															//Free data array
		free(tracker);															//Free tracker array
		data_ = (vecType**)malloc(sizeof(vecType*) * (maxSize / chunkSize));	//Allocate data array
		tracker = (uint64**)malloc(sizeof(uint64*) * (maxSize / chunkSize));	//Allocate tracker array
		chunksDynNum = dynSize = freeNum = 0;									//Reset number of chunk, number of elements, number of free elements
		head = tail = -1;														//Reset head and tail
	}


// Status -------------------------------------------------------------------------------------------------------------- //


	//Returns 0 if the index is used, 1 if the index is free, -1 if the index is invalid or there is an error, -2 if the index is out of range
	signed short status(uint64 index) {
		if (index == (int64)-1)return -1;					//Invalid index
		else if (index >= dynSize) return -2;				//Index out of range
		else if (Tracker(index) == (uint64)-1) return 0;	//Ok
		else if (Tracker(index) >= 0)return 1;				//Invalid element
		else return -1;										//Unknown error
	}


	//Returns true if the index is used, 1 if it's free or invalid (use the 'status' function for more details)
	bool isValid(uint64 index) { return (Tracker(index) == (uint64)-1); }


// Get ----------------------------------------------------------------------------------------------------------------- //


	//Returns the element at the index "index", checking if it's valid. If not, returns the invalidIndexValue
	//Alternatively, you can use the isValid(index) function
	vecType secureGet(uint64 index, vecType& invalidIndexValue) { return (isValid(index)) ? Data(index) : invalidIndexValue; }
	//Just standard insecure [] operator
	vecType& operator [](int index) { return Data(index); }

	//Returns a pointer to the first element of a chunk. The elements are guaranteed to be in contiguous order
	vecType* data(uint64 chunkIndex) {return &data_[chunkIndex][0];}

	//Returns a pointer to a new array that contains all the elements in the chunks, without the invalid ones. This operation can be really slow, try to avoid using it
	vecType* data() {
		vecType* arr = (vecType*)malloc(sizeof(vecType)*usedSize());
		int _i = 0;
		for (int i = 0; i < usedSize(); i++) {
			if (isValid(i)) {
				arr[_i] = Data(i);
				_i++;
			}
			else i++;
		}
		return arr;
	}


// Size ---------------------------------------------------------------------------------------------------------------- //


	//Returns the number of elements in the array, including the free ones
	uint64 size() { return dynSize; }
	//Returns the number of used elements
	uint64 usedSize() { return dynSize - freeNum; }
	//Returns the number of free elements
	uint64 freeSize() { return freeNum; }
};


#endif //__CRAarray