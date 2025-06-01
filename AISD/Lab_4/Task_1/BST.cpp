#include <iostream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

// Global counters
long long key_comparisons = 0;
long long pointer_accesses = 0;

// Node structure for a Binary Search Tree
struct Node {
    int data;
    Node* left;
    Node* right;
};

// Function to create a new Node
Node* createNode(int data)
{
    Node* newNode = new Node();
    newNode->data = data;
    newNode->left = newNode->right = nullptr;
    return newNode;
}

// Function to insert a node in the BST
Node* insertNode(Node* root, int data) {
    pointer_accesses++;  // root access
    if (root == nullptr) {
        return createNode(data);
    }

    key_comparisons++;
    if (data < root->data) {
        pointer_accesses++;  // root->left
        root->left = insertNode(root->left, data);
        pointer_accesses++;  // root->left assignment
    }
    else {
        key_comparisons++;
        pointer_accesses++;  // root->right
        root->right = insertNode(root->right, data);
        pointer_accesses++;  // root->right assignment
    }
    return root;
}

// Function to do inorder traversal of BST
void inorderTraversal(Node* root)
{
    if (root != nullptr) {
        inorderTraversal(root->left);
        cout << root->data << " ";
        inorderTraversal(root->right);
    }
}

// Function to find the inorder successor
Node* minValueNode(Node* node) {
    pointer_accesses++;  // node access
    Node* current = node;
    while (current && current->left != nullptr) {
        pointer_accesses += 2;  // current + current->left
        current = current->left;
        pointer_accesses++;  // assignment
    }
    return current;
}

// Function to delete a node
Node* deleteNode(Node* root, int data) {
    pointer_accesses++;  // root access
    if (root == nullptr)
        return root;

    key_comparisons++;
    if (data < root->data) {
        pointer_accesses++;  // root->left
        root->left = deleteNode(root->left, data);
        pointer_accesses++;  // root->left assignment
    }
    else {
        key_comparisons++;
        if (data > root->data) {
            pointer_accesses++;  // root->right
            root->right = deleteNode(root->right, data);
            pointer_accesses++;  // root->right assignment
        } else {
            // Deletion cases
            pointer_accesses += 2;  // root->left, root->right
            if (root->left == nullptr) {
                Node* temp = root->right;
                pointer_accesses++;  // temp = root->right
                delete root;
                return temp;
            }
            else if (root->right == nullptr) {
                Node* temp = root->left;
                pointer_accesses++;  // temp = root->left
                delete root;
                return temp;
            }

            Node* temp = minValueNode(root->right);
            pointer_accesses++;  // temp assignment
            root->data = temp->data;
            root->right = deleteNode(root->right, temp->data);
            pointer_accesses++;  // root->right assignment
        }
    }
    return root;
}

// Function to calculate the height of the tree
int height(Node* root)
{
    if (root == nullptr)
        return -1;
    return 1 + max(height(root->left), height(root->right));
}

// Function to print the BST
void print_BST(Node* root, int depth, char prefix, char* left_trace, char* right_trace) {
    if (root == nullptr) return;
    if (root->left != nullptr) {
        print_BST(root->left, depth + 1, '/', left_trace, right_trace);
    }
    if (prefix == '/') left_trace[depth - 1] = '|';
    if (prefix == '\\') right_trace[depth - 1] = ' ';
    if (depth == 0) printf("-");
    if (depth > 0) printf(" ");
    for (int i = 0; i < depth - 1; i++)
        if (left_trace[i] == '|' || right_trace[i] == '|') {
            printf("| ");
        } else {
            printf("  ");
        }
    if (depth > 0) printf("%c-", prefix);
    printf("[%d]\n", root->data);
    left_trace[depth] = ' ';
    if (root->right != nullptr) {
        right_trace[depth] = '|';
        print_BST(root->right, depth + 1, '\\', left_trace, right_trace);
    }
}

void runExperiment(int n, bool shuffledInsert, int trial) {
    vector<int> keys(n);
    for (int i = 0; i < n; ++i) keys[i] = i + 1;
    if (shuffledInsert) random_shuffle(keys.begin(), keys.end());

    Node* root = nullptr;
    long long totalCmp = 0, totalPtr = 0;
    int maxCmp = 0, maxPtr = 0, maxHeight = 0;

    for (int i = 0; i < n; ++i) {
        key_comparisons = 0;
        pointer_accesses = 0;
        root = insertNode(root, keys[i]);
        int h = height(root);
        totalCmp += key_comparisons;
        totalPtr += pointer_accesses;
        maxCmp = max(maxCmp, (int)key_comparisons);
        maxPtr = max(maxPtr, (int)pointer_accesses);
        maxHeight = max(maxHeight, h);
    }

    random_shuffle(keys.begin(), keys.end());
    for (int i = 0; i < n; ++i) {
        key_comparisons = 0;
        pointer_accesses = 0;
        root = deleteNode(root, keys[i]);
        int h = height(root);
        totalCmp += key_comparisons;
        totalPtr += pointer_accesses;
        maxCmp = max(maxCmp, (int)key_comparisons);
        maxPtr = max(maxPtr, (int)pointer_accesses);
        maxHeight = max(maxHeight, h);
    }

    cout << n << "," << (shuffledInsert ? "random" : "sorted") << ","
         << trial << "," << (2 * n) << "," 
         << (totalCmp / (2 * n)) << "," << maxCmp << ","
         << (totalPtr / (2 * n)) << "," << maxPtr << ","
         << maxHeight << endl;
}

void runDemonstration()
{
    const int n = 30;
    srand(time(nullptr));
    vector<int> keys(n);
    for (int i = 0; i < n; ++i) keys[i] = i + 1;

    // Test 1: Insert increasing, delete random
    cout << "=== Test 1: Insert increasing, delete random ===\n";
    Node* root1 = nullptr;
    for (int key : keys) {
        root1 = insertNode(root1, key);
        int h = height(root1) + 1;
        char* lt = new char[h + 1];
        char* rt = new char[h + 1];
        fill(lt, lt + h + 1, ' ');
        fill(rt, rt + h + 1, ' ');
        cout << "After inserting " << key << ":\n";
        print_BST(root1, 0, '-', lt, rt);
        cout << endl;
        delete[] lt;
        delete[] rt;
    }

    random_shuffle(keys.begin(), keys.end());
    for (int key : keys) {
        root1 = deleteNode(root1, key);
        int h = height(root1) + 1;
        char* lt = new char[h + 1];
        char* rt = new char[h + 1];
        fill(lt, lt + h + 1, ' ');
        fill(rt, rt + h + 1, ' ');
        cout << "After deleting " << key << ":\n";
        print_BST(root1, 0, '-', lt, rt);
        cout << endl;
        delete[] lt;
        delete[] rt;
    }

    // Test 2: Insert random, delete random
    cout << "\n=== Test 2: Insert random, delete random ===\n";
    Node* root2 = nullptr;
    random_shuffle(keys.begin(), keys.end());
    for (int key : keys) {
        root2 = insertNode(root2, key);
        int h = height(root2) + 1;
        char* lt = new char[h + 1];
        char* rt = new char[h + 1];
        fill(lt, lt + h + 1, ' ');
        fill(rt, rt + h + 1, ' ');
        cout << "After inserting " << key << ":\n";
        print_BST(root2, 0, '-', lt, rt);
        cout << endl;
        delete[] lt;
        delete[] rt;
    }

    random_shuffle(keys.begin(), keys.end());
    for (int key : keys) {
        root2 = deleteNode(root2, key);
        int h = height(root2) + 1;
        char* lt = new char[h + 1];
        char* rt = new char[h + 1];
        fill(lt, lt + h + 1, ' ');
        fill(rt, rt + h + 1, ' ');
        cout << "After deleting " << key << ":\n";
        print_BST(root2, 0, '-', lt, rt);
        cout << endl;
        delete[] lt;
        delete[] rt;
    }
}

// Main function
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <mode>\n";
        std::cerr << "Mode 0: Complexity experiment (large n, 20 trials, no printing)\n";
        std::cerr << "Mode 1: Demonstration with printing for n=30\n";
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "0") {
        // Print CSV header for analysis
        std::cout << "n,scenario,trial,operations,avg_key_cmp,max_key_cmp,avg_ptr_ops,max_ptr_ops,max_height\n";
        for (int n = 10000; n <= 100000; n += 10000) {
            for (int trial = 1; trial <= 1; ++trial) {
                runExperiment(n, false, trial);  // Sorted insert + random delete
                runExperiment(n, true, trial);   // Random insert + random delete
            }
        }
    } else if (mode == "1") {
        runDemonstration();  // Insert, delete, and print BST with n=30
    } else {
        std::cerr << "Invalid mode. Use 0 or 1.\n";
        return 1;
    }

    return 0;
}
