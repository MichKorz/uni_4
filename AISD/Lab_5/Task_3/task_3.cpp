#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <random>
#include <iomanip> // For std::fixed and std::setprecision
#include <limits>  // For std::numeric_limits

// Globalny licznik porównań kluczy
long long comparison_count = 0;

struct Node {
    int key;
    int degree;
    Node *parent;
    Node *child;
    Node *sibling;

    Node(int k) : key(k), degree(0), parent(nullptr), child(nullptr), sibling(nullptr) {}
};

class BinomialHeap {
public:
    Node *head;

    BinomialHeap() : head(nullptr) {}

    ~BinomialHeap() {
        clearAllNodes();
    }

    void clearAllNodes() {
        Node* current_root = head;
        while (current_root != nullptr) {
            Node* next_root_in_list = current_root->sibling;
            destroyTreeRecursive(current_root);
            current_root = next_root_in_list;
        }
        head = nullptr;
    }

    // Funkcja Make-Heap jest realizowana przez konstruktor tworzący pusty kopiec
    static BinomialHeap makeHeapInstance() {
        return BinomialHeap();
    }


    // Łączy dwa drzewa tego samego stopnia. y staje się dzieckiem z.
    // Zakłada, że klucz y jest nie mniejszy niż klucz z (lub decyzja została podjęta przez wołającego).
    static void link(Node* y, Node* z) {
        // Porównanie kluczy jest zliczane przez funkcję wołającą (heapUnion)
        y->parent = z;
        y->sibling = z->child;
        z->child = y;
        z->degree++;
    }

    // Scala listy korzeni H1 i H2 w jedną posortowaną listę według stopni.
    // Nie wykonuje porównań kluczy.
    static Node* mergeRootLists(Node* h1_head, Node* h2_head) {
        if (!h1_head) return h2_head;
        if (!h2_head) return h1_head;

        Node *new_head = nullptr;
        Node *tail = nullptr;
        Node *curr1 = h1_head;
        Node *curr2 = h2_head;

        // Ustalenie pierwszego elementu nowej listy
        if (curr1->degree <= curr2->degree) {
            new_head = curr1;
            curr1 = curr1->sibling;
        } else {
            new_head = curr2;
            curr2 = curr2->sibling;
        }
        tail = new_head;

        // Scalanie pozostałych elementów
        while (curr1 && curr2) {
            if (curr1->degree <= curr2->degree) {
                tail->sibling = curr1;
                curr1 = curr1->sibling;
            } else {
                tail->sibling = curr2;
                curr2 = curr2->sibling;
            }
            tail = tail->sibling;
        }

        // Dołączenie reszty jednej z list
        if (curr1) {
            tail->sibling = curr1;
        } else {
            tail->sibling = curr2;
        }
        return new_head;
    }

    // Scala ten kopiec z innym kopcem (other_heap).
    // Ten kopiec (this) staje się wynikiem operacji, other_heap jest opróżniany.
    void heapUnion(BinomialHeap& other_heap) {
        Node* merged_roots_head = mergeRootLists(this->head, other_heap.head);
        
        this->head = nullptr; // Przygotowanie this->head na nowy scalony kopiec
        other_heap.head = nullptr; // other_heap zostanie opróżniony

        if (!merged_roots_head) {
            return; // Oba kopce były puste lub stały się puste
        }

        Node *prev_x = nullptr;
        Node *x = merged_roots_head;
        Node *next_x = x->sibling;

        while (next_x) {
            if ((x->degree != next_x->degree) ||
                (next_x->sibling && next_x->sibling->degree == x->degree)) {
                // Przypadki 1 & 2: Przesuń wskaźniki dalej
                prev_x = x;
                x = next_x;
            } else {
                // Przypadki 3 & 4: x->degree == next_x->degree
                comparison_count++; // Porównanie kluczy x i next_x
                if (x->key <= next_x->key) {
                    // x staje się rodzicem next_x
                    x->sibling = next_x->sibling;
                    link(next_x, x);
                    // x pozostaje x
                } else {
                    // next_x staje się rodzicem x
                    if (prev_x == nullptr) { // Jeśli x był pierwszym korzeniem
                        merged_roots_head = next_x;
                    } else {
                        prev_x->sibling = next_x;
                    }
                    link(x, next_x);
                    x = next_x; // x przesuwa się na next_x (nowy korzeń połączonego drzewa)
                }
            }
            next_x = x->sibling;
        }
        this->head = merged_roots_head; // Ustawienie głowy scalonego kopca
    }

    // Wstawia nowy klucz do kopca
    void insert(int key) {
        Node* new_node = new Node(key);
        BinomialHeap temp_heap;
        temp_heap.head = new_node;
        this->heapUnion(temp_heap); // temp_heap.head zostanie ustawiony na nullptr przez heapUnion
    }

    // Zwraca wskaźnik do węzła z minimalnym kluczem w liście korzeni
    Node* findMinNodeInRootList() {
        if (!head) return nullptr;
        Node *min_node = head;
        Node *current = head->sibling;
        while (current) {
            comparison_count++; // Porównanie kluczy min_node->key i current->key
            if (current->key < min_node->key) {
                min_node = current;
            }
            current = current->sibling;
        }
        return min_node;
    }

    // Usuwa i zwraca minimalny klucz z kopca
    int extractMin() {
        if (!head) {
            std::cerr << "Błąd: Próba ExtractMin na pustym kopcu." << std::endl;
            // W praktyce można rzucić wyjątek lub zwrócić specjalną wartość
            return std::numeric_limits<int>::min(); 
        }

        // 1. Znajdź korzeń x z minimalnym kluczem
        Node *min_node = head;
        Node *min_node_prev = nullptr;
        Node *current_iter = head->sibling;
        Node *current_prev_iter = head;

        while (current_iter) {
            comparison_count++; // Porównanie kluczy
            if (current_iter->key < min_node->key) {
                min_node = current_iter;
                min_node_prev = current_prev_iter;
            }
            current_prev_iter = current_iter;
            current_iter = current_iter->sibling;
        }

        // 2. Usuń x z listy korzeni H
        if (min_node == head) {
            head = min_node->sibling;
        } else {
            min_node_prev->sibling = min_node->sibling;
        }

        // 3. Utwórz nowy kopiec H' z dzieci min_node
        BinomialHeap children_heap;
        Node *reversed_child_list_head = nullptr;
        Node *child_iter = min_node->child;
        while (child_iter) {
            Node *next_child = child_iter->sibling;
            child_iter->sibling = reversed_child_list_head;
            child_iter->parent = nullptr; // Dzieci stają się korzeniami w H'
            reversed_child_list_head = child_iter;
            child_iter = next_child;
        }
        children_heap.head = reversed_child_list_head;

        // 4. H = Union(H, H')
        this->heapUnion(children_heap); // children_heap zostanie opróżniony

        int min_key_val = min_node->key;
        delete min_node; 
        return min_key_val;
    }

    bool isEmpty() const {
        return head == nullptr;
    }

private:
    void destroyTreeRecursive(Node* node) {
        if (node == nullptr) {
            return;
        }
        Node* child = node->child;
        while (child != nullptr) {
            Node* next_child = child->sibling;
            destroyTreeRecursive(child);
            child = next_child;
        }
        delete node;
    }
};

// Funkcja pomocnicza do sprawdzania, czy wektor jest posortowany
bool is_vector_sorted(const std::vector<int>& vec) {
    for (size_t i = 0; i + 1 < vec.size(); ++i) {
        if (vec[i] > vec[i + 1]) {
            return false;
        }
    }
    return true;
}

// Eksperyment dla n=500
void run_experiment_n500(int run_id) {
    const int n_val = 500;
    std::ofstream outfile("n500_run_" + std::to_string(run_id) + ".txt");
    if (!outfile.is_open()) {
        std::cerr << "Błąd otwarcia pliku: n500_run_" << run_id << ".txt" << std::endl;
        return;
    }

    std::vector<long long> comparisons_per_extract_min; 

    // Inicjalizacja generatora liczb losowych
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 1000000); // Zakres losowych liczb

    // Reset globalnego licznika porównań dla tego konkretnego przebiegu eksperymentu
    comparison_count = 0; 

    // 1. Utwórz dwa puste kopce H1 i H2
    BinomialHeap h1 = BinomialHeap::makeHeapInstance();
    BinomialHeap h2 = BinomialHeap::makeHeapInstance();

    // 2. Wstaw losowe elementy do H1 i H2
    for (int i = 0; i < n_val; ++i) {
        h1.insert(distrib(gen));
        h2.insert(distrib(gen));
    }
    
    // 3. Scal H1 i H2 w jeden kopiec H
    BinomialHeap h_merged = BinomialHeap::makeHeapInstance();
    h_merged.heapUnion(h1); // h1 staje się częścią h_merged i jest opróżniany
    h_merged.heapUnion(h2); // h2 staje się częścią h_merged i jest opróżniany


    // 4. Wykonaj 2*n_val operacji Extract-Min
    std::vector<int> extracted_keys;
    for (int i = 0; i < 2 * n_val; ++i) {
        if (h_merged.isEmpty()) {
            std::cerr << "Błąd: Kopiec stał się pusty przed wykonaniem 2*n operacji Extract-Min. Przebieg n500: " << run_id << ", operacja: " << i + 1 << std::endl;
            break;
        }
        long long comparisons_before_op = comparison_count;
        extracted_keys.push_back(h_merged.extractMin());
        long long comparisons_after_op = comparison_count;
        comparisons_per_extract_min.push_back(comparisons_after_op - comparisons_before_op);
    }

    // Zapis liczby porównań dla każdej operacji Extract-Min
    for (size_t i = 0; i < comparisons_per_extract_min.size(); ++i) {
        outfile << (i + 1) << " " << comparisons_per_extract_min[i] << std::endl;
    }
    outfile.close();

    // Weryfikacja
    if (!is_vector_sorted(extracted_keys)) {
        std::cerr << "Błąd: Wyekstrahowane elementy nie są posortowane. Przebieg n500: " << run_id << std::endl;
    }
    if (extracted_keys.size() == static_cast<size_t>(2 * n_val) && !h_merged.isEmpty()) {
        std::cerr << "Błąd: Kopiec nie jest pusty po 2*n operacjach Extract-Min. Przebieg n500: " << run_id << std::endl;
    }
     if (extracted_keys.size() != static_cast<size_t>(2 * n_val) && !h_merged.isEmpty()) { // Check if prematurely empty
        std::cerr << "Ostrzeżenie: Wyekstrahowano " << extracted_keys.size() << " elementów zamiast " << 2*n_val << ". Przebieg n500: " << run_id << std::endl;
    }


    std::cout << "Zakończono przebieg n=500, ID: " << run_id << ". Dane w pliku n500_run_" << run_id << ".txt" << std::endl;
}

// Eksperyment dla różnych wartości n
void run_experiment_varying_n(const std::vector<int>& n_values_list, std::ofstream& avg_cost_file) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 10000000); // Szerszy zakres dla większych n

    for (int current_n : n_values_list) {
        comparison_count = 0; // Reset licznika dla tej wartości n

        BinomialHeap h1 = BinomialHeap::makeHeapInstance();
        BinomialHeap h2 = BinomialHeap::makeHeapInstance();

        // Wstawianie
        for (int i = 0; i < current_n; ++i) {
            h1.insert(distrib(gen));
            h2.insert(distrib(gen));
        }

        // Scalanie
        BinomialHeap h_merged = BinomialHeap::makeHeapInstance();
        h_merged.heapUnion(h1);
        h_merged.heapUnion(h2);
        
        // Extract-Min
        std::vector<int> extracted_keys_varying_n;
        for (int i = 0; i < 2 * current_n; ++i) {
            if (h_merged.isEmpty()) {
                std::cerr << "Błąd: Kopiec pusty przedwcześnie dla n = " << current_n << " przy operacji " << i+1 << std::endl;
                break;
            }
            extracted_keys_varying_n.push_back(h_merged.extractMin());
        }

        // Weryfikacja (opcjonalna, ale dobra praktyka)
        if (!is_vector_sorted(extracted_keys_varying_n)) {
            std::cerr << "Błąd: Wyekstrahowane elementy nie są posortowane dla n = " << current_n << std::endl;
        }
        if (extracted_keys_varying_n.size() == static_cast<size_t>(2 * current_n) && !h_merged.isEmpty()) {
            std::cerr << "Błąd: Kopiec nie jest pusty po 2*n operacjach Extract-Min dla n = " << current_n << std::endl;
        }
        if (extracted_keys_varying_n.size() != static_cast<size_t>(2 * current_n) && !h_merged.isEmpty()) {
             std::cerr << "Ostrzeżenie: Wyekstrahowano " << extracted_keys_varying_n.size() << " elementów zamiast " << 2*current_n << " dla n = " << current_n << std::endl;
        }


        // Całkowita liczba porównań dla wszystkich operacji w eksperymencie (2n insertów, 1 Union, 2n ExtractMin)
        // podzielona przez n
        double average_cost_per_n_unit = static_cast<double>(comparison_count) / current_n;
        avg_cost_file << current_n << " " << std::fixed << std::setprecision(6) << average_cost_per_n_unit << std::endl;
        std::cout << "Zakończono eksperyment dla n = " << current_n 
                  << ". Całkowita liczba porównań: " << comparison_count 
                  << ". Średni koszt/n: " << average_cost_per_n_unit << std::endl;
    }
}


int main() {
    std::cout << "Rozpoczęcie eksperymentów z kopcem dwumianowym..." << std::endl;

    // Eksperyment 1: n = 500, 5 przebiegów
    std::cout << "\n--- Eksperyment 1: n = 500 (5 przebiegów) ---" << std::endl;
    for (int i = 1; i <= 5; ++i) {
        run_experiment_n500(i);
    }

    // Eksperyment 2: Zmienne n
    std::cout << "\n--- Eksperyment 2: Zmienne n (100 do 10000) ---" << std::endl;
    std::vector<int> n_values_list;
    for (int n_val = 100; n_val <= 10000; n_val += 100) {
        n_values_list.push_back(n_val);
    }

    std::ofstream avg_cost_file("avg_cost_vs_n.txt");
    if (!avg_cost_file.is_open()) {
        std::cerr << "Błąd krytyczny: Nie można otworzyć pliku avg_cost_vs_n.txt do zapisu." << std::endl;
        return 1;
    }
    run_experiment_varying_n(n_values_list, avg_cost_file);
    avg_cost_file.close();

    return 0;
}

