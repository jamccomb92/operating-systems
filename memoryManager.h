/**********************************************************
Author       : Joe McCombs, Justin Doney, Kamil Kosidlak
Class        : C435, Spring 2020
File Name    : memoryManager.h
Last Updated :
Description  : Header file that defines the memoryManager class.
**********************************************************/

#ifndef MEM_MGR
#define MEM_MGR

#include <string>
#include <vector>
#include "listNode.h"
#include "scheduler.h"

using namespace std;

//class scheduler;

class memoryManager
{
private:
    scheduler *sched_ptr;
    node *head;
    vector<char> bit_map;
    int core_size;
    int page_size;
    int memory_handle;
    int core_nodes;
    semaphore *mem_manager;

public:
    memoryManager(int size, char default_value, scheduler *the_scheduler);
    int Mem_Allocate(int size, thread *thread_ptr);
    int Mem_Free(int memory_handle, thread *thread_ptr);

    int Mem_Read(int memory_handle, char *ch, thread *thread_ptr);
    int Mem_Read(int memory_handle, int offset, int nChars, char ch[], thread *thread_ptr);

    int Mem_Write(int memory_handle, char *ch, thread *thread_ptr);
    int Mem_Write(int memory_handle, int offset, int nChars, char *ch, thread *thread_ptr);

    node *get_by_MH(int gb_memory_handle);
    bool permissions_check(int PID, node *handle_node);
    void seg_fault(int PID);

    int Mem_Left();
    int Mem_Largest();
    int Mem_Smallest();
    int Mem_Coalesce(node *mem_freed);
    string dump();
    string Core_Dump();
};

#endif
