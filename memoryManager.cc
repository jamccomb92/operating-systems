#include "memoryManager.h"

using namespace std;

memoryManager::memoryManager(int size, char value, scheduler *the_scheduler)
{
    sched_ptr = the_scheduler;
    page_size = 128;
    memory_handle = 0;
    int num_pages = size / page_size;

    // createPages(num_pages, page_size);

    int base = 0, limit = size;

    // create free memory node
    struct node *new_node = new node();
    new_node->base = base;
    new_node->limit = limit;
    new_node->memory_handle = memory_handle;
    new_node->next_page = false;
    new_node->prev = nullptr;
    new_node->next = nullptr;

    head = new_node;

    // set up bit map with initial values
    for (int i = 0; i < size; i++)
    {
        bit_map.push_back(value);
    }
}

// void memoryManager::createPages(int num_pages, int page_size)
// {
//     int base = 0, limit = page_size - 1;

//     struct node *new_node = new node();
//     new_node->base = base;
//     new_node->limit = limit;
//     new_node->memory_handle = memory_handle;
//     new_node->next_page = false;
//     new_node->prev = nullptr;
//     new_node->next = nullptr;

//     head = free_memory = new_node;
//     struct node *previous = new_node;

//     base += page_size;
//     limit += page_size;
//     memory_handle++;

//     for (int i = 0; i < num_pages - 1; i++)
//     {
//         struct node *next_node = new node();
//         new_node->base = base;
//         new_node->limit = limit;
//         new_node->memory_handle = memory_handle;
//         new_node->next_page = false;
//         new_node->prev = previous;
//         new_node->next = nullptr;

//         previous->next = new_node;
//         previous = new_node;

//         base += page_size;
//         limit += page_size;
//         memory_handle++;
//     }
// }

string memoryManager::dump()
{
    struct node *temp = head;
    string dump = "\n ----- Memory Dump -----\n";

    while (temp != nullptr)
    {
        dump += " Base: " + to_string(temp->base) + " Limit: " + to_string(temp->limit) + " Handle: " + to_string(temp->memory_handle) + "\n";
        temp = temp->next;
    }

    dump += " ----- End Dump -------";

    return dump;
}