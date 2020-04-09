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
	    // Node* temp;
        
	public:
		Queue();
		void Enqueue(const T datum);
		T Dequeue();
		T Front();
        bool isEmpty();
        int Size();
        ~Queue();
};

template <class T>
Queue<T>::Queue()
{
	head = NULL;
	tail = NULL;
	// temp = NULL;
}

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

template <class T>
T Queue<T>::Dequeue()
{
    struct Node * temp = new Node();
	T return_Datum;

	return_Datum = head->datum;
	temp = head->next;
	delete head;
	head = temp;
	tail = (!head? NULL: tail);
	return return_Datum;
}

template <class T>
T Queue<T>::Front()
{
	T return_Datum;
	return_Datum = head->datum;
	return return_Datum;
}

template <class T>
bool Queue<T>::isEmpty()
{
  return (!tail);
}

template <class T>
int Queue<T>::Size()
{
	int count = 0;
    struct Node * temp = new Node();
	temp = head;
	
	if (isEmpty())
    {
		return count;	
	}
	else if (temp->next == NULL)
    {
		count++;
		return count;
	}
	else
    {
		while (temp != NULL)
        {
			count++;
			temp = temp->next;
		}
		
	    return count;
	}
}

template <class T>
Queue<T>::~Queue(){
    Node* temp;
    while(head){       
        temp=head->next;
        delete head;
        head = temp;
    }
}
