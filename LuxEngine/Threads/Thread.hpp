#pragma once
#define LUX_H_THREAD
#include <pthread.h>
#include <signal.h>



namespace lux{
    struct thread{
        pthread_t thr;
        // thread() : thr{ pthread_create(&thr, )} {}

        void suspend(){ pthread_kill(thr, SIGSTOP); }
        void resume( ){ pthread_kill(thr, SIGCONT); }

        void setName(const char* pName){ pthread_setname_np(thr, pName); }
        const char* getName(const char* pName){ char name[16]; pthread_getname_np(thr, name, 16); return name; }

        void join() { pthread_join(thr, nullptr); }
        void detach(){ pthread_detach(thr); }
    };

    // namespace thr{
    //     thread& thisThread(){ return pthread_self(); }

    // }
};