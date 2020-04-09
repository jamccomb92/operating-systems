#include "semaphore.h"
#include "scheduler.h"


#include <iostream>
#include <string>
#include <stdlib.h>
#include <pthread.h> // Needed for using the pthread library
#include <unistd.h>  // Needed for sleep()
#include <ncurses.h> // Needed for Curses windowing
#include <stdarg.h>  // needed for formatted output to window

using namespace std;

/***************** semaphore Constructor ********************
Input: sName, theScheduler
Output: 
*************************************************************/

semaphore::semaphore(string sName, scheduler* theScheduler){
    sema_value = 1; 
    resource_name = sName;
    sched_ptr = theScheduler;
    lucky_task = -1;
    sema_queue = new Queue<int>;
	mutex = PTHREAD_MUTEX_INITIALIZER;
}

/************************* lock ******************************
Input: none
Output: mutex lock
*************************************************************/

void semaphore::lock(){ 
    pthread_mutex_lock(&mutex); 
}

/************************* unlock ****************************
Input: none
Output: mutex unlock
*************************************************************/

void semaphore::unlock(){ 
    pthread_mutex_unlock(&mutex);
}

/************************ down ******************************
Input: none
Output: locking the resource
*************************************************************/

bool semaphore::down(int taskID){
	lock();
    if(taskID == lucky_task){
        //cout << "Task #" << taskID << " already has the resource.";
        dump();
    }
    else{
        if(sema_value == 1){
            //lock();
            sema_value--;
            lucky_task = taskID;
			unlock();
			return false;
        }
        else{
			if(sched_ptr->getProcessTable()->getStateByPID(taskID) != 'D'){	//dead tasks cannot ask for resources
				sema_queue->Enqueue(taskID);							//get in line for resource
				sched_ptr->blockByPID(taskID);							// block the current task
			}
            //lock();
			/*pthread_mutex_unlock(&sched_ptr->proc_mutex);
            pthread_cond_wait(sched_ptr->getProcessTable()->getCondByPID(taskID), &mutex);			//wait until resource is given to us by an up call from another thread using it
			pthread_mutex_lock(&sched_ptr->proc_mutex);*/
			unlock();
			return true;
        }


    }
	
	//pthread_mutex_lock(&sched_ptr->proc_mutex);
	//pthread_cond_wait(sched_ptr->getProcessTable()->getCondByPID(taskID), &sched_ptr->proc_mutex);	//wait for go-ahead from scheduler
	//unlock();
}

/************************* up *******************************
Input: none
Output: releasing the resource
*************************************************************/

void semaphore::up(int taskID){
	lock();
    if(taskID == lucky_task){		//only the task using the resource can release it
		
		while(!sema_queue->isEmpty() && !sched_ptr->unblockByPID(sema_queue->Front())){	//dequeue PIDs until either the queue is empty or the next in line can be unblock without errors
			sema_queue->Dequeue();	
		}
        
        if(sema_queue->isEmpty()){
            sema_value++;
            lucky_task = -1;
            //unlock();
        }
        
        else{
            //assign new one to resource
            taskID = sema_queue->Front();
            
            sema_queue->Dequeue();
            lucky_task = taskID;
			//unlock();
            pthread_cond_t *temp = sched_ptr->getProcessTable()->getCondByPID(taskID);	//signal the next task waiting the resource is availible
            pthread_cond_signal(temp);												
			
            pthread_mutex_unlock(&sched_ptr->proc_mutex);							//tell scheduler this is a good stopping point, scheduler runs yield
			//unlock();
        }
    }
    else{
        //cout << "Invalid Semaphore UP(). Task ID: " << sched_ptr->getProcessTable()->getHeadPID(() 
        //    << " does not own the resource " << resource_name << endl;
    }
	unlock();
}

/************************ dump ******************************
Input: none
Output: dumps the contents of the semaphore
*************************************************************/

string semaphore::dump(){
	string returnString = "\n ---- Semaphore Dump ----\n";
	Queue<int> * copyQueue = new Queue<int>();
	
	returnString += " Semaphore Name: " + resource_name + "\n";
	returnString += " Semaphore Value: " + to_string(sema_value) + "\n";
	if(lucky_task != -1){
		returnString += " Current task using resource: " + to_string(lucky_task) + "\n";
		if(!sema_queue->isEmpty()){
			returnString += " Size of queue: " + to_string(sema_queue->Size()) + "\n Semaphore Queue: ";
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
	
    delete sema_queue;
	sema_queue = copyQueue;
    
	
	returnString += "\n";
	
	return returnString;
}
