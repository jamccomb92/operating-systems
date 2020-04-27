/**********************************************************
Author       : Joe McCombs, Justin Doney, Kamil Kosidlak
Class        : C435, Spring 2020
File Name    : memoryManager.cc
Last Updated :
Description  : Used to allow tasks to dynamically
               ask for or free memory
**********************************************************/

#include "memoryManager.h"
#include "scheduler.h"

using namespace std;

/********************* memoryManager ************************
Input: size, value, *the_scheduler

allocate 1024 unsigned chars and initialize the entire
memory with "." dots
*************************************************************/
memoryManager::memoryManager(int size, char value, scheduler *the_scheduler)
{
    sched_ptr = the_scheduler;
    core_size = size;
    page_size = 128;
    memory_handle = 0;
    mem_manager = new semaphore("mem_manager", sched_ptr);
    int num_pages = size / page_size;

    int base = 0, limit = size - 1;

    // create free memory node
    struct node *new_node = new node();
    new_node->base = base;
    new_node->limit = limit;
    new_node->size = size;
    new_node->read_index = 0;
    new_node->write_index = 0;
    new_node->memory_handle = memory_handle;
    new_node->free = true;
    new_node->prev = nullptr;
    new_node->next = nullptr;

    head = new_node;
    memory_handle++;
    //core_nodes++;

    // set up bit map with initial values
    for (int i = 0; i < size; i++)
    {
        bit_map.push_back(value);
    }
}

/********************** Mem_Allocate ************************
Input: size, thread_ptr

returns a unique integer memory_handle or -1 if not enough
memory is available. Set the Current_Location for this
*************************************************************/
int memoryManager::Mem_Allocate(int size, thread *thread_ptr)
{
    sched_ptr->wait_turn(sched_ptr, thread_ptr, mem_manager->down(thread_ptr->PID));
    struct node *temp = head;
    bool found = false;

    while (temp != nullptr)
    {
        if (temp->free && (temp->limit - temp->base + 1) >= size)
        {
            int limit = 0;
            found = true;
            struct node *new_node = new node();
            new_node->permissions.push_back(thread_ptr->PID);
            new_node->base = temp->base;
            new_node->read_index = 0;
            new_node->write_index = 0;
            if (size % page_size == 0)
            {
                limit = size + temp->base - 1;
            }
            else
            {
                limit = (size / page_size + 1) * page_size + temp->base - 1;
            }

            new_node->limit = limit;
            new_node->size = size;
            new_node->memory_handle = memory_handle;
            new_node->free = false;
            //first free memory
            if (temp == head)
            {
                new_node->prev = nullptr;
                temp->prev = new_node;
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
                }
            }
            // insert anywhere but front
            else
            {
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
                    temp->prev = nullptr;
                }
            }
            if (new_node->limit >= temp->limit)
            {
                new_node->next = temp->next;
                delete temp;
                break;
            }
        }
        temp = temp->next;
    }

    if (found)
    {
        memory_handle++;
        mem_manager->up(thread_ptr->PID);
        return memory_handle - 1;
    }

    mem_manager->up(thread_ptr->PID);
    return -1;
}

/************************* Mem_Free **************************
Input: memory_handle, thread_ptr

place "#" in the memory freed. Return -1 if error occurs
*************************************************************/
int memoryManager::Mem_Free(int memory_handle, thread *thread_ptr)
{
    sched_ptr->wait_turn(sched_ptr, thread_ptr, mem_manager->down(thread_ptr->PID));
    /*char winBuff[256];
    string testing = "while";
    for (int i = 0; i < testing.length(); i++)
    {
        winBuff[i] = testing[i];
    }
    winBuff[testing.length()] = '\0';
    sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.messageWin, winBuff);*/
    struct node *temp = head;
    bool freed = false;

    while (temp != nullptr)
    {
        if (temp->memory_handle == memory_handle)
        {
            if (!permissions_check(thread_ptr->PID, temp))
            {
                return -1;
            }
            freed = true;
            temp->free = true;
            for (int i = temp->base; i <= temp->limit; i++)
            {
                bit_map[i] = '#';
            }

            if ((temp->prev != nullptr && temp->prev->free) || (temp->next != nullptr && temp->next->free))
            {
                Mem_Coalesce(temp);
            }
            temp->size = temp->limit - temp->base + 1;
        }
        temp = temp->next;
    }

    mem_manager->up(thread_ptr->PID);
    if (freed)
        return 0;
    else
        return -1;
}

/*********************** Mem_Read ****************************
Input: memory_handle, ch, thread_ptr

read a character from current location in memory and bring
in back in "ch", return a -1 if at end of bound. Keep track
of the "Current Location" or the location of the next char
to be read.
*************************************************************/
int memoryManager::Mem_Read(int memory_handle, char *ch, thread *thread_ptr)
{
    if (memory_handle >= 0)
    {
        node *read_from = get_by_MH(memory_handle);
        if (!permissions_check(thread_ptr->PID, read_from))
        {
            return -1;
        }
        if (read_from && read_from->read_index < read_from->size)
        {
            ch[0] = bit_map[read_from->base + read_from->read_index];
            read_from->read_index++;
            return 0;
        }
    }

    seg_fault(thread_ptr->PID);
    return -1; //segfault
}

// Overloaded multi-byte read
int memoryManager::Mem_Read(int memory_handle, int offset, int nChars, char *ch, thread *thread_ptr)
{
    if (memory_handle >= 0)
    {
        node *read_from = get_by_MH(memory_handle);
        if (!permissions_check(thread_ptr->PID, read_from))
        {
            return -1;
        }
        if (read_from && offset + nChars < read_from->size)
        {
            for (int i = 0; i < nChars; i++)
            {
                ch[i] = bit_map[read_from->base + offset + i];
            }
            return 0;
        }
    }

    seg_fault(thread_ptr->PID);
    return -1; //segfault
}

/********************** Mem_Write ****************************
Input: memory_handle, ch, thread_ptr

write a character to the current_location in memory, return 
a -1 if at end of bounds.
*************************************************************/
int memoryManager::Mem_Write(int memory_handle, char *ch, thread *thread_ptr)
{
    if (memory_handle >= 0)
    {
        node *write_to = get_by_MH(memory_handle);
        if (!permissions_check(thread_ptr->PID, write_to))
        {
            return -1;
        }
        if (write_to && write_to->write_index < write_to->size)
        {
            bit_map[write_to->base + write_to->write_index] = ch[0];
            write_to->write_index++;
            return 0;
        }
    }

    seg_fault(thread_ptr->PID);
    return -1; //segfault
}

// Overloaded multi-byte write
int memoryManager::Mem_Write(int memory_handle, int offset, int nChars, char *ch, thread *thread_ptr)
{
    if (memory_handle >= 0)
    {
        node *write_to = get_by_MH(memory_handle);
        if (!permissions_check(thread_ptr->PID, write_to))
        {
            return -1;
        }
        if (write_to && offset + nChars < write_to->size)
        {
            for (int i = 0; i < nChars; i++)
            {
                bit_map[write_to->base + i + offset] = ch[i];
            }
            return 0;
        }
    }

    seg_fault(thread_ptr->PID);
    return -1; //segfault
}

node *memoryManager::get_by_MH(int gb_memory_handle)
{
    node *temp = head;
    while (temp)
    {
        if (temp->memory_handle == gb_memory_handle)
        {
            return temp;
        }
        temp = temp->next;
    }
    return nullptr;
}

bool memoryManager::permissions_check(int PID, node *handle_node)
{
    for (int i = 0; i < handle_node->permissions.size(); i++)
    {
        if (handle_node->permissions[i] == PID)
        {
            return true;
        }
    }
    return false;
}

void memoryManager::seg_fault(int PID)
{
    string seg_fault = " \n Task " + to_string(PID) + " tried to access memory that it does"
                       //seg_fault +=
                       + " not have access to: \n SEGMENTATION FAULT!\n";
    char buffer[256];

    for (int i = 0; i < seg_fault.length(); i++)
    {
        buffer[i] = seg_fault[i];
    }

    buffer[seg_fault.length()] = '\n';
    buffer[seg_fault.length() + 1] = '\0';
    sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.logWin, buffer);
}

/********************** Mem_Left ****************************
Input: none

return the amount of core memory left in the OS
*************************************************************/
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

/********************** Mem_Largest **************************
Input: none

return the size of largest available memory segment
*************************************************************/
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

/********************** Mem_Smallest ************************
Input: none

return the size of smallest available memory segment
*************************************************************/
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

/********************** Mem_Coalesce ************************
Input: none

combine two or more contiguous blocks of free space, and
place "." Dots in the coalesced memory
*************************************************************/
int memoryManager::Mem_Coalesce(node *freed_mem)
{
    struct node *temp = head;
    bool coalesce_prev = false;

    while (temp != nullptr)
    {
        if (temp->prev != nullptr && temp->prev->free && temp->free)
        {
            temp->prev->limit = temp->limit;
            temp->prev->next = temp->next;
            if (temp->next != nullptr)
            {
                temp->next->prev = temp->prev;
            }

            for (int i = temp->prev->base; i < temp->prev->limit; i++)
            {
                bit_map[i] = '.';
            }

            struct node *coalesce_node = temp;
            coalesce_prev = true;
            temp = temp->next;
            delete coalesce_node;
        }
        else if (temp->next != nullptr && temp->free && temp->next->free)
        {
            temp->limit = temp->next->limit;
            struct node *coalesce_node = temp->next;
            temp->next = temp->next->next;
            if (temp->next)
            {
                temp->next->prev = temp;
            }
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

/************************ dump *******************************
Input: none

dump the contents of memory
*************************************************************/
string memoryManager::dump()
{
    struct node *temp = head;
    string dump = "\n ----- Memory Dump -----\n";

    int left = Mem_Left();
    int largest = Mem_Largest();
    int smallest = Mem_Smallest();

    dump += " Left: " + to_string(left) + " Largest: " + to_string(largest) + " Smallest: " + to_string(smallest) + "\n";
    dump += " Status | Handle | Start Loc | End Loc | Size | Task ID \n";
    while (temp != nullptr)
    {
        if (temp->free)
        {
            dump += " Free   | ";
        }
        else
        {
            dump += " Used   | ";
        }
        dump += to_string(temp->memory_handle);
        int mem_handle = temp->memory_handle;
        for (int i = 0; i < 7; i++)
        {
            if (mem_handle == 0 && i != 0)
            {
                dump += " ";
            }
            mem_handle /= 10;
        }
        dump += "| ";

        dump += to_string(temp->base);
        int base = temp->base;
        for (int i = 0; i < 10; i++)
        {
            if (base == 0 && i != 0)
            {
                dump += " ";
            }
            base /= 10;
        }
        dump += "| ";

        dump += to_string(temp->limit);
        int limit = temp->limit;
        for (int i = 0; i < 8; i++)
        {
            if (limit == 0 && i != 0)
            {
                dump += " ";
            }
            limit /= 10;
        }
        dump += "| ";

        dump += to_string(temp->size);
        int size = temp->size;
        for (int i = 0; i < 5; i++)
        {
            if (size == 0 && i != 0)
            {
                dump += " ";
            }
            size /= 10;
        }
        dump += "| ";

        if (temp->permissions.size() == 0)
        {
            dump += "MM     \n";
        }
        else
        {
            dump += to_string(temp->permissions[0]) + "      \n";
        }

        temp = temp->next;
    }

    dump += " \n";

    return dump;
}

string memoryManager::Core_Dump()
{
    string dump = "\n Core Dump";
    for (int i = 0; i < core_size; i++)
    {
        if (i % 64 == 0)
        {
            dump += "\n ";
            if (i % 128 == 0)
            {
                dump += "\n ";
            }
        }
        dump += bit_map[i];
    }

    dump += "\n";
    return dump;
}