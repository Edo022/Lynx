#pragma once
////#define LNX_H_SYSTEM
#include <filesystem>
#include <cstdio>
#include "Lynx/Types/Containers/String.hpp"







namespace lnx::sys {
	namespace dir {
		_lnx_init_var_dec((String), thisDir);
		void fixWindowsPath(const lnx::String& pStr);
	}
}