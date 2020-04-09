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
    size = 0;
    index = 0;
}

void TCB::insert(thread* newThread){
    // struct thread* newThread = new thread();
    // newThread->PID = nextPID;
    // newThread->name = tname;
    // newThread->next = nullptr;
    // newThread->state = 'W';
    // newThread->sleep = false; 

    // newThread->cond = PTHREAD_COND_INITIALIZER;
    
    if(size > 0){
        //head->state = 'W';
        
        /*newThread->next = head;
        tail->next = newThread;
        tail = newThread;*/
        
        newThread->next = head->next;
        head->next = newThread;
    }
    else{
        newThread->state = 'R';
        head = newThread;
        tail = newThread;
        newThread->next = newThread;
    }
    
    //int resultCode = pthread_create(&newThread->realThread, NULL, my_thread_function, this);
    //pthread_kill(newThread->realThread, SIGSTOP);
    
    size++;
}

void TCB::remove(){
	//pthread_exit(head->realThread);
    tail->next = head->next;
    delete head;
    size--;
    if(size > 0){
        head = tail->next;
    }
    else{
        head = nullptr;
        tail = nullptr;
    }
}

thread * TCB::getbyPID(int gbPID){
	thread *temp = head;
    for(int i = 0; i < size; i++){
        if(temp->PID == gbPID){
            return temp;
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
/*	void TCB::blockProcess(){
    head->state = 'B';
}
*/	

void TCB::cycle(){
    if(size > 0){
        tail = head;
        head = head->next;
        index++;
    }
}

void TCB::setState(char newState){
    if(head){
        head->state = newState;
    }
}

char TCB::getState(){
    if(head){
        return head->state;
    }
}

pthread_cond_t *TCB::getPIDCond(int gcPID){
	thread *temp = head;
    for(int i = 0; i < size; i++){
        if(temp->PID == gcPID){
            return &temp->cond;
        }
        temp = temp->next;
    }
}

int TCB::getPID(){
    return head->PID;
}

char TCB::getPIDState(int gsPID){
	thread *temp = head;
    for(int i = 0; i < size; i++){
        if(temp->PID == gsPID){
            return temp->state;
        }
        temp = temp->next;
    }
}

/*void TCB::setPIDState(int ssPID, char newState){
    for(int i = 0; i < size; i++){
        if(head->PID == ssPID){
            head->state = newState;
        }
        cycle();
    }
}*/

int TCB::getSize(){
    return size;
}

thread* TCB::getHead(){
    return head;
}

string TCB::dump(){
    //system("clear");
    //cout<<"Current thread - ";
    struct thread *temp = head;
    string dumpString = "\n -------- TCB Table Dump --------\n";
    dumpString += " Key: R - Running, W - Ready, B - Blocked, D - Dead\n";

    for(int i = 0; i < size; i++){
        //head->print();
        dumpString += " Name: " + temp->name + " -- ";
        dumpString += "ID: " + to_string(temp->PID) + " -- ";
        dumpString += "State: ";
        dumpString.push_back(temp->state);
        dumpString += "\n";
        temp=temp->next;
    }

    return dumpString;
}    

TCB::~TCB(){
    while(head != tail && head && tail){
        remove();
    }
    if(head){
        delete head;
    }
}
