#pragma once
#define LUX_H_SYSTEM
#include <filesystem>
#include <cstdio>
#include "Lynx/Types/Containers/LuxString.hpp"
#include "Lynx/System/SystemMacros.hpp"







namespace lux::sys {
	namespace dir {
		extern String thisDir;
		void fixWindowsPath(const lux::String& pStr);
		lux::String getExtensionFromPath(const lux::String& pStr);
		lux::String getFileNameFromPath (const lux::String& pStr);
	}
}