#include "semaphore.h"
#include "scheduler.h"
// #include "queue.h"


#include <iostream>
#include <string>
#include <stdlib.h>
// #include <queue>
#include <pthread.h> // Needed for using the pthread library
#include <unistd.h>  // Needed for sleep()
#include <ncurses.h> // Needed for Curses windowing
#include <stdarg.h>  // needed for formatted output to window

using namespace std;


semaphore::semaphore(string sName, scheduler* theScheduler){
    sema_value = 1; 
    resource_name = sName;
    sched_ptr = theScheduler;
    lucky_task = -1;
    sema_queue = new Queue<int>;
	mutex = createMutex();
}

pthread_mutex_t semaphore::createMutex(){
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    return mutex;
}

void semaphore::lock(){ 
    pthread_mutex_lock(&mutex); 
}

void semaphore::unlock(){ 
    pthread_mutex_unlock(&mutex);
}

void semaphore::down(int taskID){
    if(taskID == lucky_task){
        //cout << "Task #" << taskID << " already has the resource.";
        dump();
    }
    else {
        if(sema_value == 1){
            lock();
            sema_value--;
            // sema_queue->push(taskID); //first place in queue is used by the resource
            //unlock();
            lucky_task = taskID;
            //dump();
			//cout<<"Sema 1 down for: "<<taskID<<endl;
        }
        else{
            //lock();
			if(sched_ptr->process_table->getPIDState(taskID) != 'D'){
				sema_queue->Enqueue(taskID);
				sched_ptr->blockPID(taskID); // block the current task and yield (see block())
			}
            lock();
            pthread_cond_wait(sched_ptr->process_table->getPIDCond(taskID), &mutex);
            //dump();
			//cout<<"Sema not 1 down"<<endl;
        }


    }
	//pthread_cond_t trycond= PTHREAD_COND_INITIALIZER;
	////cout<<EINVAL<<endl;
	////cout<<sched_ptr->proc_mutex<<endl;
	////cout<<"Lock error: "<< pthread_mutex_lock(&sched_ptr->proc_mutex)<<endl;
	////cout<<"nextPID is: "<<sched_ptr->nextPID<<endl;
	//sched_ptr->process_table->getPIDCond(taskID)
	////cout<<"Error code for cond wait: "<<pthread_cond_wait(sched_ptr->process_table->getPIDCond(taskID), &sched_ptr->proc_mutex)<<endl;
	pthread_cond_wait(sched_ptr->process_table->getPIDCond(taskID), &sched_ptr->proc_mutex);
    ////cout<<"done";
	////cout<<"Error code for cond wait: "<<pthread_cond_wait(&sched_ptr->process_table->getHead()->cond, &sched_ptr->proc_mutex)<<endl;

}

void semaphore::up(int taskID){
    

    //cout << "TaskID: " << sched_ptr->process_table->getPID() << ", Current using " 
    //   << resource_name << ": " << lucky_task << endl;

    if(taskID == lucky_task){
        // current one using the resource is in the queue
        // also basically isempty()
		
		while(!sema_queue->isEmpty() && !sched_ptr->unblockPID(sema_queue->Front())){
			sema_queue->Dequeue();	
		}
        
        if(sema_queue->isEmpty()){
            sema_value++;
            lucky_task = -1;
            unlock();
            //dump();
			//cout<<"Sema up empty"<<endl;
        }
        
        else{
            //assign new one to resource
            taskID = sema_queue->Front();
            
            // get FROM JUSTIN!!!!!!!!!
            // while(!sema_queue->isEmpty() && !sched_ptr->unblockPID(taskID)){
            //     sema_queue->pop();
            // }
            //sched_ptr->unblockPID(taskID);
            //cout << "Unblocked: " << taskID << " and move to front of queue." << endl;
            //sched_ptr->dump();
			// //cout << "Front of queue using resource: " << sema_queue->front() << endl;
            sema_queue->Dequeue();
            lucky_task = taskID;
			//cout<<"Sema up give to: "<< taskID <<endl;
            pthread_cond_t *temp = sched_ptr->process_table->getPIDCond(taskID);
            pthread_cond_signal(temp);
            pthread_mutex_unlock(&sched_ptr->proc_mutex);
			unlock();
            //dump();
        }
    }
    else{
        //cout << "Invalid Semaphore UP(). Task ID: " << sched_ptr->process_table->getPID() 
        //    << " does not own the resource " << resource_name << endl;
    }

}


string semaphore::dump(){
    //cout << "Semaphore: "<<resource_name<<endl;
    //cout << "Semaphore Value: "<<sema_value<<endl;
    //cout << "Current task using resource: "<< lucky_task <<endl;
    //cout << "Semaphore queue: \n";
	
	

	string returnString = "\n ---- Semaphore Dump ----\n";
	Queue<int> * copyQueue = new Queue<int>();
	
	returnString += " Semaphore Name: " + resource_name + "\n";
	returnString += " Semaphore Value: " + to_string(sema_value) + "\n";
	if(lucky_task != -1){
		returnString += " Current task using resource: " + to_string(lucky_task) + "\n";
		if(!sema_queue->isEmpty()){
			returnString += " Sie of queue: " + to_string(sema_queue->Size()) + "\n Semaphore Queue: ";
			while(!sema_queue->isEmpty()){
				returnString += " PID: " + to_string(sema_queue->Front());
				copyQueue->Enqueue(sema_queue->Front());
				sema_queue->Dequeue();
			}
        }
	}
	else{
		returnString += " No task currently using resource\n Queue is currently empty.";
	}
	
	sema_queue = copyQueue;
	
	returnString += "\n";
	
	return returnString;
	
	
	/*
    // queue<TCB::task*> copyQueue;
    Queue<int>* copyQueue;
    if(!sema_queue->isEmpty()){
        //cout << "Size of queue: "<<sema_queue->Size()<<endl;
		returnString += " 
        while(!sema_queue->isEmpty()){
            //cout << "PID: "<< sema_queue->Front()<<endl;
            copyQueue->Enqueue(sema_queue->Front());
            sema_queue->Dequeue();
        }
    // if(!sema_queue->empty()){
    //     //cout << "Size of queue: "<<sema_queue->size()<<endl;
    //     while(!sema_queue->empty()){
    //         //cout << "PID: "<< sema_queue->front()<<endl;
    //         copyQueue->push(sema_queue->front());
    //         sema_queue->pop();
    //     }
    
        sema_queue = copyQueue;
    }
    else{
        //cout << "No process waiting for semaphore" << endl;
    }
	*/
}
