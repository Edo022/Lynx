#pragma once
////#define LNX_H_STRING
#include "Lynx/Types/Containers/ContainerBase.hpp"
//TODO improve concatenation performance








namespace lnx {
	#define updateViewc() _dbg(viewer = (char*)Super::data)




	class String : public ContainerBase<char8, uint32>{
		using Super = ContainerBase<char8, uint32>;
		genInitCheck;
	private:
		_dbg(const char* viewer;)


		//FIXME FIX
		inline void cat(const char8* vStr) {
			Super::resize(Super::count() - 1); //Remove \0
			Super::cat(String(vStr));
			updateViewc();
		}

	public:




		// Constructors -------------------------------------------------------------------------------------------------------- //




		/**
		 * @brief Default constructor
		 *     Creates an empty string that contains a '\0' only
		 * Complexity: O(1)
		 */
		inline String() : Super(1) {
			Super::data[0] = '\0';
			updateViewc();
		}

		/**
		 * @brief Copy constructor for C strings
		 *     The lenght of vStr can be specified as second argument to improve performance
		 * Complexity: O(n)
		 *     Where n = strlen(vStr)
		 * @param vStr The string to copy
		 */
		inline String(const char8* vStr) : Super(strlen(vStr) + 1) {
			ram::cpy(vStr, Super::data, Super::data.size());
			updateViewc();
		}


		/**
		 * @brief Copy constructor for C strings
		 * Complexity: O(n)
		 *     Where n = vLen
		 * @param vStr The string to copy
		 */
		inline String(const char8* vStr, uint64 vLen) : Super(vLen) {
			ram::cpy(vStr, Super::data, Super::data.size());
			updateViewc();
		}


		/**
		 * @brief Move constructor
		 * Complexity: O(1)
		 * @param pStr The string to move
		 */
		inline String(String&& pStr) {
			Super::move(pStr);
			updateViewc();
		}


		/**
		 * @brief Copy constructor
		 * Complexity: O(n)
		 *     Where n = vStr.count()
		 * @param vStr The string to copy
		 */
		inline String(const String& pStr) : Super(pStr, {}) {
			updateViewc();
		}




		// Get ----------------------------------------------------------------------------------------------------------------- //




		/**
		 * @brief Returns the size of the string
		 * Complexity: O(1)
		 * @return The size of the string in bytes
		 */
		inline uint64 size() const {
			checkInit(); return Super::data.size();
		}


		/**
		 * @brief Returns a reference to a caracter
		 * Complexity: O(1)
		 * @param vIndex The index of the character
		 * @return A rvalue reference to the vIndex-th character
		 */
		inline char8& operator[](const uint32 vIndex) const {
			checkInit();
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			return Super::operator[](vIndex);
		}




		// String concatenation - nya ------------------------------------------------------------------------------------------ //




		/**
		 * @brief Concatenates a String
		 * Complexity:
		 *     O(n+m)[Memory reallocation required]
		 *     O(m)
		 *     Where n = this->count() and m = pStr.count()
		 * @param pStr The string to concatenate
		 */
		inline void operator+=(const String& pStr) {
			checkInit(); cat(pStr.Super::data);
		}


		/**
		 * @brief Concatenates C string
		 * Complexity:
		 *     O(n+m)[Memory reallocation required]
		 *     O(m)
		 *     Where n = this->count() and m = strlen(vStr)
		 * @param pStr The C string to concatenate
		 */
		inline void operator+=(const char8* vStr ) {
			checkInit(); cat(vStr);
		}


		//FIXME use sum chain struct instead of copying the string data
		inline String operator+(const String& pStr) const {
			checkInit();
			String _string(Super::data);
			_string += pStr;
			return _string;
		}
		//FIXME same here
		inline String operator+(const char8* vStr ) const {
			checkInit();
			String _string(Super::data);
			_string += vStr;
			return _string;
		}








		// Assignment and comparison ------------------------------------------------------------------------------------------- //








		/**
		 * @brief move assignment
		 * Complexity: O(1)
		 * @param pStr The string to move
		 * @return A rvalue reference to this object
		 */
		inline auto& operator=(String&& pStr) {
			Super::move(pStr);
			updateViewc();
			return *this;
		}


		/**
		 * @brief Copy assignment
		 * Complexity: O(n)
		 * @param vStr The String to copy
		 * @return A rvalue reference to this object
		 */
		inline auto& operator=(const String& pStr) {
			checkInit(); isInit(pStr);
			Super::copy(pStr);
			updateViewc();
			return *this;
		}


		/**
		 * @brief Copy assignment for C strings
		 * Complexity: O(n)
		 * @param vStr The C string to copy
		 *     The string must be null terminated
		 * @return A rvalue reference to this object
		 */
		inline auto& operator=(const char8* vStr) {
			//FIXME write an actual function
			//FIXME or just check that this is as fast as directly copying it
			//FIXME it should be, with the memory pool
			return operator=(String(vStr));
		}




		/**
		 * @brief Compares the string to another string
		 * Complexity:
		 *     O(1) [Different length]
		 *     Best:  O(1)
		 *     Worst: O(n)
		 * @param pStr The String to compare
		 * @return True if the strings are equal, false otherwise
		 */
		inline bool operator==(const String& pStr) const {
			checkInit(); isInit(pStr);
			return (Super::count() == pStr.count()) && 0 == strcmp(pStr.data, Super::data);
		}


		/**
		 * @brief Compares the string to a C string
		 * Complexity:
		 *     Best:  O(1)
		 *     Worst: O(n)
		 * @param vStr The C string to compare
		 *     The string must be null terminated
		 * @return True if the strings are equal, false otherwise
		 */
		inline bool operator==(const char* vStr) const {
			checkInit();
			return 0 == strcmp(vStr, Super::data);
		}
	};
	#undef updateViewc
}
