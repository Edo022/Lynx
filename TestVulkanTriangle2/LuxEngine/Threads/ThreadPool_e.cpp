
#include "ThreadPool.h"


lux::Array<std::thread, uint32> lux::thr::threads;
lux::Queue<void*> lux::thr::maxpq;
lux::Queue<void*> lux::thr::highpq;
lux::Queue<void*> lux::thr::lowpq;
lux::Queue<void*> lux::thr::minpq;