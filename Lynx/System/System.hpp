#pragma once
#define LNX_H_SYSTEM
#include <filesystem>
#include <cstdio>
#include "Lynx/Types/Containers/String.hpp"
#include "Lynx/System/SystemMacros.hpp"







namespace lnx::sys {
	namespace dir {
		extern String thisDir;
		void fixWindowsPath(const lnx::String& pStr);
	}
}