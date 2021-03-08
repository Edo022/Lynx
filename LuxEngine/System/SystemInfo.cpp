#include "LuxEngine/System/SystemInfo.hpp"
#include "LuxEngine/Debug/Debug.hpp"

//TODO ADD WINDOWS INFOS
#ifdef _WIN64
#	include <windows.h>
#elif defined __linux__
#	include <unistd.h>
#endif








namespace lux::sys{
    const CpuInfo cpu = {
        .name    =              dbg::cmdOutput("cat /proc/cpuinfo | grep -m1 'model.*name' | grep     -o ':.*' | grep -o '[^: ].*'"),
        .cores   = (uint32)atoi(dbg::cmdOutput("cat /proc/cpuinfo | grep -m1 'cores'       | grep     -o '[0-9].*'")),
        .threads = (uint32)atoi(dbg::cmdOutput("cat /proc/cpuinfo | grep -m1 'threads'     | grep     -o '[0-9].*'")),
        .minFreq = (uint32)atoi(dbg::cmdOutput("lscpu | grep -m1 'min[ \\-_]*[Mm][Hh][Zz]' | grep -m1 -o '[0-9]*'")),
        .maxFreq = (uint32)atoi(dbg::cmdOutput("lscpu | grep -m1 'max[ \\-_]*[Mm][Hh][Zz]' | grep -m1 -o '[0-9]*'")),
        .L1D = {
            .size     = (uint32)sysconf(_SC_LEVEL1_DCACHE_SIZE),
            .lineSize = (uint32)sysconf(_SC_LEVEL1_DCACHE_LINESIZE),
            .assoc    = (uint32)sysconf(_SC_LEVEL1_DCACHE_ASSOC)
        },
        .L1I = {
            .size     = (uint32)sysconf(_SC_LEVEL1_ICACHE_SIZE),
            .lineSize = (uint32)sysconf(_SC_LEVEL1_ICACHE_LINESIZE),
            .assoc    = (uint32)sysconf(_SC_LEVEL1_ICACHE_ASSOC)
        },
        .L2 = {
            .size     = (uint32)sysconf(_SC_LEVEL2_CACHE_SIZE),
            .lineSize = (uint32)sysconf(_SC_LEVEL2_CACHE_LINESIZE),
            .assoc    = (uint32)sysconf(_SC_LEVEL2_CACHE_ASSOC)
        },
        .L3 = {
            .size     = (uint32)sysconf(_SC_LEVEL3_CACHE_SIZE),
            .lineSize = (uint32)sysconf(_SC_LEVEL3_CACHE_LINESIZE),
            .assoc    = (uint32)sysconf(_SC_LEVEL3_CACHE_ASSOC)
        },
        .L4 = {
            .size     = (uint32)sysconf(_SC_LEVEL4_CACHE_SIZE),
            .lineSize = (uint32)sysconf(_SC_LEVEL4_CACHE_LINESIZE),
            .assoc    = (uint32)sysconf(_SC_LEVEL4_CACHE_ASSOC)
        },
    };




    const RamInfo ram = {
        #ifdef _WIN64
            .pageNum = []() {
		    	MEMORYSTATUSEX status;
		    	status.dwLength = sizeof(status);
		    	GlobalMemoryStatusEx(&status);
		    	return status.ullTotalPageFile;
		    }();
            .pageSize = []() {
		    	MEMORYSTATUSEX status;
		    	status.dwLength = sizeof(status);
		    	GlobalMemoryStatusEx(&status);
		    	return status.ullAvailPageFile;
		    }();
            .size = []() {
		    	MEMORYSTATUSEX status;
		    	status.dwLength = sizeof(status);
		    	GlobalMemoryStatusEx(&status);
		    	return status.ullTotalPhys;
		    }();
        #else
            .pageNum  = (uint32)sysconf(_SC_PHYS_PAGES),
            .pageSize = (uint64)sysconf(_SC_PAGE_SIZE),
            .size     = (uint64)(sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGE_SIZE))
        #endif
    };
}