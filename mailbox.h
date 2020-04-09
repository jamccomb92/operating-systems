#ifndef MAIL_H
#define MAIL_H

#include "queue.h"
#include "message.h"
#include "semaphore.h"

struct mailbox {
    	Queue<message *> *messagesInbox;
		Queue<message *> *messagesSent;
		Queue<message *> *messagesRead;
		Queue<message *> *messagesAll;
		message *currentMessage;
		semaphore * sema_mailbox;
};

#endif

