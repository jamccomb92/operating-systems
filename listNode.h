#ifndef NODE_H
#define NODE_H

struct node
{
    int base;
    int limit;
    int memory_handle;
    bool next_page;
    // 0 = free, 1 = used
    int free_used;
    node *next;
    node *prev;
    ~node() {}
};

#endif