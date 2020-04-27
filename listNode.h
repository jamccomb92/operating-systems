#ifndef NODE_H
#define NODE_H

#include <vector>

struct node
{
    std::vector<int> permissions;
    int base;
    int limit;
    int size;
    int memory_handle;
    int read_index;
    int write_index;
    // bool next_page;
    // 0 = free, 1 = used
    bool free;
    node *next;
    node *prev;
    ~node() {}
};

#endif
