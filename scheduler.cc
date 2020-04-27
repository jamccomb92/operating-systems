/**********************************************************
Author       : Joe McCombs, Justin Doney, Kamil Kosidlak
Class        : C435, Spring 2020
File Name    : scheduler.cc
Last Updated :
Description  : Used to manipulate and control the processes
               running in our environment.
**********************************************************/

#include "scheduler.h"
#include "semaphore.h"
#include "thread.h"
#include "message.h"
#include "UltimaWindows.h"
#include "message.h"
#include "ipc.h"
#include "memoryManager.h"

#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

class memoryManager;

using namespace std;

void scheduler::wait_turn(scheduler *sched_ptr, thread *thread_ptr, bool blocked)
{
    pthread_cond_wait(&thread_ptr->cond, &sched_ptr->proc_mutex);
    if (blocked)
    {
        pthread_cond_wait(&thread_ptr->cond, &sched_ptr->proc_mutex);
    }
    if (thread_ptr->state == 'K')
    {
        pthread_mutex_unlock(&sched_ptr->proc_mutex);
        pthread_exit(NULL);
    }
}

time_t scheduler::getTime()
{
    return time(NULL);
}

void *thread_function(void *void_ptr)
{
    scheduler *sched_ptr = (scheduler *)void_ptr;
    thread *thread_ptr = sched_ptr->getProcessTable()->getLast();

    int i = 0;
    char buff[256];
    string print_message;
    string mail_count;
    string total_count;
    string read_string_s;
    string print_inbox;
    int totalNewlineRewind = 0;
    int nextNewlineRewind = 0;
    int receiveCode = -1;
    int serviceMessageLength = 0;
    bool write_toggle = true;
    int write_to_handle;
    char read_string[256];
    char *write_string;
    char rbuff[1024];
    int read_success, offset = 0, locat_single_read;
    bool alloc_toggle = false;

    pthread_mutex_lock(&sched_ptr->proc_mutex);
    sched_ptr->wait_turn(sched_ptr, thread_ptr);
    sched_ptr->wait_turn(sched_ptr, thread_ptr, sched_ptr->io->down(thread_ptr->PID));
    if (thread_ptr->PID == 0)
    { //multichar
        write_to_handle = sched_ptr->memory_manager->Mem_Allocate(275, thread_ptr);
        write_string = "This is thread 0's memory allocation. I enjoy writing to this Memory. No other thread";
        sched_ptr->memory_manager->Mem_Write(write_to_handle, 0, strlen(write_string), write_string, thread_ptr);
        sched_ptr->sched_windows.write_window(thread_ptr->window, " Allocating memory.\n");
        sched_ptr->sched_windows.write_window(thread_ptr->window, " Writing to memory.\n");
    }
    else if (thread_ptr->PID == 1)
    { //single char
        write_to_handle = sched_ptr->memory_manager->Mem_Allocate(2, thread_ptr);
        write_string = "Thr";
        sched_ptr->memory_manager->Mem_Write(write_to_handle, &write_string[offset], thread_ptr);
        offset++;
        sched_ptr->sched_windows.write_window(thread_ptr->window, " Writing to memory.\n");
    }
    else if (thread_ptr->PID == 2)
    { //free then alloc
        write_to_handle = sched_ptr->memory_manager->Mem_Allocate(130, thread_ptr);
        write_string = "This is thread 2's memory allocation. I enjoy writing to this Memory. No other thread can.";
        sched_ptr->memory_manager->Mem_Write(write_to_handle, 0, strlen(write_string), write_string, thread_ptr);
        sched_ptr->sched_windows.write_window(thread_ptr->window, " Allocating memory.\n");
    }
    sched_ptr->io->up(thread_ptr->PID);

    string filename;
    int filesize;
    string fs_string;
    int pos = 0;
    if (thread_ptr->PID == 0)
    {
        filename = "T0file1";
        fs_string = "0file0123456789";
        filesize = 64;
    }
    else if (thread_ptr->PID == 1)
    {
        filename = "T1file1";
        fs_string = "1file0123456789";
        filesize = 3;
    }
    else if (thread_ptr->PID == 2)
    {
        filename = "T2file1";
        fs_string = "2file0123456789";
        filesize = 512;
    }

    sched_ptr->sched_windows.write_window(thread_ptr->window, " Creating file\n");
    int file_handle = sched_ptr->file_system->Create_File(thread_ptr->PID, filename, filesize, bitset<4>(0xf), 'B');
    int status;

    /*
	//open file without permissions
	int file_handle = sched_ptr->file_system->Create_File(thread_ptr->PID, filename, filesize, bitset<4>(0x3), 'B');
	sched_ptr->file_system->Close(thread_ptr->PID, file_handle);
	
	// task 1 attempts to open file belonging to task 0 when permissions are 1100
	if(thread_ptr->PID == 1){
		if(sched_ptr->file_system->Open(thread_ptr->PID, 0,"T0file1", 'B') < 0){
			sched_ptr->sched_windows.write_window(thread_ptr->window, " Open Unsuccessful\n");
		}
	}*/

    // bool change_perm_toggle = true;

    while (true)
    {
        /*
		// Changing the permissions 
		if(thread_ptr->PID == 0 && change_perm_toggle) {
			sched_ptr->sched_windows.write_window(thread_ptr->window, " Changing Permissions to ----\n");
			sched_ptr->file_system->Close(thread_ptr->PID, file_handle);
			sched_ptr->file_system->Change_Permissions(thread_ptr->PID, filename, bitset<4>(0x0));
			sched_ptr->file_system->Open(thread_ptr->PID, file_handle,filename, 'B');
		}
		else{
			sched_ptr->sched_windows.write_window(thread_ptr->window, " Changing Permissions to rwrw\n");
			sched_ptr->file_system->Close(thread_ptr->PID, file_handle);
			sched_ptr->file_system->Change_Permissions(thread_ptr->PID, filename, bitset<4>(0xf));
			sched_ptr->file_system->Open(thread_ptr->PID, file_handle,filename, 'B');
		}
		change_perm_toggle = !change_perm_toggle;
		*/

        /*
		//create files until out of disk space and format testing
		if(thread_ptr->PID == 2){
			
			// Formatting the memory so that i always get memory
			sched_ptr->file_system->format();
			sched_ptr->sched_windows.write_window(thread_ptr->window, " Format the file system\n");
			
			//used to create until out of disk space
			filename[6]++;
			sched_ptr->sched_windows.write_window(thread_ptr->window, " Creating file\n");
			if(sched_ptr->file_system->Create_File(thread_ptr->PID, filename, filesize, bitset<4>(0xf), 'B') < 0){
				sched_ptr->sched_windows.write_window(thread_ptr->window, " Create Unsuccessful\n");
			}
		}
		*/

        // write and read characters to/from respective files until out of bounds
        /*sched_ptr->sched_windows.write_window(thread_ptr->window, " Writing to file\n");
		if(sched_ptr->file_system->Write_Char(thread_ptr->PID, file_handle, fs_string[pos++]) < 0){
			sched_ptr->sched_windows.write_window(thread_ptr->window, " Write Unsuccessful\n");
		}
		else{
			rbuff[1] = fs_string[pos-1];
			rbuff[0] = ' ';
			rbuff[2] = '\n';
			rbuff[3] = '\0';
			sched_ptr->sched_windows.write_window(thread_ptr->window, rbuff);
		}
		
		sched_ptr->sched_windows.write_window(thread_ptr->window, " Reading from file\n");
		status = sched_ptr->file_system->Read_Char(thread_ptr->PID, file_handle, rbuff);
		if(status < 0){
			sched_ptr->sched_windows.write_window(thread_ptr->window, " Read Unsuccessful\n");
		}
		else if(status > 0){
				rbuff[1] = rbuff[0];
				rbuff[0] = ' ';
				rbuff[2] = '\n';
				rbuff[3] = '\0';
				sched_ptr->sched_windows.write_window(thread_ptr->window, rbuff);
		}*/

        sched_ptr->wait_turn(sched_ptr, thread_ptr, sched_ptr->io->down(thread_ptr->PID));
        if (thread_ptr->PID == 0)
        {
            read_success = sched_ptr->memory_manager->Mem_Read(write_to_handle, offset, strlen(write_string) / 3, read_string, thread_ptr);
            if (read_success == 0)
            {
                rbuff[0] = ' ';
                for (int i = 0; i < strlen(read_string); i++)
                {
                    rbuff[i + 1] = read_string[i];
                }
                rbuff[strlen(read_string) + 1] = '\n';
                rbuff[strlen(read_string) + 2] = '\0';
                sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.logWin, rbuff);
                offset += strlen(write_string) / 3;
            }
            else
            {
                // seg fault
                offset = 0;
            }
            sched_ptr->sched_windows.write_window(thread_ptr->window, " Reading from memory.\n");
        }
        else if (thread_ptr->PID == 1)
        {
            if (write_toggle)
            {
                sched_ptr->memory_manager->Mem_Write(write_to_handle, &write_string[offset], thread_ptr);
                offset++;
                if (offset >= strlen(write_string))
                {
                    write_toggle = false;
                }
                sched_ptr->sched_windows.write_window(thread_ptr->window, " Writing to memory.\n"); //memory.\n");
                                                                                                    //sched_ptr->sched_windows.write_window(thread_ptr->window, " Writing to memory.\n");
            }
            else
            {
                read_success = sched_ptr->memory_manager->Mem_Read(write_to_handle, read_string, thread_ptr);
                if (read_success == 0)
                {
                    rbuff[0] = ' ';
                    rbuff[1] = read_string[0];
                    rbuff[strlen(read_string) + 1] = '\n';
                    rbuff[strlen(read_string) + 2] = '\0';
                    sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.logWin, rbuff);
                }
                sched_ptr->sched_windows.write_window(thread_ptr->window, " Reading from memory.\n");
            }
        }
        else if (thread_ptr->PID == 2)
        {
            if (alloc_toggle)
            {
                write_to_handle = sched_ptr->memory_manager->Mem_Allocate(130, thread_ptr);
                write_string = "This is thread 2's memory allocation. I enjoy writing to this Memory. No other thread can.";
                sched_ptr->memory_manager->Mem_Write(write_to_handle, 0, strlen(write_string), write_string, thread_ptr);
                sched_ptr->sched_windows.write_window(thread_ptr->window, " Allocating memory.\n");
            }
            else
            {
                if (sched_ptr->memory_manager->Mem_Free(write_to_handle, thread_ptr) == 0)
                {
                    string mem_free = " Task " + to_string(thread_ptr->PID) + " freed handle " + to_string(write_to_handle);
                    for (int i = 0; i < mem_free.length(); i++)
                    {
                        rbuff[i] = mem_free[i];
                    }
                    rbuff[mem_free.length()] = '\n';
                    rbuff[mem_free.length() + 1] = '\0';
                    sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.logWin, rbuff);

                    write_to_handle = -1;
                }
                sched_ptr->sched_windows.write_window(thread_ptr->window, " Freeing memory.\n");
            }
            alloc_toggle = !alloc_toggle;
        }
        sched_ptr->io->up(thread_ptr->PID);

        struct message *new_message = new message();
        struct message *notification_message = new message();
        new_message->Msg_text = "This is the message";
        new_message->Msg_Type.Message_Type_ID = 0;
        new_message->task_running = i;
        new_message->Destination_ID = (thread_ptr->PID + 1) % 3;

        if (thread_ptr->PID == 0 || thread_ptr->PID == 2 || (thread_ptr->PID == 1 && i % 3 != 0))
        {
            new_message->Msg_text = "This is the message";
            new_message->Msg_Type.Message_Type_ID = 0;
            new_message->task_running = i;
            new_message->Destination_ID = (thread_ptr->PID + 1) % 3;
            sched_ptr->sched_ipc->Message_Send(thread_ptr, new_message);
        }
        // send service message every third time
        else if (thread_ptr->PID == 1 && i % 3 == 0)
        {
            new_message->Msg_text = "You can now print my file test.txt";
            new_message->Msg_Type.Message_Type_ID = 1;
            new_message->task_running = i;
            new_message->Destination_ID = (thread_ptr->PID + 1) % 3;
            sched_ptr->sched_ipc->Message_Send(thread_ptr, new_message);
        }

        if (!thread_ptr->my_mailbox->messagesInbox->isEmpty())
        {
            receiveCode = sched_ptr->sched_ipc->Message_Receive(thread_ptr);
            // total_count = sched_ptr->sched_ipc->Message_Count();
            //sched_ptr->wait_turn(sched_ptr, thread_ptr, sched_ptr->io->down(thread_ptr->PID));	 //ask for io resource, waits for go-ahead from scheduler in down function
            //print_message = sched_ptr->sched_ipc->Message_Echo(thread_ptr);
            // print_inbox = sched_ptr->sched_ipc->Message_Print(thread_ptr);
        }

        if (receiveCode == 2)
        {
            notification_message->Msg_text = "Got your service request.";
            notification_message->Msg_Type.Message_Type_ID = 2;
            notification_message->task_running = i;
            notification_message->Destination_ID = thread_ptr->my_mailbox->currentMessage->Source_Task_ID;
            sched_ptr->sched_ipc->Message_Send(thread_ptr, notification_message);
        }

        //dump = sched_ptr->sched_ipc->ipc_dump();
        sched_ptr->wait_turn(sched_ptr, thread_ptr, sched_ptr->io->down(thread_ptr->PID));

        sprintf(buff, " Task %d running task #%d\n", thread_ptr->PID, i);
        sched_ptr->sched_windows.write_window(thread_ptr->window, buff);

        //if(thread_ptr->PID == 1){
        // **DEBUGGING** print message contents for thread used with echo
        //for(int i = 0; i < print_message.length(); i++){
        //     buff[i] = print_message[i];
        //}

        // buff[print_message.length()] = '\0';
        // sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.messageWin, buff);

        // **DEBUGING** count for all mailbox
        //for(int i = 0; i < total_count.length(); i++){
        //    buff[i] = total_count[i];
        //}

        //buff[total_count.length()] = '\0';
        //sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.messageWin, buff);

        // // **DEBUGGING** print entire inbox for PID
        /*for(int iter = 0; iter * 255 - totalNewlineRewind < print_inbox.length(); iter++){
				for(int i = 0; i < print_inbox.length() && i < 255; i++){
					buff[i] = print_inbox[iter * 255 + i - totalNewlineRewind];
					nextNewlineRewind++;
					if(buff[i] == '\n'){
						nextNewlineRewind = 0;
					}
				}
				if(print_inbox.length() - iter * 256 > 0){
					buff[255 - nextNewlineRewind] = '\0';
				}
				else{
					buff[print_inbox.length() - iter * 256 - nextNewlineRewind] = '\0';
				}
				totalNewlineRewind += nextNewlineRewind;
				//buff[print_inbox.length()] = '\0';
				sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.messageWin, buff);
			}
			totalNewlineRewind = 0;
			nextNewlineRewind = 0;*/
        //}

        // **DEBUGGING** count for each mailbox
        //for(int i = 0; i < mail_count.length(); i++){
        //        buff[i] = mail_count[i];
        //}

        //    buff[mail_count.length()] = '\0';
        //    sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.messageWin, buff);

        sched_ptr->io->up(thread_ptr->PID); //release io resource
        i++;
    }
}

void *lazy_thread_function(void *void_ptr)
{
    scheduler *sched_ptr = (scheduler *)void_ptr;
    thread *thread_ptr = sched_ptr->getProcessTable()->getLast();

    int i = 0;
    char buff[256];
    pthread_mutex_lock(&sched_ptr->proc_mutex);
    while (true)
    {
        sched_ptr->wait_turn(sched_ptr, thread_ptr);
        sleep(1);
        i++;
    }
}

/******************* Scheduler Constructor *******************
Input: none
Output: 
*************************************************************/

scheduler::scheduler()
{
    process_table = new TCB;
    file_system = new ufs("UFS", 16, 128, '.', this);
    nextPID = 0;
    clean = true;
    deadlock = false;
    quantum = 3;
    startTime = 0;
    proc_mutex = procMutex();
    io = new semaphore("io", this);
    pthread_mutex_lock(&proc_mutex);

    sched_ipc = new ipc(this);
    UltimaWindows sched_windows;

    memory_manager = new memoryManager(1024, '.', this);
}

pthread_mutex_t scheduler::procMutex()
{
    pthread_mutex_t proc_mutex = PTHREAD_MUTEX_INITIALIZER;
    return proc_mutex;
}

/****************** create_task ******************************
Input: name
Output: creates appropriate data structures
*************************************************************/

void scheduler::create_task(string name)
{
    struct thread *newThread = new thread();
    newThread->PID = nextPID;
    newThread->name = name;
    newThread->next = nullptr;
    newThread->state = 'W';
    newThread->my_mailbox = new mailbox();

    newThread->my_mailbox->messagesInbox = new Queue<message *>;
    newThread->my_mailbox->messagesRead = new Queue<message *>;
    newThread->my_mailbox->messagesSent = new Queue<message *>;
    newThread->my_mailbox->messagesAll = new Queue<message *>;
    string mailSema = "sema_mailbox" + to_string(nextPID);
    newThread->my_mailbox->sema_mailbox = new semaphore(mailSema, this);

    //newThread->cond = PTHREAD_COND_INITIALIZER;

    getProcessTable()->insert(newThread);

    int resultCode = pthread_create(&getProcessTable()->getLast()->realThread, NULL, thread_function, this);
    if (resultCode != 0)
    {
        //cout << "Error creating thread " << name << endl;
    }
    else
    {
        nextPID++;
        deadlock = false;
    }
}

/********************* thread_function ***********************
Input: *void_ptr
Output: 
*************************************************************/

void scheduler::create_lazy_task(string name)
{
    struct thread *newThread = new thread();
    newThread->PID = nextPID;
    newThread->name = name;
    newThread->next = nullptr;
    newThread->state = 'W';

    newThread->cond = PTHREAD_COND_INITIALIZER;
    getProcessTable()->insert(newThread);
    int resultCode = pthread_create(&getProcessTable()->getLast()->realThread, NULL, lazy_thread_function, this);
    if (resultCode != 0)
    {
        //cout << "Error creating thread " << name << endl;
    }
    else
    {
        nextPID++;
        deadlock = false;
    }
}

/********************* destroy_task **************************
Input: dPID
Output: killing a task by setting its status to dead
*************************************************************/

bool scheduler::destroy_task(int dPID)
{
    thread *temp = getProcessTable()->getHead();
    for (int i = 0; i < getProcessTable()->getSize(); i++)
    {
        if (temp->PID == dPID && temp->state != 'D')
        {
            temp->state = 'D';
            clean = false;
            return true;
        }
        temp = temp->next;
    }
    return false;
}

/************************ yield *****************************
Input: none
Output: strict round robin process switch
*************************************************************/

bool scheduler::yield()
{
    //bool switching;

    if (getProcessTable()->getHeadState() != 'R')
    {
        rotate(getProcessTable()->getHeadState());
        startTime = time(NULL);
        return true;
        //switching = true;
    }
    else if (time(NULL) - startTime > quantum)
    {
        rotate('W');
        startTime = time(NULL);
        return true;
        //switching = true;
    }
    else
    {
        //pthread_cond_signal(&getProcessTable()->getHead()->cond);
        pthread_mutex_unlock(&proc_mutex);
        pthread_cond_signal(&getProcessTable()->getHead()->cond);
        sleep(1);
        pthread_mutex_lock(&proc_mutex);
        return false;
        //switching = false;
    }

    //return switching;
}

/************************ start *****************************
Input: none
Output: strict round robin process switch
*************************************************************/

void scheduler::start()
{
    if (getProcessTable()->getHeadState() != 'W')
    {
        rotate(getProcessTable()->getHeadState());
        startTime = time(NULL);
    }
    else
    {
        getProcessTable()->setHeadState('R');
        startTime = time(NULL);
    }
}

/************************* rotate ****************************
Input: state
Output: 
*************************************************************/

void scheduler::rotate(char state)
{
    getProcessTable()->setHeadState(state);

    int i = 0;
    do
    {
        getProcessTable()->cycle();
        i++;
    } while (getProcessTable()->getHeadState() != 'W' && i < getProcessTable()->getSize());

    if (getProcessTable()->getHeadState() == 'W')
    {
        getProcessTable()->setHeadState('R');
    }
    else
    {
        deadlock = true;
    }
}

/************************* cleanup ****************************
Input: none
Output: 
*************************************************************/

bool scheduler::cleanup()
{
    if (!clean)
    {
        for (int i = 0; i < getProcessTable()->getSize(); i++)
        {
            if (getProcessTable()->getHeadState() == 'D')
            {
                getProcessTable()->setHeadState('K');

                //Must be repeated in case the thread is waiting on a resource.

                pthread_mutex_unlock(&proc_mutex);
                pthread_cond_signal(&getProcessTable()->getHead()->cond);
                sleep(1);
                //pthread_mutex_lock(&proc_mutex);

                //pthread_mutex_unlock(&proc_mutex);
                pthread_cond_signal(&getProcessTable()->getHead()->cond);
                sleep(1);
                pthread_mutex_lock(&proc_mutex);

                pthread_join(getProcessTable()->getHead()->realThread, NULL);
                getProcessTable()->remove();
                i--;
            }
            else
            {
                getProcessTable()->cycle();
            }
        }
        clean = true;
        return true;
    }
    return false;
}

/************************* getDL ****************************
Input: none
Output: returns deadlock
*************************************************************/

bool scheduler::getDL()
{
    return deadlock;
}

TCB *scheduler::getProcessTable()
{
    return process_table;
}

/************************* block ****************************
Input: none
Output: 
*************************************************************/

void scheduler::block()
{
    getProcessTable()->setHeadState('B');
}

/************************* blockByPID *************************
Input: bPID
Output: 
*************************************************************/

void scheduler::blockByPID(int bPID)
{
    thread *temp = getProcessTable()->getHead();
    for (int i = 0; i < getProcessTable()->getSize(); i++)
    {
        if (temp->PID == bPID)
        {
            temp->state = 'B';
        }
        temp = temp->next;
    }
}

/************************* unblockByPID ***********************
Input: ubPID
Output: 
*************************************************************/

bool scheduler::unblockByPID(int ubPID)
{
    thread *temp = getProcessTable()->getHead();
    for (int i = 0; i < getProcessTable()->getSize(); i++)
    {
        if (temp->PID == ubPID && temp->state == 'B')
        {
            temp->state = 'W';
            return true;
        }
        temp = temp->next;
    }
    return false;
}

/************************ dump ******************************
Input: none
Output: dumps the contents of the process table
*************************************************************/

std::string scheduler::dump()
{
    string tcbDump = getProcessTable()->dump();
    if (clean)
    {
        //cout<<"Process table has no dead tasks"<<endl;
    }
    else
    {
        //cout<<"Process table has dead tasks"<<endl;
    }
    if (deadlock)
    {
        //cout<<"There are no valid processes to run. The OS is in deadlock."<<endl;
    }
    return tcbDump;
}

/******************** ~scheduler Destructor *****************
Input: none
Output: deletes the process table
*************************************************************/

scheduler::~scheduler()
{
    delete process_table;
    delete io;
}
