#include "tcb.h"
#include "thread.h"

#include <iostream>
#include <string>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

TCB::TCB(){
    head = nullptr;
    tail = nullptr;
	first = nullptr;
	last = nullptr;
    size = 0;
}

/************************ insert ****************************
Input: newThread
Output: inserting a new thread
*************************************************************/

void TCB::insert(thread* newThread){
    if(size > 1){
        newThread->next = first;
        last->next = newThread;
		last = newThread;
    }
	else if(size == 1){
        newThread->next = first;
        head->next = newThread;
		tail = newThread;
		last = newThread;
	}
	
    /*if(size > 0){
        newThread->next = head->next;
        head->next = newThread;
    }*/
    else{
        newThread->state = 'W';
        head = newThread;
        tail = newThread;
		first = newThread;
		last = newThread;
        newThread->next = newThread;
    }
    
    size++;
}

/************************ remove ****************************
Input: none
Output: deletes the dead task
*************************************************************/

void TCB::remove(){
	if(size > 0){
		if(head == first){
			first = first->next;
		}
		if(head == last){
			thread *temp = head;
			while(temp->next != head){
				temp = temp->next;
			}
			last = temp;
		}
		tail->next = head->next;
		delete head;
		size--;
		if(size > 0){	
			head = tail->next;
		}
		else{
			head = nullptr;
			tail = nullptr;
			first = nullptr;
			last = nullptr;
		}
	}
    else{
        head = nullptr;
        tail = nullptr;
		first = nullptr;
		last = nullptr;
    }
}

thread * TCB::getThreadByPID(int gtPID){
	thread *temp = head;
    for(int i = 0; i < size; i++){
        if(temp->PID == gtPID){
            return temp;
        }
        temp = temp->next;
    }
}

pthread_t* TCB::getHeadRealThread(){
	if(head){
		return &head->realThread;
	}
}

pthread_t* TCB::getRealThreadByPID(int grtPID){
	thread *temp = head;
    for(int i = 0; i < size; i++){
        if(temp->PID == grtPID){
            return &temp->realThread;
        }
        temp = temp->next;
    }
}
    
/*	void TCB::killProcess(){
    head->state = 'D';
}
*/
/*	void TCB::killPID(int kPID){
    for(int i = 0; i < size; i++){
        if(head->PID == kPID){
            killProcess();
        }
        cycle();
    }
}
*/

/************************** cycle ***************************
Input: none
Output: 
*************************************************************/

void TCB::cycle(){
    if(size > 0){
        tail = head;
        head = head->next;
    }
}

/************************** setHeadState *************************
Input: newState
Output: 
*************************************************************/

void TCB::setHeadState(char newState){
    if(head){
        head->state = newState;
    }
}

/************************** getHeadState *************************
Input: none
Output: returns the state of the current task
*************************************************************/

char TCB::getHeadState(){
    if(head){
        return head->state;
    }
}

/************************** getCondByPID ***********************
Input: gcPID
Output: 
*************************************************************/

pthread_cond_t *TCB::getCondByPID(int gcPID){
	thread *temp = head;
    for(int i = 0; i < size; i++){
        if(temp->PID == gcPID){
            return &temp->cond;
        }
        temp = temp->next;
    }
}

/************************** getHeadPID( ***************************
Input: none
Output: returns the PID of the current task
*************************************************************/

int TCB::getHeadPID(){
	if(head){
		return head->PID;
	}
}

char TCB::getStateByPID(int gsPID){
	thread *temp = head;
    for(int i = 0; i < size; i++){
        if(temp->PID == gsPID){
            return temp->state;
        }
        temp = temp->next;
    }
}

void TCB::setStateByPID(int ssPID, char newState){
	thread *temp = head;
    for(int i = 0; i < size; i++){
        if(temp->PID == ssPID){
            temp->state = newState;
			return;
        }
        temp = temp->next;
    }
}

string TCB::getHeadName(){
	if(head){
		return head->name;
	}
}

void TCB::setHeadName(string sName){
	if(head){
		head->name = sName;
	}
}

string TCB::getNameByPID(int gnPID){
	thread *temp = head;
    for(int i = 0; i < size; i++){
        if(temp->PID == gnPID){
			return temp->name;
        }
        temp = temp->next;
    }
}
		
void TCB::setNameByPID(int snPID, string sName){
	thread *temp = head;
    for(int i = 0; i < size; i++){
        if(temp->PID == snPID){
			temp->name = sName;
			return;
        }
        temp = temp->next;
    }
}

thread* TCB::getHeadNext(){
	if(head){
		return head->next;
	}
}

void TCB::setHeadNext(thread* sNext){
	if(head){
		head->next = sNext;
	}
}

thread* TCB::getNextByPID(int gnPID){
	thread *temp = head;
    for(int i = 0; i < size; i++){
        if(temp->PID == gnPID){
			return temp->next;
        }
        temp = temp->next;
    }
}
		
void TCB::setNextByPID(int snPID, thread* sNext){
	thread *temp = head;
    for(int i = 0; i < size; i++){
        if(temp->PID == snPID){
			temp->next = sNext;
			return;
        }
        temp = temp->next;
    }
}

WINDOW* TCB::getHeadWindow(){
	if(head){
		return head->window;
	}
}

void TCB::setHeadWindow(WINDOW* sWindow){
	if(head){
		head->window = sWindow;
	}
}

WINDOW* TCB::getWindowByPID(int gwPID){
	thread *temp = head;
    for(int i = 0; i < size; i++){
        if(temp->PID == gwPID){
			return temp->window;
        }
        temp = temp->next;
    }
}
		
void TCB::setWindowByPID(int swPID, WINDOW* sWindow){
	thread *temp = head;
    for(int i = 0; i < size; i++){
        if(temp->PID == swPID){
			temp->window = sWindow;
			return;
        }
        temp = temp->next;
    }
}

/************************ getSize ***************************
Input: none
Output: returns the size of the TCB
*************************************************************/

int TCB::getSize(){
    return size;
}

/************************ getHead ***************************
Input: none
Output: returns the head of the TCB
*************************************************************/

thread* TCB::getHead(){
	if(head){
		return head;
	}
}

thread* TCB::getTail(){
	if(tail){
		return tail;
	}
}

thread* TCB::getFirst(){
	if(first){
		return first;
	}
}

thread* TCB::getLast(){
	if(last){
		return last;
	}
}

/********************** dump ********************************
Input: none
Output: dump the contents of the tcp in a readable format
*************************************************************/

string TCB::dump(){
    struct thread *temp = first;
    string dumpString = "\n -------- TCB Table Dump --------\n";
    dumpString += " Key: R - Running, W - Ready, B - Blocked, D - Dead\n";
	if(size == 0){
		dumpString += " The process table is empty.\n";
	}
    for(int i = 0; i < size; i++){
        //dumpString += " Name: " + temp->name + " -- ";
        dumpString += "ID: " + to_string(temp->PID) + " -- ";
        dumpString += "State: ";
        dumpString.push_back(temp->state);
		dumpString += " -- Name: " + temp->name;
        dumpString += "\n";
        temp=temp->next;
    }

    return dumpString;
} 
   

/************* ~TCB Destructor **********************
Input: NULL
Output: removes the head from the TCB
*****************************************************/

TCB::~TCB(){
    while(head){
        remove();
    }
    /*if(head){
        delete head;
    }*/
}
