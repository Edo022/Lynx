
#include "ThreadPool.h"


lux::Array<std::thread*> lux::thr::threads;
lux::Queue<lux::thr::ExecFuncDataBase*> lux::thr::maxpq;
lux::Queue<lux::thr::ExecFuncDataBase*> lux::thr::highpq;
lux::Queue<lux::thr::ExecFuncDataBase*> lux::thr::lowpq;
lux::Queue<lux::thr::ExecFuncDataBase*> lux::thr::minpq;