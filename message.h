#ifndef MESSAGE_H
#define MESSAGE_H

#include <time.h>
#include "message_type.h"
#include <string>

struct message {
    int Source_Task_ID;
    int Destination_ID;
    std::string Message_Arrival_Time;
    message_type Msg_Type;
    int Msg_Size;
    char *Msg_text;
    int task_running;
};

#endif
