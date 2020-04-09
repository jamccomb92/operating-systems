#ifndef THREAD_H
#define THREAD_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <ncurses.h>
#include "queue.h"
#include "semaphore.h"
#include "mailbox.h"


struct thread{
		int PID;
		std::string name;
		thread* next;
		char state;	// R - running, W - waiting(ready), B - blocked, D - dead, K - end thread
		pthread_t realThread;
		pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
		WINDOW *window;
		mailbox *my_mailbox;
		
		void print(){
			std::cout<<PID<<": "<<name<<" ["<<state<<"] - "<< sleep<<std::endl;
		}
		
		~thread(){}
};

#endif

