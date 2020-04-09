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
    int page_size;
    int memory_handle;

public:
    memoryManager(int size, char default_value, scheduler *the_scheduler);
    void createPages(int num_pages, int page_size);
    string dump();
};

#endif
