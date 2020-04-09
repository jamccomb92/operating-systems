#ifndef THREAD_H
#define THREAD_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <ncurses.h>


struct thread{
		int PID;
		std::string name;
		thread* next;
		char state;	// R - running, W - waiting(ready), B - blocked, D - dead
		bool sleep;
		pthread_t realThread;
		pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
		WINDOW *my_window;
		
		void print(){
			std::cout<<PID<<": "<<name<<" ["<<state<<"] - "<< sleep<<std::endl;
		}
		
		~thread(){}
};

#endif

