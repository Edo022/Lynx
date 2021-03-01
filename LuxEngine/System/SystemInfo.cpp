#include "LuxEngine/System/SystemInfo.hpp"
#include "LuxEngine/Debug/Debug.hpp"
#include <regex>
#ifdef __linux__
    #include <unistd.h>
#endif




namespace lux::sys{
    CpuInfo cpu = {
        .cores = []()->uint32{
            char* out = dbg::cmdOutput("dmidecode -t processor | grep 'Core Count'");
            char* i = out; while(*i < '0' && *i > '9' && *i != '.' && *i != ',') ++i;
            uint32 ret = atoi(i); free(out); return ret;
        }(),
        .threads = []()->uint32{
            char* out = dbg::cmdOutput("dmidecode -t processor | grep 'Thread Count'");
            char* i = out; while(*i < '0' && *i > '9' && *i != '.' && *i != ',') ++i;
            uint32 ret = atoi(i); free(out); return ret;
        }(),
        .minFreq = []()->uint32{
            char* out = dbg::cmdOutput("lscpu | grep 'min[ \\-_]*[Mm][Hh][Zz]'");
            char* i = out; while(*i < '0' && *i > '9' && *i != '.' && *i != ',') ++i;
            uint32 ret = atoi(i); free(out); return ret;
        }(),
        .maxFreq = []()->uint32{
            char* out = dbg::cmdOutput("lscpu | grep 'max[ \\-_]*[Mm][Hh][Zz]'");
            char* i = out; while(*i < '0' && *i > '9' && *i != '.' && *i != ',') ++i;
            uint32 ret = atoi(i); free(out); return ret;
        }(),
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
    RamInfo ram;

    GpuInfo gpu;
    VRamInfo vram;
}