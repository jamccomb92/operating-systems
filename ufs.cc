#include <bitset>
#include <string>
#include <cstring>
#include <fstream>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "ufs.h"
#include "scheduler.h"

using namespace std;
#define OUTPUT_MODE 0777

ufs::ufs(std::string name, int nblocks, int block_size, char init_char, scheduler *The_Scheduler)
{
    fs_name = name;
    fs_block_size = block_size;
    fs_nblocks = nblocks;
    next_file_handle = 0;
    num_inodes = 0;
    fs_init_char = init_char;
    head = nullptr;
    nfree_blocks = nblocks;
    sched_ptr = The_Scheduler;
}

void ufs::format()
{
    //check permissions

    //delete all files
    inode *temp = head;
    while (head)
    {
        temp = head;
        head = head->next;
        temp->real_file.close();
        remove(temp->filename);
        delete temp;
    }

    //set attributes
}

int ufs::Open(int PID, int file_handle, std::string file_name, char mode)
{
    inode *temp = find_file(file_handle);

    //owner trying to acess file
    if (temp != nullptr && temp->mode == 'C')
    {
        temp->read_index = 0;
        temp->write_index = 0;
        if (temp->owner_PID == PID)
        {
            switch (mode)
            {
            // Read
            case 'R':
                if (temp->permissions[0])
                {
                    temp->file_descriptor = open(temp->filename, ios::in);
                    temp->mode = mode;
                    return temp->file_handle;
                }
                break;
            // Write
            case 'W':
                if (temp->permissions[1])
                {
                    temp->file_descriptor = open(temp->filename, ios::out);
                    temp->mode = mode;
                    return temp->file_handle;
                }
                break;
            // Both read and write
            case 'B':
                if (temp->permissions[0] && temp->permissions[1])
                {
                    temp->file_descriptor = open(temp->filename, ios::in | ios::out);
                    temp->mode = mode;
                    return temp->file_handle;
                }
                break;

            default:
                break;
            }
        }
        // non owner trying to access file
        else
        {
            switch (mode)
            {

            case 'R':
                if (temp->permissions[2])
                {
                    temp->file_descriptor = open(temp->filename, ios::in);
                    temp->mode = mode;
                    return temp->file_handle;
                }
                break;

            case 'W':
                if (temp->permissions[3])
                {
                    temp->file_descriptor = open(temp->filename, ios::out);
                    temp->mode = mode;
                    return temp->file_handle;
                }
                break;

            case 'B':
                if (temp->permissions[2] && temp->permissions[3])
                {
                    temp->file_descriptor = open(temp->filename, ios::in | ios::out);
                    temp->mode = mode;
                    return temp->file_handle;
                }
                break;

            default:
                break;
            }
        }
    }

    // file name does not exist or permissions not met
    return -1;
}

int ufs::Close(int PID, int file_handle)
{
    // char buffer[8] = "testtex";
    inode *temp = find_file(file_handle);
    // write(temp->file_descriptor, buffer, 8);

    if (temp != nullptr && temp->mode != 'C')
    {
        if (temp->owner_PID == PID)
        {
            switch (temp->mode)
            {
            case 'R':
                //if (temp->permissions[0])
                //{
                close(temp->file_descriptor);
                temp->mode = 'C';
                return temp->file_handle;
                //}
                break;

            case 'W':
                //if (temp->permissions[1])
                //{
                close(temp->file_descriptor);
                temp->mode = 'C';
                return temp->file_handle;
                //}
                break;

            case 'B':
                //if (temp->permissions[0] && temp->permissions[1])
                //{
                close(temp->file_descriptor);
                temp->mode = 'C';
                return temp->file_handle;
                //}
                break;

            default:
                break;
            }
        }

        else
        {
            switch (temp->mode)
            {
            case 'R':
                //if (temp->permissions[2])
                //{
                close(temp->file_descriptor);
                temp->mode = 'C';
                return temp->file_handle;
                //}
                break;

            case 'W':
                //if (temp->permissions[3])
                //{
                close(temp->file_descriptor);
                temp->mode = 'C';
                return temp->file_handle;
                //}
                break;

            case 'B':
                //if (temp->permissions[2] && temp->permissions[3])
                //{
                close(temp->file_descriptor);
                temp->mode = 'C';
                return temp->file_handle;
                //}
                break;

            default:
                break;
            }
        }
    }
    return -1;
}

int ufs::Read_Char(int PID, int file_handle, char *ch)
{
    inode *temp = find_file(file_handle);

    if (temp != nullptr && temp->read_index < temp->size && (temp->mode == 'R' || temp->mode == 'B'))
    {
        if ((temp->owner_PID == PID && temp->permissions[0]) || (temp->owner_PID != PID && temp->permissions[2]))
        {
            temp->real_file.seekg(temp->read_index++, ios::beg);
            char c[1];

            temp->real_file.get(ch[0]);

            return 1;
        }
    }

    return -1;
}

int ufs::Write_Char(int PID, int file_handle, char ch)
{

    char myString[3];
    myString[1] = ch;
    myString[0] = ' ';
    myString[2] = '\n';

    // sched_ptr->sched_windows.write_string_window(sched_ptr->getProcessTable()->getWindowByPID(PID), test);
    inode *temp = find_file(file_handle);

    // close(temp->file_descriptor);

    if (temp != nullptr && temp->write_index < temp->size && (temp->mode == 'W' || temp->mode == 'B'))
    {
        //sched_ptr->sched_windows.write_window(sched_ptr->getProcessTable()->getWindowByPID(PID), myString);
        if ((temp->owner_PID == PID && temp->permissions[1]) || (temp->owner_PID != PID && temp->permissions[3]))
        {
            //sched_ptr->sched_windows.write_window(sched_ptr->getProcessTable()->getWindowByPID(PID), myString);

            string file_name = "";
            for (int i = 0; i < 7; i++)
            {
                file_name += temp->filename[i];
            }

            temp->real_file.seekp(temp->write_index, ios::beg);
            temp->real_file.put(ch);

            //temp->real_file.seekp(0, ios::end);	//forces file to update, might need to be reworked
            //temp->real_file.put('EOF');
            temp->write_index++;

            return 1;
        }
    }

    return -1;
}

int ufs::Create_File(int PID, std::string file_name, int file_size, std::bitset<4> permissions, char mode)
{
    string string_name = "";
    inode *temp = new inode;
    temp->file_handle = next_file_handle;
    next_file_handle++;
    temp->read_index = 0;
    temp->write_index = 0;
    for (int i = 0; i < 8 && i < file_name.length(); i++)
    {
        temp->filename[i] = file_name[i];
        string_name += file_name[i];
    }

    temp->owner_PID = PID;
    temp->permissions = permissions;

    temp->size = file_size;
    //What is in use for?
    temp->in_use = true;

    // to find free blocks for new file
    //int num_inodes = file_size / 40;
    // file too big
    /*if (num_inodes > 4){
        return -1;
	}*/

    int blocks_needed = (file_size - 1) / fs_block_size + 1;
    if (blocks_needed > 4)
    {
        return -1;
    }
    //bool new_inode = true;
    inode *check = head;
    inode *prev = nullptr;
    bitset<16> free_blocks(0x0000);
    while (blocks_needed > 0 && check)
    {
        free_blocks |= check->blocks;
        if (!check->in_use)
        {
            /*			if((file_size - 1) / fs_block_size + 1 > blocks_needed){
				for(int i = 0; blocks_needed > 0; i++){
					if(check->blocks[i]){
						temp->blocks[i] = 1;
						check->blocks[i] = 0;
						blocks_needed--;
					}
					else if(i > fs_nblocks){
						return -1;
					}
				}
				check->size -= blocks_needed * fs_block_size;
			}
			else{
				//blocks_needed -= (file_size - 1) / fs_block_size + 1;
				if(temp->blocks.any()){
					for(int i = 0; blocks_needed > 0 && i < fs_nblocks; i++){
						if(check->blocks[i]){
							temp->blocks[i] = 1;
							check->blocks[i] = 0;
							blocks_needed--;
						}
					}
					if(prev){
						prev->next = check->next;
					}
					else{
						head = check->next;
					}
					delete check;
				}
				else{
					blocks_needed -= (file_size - 1) / fs_block_size + 1;
					temp->blocks = check->blocks;
					if(prev){
						prev->next = check->next;
					}
					else{
						head = check->next;
					}
					delete check;
				}
			}*/
            for (int i = 0; blocks_needed > 0 && i < fs_nblocks; i++)
            {
                if (check->blocks[i])
                {
                    temp->all_blocks.push_back(i);
                    temp->blocks[i] = 1;
                    check->blocks[i] = 0;
                    blocks_needed--;
                    check->size -= fs_block_size;
                }
            }
            if (check->size <= 0)
            {
                if (prev)
                {
                    prev->next = check->next;
                }
                else
                {
                    head = check->next;
                }
                delete check;
            }
        }
        else if (!strcmp(temp->filename, check->filename))
        {
            return -1;
        }
        prev = check;
        check = check->next;
    }

    if (blocks_needed && free_blocks.all())
    {
        return -1;
    }

    for (int i = 0; blocks_needed > 0 && i < fs_nblocks; i++)
    {
        if (!free_blocks[i])
        {
            temp->all_blocks.push_back(i);
            temp->blocks[i] = 1;
            blocks_needed--;
        }
    }

    if (blocks_needed)
    {
        return -1;
    }

    /*std::bitset<16> free_blocks = find_free_blocks();
    int count = 0;
    int position;
    for (int i = 0; i < 16; i++)
    {
        if (!free_blocks[i] && count < num_inodes - 1)
        {
            count++;
            position = i;
            break;
        }
        else if (!free_blocks[i])
        {
            count++;
        }
        else
        {
            count = 0;
        }
    }

    if (count == num_inodes)
    {
        while (num_inodes > 0)
        {
            temp->blocks[position] = 1;
            num_inodes--;
            position--;
        }
    }
    else
    {
        // not enough disk available
        return -1;
    }*/

    // why temp->next head?
    // wouldnt these both depend if this is first file created or not?
    // temp->next = head;
    // head = temp;

    //temp->blocks;

    switch (mode)
    {
    case 'R':
        temp->file_descriptor = creat(temp->filename, OUTPUT_MODE);
        //temp->real_file = new fstream myfile(string_name, ios::in);
        temp->real_file.open(file_name, ios::out | ios::in);
        break;

    case 'W':
        temp->file_descriptor = creat(temp->filename, OUTPUT_MODE);
        //temp->real_file = new fstream myfile(string_name, ios::out);
        temp->real_file.open(file_name, ios::out | ios::in);
        break;

    case 'B':
        temp->file_descriptor = creat(temp->filename, OUTPUT_MODE);
        //temp->real_file = new fstream myfile(string_name, ios::out | ios::in);
        temp->real_file.open(file_name, ios::out | ios::in);
        break;

    default:
        return -1;
    }
    //temp->real_file.seekp(temp->write_index, ios::beg);
    //temp->real_file.put('v');
    temp->mode = mode;

    temp->next = head;
    head = temp;

    close(temp->file_descriptor);
    return temp->file_handle;
}

int ufs::Delete_File(int PID, std::string file_name)
{
    inode *temp = find_file(file_name);
    if (temp && temp->owner_PID == PID && temp->mode == 'C')
    {
        temp->in_use = false;
        temp->real_file.close();
        remove(temp->filename);
        return 0;
    }
    /*else if(temp){
		int hold_fd = creat("test1", O_RDWR);
		close(hold_fd);
	}
	else{
		int hold_fd = creat("test2", O_RDWR);
		close(hold_fd);
	}*/
    return -1;
}

int ufs::Change_Permissions(int PID, string file_name, std::bitset<4> new_permissions)
{
    inode *temp = find_file(file_name);
    if (temp && temp->owner_PID == PID && temp->mode == 'C')
    {
        temp->permissions = new_permissions;
        return 0;
    }
    return -1;
}

void ufs::Dir()
{
    inode *temp = head;
    string write_string = "\n " + fs_name + " Directory \n";
    while (temp)
    {
        if (temp->in_use)
        {
            write_string = write_string + " " + to_string(temp->owner_PID) + " ";
            if (temp->permissions[0])
            {
                write_string += "r";
            }
            else
            {
                write_string += "-";
            }
            if (temp->permissions[1])
            {
                write_string += "w";
            }
            else
            {
                write_string += "-";
            }
            if (temp->permissions[2])
            {
                write_string += "r";
            }
            else
            {
                write_string += "-";
            }
            if (temp->permissions[3])
            {
                write_string += "w";
            }
            else
            {
                write_string += "-";
            }

            write_string += " ";
            write_string += temp->filename;
            write_string += "\n ";
        }
        temp = temp->next;
    }
    sched_ptr->sched_windows.write_string_window(sched_ptr->sched_windows.messageWin, write_string);
}

void ufs::Dir(int PID)
{
    inode *temp = head;
    string write_string = "\n " + fs_name + " files belonging to PID: " + to_string(PID) + "\n";
    while (temp)
    {
        if (temp->in_use && temp->owner_PID == PID)
        {
            write_string = write_string + " " + to_string(temp->owner_PID) + " ";
            if (temp->permissions[0])
            {
                write_string += "r";
            }
            else
            {
                write_string += "-";
            }
            if (temp->permissions[1])
            {
                write_string += "w";
            }
            else
            {
                write_string += "-";
            }
            if (temp->permissions[2])
            {
                write_string += "r";
            }
            else
            {
                write_string += "-";
            }
            if (temp->permissions[3])
            {
                write_string += "w";
            }
            else
            {
                write_string += "-";
            }

            write_string += " ";
            write_string += temp->filename;
            write_string += "\n ";
        }
        temp = temp->next;
    }
    sched_ptr->sched_windows.write_string_window(sched_ptr->sched_windows.messageWin, write_string);
}

void ufs::dump()
{
    inode *temp = head;
    string dump = " ----------------- UFS DUMP ------------------\n";
    dump += " F Handle | Permission | File Name | Blocks Used | Size | Start Block | Status          | OwnerID \n";

    char system_dump[2048 + 32 + 32 + 16 - 2];
    fill_n(system_dump, 2048 + 32 + 32 + 16 - 2, '^');
    for (int i = 0; i < 32; i++)
    {
        system_dump[i * 66 + i / 2] = ' ';
        system_dump[i * 66 + i / 2 + 65] = '\n';
    }

    for (int i = 0; i < 16; i++)
    {
        system_dump[i * 132 + i - 1] = '\n';
    }

    int out_fn;
    char buffer[128];
    int rd_count;
    while (temp != nullptr)
    {
        if (temp->in_use)
        {
            dump += " " + to_string(temp->file_handle) + dump_extra_spaces(temp->file_handle, 9);
            if (temp->permissions[0])
            {
                dump += "r";
            }
            else
            {
                dump += "-";
            }
            if (temp->permissions[1])
            {
                dump += "w";
            }
            else
            {
                dump += "-";
            }
            if (temp->permissions[2])
            {
                dump += "r";
            }
            else
            {
                dump += "-";
            }
            if (temp->permissions[3])
            {
                dump += "w";
            }
            else
            {
                dump += "-";
            }
            dump += "       | ";
            dump += temp->filename + dump_extra_spaces(temp->filename, 10);
            string blocks = "";
            for (int i = 0; i < temp->all_blocks.size(); i++)
            {
                dump += to_string(temp->all_blocks[i]) + " ";
                blocks += to_string(temp->all_blocks[i]) + " ";
            }
            dump += dump_extra_spaces(blocks, 12);
            dump += to_string(temp->size) + dump_extra_spaces(temp->size, 5);
            dump += to_string(temp->all_blocks.front()) + dump_extra_spaces(temp->all_blocks.front(), 12);
            if (temp->mode == 'R')
            {
                dump += "Open Read       | ";
            }
            else if (temp->mode == 'W')
            {
                dump += "Open Write      | ";
            }
            else if (temp->mode == 'B')
            {
                dump += "Open Read/Write | ";
            }
            else if (temp->mode == 'C')
            {
                dump += "Closed          | ";
            }

            dump += "Tid-" + to_string(temp->owner_PID) + "\n";

            out_fn = open(temp->filename, O_RDONLY);
            for (int i = 0; i < fs_nblocks; i++)
            {
                if (temp->blocks[i])
                {
                    //buffer.fill('.');
                    rd_count = read(out_fn, buffer, 128);
                    // system_dump[0] = ' ';
                    for (int j = 0; j < rd_count && buffer[j]; j++)
                    {
                        if (j < 64)
                        {
                            system_dump[i * (fs_block_size + 5) + j + 1] = buffer[j];
                        }
                        else
                        {
                            system_dump[i * (fs_block_size + 5) + j + 3] = buffer[j];
                        }
                    }
                    for (int j = rd_count; j < 128; j++)
                    {
                        if (j < 64)
                        {
                            system_dump[i * (fs_block_size + 5) + j + 1] = '.';
                        }
                        else
                        {
                            system_dump[i * (fs_block_size + 5) + j + 3] = '.';
                        }
                    }
                }
            }
            close(out_fn);
        }
        else
        {
            for (int i = 0; i < fs_nblocks; i++)
            {
                if (temp->blocks[i])
                {
                    for (int j = 0; j < 128; j++)
                    {
                        system_dump[i * (fs_block_size + 4) + j + 2 * int(j / 64) + 1] = '$';
                    }
                }
            }
        }
        temp = temp->next;
    }

    sched_ptr->sched_windows.write_string_window(sched_ptr->sched_windows.messageWin, dump);
    sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.messageWin, "\n");

    sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.messageWin, system_dump);
    //sched_ptr->sched_windows.write_window(sched_ptr->sched_windows.messageWin, "\n\n\n\n");
}

string ufs::dump_extra_spaces(int output, int sizeLoc)
{
    string spaces = "";

    for (int i = 0; i < sizeLoc; i++)
    {
        if (output == 0 && i != 0)
        {
            spaces += " ";
        }
        output /= 10;
    }
    spaces += "| ";
    return spaces;
}

string ufs::dump_extra_spaces(string output, int sizeLoc)
{
    string spaces = "";
    int string_size = output.size();

    for (int i = 0; i < sizeLoc - string_size; i++)
    {
        spaces += " ";
    }
    spaces += "| ";
    return spaces;
}

inode *ufs::find_file(string name)
{
    if (name.length() > 8)
    {
        return nullptr;
    }

    char filename[8];
    for (int i = 0; i < 8; i++)
    {
        filename[i] = name[i];
    }

    inode *temp = head;
    while (temp)
    {
        if (!strcmp(filename, temp->filename) && temp->in_use)
        {
            return temp;
        }
        temp = temp->next;
    }
    return nullptr;
}

inode *ufs::find_file(int file_handle)
{
    inode *temp = head;
    while (temp)
    {
        if (file_handle == temp->file_handle && temp->in_use)
        {
            return temp;
        }
        temp = temp->next;
    }
    return nullptr;
}

std::bitset<16> ufs::find_free_blocks()
{
    inode *temp = head;
    std::bitset<16> free_blocks = std::bitset<16>(0);

    while (temp != nullptr)
    {
        free_blocks = free_blocks | temp->blocks;
    }

    return free_blocks;
}

const char *ufs::get_full_path(string file_name)
{
    string path = "files/";

    for (int i = 0; i < file_name.size(); i++)
    {
        path += file_name[i];
    }

    const char *finalPath = path.c_str();
}
