#include "Lynx/System/SystemInfo.hpp"
#include "Lynx/Debug/Debug.hpp"
#include "Lynx/Core/AutoInit.hpp"




//FIXME FIX WINDOWS INFOS
#ifdef _WIN64
#	include <windows.h>
#elif defined __linux__
#	include <unistd.h>
#endif




namespace lnx::sys{
    CfuVarDef(const CpuInfo, cpu){
        CpuInfo _cpu = {
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


        auto name    = dbg::cmdOutput("cat /proc/cpuinfo | grep -m1 'model.*name' | grep     -o ':.*' | grep -o '[^: ].*'");
        auto threads = dbg::cmdOutput("cat /proc/cpuinfo | grep -c 'processor.*[0-9]*'");
        auto cores   = dbg::cmdOutput("cat /proc/cpuinfo | grep -m1 'cores'       | grep     -o '[0-9].*'");
        auto minFreq = dbg::cmdOutput("lscpu | grep -m1 'min[ \\-_]*[Mm][Hh][Zz]' | grep -m1 -o '[0-9]*'");
        auto maxFreq = dbg::cmdOutput("lscpu | grep -m1 'max[ \\-_]*[Mm][Hh][Zz]' | grep -m1 -o '[0-9]*'");

        _cpu.name    = name;
        _cpu.cores   = (uint32)atoi(cores);
        _cpu.threads = (uint32)atoi(threads);
        _cpu.minFreq = (uint32)atoi(minFreq);
        _cpu.maxFreq = (uint32)atoi(maxFreq);

        free(name);
        free(cores);
        free(threads);
        free(minFreq);
        free(maxFreq);

        return _cpu;
    }




    CfuVarDef(const RamInfo, ram){
        RamInfo _ram;

        #ifdef _WIN64
            MEMORYSTATUSEX status;
            status.dwLength = sizeof(status);
            GlobalMemoryStatusEx(&status);

            _ram.pageNum  = status.ullTotalPageFile;
            _ram.pageSize = status.ullAvailPageFile;
            _ram.size     = status.ullTotalPhys;
        #else
            _ram.pageNum  = (uint64)sysconf(_SC_PHYS_PAGES);
            _ram.pageSize = (uint64)sysconf(_SC_PAGE_SIZE);
            _ram.size     = _ram.pageNum * _ram.pageSize;
        #endif

        return _ram;
    }
}