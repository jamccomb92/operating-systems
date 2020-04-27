#ifndef TCB_H
#define TCB_H

#include <string>
#include <stdlib.h>
#include <pthread.h>

#include "thread.h"

class TCB
{
private:
	thread *head;
	thread *tail;
	thread *first;
	thread *last;
	int size;

public:
	TCB();
	void insert(thread *newThread);
	void remove();

	void cycle();
	void setHeadState(char newState);
	char getHeadState();

	pthread_cond_t *getCondByPID(int gcPID);

	int getHeadPID();

	thread *getThreadByPID(int gtPID);

	char getStateByPID(int gsPID);
	void setStateByPID(int ssPID, char newState);

	std::string getHeadName();
	void setHeadName(std::string sName);
	std::string getNameByPID(int gnPID);
	void setNameByPID(int snPID, std::string sName);

	thread *getHeadNext();
	void setHeadNext(thread *sNext);
	thread *getNextByPID(int gnPID);
	void setNextByPID(int snPID, thread *sNext);

	pthread_t *getHeadRealThread();
	pthread_t *getRealThreadByPID(int grtPID);

	WINDOW *getHeadWindow();
	void setHeadWindow(WINDOW *sWindow);
	WINDOW *getWindowByPID(int gwPID);
	void setWindowByPID(int swPID, WINDOW *sWindow);

	int getSize();
	thread *getHead();
	thread *getTail();
	thread *getFirst();
	thread *getLast();
	std::string dump();
	~TCB();
};

#endif
