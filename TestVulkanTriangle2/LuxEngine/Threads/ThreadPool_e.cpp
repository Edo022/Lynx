
#include "ThreadPool.h"


lux::Array<std::thread, uint32> lux::Thread::threads;
lux::Queue<void*> lux::Thread::functions;