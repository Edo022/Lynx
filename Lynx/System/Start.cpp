#include <iostream>
#include <new>
#ifndef _WIN32

extern std::string stringa;

extern "C" void lynx_start() {

    asm volatile("call _start@PLT");
}

#else

// TODO Add windows code here

#endif