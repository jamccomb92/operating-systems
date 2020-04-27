#ifndef UFS_H
#define UFS_H

#include "iNode.h"
#include <string>

class scheduler;

class ufs
{
private:
    std::string fs_name;
    int fs_block_size;
    int fs_nblocks;
    int nfree_blocks;
    int next_file_handle;
    int num_inodes;
    char fs_init_char;
    scheduler *sched_ptr;
    inode *head;
    inode *tail;

public:
    ufs(std::string name, int nblocks, int blockSize, char init_char, scheduler *The_Scheduler);
    void format();

    // File operators
    int Open(int PID, int file_handle, std::string file_name, char mode);
    int Close(int PID, int file_handle);
    int Read_Char(int PID, int file_handle, char *ch);
    int Write_Char(int PID, int file_handle, char ch);

    inode *find_file(std::string name);
    inode *find_file(int file_handle);

    // Directory
    int Create_File(int PID, std::string file_name, int file_size, std::bitset<4> permissions, char mode);
    int Delete_File(int PID, std::string file_name);
    int Change_Permissions(int PID, std::string file_name, std::bitset<4> new_permissions);
    std::bitset<16> find_free_blocks();
    const char *get_full_path(std::string file_name);

    void Dir();
    void Dir(int PID);
    void dump();

    std::string dump_extra_spaces(int output, int sizeLoc);
    std::string dump_extra_spaces(std::string output, int sizeLoc);
};

#endif
