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
        Queue<int>* sema_queue;
        pthread_mutex_t mutex;

        scheduler* sched_ptr;
        void lock();
        void unlock();
		
    public:
        semaphore(std::string sName, scheduler* theScheduler);
        bool down(int taskID);
        void up(int taskID);
        std::string dump();
};

#endif
