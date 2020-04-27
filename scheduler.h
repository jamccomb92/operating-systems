/**********************************************************
Author       : Joe McCombs, Justin Doney, Kamil Kosidlak
Class        : C435, Spring 2020
File Name    : scheduler.h
Last Updated :
Description  : Header file that defines the scheduler class.
**********************************************************/

#ifndef SCHEDULER_H
#define SCHEDULER_H

class semaphore;

#include <pthread.h>

#include "tcb.h"
#include "UltimaWindows.h"
#include "ipc.h"
#include "memoryManager.h"
#include "ufs.h"

class memoryManager;

class scheduler
{
private:
	bool clean;
	bool deadlock;
	time_t startTime;
	int quantum;
	TCB *process_table;

public:
	ipc *sched_ipc;
	pthread_mutex_t proc_mutex;
	semaphore *io;
	int nextPID;
	UltimaWindows sched_windows; // potentially for easier creation
	memoryManager *memory_manager;
	ufs *file_system;

	TCB *getProcessTable();
	scheduler();
	void start();
	pthread_mutex_t procMutex();
	void create_task(std::string name);
	void create_lazy_task(std::string name);
	bool destroy_task(int dPID);
	bool yield();
	void rotate(char state);
	bool cleanup();
	bool getDL();
	void block();
	void blockByPID(int bPID);
	bool unblockByPID(int ubPID);
	void wait_turn(scheduler *sched_ptr, thread *thread_ptr, bool blocked = false);
	time_t getTime();
	std::string dump();
	~scheduler();

	friend class semaphore;
	//friend class ipc;
};

#endif
