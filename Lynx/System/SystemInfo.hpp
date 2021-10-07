#pragma once
#define LUX_H_SYSTEM_INFO
#include "Lynx/Core/IncludeVulkan.hpp"
#include "Lynx/Types/Integers/Integers.hpp"
#include "Lynx/Core/Init.hpp"



namespace lnx::sys{
    struct CpuInfo{
        const char* name;   //Name of the CPU
        uint32 cores;       //Number of physical cores. This information is implementation dependant and very unreliable.
        uint32 threads;     //Number of threads
        uint32 minFreq;     //Minimum frequence in Mhz
        uint32 maxFreq;     //Maximum frequence in Mhz
        struct CacheInfo{
            uint64 size;        //Size of the cache in bytes
            uint64 lineSize;    //Size of the cache line in bytes
            uint64 assoc;       //Cache associativity
        } L1D, L1I, L2, L3, L4;
    };
    _lnx_init_var_dec(const CpuInfo, cpu);


    struct RamInfo{
        // uint32 freq;        //Frequence of the memory in Mhz
        // uint32 config;      //Configuration: 1 = single channel, 2 = dual channel, 4 = quad channel
        uint64 pageNum;         //Number of memory pages
        uint64 pageSize;        //Size of each memory page in bytes
        uint64 size;            //Total size of the memory in bytes (pageNum * pageSize)
    };
    _lnx_init_var_dec(const RamInfo, ram);


    enum class DeviceType{ //FIXME
        eDiscrete,
        eIntegrated
    };
    struct GpuInfo{
        const char* name;       //Name of the GPU
        DeviceType type;        //Type of device (Discrete or Integrated gpu)
        uint32 maxWgSize[3];    //Maximum size of each workgroup //FIXME
        uint32 maxWgInvoc;      //Maximum thread count in a workgroup //FIXME
        uint32 maxWgs[3];       //Maximum number of workgroups per dispatch //FIXME

        uint32 freq = 0;        //Frequence of the memory in Mhz //FIXME
        uint64 size = 0;        //Total size of the VRAM memory in bytes
        struct HeapsInfo{       //Informations about the available memory heaps
            uint32 num = 0;         //Number of heaps
            alwaysInline auto& operator[](uint32 i) { return heaps_[i]; }
        private:
            struct HeapInfo{        //Informations about a specific heap
                uint64 size = 0;        //Size of the heap in bytes
                vk::MemoryHeapFlags flags = (vk::MemoryHeapFlags)0;       //Vulkan heap flags
            } heaps_[16];
        } heaps;
    };
    _lnx_init_var_dec(const GpuInfo, vram);
}