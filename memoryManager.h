#ifndef MEM_MGR
#define MEM_MGR

#include <string>
#include <vector>
#include "listNode.h"

using namespace std;

class scheduler;

class memoryManager
{
private:
    scheduler *sched_ptr;
    node *head;
    node *free_memory;
    vector<char> bit_map;
    int core_size;
    int page_size;
    int memory_handle;
    // int core_nodes;

public:
    memoryManager(int size, char default_value, scheduler *the_scheduler);
    // void createPages(int num_pages, int page_size);
    int Mem_Allocate(int size);
    int Mem_Free(int memory_hanlde);

    int Mem_Left();
    int Mem_Largest();
    int Mem_Smallest();
    int Mem_Coalesce();
    string dump();
    string Core_Dump();
};

#endif
