#ifndef INODE_H
#define INODE_H

#include <bitset>
#include <fstream>
#include <vector>

struct inode
{
    char filename[8];
    int owner_PID;
    int starting_block;
    std::vector<int> all_blocks;
    int size;
    bool in_use;
    inode *next;
    int file_handle;
    int file_descriptor;
    //std::streamoff read_index;
    int read_index;
    //std::streamoff write_index;
    int write_index;
    std::fstream real_file;
    char mode;

    std::bitset<4> permissions;
    std::bitset<16> blocks;
    ~inode() {}
};

#endif
