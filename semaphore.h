#ifndef SEMAPHORE_H
#define SEMAPHORE_H

// #include "scheduler.h"
#include "queue.h"
#include <string>
#include "pthread.h"
// #include <queue>

using namespace std;

class scheduler;

class semaphore{
    private:
        std::string resource_name;
        int sema_value;
        int lucky_task;
        // queue<TCB::task*> sema_queue;
        Queue<int>* sema_queue;
        pthread_mutex_t mutex;
        // pthread_cond_t cond;

        scheduler* sched_ptr;
        // pthread_cond_t condition;
        void lock();
        void unlock();
    public:
        semaphore(std::string sName, scheduler* theScheduler);
        pthread_mutex_t createMutex();
        void down(int taskID);
        void up(int taskID);
        std::string dump();
    
    // friend class scheduler;
};

#endif
