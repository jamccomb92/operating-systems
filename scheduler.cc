#include "scheduler.h"
#include "semaphore.h"
#include "thread.h"
// #include "tcb.h"

#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <ncurses.h>

using namespace std;

void *thread_function(void *void_ptr){
	scheduler* sched_ptr = (scheduler *) void_ptr;
	thread* thread_ptr = sched_ptr->process_table->getHead()->next;
	////cout<<"nextPID is: "<<sched_ptr->nextPID<<endl;
	
	//pthread_cond_wait(&thread_ptr->cond, &sched_ptr->proc_mutex);
	
	int i = 0;
	char buff[256];
	while(true){
		sched_ptr->io->down(thread_ptr->PID);
		
		
		sprintf(buff, " Task %d running task #%d\n", thread_ptr->PID, i);
		wprintw(thread_ptr->my_window, buff);
		box(thread_ptr->my_window, 0,0);
		wrefresh(thread_ptr->my_window);

		
		sched_ptr->io->up(thread_ptr->PID);
		//sleep(1);
		i++;
    }
}

void *lazy_thread_function(void *void_ptr){
	scheduler* sched_ptr = (scheduler *) void_ptr;
	thread* thread_ptr = sched_ptr->process_table->getHead()->next;
	////cout<<"nextPID is: "<<sched_ptr->nextPID<<endl;
	
	//pthread_cond_wait(&thread_ptr->cond, &sched_ptr->proc_mutex);
	
	int i = 0;
	char buff[256];
	while(true){
		pthread_cond_wait(&sched_ptr->process_table->getHead()->cond, &sched_ptr->proc_mutex);
		/*sprintf(buff, " Task %d running task #%d\n", thread_ptr->PID, i);
		wprintw(thread_ptr->my_window, buff);
		box(thread_ptr->my_window, 0,0);
		wrefresh(thread_ptr->my_window);*/

		sleep(1);
		i++;
    }
}

scheduler::scheduler(){
    process_table = new TCB;
    nextPID = 0;
    clean = true;
    deadlock = true;
    quantum = 3;
    startTime = 0;	
	proc_mutex = procMutex();
    io = new semaphore("io", this);
	pthread_mutex_lock(&proc_mutex);
}

pthread_mutex_t scheduler::procMutex(){
    pthread_mutex_t proc_mutex = PTHREAD_MUTEX_INITIALIZER;
    return proc_mutex;
}

void scheduler::create_task(string name){
    struct thread* newThread = new thread();
    newThread->PID = nextPID;
    newThread->name = name;
    newThread->next = nullptr;
    newThread->state = 'W';
    newThread->sleep = false; 
	
	newThread->cond = PTHREAD_COND_INITIALIZER;
	////cout<<"Error code for cond wait: "<<pthread_cond_wait(&newThread->cond, &proc_mutex);//sched_ptr->proc_mutex)<<endl;
    process_table->insert(newThread);
	//int resultCode = pthread_create(newThread, NULL, scheduler::thread_function, this);
    int resultCode = pthread_create(&process_table->getHead()->next->realThread, NULL, thread_function, this);
    if(resultCode != 0){
        //cout << "Error creating thread " << name << endl;
    }
    else{
        nextPID++;
        if(deadlock){
            rotate(process_table->getState());
            deadlock = false;
        }
    }
}

void scheduler::create_lazy_task(string name){
    struct thread* newThread = new thread();
    newThread->PID = nextPID;
    newThread->name = name;
    newThread->next = nullptr;
    newThread->state = 'W';
    newThread->sleep = false; 
	
	newThread->cond = PTHREAD_COND_INITIALIZER;
	////cout<<"Error code for cond wait: "<<pthread_cond_wait(&newThread->cond, &proc_mutex);//sched_ptr->proc_mutex)<<endl;
    process_table->insert(newThread);
	//int resultCode = pthread_create(newThread, NULL, scheduler::lazy_thread_function, this);
    int resultCode = pthread_create(&process_table->getHead()->next->realThread, NULL, lazy_thread_function, this);
    if(resultCode != 0){
        //cout << "Error creating thread " << name << endl;
    }
    else{
        nextPID++;
        if(deadlock){
            rotate(process_table->getState());
            deadlock = false;
        }
    }
}

/*void *scheduler::thread_function(void *void_ptr){
	scheduler* sched_ptr = (scheduler *) void_ptr;
	thread* thread_ptr = sched_ptr->process_table->getHead()->next;
	
	pthread_cond_wait(&thread_ptr->cond, &sched_ptr->proc_mutex);
	
	
	while(true){
		sched_ptr->io->down();
		////cout<<"Thread "<<thread_ptr->PID<<" is running\n";
		sleep(5);
		sched_ptr->io->up();
    }
}*/

void scheduler::destroy_task(int dPID){
	cout<<"destroying: "<<dPID<<endl;
	thread *temp = process_table->getHead();
    for(int i = 0; i < process_table->getSize(); i++){
        if(temp->PID == dPID){
            temp->state = 'D';
			cout<<"destroyed: "<<dPID<<endl;
            clean = false;
        }
        temp = temp->next;
    }
    
    //if(process_table->getState() == 'D'){
    //    rotate('D');
    //}
}
		
bool scheduler::yield(){
	bool stop;
	
	
    if(process_table->getState() != 'R'){
        rotate(process_table->getState());
        startTime = time(NULL);
		stop = false;
		
    }
    else if(time(NULL) - startTime > quantum){
        rotate('W');
        startTime = time(NULL);
		stop = false;
    }
    else{
		////cout<<"yielding"<<endl;
        pthread_cond_signal(&process_table->getHead()->cond);
        pthread_mutex_unlock(&proc_mutex);
        sleep(1);
        pthread_mutex_lock(&proc_mutex);
		stop = true;
    }
	
	return stop;
}

void scheduler::rotate(char state){
    process_table->setState(state);
    //pthread_kill(process_table->getHead()->realThread, SIGSTOP);			
    
    int i = 0;
    do{
        process_table->cycle();
        i++;
    }while(process_table->getState() != 'W' && i < process_table->getSize());
    
    if(process_table->getState() == 'W'){
		//////cout<<"set process to R"<<endl;
		//process_table->dump();
        process_table->setState('R');
		//process_table->dump();
        //pthread_kill(process_table->getHead()->realThread, SIGCONT);
    }
    else{
        deadlock = true;
    }
}

void scheduler::cleanup(){
    if(!clean){
        for(int i = 0; i < process_table->getSize(); i++){
            if(process_table->getState() == 'D'){
                //pthread_kill(process_table->getHead()->realThread, SIGKILL);
                process_table->remove();
                i--;
            }
            else{
                process_table->cycle();
            }
        }
        clean = true;
    }
}

bool scheduler::getDL(){
    return deadlock;
}

void scheduler::block(){
    process_table->setState('B');
}

void scheduler::blockPID(int bPID){
	thread* temp = process_table->getHead();
    for(int i = 0; i < process_table->getSize(); i++){
        if(temp->PID == bPID){
            temp->state = 'B';
        }
        temp = temp->next;
    }
}

bool scheduler::unblockPID(int ubPID){
	thread* temp = process_table->getHead();
    bool exists = false;
    for(int i = 0; i < process_table->getSize(); i++){
        if(temp->PID == ubPID && temp->state == 'B'){
            temp->state = 'W';
            exists = true;
        }
        temp = temp->next;
    }
    return exists;
}

string scheduler::dump(){
    string tcbDump = process_table->dump();

    //cout<<endl;
    //cout<<"Next PID will be: "<<nextPID<<endl;
    if(clean){
        //cout<<"Process table has no dead tasks"<<endl;
    }
    else{
        //cout<<"Process table has dead tasks"<<endl;
    }
    if(deadlock){
        //cout<<"There are no valid processes to run. The OS is in deadlock."<<endl;
    }

    return tcbDump;
}	

scheduler::~scheduler(){
    delete process_table;
}
