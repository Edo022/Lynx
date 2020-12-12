#pragma once
#define LUX_H_SYSTEM
#include <filesystem>
#include <cstdio>
#include <thread>
#include "LuxEngine/Types/Containers/LuxString.hpp"
#include "LuxEngine/System/SystemMacros.hpp"







namespace lux::sys {
	namespace dir {
		extern String thisDir;
		void fixWindowsPath(lux::String& pStr);
		lux::String getExtensionFromPath(const lux::String& pStr);
		lux::String getFileNameFromPath(const lux::String& pStr);
	}

	extern uint32 threadNum;
	void init( );
}