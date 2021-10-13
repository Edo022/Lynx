# 0 "/tmp/LynxEngineInit-b17a4b8f-f29a-4653-8d9f-6af067a25c63-SystemInfo.cpp"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
#undef _STDC_PREDEF_H
#define __GCC_IEC_559 2
#define __GCC_IEC_559_COMPLEX 2
# 0 "<command-line>" 2
# 1 "././src/Lynx/Core/Init.hpp" 1

#undef __LNX_INITIALIZER_GENERATOR__
# 0 "<command-line>" 2
# 1 "/tmp/LynxEngineInit-b17a4b8f-f29a-4653-8d9f-6af067a25c63-SystemInfo.cpp"
#define __linux__ 1
#undef _WIN64
# 18 "/tmp/LynxEngineInit-b17a4b8f-f29a-4653-8d9f-6af067a25c63-SystemInfo.cpp"
namespace lnx::sys{
    static_assert(std::is_const_v<const CpuInfo>, "Non-const variable defined with \"_lnx_init_var_const_def\""); namespace _pvt{ used const CpuInfo _lnx_init_var_cpu_init_f(); used bool _lnx_init_var_cpu_is_init; used const CpuInfo* _lnx_init_var_cpu_get(){ used static const CpuInfo* var = new (const CpuInfo)(_lnx_init_var_cpu_init_f()); _lnx_init_var_cpu_is_init = true; return var; } used const CpuInfo* _lnx_init_var_cpu_v; } _dbg(used inline const CpuInfo& g_cpu(){) _dbg( lnx::dbg::assertCond(_pvt::_lnx_init_var_cpu_is_init, "Global variable \"g_" "cpu" "\" used before initialization");) _dbg( return *_pvt::_lnx_init_var_cpu_v;) _dbg(};) used const CpuInfo _pvt::_lnx_init_var_cpu_init_f(){
#define DEL_P2(...) __VA_ARGS__
#define DEL_P(v) DEL_P2 v
#define _lnx_init_var_const_def2(type,name,id) static_assert(std::is_const_v<DEL_P(type)>, "Non-const variable defined with \"_lnx_init_var_const_def\""); namespace _pvt{ used DEL_P(type) id ##_init_f(); used bool id ##_is_init; used DEL_P(type)* id ##_get(){ used static DEL_P(type)* var = new (DEL_P(type))(id ##_init_f()); id ##_is_init = true; return var; } used DEL_P(type)* id ##_v; } _dbg(used inline DEL_P(type)& g_ ##name(){) _dbg( lnx::dbg::assertCond(_pvt::id ##_is_init, "Global variable \"g_" #name "\" used before initialization");) _dbg( return *_pvt::id ##_v;) _dbg(};) used DEL_P(type) _pvt::id ##_init_f()
#define _lnx_init_var_const_def(type,name,fullNs) _lnx_init_var_const_def2(type, name, _lnx_init_var_ ##name)
# 20 "/tmp/LynxEngineInit-b17a4b8f-f29a-4653-8d9f-6af067a25c63-SystemInfo.cpp"
        CpuInfo _cpu = {
            .L1D = {
                .size = (uint32)sysconf(_SC_LEVEL1_DCACHE_SIZE),
                .lineSize = (uint32)sysconf(_SC_LEVEL1_DCACHE_LINESIZE),
                .assoc = (uint32)sysconf(_SC_LEVEL1_DCACHE_ASSOC)
            },
            .L1I = {
                .size = (uint32)sysconf(_SC_LEVEL1_ICACHE_SIZE),
                .lineSize = (uint32)sysconf(_SC_LEVEL1_ICACHE_LINESIZE),
                .assoc = (uint32)sysconf(_SC_LEVEL1_ICACHE_ASSOC)
            },
            .L2 = {
                .size = (uint32)sysconf(_SC_LEVEL2_CACHE_SIZE),
                .lineSize = (uint32)sysconf(_SC_LEVEL2_CACHE_LINESIZE),
                .assoc = (uint32)sysconf(_SC_LEVEL2_CACHE_ASSOC)
            },
            .L3 = {
                .size = (uint32)sysconf(_SC_LEVEL3_CACHE_SIZE),
                .lineSize = (uint32)sysconf(_SC_LEVEL3_CACHE_LINESIZE),
                .assoc = (uint32)sysconf(_SC_LEVEL3_CACHE_ASSOC)
            },
            .L4 = {
                .size = (uint32)sysconf(_SC_LEVEL4_CACHE_SIZE),
                .lineSize = (uint32)sysconf(_SC_LEVEL4_CACHE_LINESIZE),
                .assoc = (uint32)sysconf(_SC_LEVEL4_CACHE_ASSOC)
            },
        };


        auto name = dbg::cmdOutput("cat /proc/cpuinfo | grep -m1 'model.*name' | grep     -o ':.*' | grep -o '[^: ].*'");
        auto threads = dbg::cmdOutput("cat /proc/cpuinfo | grep -c 'processor.*[0-9]*'");
        auto cores = dbg::cmdOutput("cat /proc/cpuinfo | grep -m1 'cores'       | grep     -o '[0-9].*'");
        auto minFreq = dbg::cmdOutput("lscpu | grep -m1 'min[ \\-_]*[Mm][Hh][Zz]' | grep -m1 -o '[0-9]*'");
        auto maxFreq = dbg::cmdOutput("lscpu | grep -m1 'max[ \\-_]*[Mm][Hh][Zz]' | grep -m1 -o '[0-9]*'");

        _cpu.name = name;
        _cpu.cores = (uint32)atoi(cores);
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




    static_assert(std::is_const_v<const RamInfo>, "Non-const variable defined with \"_lnx_init_var_const_def\""); namespace _pvt{ used const RamInfo _lnx_init_var_ram_init_f(); used bool _lnx_init_var_ram_is_init; used const RamInfo* _lnx_init_var_ram_get(){ used static const RamInfo* var = new (const RamInfo)(_lnx_init_var_ram_init_f()); _lnx_init_var_ram_is_init = true; return var; } used const RamInfo* _lnx_init_var_ram_v; } _dbg(used inline const RamInfo& g_ram(){) _dbg( lnx::dbg::assertCond(_pvt::_lnx_init_var_ram_is_init, "Global variable \"g_" "ram" "\" used before initialization");) _dbg( return *_pvt::_lnx_init_var_ram_v;) _dbg(};) used const RamInfo _pvt::_lnx_init_var_ram_init_f(){
        RamInfo _ram;
# 85 "/tmp/LynxEngineInit-b17a4b8f-f29a-4653-8d9f-6af067a25c63-SystemInfo.cpp"
            _ram.pageNum = (uint64)sysconf(_SC_PHYS_PAGES);
            _ram.pageSize = (uint64)sysconf(_SC_PAGE_SIZE);
            _ram.size = _ram.pageNum * _ram.pageSize;


        return _ram;
    }
}
