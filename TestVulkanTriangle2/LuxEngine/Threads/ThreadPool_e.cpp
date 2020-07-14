
#include "ThreadPool.h"


bool lux::thr::update;
HANDLE lux::thr::mngThr;
lux::Array<lux::thr::ThrPoolElm> lux::thr::threads;
lux::Map<lux::thr::ThrState, uint32> lux::thr::thrStates;
lux::Queue<lux::thr::ExecFuncDataBase*> lux::thr::maxpq;
lux::Queue<lux::thr::ExecFuncDataBase*> lux::thr::highpq;
lux::Queue<lux::thr::ExecFuncDataBase*> lux::thr::lowpq;
lux::Queue<lux::thr::ExecFuncDataBase*> lux::thr::minpq;