#include "memoryManager.h"
#include "scheduler.h"

using namespace std;

memoryManager::memoryManager(int size, char value, scheduler *the_scheduler)
{
    sched_ptr = the_scheduler;
    core_size = size;
    page_size = 128;
    memory_handle = 0;
    int num_pages = size / page_size;

    // createPages(num_pages, page_size);

    int base = 0, limit = size - 1;

    // create free memory node
    struct node *new_node = new node();
    new_node->base = base;
    new_node->limit = limit;
    new_node->memory_handle = memory_handle;
    new_node->free = true;
    new_node->prev = nullptr;
    new_node->next = nullptr;

    head = new_node;
    memory_handle++;
    // core_nodes++;

    // set up bit map with initial values
    for (int i = 0; i < size; i++)
    {
        bit_map.push_back(value);
    }
}

int memoryManager::Mem_Allocate(int size)
{
    // check for largest
    // add burping if time and desire

    struct node *temp = head;
    bool found = false;
    // char winBuff[256];
    // string testing = "before loop";
    // for (int i = 0; i < testing.length(); i++)
    // {
    //     winBuff[i] = testing[i];
    // }
    // winBuff[testing.length()] = '\0';
    // sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.logWin, winBuff);

    while (temp != nullptr)
    {
        // testing = "in loop";
        // for (int i = 0; i < testing.length(); i++)
        // {
        //     winBuff[i] = testing[i];
        // }
        // winBuff[testing.length()] = '\0';
        // sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.logWin, winBuff);
        if (temp->free && (temp->limit - temp->base + 1) >= size)
        {
            found = true;
            int limit = 0;
            struct node *new_node = new node();
            new_node->base = temp->base;

            //  new_node-?>loimit = limit = ((size -1) / page_size + 1) * page_size + temp->base - 1;
            // instead of conditional -- TEST but should work
            if (size % page_size == 0)
            {
                limit = size + temp->base - 1;
            }
            else
            {
                limit = (size / page_size + 1) * page_size + temp->base - 1;
            }

            new_node->limit = limit;
            new_node->memory_handle = memory_handle;
            new_node->free = false;
            //first freee memory
            if (temp == head)
            {
                new_node->prev = nullptr;
                temp->prev = new_node;
                // limit == 1023 inverse
                if (limit < 1023)
                {
                    temp->base = limit + 1;
                    new_node->next = temp;
                    head = new_node;
                }
                else
                {
                    new_node->next = nullptr;
                    head = new_node;
                    delete temp;
                }
            }
            // insert anywhere but front
            else
            {
                // check if temp->size == size of allocation
                new_node->prev = temp->prev;
                temp->prev->next = new_node;
                if (limit < 1023)
                {
                    temp->base = limit + 1;
                    new_node->next = temp;
                    temp->prev = new_node;
                }
                else
                {
                    //end of list
                    new_node->next = nullptr;
                    // temp->prev = nullptr;
                    delete temp;
                }
            }
        }
        temp = temp->next;
    }

    // move this to end of the if (where )
    if (found)
    {
        // core_nodes++;
        memory_handle++;
        return memory_handle - 1;
    }

    return -1;
}

int memoryManager::Mem_Free(int memory_handle)
{
    // char winBuff[256];
    // string testing = "while";
    // for (int i = 0; i < testing.length(); i++)
    // {
    //     winBuff[i] = testing[i];
    // }
    // winBuff[testing.length()] = '\0';
    // sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.messageWin, winBuff);
    struct node *temp = head;
    bool freed = false;

    while (temp != nullptr)
    {
        if (temp->memory_handle == memory_handle)
        {
            freed = true;
            temp->free = true;
            for (int i = temp->base; i <= temp->limit; i++)
            {
                bit_map[i] = '#';
            }

            // testing = "before if";
            // for (int i = 0; i < testing.length(); i++)
            // {
            //     winBuff[i] = testing[i];
            // }
            // winBuff[testing.length()] = '\0';
            // sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.messageWin, winBuff);
            if ((temp->prev != nullptr && temp->prev->free) || (temp->next != nullptr && temp->next->free))
            {
                // testing = "inside if coalesce";
                // for (int i = 0; i < testing.length(); i++)
                // {
                //     winBuff[i] = testing[i];
                // }
                // winBuff[testing.length()] = '\0';
                // sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.messageWin, winBuff);
                Mem_Coalesce();
            }
        }
        temp = temp->next;
    }

    // move return 0 or whatever that may be(value) inside of if statement
    if (freed)
        return 0;
    else
        return -1;
}

int memoryManager::Mem_Left()
{
    struct node *temp = head;
    int free_memory = 0;

    while (temp != nullptr)
    {
        if (temp->free)
        {
            free_memory += temp->limit - temp->base + 1;
        }

        temp = temp->next;
    }

    return free_memory;
}

int memoryManager::Mem_Largest()
{
    struct node *temp = head;
    int largest_memory = 0;

    while (temp != nullptr)
    {
        if (temp->free && (temp->limit - temp->base) > largest_memory)
        {
            largest_memory = temp->limit - temp->base + 1;
        }

        temp = temp->next;
    }

    return largest_memory;
}

int memoryManager::Mem_Smallest()
{
    struct node *temp = head;
    int smallest_memory = 1025;

    while (temp != nullptr)
    {
        if (temp->free && (temp->limit - temp->base) < smallest_memory)
        {
            smallest_memory = temp->limit - temp->base + 1;
        }

        temp = temp->next;
    }

    if (smallest_memory == 1025)
    {
        return 0;
    }
    return smallest_memory;
}

int memoryManager::Mem_Coalesce()
{
    struct node *temp = head;
    // struct node *coalesce_node = new node();
    bool coalesce_prev = false;

    // char winBuff[256];
    // string testing = " \n before-loop-coal";
    // for (int i = 0; i < testing.length(); i++)
    // {
    //     winBuff[i] = testing[i];
    // }
    // winBuff[testing.length()] = '\0';
    // sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.messageWin, winBuff);

    while (temp != nullptr)
    {
        //     testing = "\n while-coal";
        //     for (int i = 0; i < testing.length(); i++)
        //     {
        //         winBuff[i] = testing[i];
        //     }
        //     winBuff[testing.length()] = '\0';
        //     sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.messageWin, winBuff);

        if (temp->prev != nullptr && temp->prev->free && temp->free)
        {
            // testing = "\n if-1-coal";
            // for (int i = 0; i < testing.length(); i++)
            // {
            //     winBuff[i] = testing[i];
            // }
            // winBuff[testing.length()] = '\0';
            // sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.messageWin, winBuff);

            temp->prev->limit = temp->limit;
            temp->prev->next = temp->next;
            if (temp->next != nullptr)
            {
                temp->next->prev = temp->prev;
            }

            // testing = "\n if-1-2";
            // for (int i = 0; i < testing.length(); i++)
            // {
            //     winBuff[i] = testing[i];
            // }
            // winBuff[testing.length()] = '\0';
            // sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.messageWin, winBuff);

            for (int i = temp->prev->base; i < temp->prev->limit; i++)
            {
                bit_map[i] = '.';
            }

            struct node *coalesce_node = temp;
            coalesce_prev = true;
            temp = temp->next;
            delete coalesce_node;

            // testing = "\n if-1-3";
            // for (int i = 0; i < testing.length(); i++)
            // {
            //     winBuff[i] = testing[i];
            // }
            // winBuff[testing.length()] = '\0';
            // sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.messageWin, winBuff);
        }
        else if (temp->next != nullptr && temp->free && temp->next->free)
        {
            // testing = "\n if-2-coal";
            // for (int i = 0; i < testing.length(); i++)
            // {
            //     winBuff[i] = testing[i];
            // }
            // winBuff[testing.length()] = '\0';
            // sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.messageWin, winBuff);
            temp->limit = temp->next->limit;
            struct node *coalesce_node = temp->next;
            temp->next = temp->next->next;
            temp->next->prev = temp;
            delete coalesce_node;

            for (int i = temp->base; i < temp->limit; i++)
            {
                bit_map[i] = '.';
            }
        }

        if (!coalesce_prev)
            temp = temp->next;

        coalesce_prev = false;
    }

    return 1;
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

    int left = Mem_Left();
    int largest = Mem_Largest();
    int smallest = Mem_Smallest();

    dump += " Left: " + to_string(left) + " Largest: " + to_string(largest) + " Smallest: " + to_string(smallest) + "\n";

    while (temp != nullptr)
    {
        dump += " Base: " + to_string(temp->base) + " Limit: " + to_string(temp->limit) + " Handle: " + to_string(temp->memory_handle);
        if (temp->free)
        {
            dump += " Free: True\n";
        }
        else
        {
            dump += " Free: False\n";
        }

        temp = temp->next;
    }

    dump += " \n";

    return dump;
}

string memoryManager::Core_Dump()
{
    string dump = "\n Core Dump: \n";

    for (int i = 0; i < core_size; i++)
    {
        dump += bit_map[i];
        if (i % 128 == 0)
            dump += "\n";
    }

    dump += "\n";
    return dump;
}