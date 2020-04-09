#ifndef SCHEDULER_H
#define SCHEDULER_H

class semaphore;

#include <pthread.h>

#include "tcb.h"

extern pthread_mutex_t proc_mutex;
extern pthread_mutex_t mutex;

class scheduler{
    private:
		//TCB *process_table;
		//int nextPID;
		bool clean;
		bool deadlock;
		time_t startTime;
		int quantum;
		//semaphore *io;
    public:
		pthread_mutex_t proc_mutex;
		TCB *process_table;
		semaphore *io;
		int nextPID;
	
		scheduler();
        pthread_mutex_t procMutex();
        void create_task(std::string name);
		void create_lazy_task(std::string name);
        void destroy_task(int dPID);
        //static void *thread_function(void *);
        bool yield();
        void rotate(char state);
        void cleanup();
        bool getDL();
        void block();
        void blockPID(int bPID);
        bool unblockPID(int ubPID);
        std::string dump();
        ~scheduler();

    friend class semaphore;
};

#endif
