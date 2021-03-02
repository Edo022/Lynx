#pragma once
#include "LuxEngine/Types/Integers/Integers.hpp"




namespace lux::sys{
    struct CpuInfo{
        const char* name;   //Name of the CPU
        uint32 cores;       //Number of physical cores
        uint32 threads;     //Number of threads
        uint32 minFreq;     //Minimum frequence in Mhz
        uint32 maxFreq;     //Maximum frequence in Mhz
        struct CacheInfo{
            uint64 size;    //Size of the cache in bytes
            uint64 lineSize;//Size of the cache line in bytes
            uint64 assoc;   //Cache associativity
        } L1D, L1I, L2, L3, L4;
    };
    extern const CpuInfo cpu;


    struct RamInfo{
        // uint32 freq;        //Frequence of the memory in Mhz
        // uint32 config;      //Configuration: 1 = single channel, 2 = dual channel, 4 = quad channel
        uint32 pageNum;     //Number of memory pages
        uint64 pageSize;    //Size of each memory page in bytes
        uint64 size;        //Total size of the memory in bytes (pageNum * pageSize)
    };
    extern const RamInfo ram;


    enum DeviceType{
        Discrete,
        Integrated
    };
    struct GpuInfo{
        //TODO use array of gpus
        const char* name;   //Name of the GPU
        DeviceType type;    //Type of device (Discrete or Integrated gpu)
        uint32 freq;        //Core frequence in Mhz
        uint32 cores;       //Number of cores //TODO specify core type
        uint32 maxWgSize[3];//Maximum size of each workgroup //FIXME
        uint32 maxWgInvoc;  //Maximum thread count in a workgroup //FIXME
        uint32 maxWgs[3];   //Maximum number of workgroups per dispatch //FIXME
    };
    // extern const GpuInfo gpu;
    extern GpuInfo gpu;


    struct VRamInfo{
        uint32 freq;        //Frequence of the memory in Mhz
        uint32 pageNum;     //Number of memory pages
        uint64 pageSize;    //Size of each memory page in bytes
        uint64 size;        //Total size of the memory in bytes (pageNum * pageSize)
    };
    // #ifdef __INTELLISENSE__
        // extern const VRamInfo vram;
    // #else
        extern VRamInfo vram;
}