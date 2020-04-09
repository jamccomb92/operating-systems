#ifndef IPC_H
#define IPC_H

#include "thread.h"
#include "message.h"

#include <string>

class scheduler;

class ipc
{
private:
    scheduler *sched_ptr;

public:
    ipc(scheduler *The_Scheduler);
    int Message_Send(thread *thread_ptr, message *Message);
    int Message_Receive(thread *thread_ptr);
    std::string Message_Count(thread *thread_ptr);
    std::string Message_Count();
    std::string Message_Echo(thread *thread_ptr);
    std::string Message_Print(thread *thread_ptr);
    int Message_Delete_All(thread *thread_ptr);
    std::string ipc_dump();
};

#endif
