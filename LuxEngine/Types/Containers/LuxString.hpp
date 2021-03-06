#pragma once
#define LUX_H_STRING
#include "LuxEngine/Types/Containers/ContainerBase.hpp"






//FIXME FIX LUXSTR. CHECK CONSTRUCTORS AND DESTRUCTORS

namespace lux {
	//FIXME REMOVE. USE NORMAL STRLEN
	static inline constexpr uint32 strlenl(const char8* str ) { for(uint32 len = 0; ; ++len) if(str[len] == '\0') return len; }
	static inline constexpr uint32 strlenl(const wchar8* str) { for(uint32 len = 0; ; ++len) if(str[len] == '\0') return len; }
	#define updateView() _dbg(viewer = (char*)Super::data)



	//TODO add constructor that takes the count in input. IF it's already known, there is no reason to recalculate it
	//TODO improve concatenation performance
	class String : public ContainerBase<char8, uint32>{
		using Super = ContainerBase<char8, uint32>;
		genInitCheck;
	private:
		_dbg(const char* viewer;)


		//FIXME FIX
		inline void cat(const char8* vString) {
			Super::resize(Super::count() - 1); //Remove \0
			Super::cat(String(vString));
			updateView();
		}

	public:




		// Constructors -------------------------------------------------------------------------------------------------------- //




		inline String(                                 ) : Super(1) { Super::data[0] = '\0'; updateView(); }
		inline String(const char8* vString             ) : Super(strlenl(vString) + 1) { ram::cpy(vString, Super::data, Super::data.size()); updateView(); }
		inline String(const char8* vString, uint64 vLen) : Super(vLen)	            { ram::cpy(vString, Super::data, Super::data.size()); updateView(); }

		//Move constructor
		inline String(String&& pString) { Super::move(pString); updateView(); }
		//Copy constructor
		inline String(const String& pString) : Super(pString, {}) { updateView(); }




		// Get ----------------------------------------------------------------------------------------------------------------- //




		inline uint64 size(  ) const { checkInit(); return Super::data.size(); }


		inline char8& operator[](const uint32 vIndex) const {
			checkInit();
			dbg::checkIndex(vIndex, 0, count() - 1, "vIndex");
			return Super::operator[](vIndex);
		}




		// String concatenation - nya ------------------------------------------------------------------------------------------ //




		inline void operator+=(const String& pString) { checkInit(); cat(pString.Super::data); }
		inline void operator+=(const char8* vString ) { checkInit(); cat(vString); }


		//FIXME use sum chain struct instead of copying the string data
		inline String operator+(const String& pString) const {
			checkInit();
			String vLuxString(Super::data);
			vLuxString += pString;
			return vLuxString;
		}
		//FIXME same here
		inline String operator+(const char8* vString ) const {
			checkInit();
			String vLuxString(Super::data);
			vLuxString += vString;
			return vLuxString;
		}




		// Assignment and comparison ------------------------------------------------------------------------------------------- //




		//move assignment
		inline auto& operator=(String&& pString) {
			Super::move(pString);
			updateView();
			return *this;
		}

		//copy assignment
		inline auto& operator=(const String& pString) {
			checkInit(); isInit(pString);
			Super::copy(pString);
			updateView();
			return *this;
		}

		//Copy from C-style string
		inline auto& operator=(const char8* vString) { return operator=(String(vString)); }





		//The cooler strcmp
		inline bool operator==(const String& pString) const {
			checkInit(); isInit(pString);
			return (Super::count() == pString.count()) && 0 == memcmp(pString.data, Super::data, Super::count());
		}

		/**
		 * @brief Compares the string to a C string.
		 *		If the string lenght is known, the cmp() function can be used to avoid unnecessary strlen calls
		 * @param vString The string to compare
		 */
		inline bool operator==(const char* vString) const {
			checkInit();
			return (Super::count() == strlenl(vString) + 1) && 0 == memcmp(vString, Super::data, Super::count());
		}

		/**
		 * @brief operator==, but the string length can be specified
		 * @param vString The tring to compare
		 * @param vStrLen The length of the string, including the null terminator '\\0'
		 */
		inline bool cmp(const char* vString, const uint32 vStrLen) const {
			checkInit();
			return (Super::count() == vStrLen) && 0 == memcmp(vString, Super::data, Super::count());
		}
	};
}

