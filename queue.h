#ifndef QUEUE_H
#define QUEUE_H
// Queue template class

#include <cstddef>

template <class T>
class Queue
{
    private:
        struct Node
        {
	        T datum;
		    Node* next;
	    };

        Node* head;
        Node* tail;
        
	public:
		Queue();
		void Enqueue(const T datum);
		T Dequeue();
		T Front();
        bool isEmpty();
        int Size();
        ~Queue();
};

/******************* Queue Constructor **********************
Input: none
Output: none
*************************************************************/

template <class T>
Queue<T>::Queue()
{
	head = NULL;
	tail = NULL;
}

/********************** Enqueue *****************************
Input: datum
Output: enqueue a new node
*************************************************************/

template <class T> 
void Queue<T>::Enqueue(const T datum)
{
	if (tail == NULL)
    {
		head = tail = new Node;
		tail->next = NULL;
		tail->datum = datum;
	}
	else 
    {
	    tail->next = new Node;
	    tail->next->datum = datum;
	    tail->next->next = NULL;
	    tail = tail->next;
	}
}

/********************** Dequeue *****************************
Input: none
Output: returns datum stored in the node that was dequeued
*************************************************************/

template <class T>
T Queue<T>::Dequeue()
{
    struct Node * temp;
	T return_Datum;

	return_Datum = head->datum;
	temp = head->next;
	delete head;
	head = temp;
	tail = (!head? NULL: tail);
	return return_Datum;
}

/************************ Front *****************************
Input: none
Output: return the datum that is the front
*************************************************************/

template <class T>
T Queue<T>::Front()
{
	T return_Datum;
	return_Datum = head->datum;
	return return_Datum;
}

/************************ isEmpty ***************************
Input: none
Output: checks if the queue is empty or not
*************************************************************/

template <class T>
bool Queue<T>::isEmpty()
{
  return (!tail);
}

/************************* Size *****************************
Input: none
Output: returns the size in the queue
*************************************************************/

template <class T>
int Queue<T>::Size()
{
	struct Node *temp;
	temp = head;
	int count = 0;
	
	while(temp){
		count++;
		temp = temp->next;
	}
	return count;
}

/************************ Queue Destructor ******************
Input: none
Output: none
*************************************************************/

template <class T>
Queue<T>::~Queue(){
	tail = head;
    while(tail){
        head = tail->next;
        delete tail;
        tail = head;
    }
}

#endif
