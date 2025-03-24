#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

struct Node
{
    int data;
    Node* next;
    Node* prev;
};

struct List
{
    int count;
    Node* head;
};

void insert(List &l, int value)
{
    Node* newNode = new Node;
    newNode->data = value;
    if (l.head == nullptr)
    {
        newNode->next = newNode;
        newNode->prev = newNode;
        l.head = newNode;
        l.count = 1;
    }
    else
    {
        Node* tail = l.head->prev;
        newNode->next = l.head;
        newNode->prev = tail;
        tail->next = newNode;
        l.head->prev = newNode;
        l.head = newNode;
        l.count++;
    }
}

void printList(const List &l)
{
    if (l.head == nullptr)
  {
        cout << "Lista jest pusta." << endl;
        return;
    }
    Node* current = l.head;
    for (int i = 0; i < l.count; i++)
    {
        cout << current->data << " ";
        current = current->next;
    }
    cout << endl;
}

void merge(List &l1, List &l2)
{
    if (l1.head == nullptr || l2.head == nullptr)
    {
        cout << "At least one list is empty" << endl;
        return;
    }

    Node* tail1 = l1.head->prev;
    Node* tail2 = l2.head->prev;

    tail1->next = l2.head;
    l2.head->prev = tail1;
    tail2->next = l1.head;
    l1.head->prev = tail2;
    l1.count += l2.count;

    l2.head = nullptr;
    l2.count = 0;
}

int searchCost(const List &l, int target)
{
    if (l.head == nullptr) return 0;
    int comparisons = 0;

    int direction = rand() % 2;
    Node* current = (direction == 0) ? l.head : l.head->prev;
    for (int i = 0; i < l.count; i++)
    {
        comparisons++;
        if (current->data == target)
        {
            return comparisons;
        }
        current = (direction == 0) ? current->next : current->prev;
    }
    return comparisons;
}

int main()
{
    srand(static_cast<unsigned>(time(NULL)));


    List list1 = {0, nullptr};
    List list2 = {0, nullptr};


    for (int i = 10; i < 20; i++) {
        insert(list1, i);
    }
    for (int i = 20; i < 30; i++) {
        insert(list2, i);
    }

    cout << "List 1:" << endl;
    printList(list1);
    cout << "List 2:" << endl;
    printList(list2);


    merge(list1, list2);
    cout << "After merging:" << endl;
    printList(list1);


    const int T_SIZE = 10000;
    const int SEARCH_COUNT = 1000;
    const int I_min = 0;
    const int I_max = 100000;


    vector<int> T(T_SIZE);
    for (int i = 0; i < T_SIZE; i++) {
        T[i] = I_min + rand() % (I_max - I_min + 1);
    }


    List L = {0, nullptr};
    for (int i = 0; i < T_SIZE; i++) {
        insert(L, T[i]);
    }


    long long totalCostIn = 0;
    for (int i = 0; i < SEARCH_COUNT; i++) {
        int index = rand() % T_SIZE;
        int target = T[index];
        totalCostIn += searchCost(L, target);
    }
    double avgCostIn = static_cast<double>(totalCostIn) / SEARCH_COUNT;


    long long totalCostNotIn = 0;
    for (int i = 0; i < SEARCH_COUNT; i++) {
        int target = I_min + rand() % (I_max - I_min + 1);
        totalCostNotIn += searchCost(L, target);
    }
    double avgCostNotIn = static_cast<double>(totalCostNotIn) / SEARCH_COUNT;

    cout << "\nAverage search cost (for present elements): " << avgCostIn << endl;
    cout << "Average search cost (for elements from I): " << avgCostNotIn << endl;

    return 0;
}
