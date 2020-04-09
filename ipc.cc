#include "ipc.h"
#include "message.h"
#include "message_type.h"
#include "thread.h"
#include "scheduler.h"
#include "queue.h"
#include <time.h>
#include <string.h>
#include <sstream>

using namespace std;

/******************** ipc Constructor ***********************
Input: The_Scheduler
Output: none
*************************************************************/
ipc::ipc(scheduler *The_Scheduler)
{
    sched_ptr = The_Scheduler;
    srand(time(NULL));
}
/********************** Message_Send ************************
Input: thread_ptr, *Message
Output: Function that will allocate a new message node, copy
        the message and other information onto it, and place
        the message in the destination task's mailbox.
*************************************************************/
int ipc::Message_Send(thread *thread_ptr, message *Message)
{

    string temp = "";
    // check for text message
    if (Message->Msg_Type.Message_Type_ID == 0)
    {
        // Text message
        temp = "";
        temp = " Text";
        for (int i = 0; i < temp.length(); i++)
        {
            Message->Msg_Type.Message_Type_Description[i] = temp[i];
        }
    }
    // service message
    else if (Message->Msg_Type.Message_Type_ID == 1)
    {
        temp = "";
        temp = " Service";
        for (int i = 0; i < temp.length(); i++)
        {
            Message->Msg_Type.Message_Type_Description[i] = temp[i];
        }
    }
    // notification message
    else if (Message->Msg_Type.Message_Type_ID == 2)
    {
        temp = "";
        temp = " Notification";
        for (int i = 0; i < temp.length(); i++)
        {
            Message->Msg_Type.Message_Type_Description[i] = temp[i];
        }
    }

    Message->Source_Task_ID = thread_ptr->PID;
    Message->Msg_Size = strlen(Message->Msg_text);

    struct thread *dest_thread_ptr = sched_ptr->getProcessTable()->getThreadByPID(Message->Destination_ID);

    // check for valid destination id
    if (dest_thread_ptr == nullptr || dest_thread_ptr->state == 'D')
    {
        return -1;
    }
    else
    {
        sched_ptr->wait_turn(sched_ptr, thread_ptr, dest_thread_ptr->my_mailbox->sema_mailbox->down(thread_ptr->PID));
        // put the message in both Queues
        // so that its easier to dump the entire mailbox
        dest_thread_ptr->my_mailbox->messagesInbox->Enqueue(Message);
        dest_thread_ptr->my_mailbox->messagesAll->Enqueue(Message);
        dest_thread_ptr->my_mailbox->sema_mailbox->up(thread_ptr->PID);

        // put the message in both Queues
        // so that its easier to dump the entire mailbox
        thread_ptr->my_mailbox->messagesAll->Enqueue(Message);
        thread_ptr->my_mailbox->messagesSent->Enqueue(Message);

        return 1;
    }
}
/******************** Message_Receive ***********************
Input: thread_ptr
Output: Function that will check the count on the message
        queue, do a down() on the task's mailbox semaphore,
        DeQueue and return the first message from the mailbox,
        and then do an up() on the task's mailbox.
*************************************************************/

int ipc::Message_Receive(thread *thread_ptr)
{
    struct message *load_message;

    sched_ptr->wait_turn(sched_ptr, thread_ptr, thread_ptr->my_mailbox->sema_mailbox->down(thread_ptr->PID));
    if (thread_ptr->my_mailbox->messagesInbox->isEmpty())
    {
        thread_ptr->my_mailbox->sema_mailbox->up(thread_ptr->PID);
        return 0;
    }
    else
    {
        load_message = thread_ptr->my_mailbox->messagesInbox->Dequeue();

        time_t seconds;
        time(&seconds);
        std::stringstream ss;
        ss << seconds;
        std::string ts = ss.str();

        load_message->Message_Arrival_Time = ts;

        thread_ptr->my_mailbox->currentMessage = load_message;
        thread_ptr->my_mailbox->messagesRead->Enqueue(load_message);
        thread_ptr->my_mailbox->sema_mailbox->up(thread_ptr->PID);

        if (load_message->Msg_Type.Message_Type_ID == 0)
        {
            return 1;
        }
        else if (load_message->Msg_Type.Message_Type_ID == 1)
            return 2;
    }
}
/********************** Message_Count ***********************
Input: thread_ptr
Output: Returns the number of messages in Task_ID's
        message queue
*************************************************************/
std::string ipc::Message_Count(thread *thread_ptr)
{
    string mail_count = "\n ---- Mailbox Count for " + to_string(thread_ptr->PID) + " ----";
    mail_count += "\n Messages inbox (to be read): " + to_string(thread_ptr->my_mailbox->messagesInbox->Size());
    mail_count += "\n Messages read: " + to_string(thread_ptr->my_mailbox->messagesRead->Size());
    mail_count += "\n Messages sent: " + to_string(thread_ptr->my_mailbox->messagesSent->Size()) + "\n";

    return mail_count;
}
/********************** Message_Count ***********************
Input: none
Output: Returns the total number of messages in all the
        message queues
*************************************************************/
std::string ipc::Message_Count()
{
    thread *temp = sched_ptr->getProcessTable()->getHead();
    int total_inbox = 0, total_read = 0, total_sent = 0;
    for (int i = 0; i < sched_ptr->getProcessTable()->getSize(); i++)
    {
        total_inbox += temp->my_mailbox->messagesInbox->Size();
        total_read += temp->my_mailbox->messagesRead->Size();
        total_sent += temp->my_mailbox->messagesSent->Size();
        temp = temp->next;
    }

    string total_count = "\n ---- Total Message Count ----";
    total_count += "\n Total inbox count: " + to_string(total_inbox);
    total_count += "\n Total read count: " + to_string(total_read);
    total_count += "\n Total sent count: " + to_string(total_sent) + "\n";

    return total_count;
}
/********************** Message_Echo ***********************
Input: thread_ptr
Output: Returns the source task, running task, destination
        task, message type, message size, and the message text
*************************************************************/
std::string ipc::Message_Echo(thread *thread_ptr)
{
    if (thread_ptr->my_mailbox->currentMessage)
    {
        string print_string = "\n ---- Message Print for Task: " + to_string(thread_ptr->PID) + " ----";
        print_string += "\n Source Task: " + to_string(thread_ptr->my_mailbox->currentMessage->Source_Task_ID);
        print_string += "\n Task Running: " + to_string(thread_ptr->my_mailbox->currentMessage->task_running);
        print_string += "\n Destination Task: " + to_string(thread_ptr->my_mailbox->currentMessage->Destination_ID);
        print_string += "\n -- Message Type -- \n Message Type ID: " + to_string(thread_ptr->my_mailbox->currentMessage->Msg_Type.Message_Type_ID);
        print_string += "\n Message Type Description: ";

        for (int i = 0; i < strlen(thread_ptr->my_mailbox->currentMessage->Msg_Type.Message_Type_Description); i++)
        {
            print_string += thread_ptr->my_mailbox->currentMessage->Msg_Type.Message_Type_Description[i];
        }

        print_string += "\n Message Size: " + to_string(thread_ptr->my_mailbox->currentMessage->Msg_Size);
        print_string += "\n Message text: ";

        int size = thread_ptr->my_mailbox->currentMessage->Msg_Size;
        for (int i = 0; i < size; i++)
        {
            print_string += thread_ptr->my_mailbox->currentMessage->Msg_text[i];
        }

        print_string += "\n";
        return print_string;
    }
    return "";
}

std::string ipc::Message_Print(thread *thread_ptr)
{
    string inbox_dump;

    inbox_dump += "\n Source | Dest ID | Size | Type | Arvl Time | Message Content\n";

    // go through easch message in thread_ptr's mailbox
    // and output each one
    for (int i = 0; i < thread_ptr->my_mailbox->messagesInbox->Size(); i++)
    {

        message *temp = thread_ptr->my_mailbox->messagesInbox->Dequeue();
        thread_ptr->my_mailbox->messagesInbox->Enqueue(temp);

        inbox_dump += " " + to_string(temp->Source_Task_ID) + "      | ";
        inbox_dump += to_string(temp->Destination_ID) + "       | ";
        inbox_dump += to_string(temp->Msg_Size) + "   | ";

        for (int i = 0; i < strlen(temp->Msg_Type.Message_Type_Description); i++)
        {
            inbox_dump += temp->Msg_Type.Message_Type_Description[i];
        }

        inbox_dump += "   | ";

        inbox_dump += to_string(temp->task_running);

        for (int i = 0; i < temp->Msg_Size; i++)
        {
            inbox_dump += temp->Msg_text[i];
        }

        inbox_dump += "\n";
    }

    inbox_dump += "\n -------------------------------------------------\n";

    return inbox_dump;
}
/********************** Message_Print ***********************
Input: thread_ptr
Output: Returns an organized heading to print
*************************************************************/
int ipc::Message_Delete_All(thread *thread_ptr)
{

    delete thread_ptr->my_mailbox->messagesInbox;
    delete thread_ptr->my_mailbox->messagesRead;
    delete thread_ptr->my_mailbox->messagesSent;
    delete thread_ptr->my_mailbox->messagesAll;

    thread_ptr->my_mailbox->messagesInbox = new Queue<message *>;
    thread_ptr->my_mailbox->messagesRead = new Queue<message *>;
    thread_ptr->my_mailbox->messagesSent = new Queue<message *>;
    thread_ptr->my_mailbox->messagesAll = new Queue<message *>;

    return 1;
}

/********************** ipc_dump ****************************
Input: none
Output: Returns the task number, message total count for the
        mailboxes, and a count for each mailbox
*************************************************************/
std::string ipc::ipc_dump()
{
    string inbox_dump = "\n";

    thread *thread_ptr = sched_ptr->getProcessTable()->getFirst();

    // go through all threads in process table
    for (int i = 0; i < sched_ptr->getProcessTable()->getSize(); i++)
    {
        Queue<message *> *copy_Queue = new Queue<message *>;

        inbox_dump += "\n Task Number: " + to_string(thread_ptr->PID);
        inbox_dump += "   Message Count: " + to_string(thread_ptr->my_mailbox->messagesAll->Size());
        inbox_dump += "\n Mailbox:";

        // check each threads messages
        if (thread_ptr->my_mailbox->messagesAll->isEmpty())
        {
            inbox_dump += "\n The thread has no messages. \n";
        }
        else
        {
            inbox_dump += "\n Source | Dest ID | Size | Type         | Arvl Time  | Message Content\n";

            // output each message for each thread
            while (!thread_ptr->my_mailbox->messagesAll->isEmpty())
            {
                inbox_dump += " " + to_string(thread_ptr->my_mailbox->messagesAll->Front()->Source_Task_ID) + "      | ";
                inbox_dump += to_string(thread_ptr->my_mailbox->messagesAll->Front()->Destination_ID) + "       | ";
                inbox_dump += to_string(thread_ptr->my_mailbox->messagesAll->Front()->Msg_Size) + "   |";

                for (int i = 0; i < strlen(thread_ptr->my_mailbox->messagesAll->Front()->Msg_Type.Message_Type_Description); i++)
                {
                    inbox_dump += thread_ptr->my_mailbox->messagesAll->Front()->Msg_Type.Message_Type_Description[i];
                }

                if (thread_ptr->my_mailbox->messagesAll->Front()->Msg_Type.Message_Type_ID == 0)
                {
                    inbox_dump += "         | ";
                }
                else if (thread_ptr->my_mailbox->messagesAll->Front()->Msg_Type.Message_Type_ID == 1)
                {
                    inbox_dump += "      | ";
                }
                else
                {
                    inbox_dump += " | ";
                }

                inbox_dump += thread_ptr->my_mailbox->messagesAll->Front()->Message_Arrival_Time;
                if (thread_ptr->my_mailbox->messagesAll->Front()->Message_Arrival_Time == "")
                {
                    inbox_dump += "In Transit | ";
                }
                else
                {
                    inbox_dump += " | ";
                }

                for (int i = 0; i < thread_ptr->my_mailbox->messagesAll->Front()->Msg_Size; i++)
                {
                    inbox_dump += thread_ptr->my_mailbox->messagesAll->Front()->Msg_text[i];
                }

                inbox_dump += "\n";

                copy_Queue->Enqueue(thread_ptr->my_mailbox->messagesAll->Front());
                thread_ptr->my_mailbox->messagesAll->Dequeue();
            }

            thread_ptr->my_mailbox->messagesAll = copy_Queue;

            thread_ptr = thread_ptr->next;
        }
    }

    inbox_dump += "\n";

    return inbox_dump;
}
