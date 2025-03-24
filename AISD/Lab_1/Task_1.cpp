#include <iostream>

using namespace std;

// =============================
// FIFO
// =============================

struct QNode
{
    int data;
    QNode* next;
};

class MyQueue
{
private:
    QNode* head;
    QNode* tail;
public:
    MyQueue() : head(nullptr), tail(nullptr) { }

    ~MyQueue()
    {
        while (!isEmpty())
        {
            dequeue();
        }
    }

    bool isEmpty()
    {
        return head == nullptr;
    }


    void enqueue(int x)
    {
        QNode* newNode = new QNode;
        newNode->data = x;
        newNode->next = nullptr;
        if (isEmpty())
            {
            head = tail = newNode;
        }
        else
            {
            tail->next = newNode;
            tail = newNode;
        }
    }

    int dequeue()
    {
        if (isEmpty())
        {
            cout << "Queue is empty" << endl;
            return -1;
        }
        QNode* temp = head;
        int value = head->data;
        head = head->next;
        if (head == nullptr)
        {
            tail = nullptr;
        }
        delete temp;
        return value;
    }
};

// =============================
// LIFO
// =============================

struct SNode
{
    int data;
    SNode* next;
};

class MyStack
{
private:
    SNode* top;
public:
    MyStack() : top(nullptr) { }

    ~MyStack() {
        while (!isEmpty())
        {
            pop();
        }
    }

    bool isEmpty()
    {
        return top == nullptr;
    }

    void push(int x)
    {
        SNode* newNode = new SNode;
        newNode->data = x;
        newNode->next = top;
        top = newNode;
    }

    int pop()
    {
        if (isEmpty())
        {
            cout << "Stack is empty" << endl;
            return -1;
        }
        SNode* temp = top;
        int value = top->data;
        top = top->next;
        delete temp;
        return value;
    }
};

// =============================
// Testing
// =============================

int main()
{
    MyQueue queue;
    MyStack stack;

    cout << "=== Queue (FIFO) ===" << endl;
    cout << "Adding elements: ";
    for (int i = 1; i <= 50; i++)
    {
        queue.enqueue(i);
        cout << i << " ";
    }
    cout << "\n\nExtracting elements: ";
    while (!queue.isEmpty())
    {
        int val = queue.dequeue();
        cout << val << " ";
    }
    cout << endl;

    cout << "\nTrying to extract from an empty queue:" << endl;
    queue.dequeue();

    cout << "\n=== Stack (LIFO) ===" << endl;
    cout << "Adding elements: ";
    for (int i = 1; i <= 50; i++)
    {
        stack.push(i);
        cout << i << " ";
    }
    cout << "\n\nExtracting elements: ";
    while (!stack.isEmpty())
    {
        int val = stack.pop();
        cout << val << " ";
    }
    cout << endl;

    cout << "\nTrying to extract from an empty stack:" << endl;
    stack.pop();

    return 0;
}