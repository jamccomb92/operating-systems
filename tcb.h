#ifndef TCB_H
#define TCB_H

#include <string>
#include <stdlib.h>
#include <pthread.h>

#include "thread.h"

class TCB {
    private:
        thread* head;
		thread* tail;
		int size;
		int index;
    public:
        TCB();
        void insert(thread* newThread);
        void remove();
        // void killProcess();
        // void killPID(int kPID);
        // void blockProcess();
        void cycle();
        void setState(char newState);
        char getState();
        pthread_cond_t *getPIDCond(int gcPID);
        int getPID();
		thread *getbyPID(int gbPID);
        char getPIDState(int gsPID);
        // void setPIDState(int ssPID, char newState);
        int getSize();
        thread* getHead();
        std::string dump();
        ~TCB();

};

#endif
