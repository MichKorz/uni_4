#include <iostream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>

using namespace std;

// Global counters
long long key_comparisons = 0;
long long pointer_accesses = 0;

enum Color { RED, BLACK };

struct Node {
    int data;
    bool color;
    Node* left;
    Node* right;
    Node* parent;
};

Node* NIL = new Node{0, BLACK, nullptr, nullptr, nullptr};

Node* createNode(int data) {
    Node* node = new Node;
    node->data = data;
    node->color = RED;
    node->left = NIL;
    node->right = NIL;
    node->parent = nullptr;
    return node;
}

void leftRotate(Node*& root, Node* x) {
    pointer_accesses += 3;
    Node* y = x->right;
    x->right = y->left;
    if (y->left != NIL) {
        pointer_accesses++;
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nullptr) root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void rightRotate(Node*& root, Node* y) {
    pointer_accesses += 3;
    Node* x = y->left;
    y->left = x->right;
    if (x->right != NIL) {
        pointer_accesses++;
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == nullptr) root = x;
    else if (y == y->parent->right) y->parent->right = x;
    else y->parent->left = x;
    x->right = y;
    y->parent = x;
}

void fixInsert(Node*& root, Node* k) {
    while (k != root && k->parent->color == RED) {
        pointer_accesses += 2;
        if (k->parent == k->parent->parent->left) {
            Node* u = k->parent->parent->right;
            if (u->color == RED) {
                u->color = BLACK;
                k->parent->color = BLACK;
                k->parent->parent->color = RED;
                k = k->parent->parent;
            } else {
                if (k == k->parent->right) {
                    k = k->parent;
                    leftRotate(root, k);
                }
                k->parent->color = BLACK;
                k->parent->parent->color = RED;
                rightRotate(root, k->parent->parent);
            }
        } else {
            Node* u = k->parent->parent->left;
            if (u->color == RED) {
                u->color = BLACK;
                k->parent->color = BLACK;
                k->parent->parent->color = RED;
                k = k->parent->parent;
            } else {
                if (k == k->parent->left) {
                    k = k->parent;
                    rightRotate(root, k);
                }
                k->parent->color = BLACK;
                k->parent->parent->color = RED;
                leftRotate(root, k->parent->parent);
            }
        }
    }
    root->color = BLACK;
}

void transplant(Node*& root, Node* u, Node* v) {
    if (u->parent == nullptr) root = v;
    else if (u == u->parent->left) u->parent->left = v;
    else u->parent->right = v;
    v->parent = u->parent;
}

Node* treeMinimum(Node* node) {
    while (node->left != NIL) {
        pointer_accesses++;
        node = node->left;
    }
    return node;
}

void fixDelete(Node*& root, Node* x) {
    while (x != root && x->color == BLACK) {
        if (x == x->parent->left) {
            Node* w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                leftRotate(root, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rightRotate(root, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                leftRotate(root, x->parent);
                x = root;
            }
        } else {
            Node* w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rightRotate(root, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    leftRotate(root, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rightRotate(root, x->parent);
                x = root;
            }
        }
    }
    x->color = BLACK;
}

Node* deleteNode(Node*& root, Node* z) {
    Node* y = z;
    Node* x;
    bool yOriginalColor = y->color;
    if (z->left == NIL) {
        x = z->right;
        transplant(root, z, z->right);
    } else if (z->right == NIL) {
        x = z->left;
        transplant(root, z, z->left);
    } else {
        y = treeMinimum(z->right);
        yOriginalColor = y->color;
        x = y->right;
        if (y->parent == z) x->parent = y;
        else {
            transplant(root, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(root, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    if (yOriginalColor == BLACK) fixDelete(root, x);
    delete z;
    return root;
}

Node* search(Node* root, int key) {
    while (root != NIL && key != root->data) {
        key_comparisons++;
        pointer_accesses++;
        if (key < root->data)
            root = root->left;
        else
            root = root->right;
    }
    return root;
}

Node* insertNode(Node*& root, int key) {
    Node* node = createNode(key);
    Node* y = nullptr;
    Node* x = root;
    while (x != NIL) {
        key_comparisons++;
        y = x;
        if (node->data < x->data)
            x = x->left;
        else
            x = x->right;
    }
    node->parent = y;
    if (y == nullptr)
        root = node;
    else if (node->data < y->data)
        y->left = node;
    else
        y->right = node;
    node->left = NIL;
    node->right = NIL;
    node->color = RED;
    fixInsert(root, node);
    return root;
}

int height(Node* node) {
    if (node == NIL) return -1;
    return 1 + max(height(node->left), height(node->right));
}

void printTree(Node* root, int depth, char prefix, string indent = "") {
    if (root == NIL) return;
    if (root->right != NIL) printTree(root->right, depth + 1, '\\', indent + (prefix == '/' ? "|   " : "    "));
    cout << indent;
    if (depth > 0) cout << prefix << "-";
    cout << "[" << root->data << (root->color == RED ? "R" : "B") << "]\n";
    if (root->left != NIL) printTree(root->left, depth + 1, '/', indent + (prefix == '\\' ? "|   " : "    "));
}

void runExperiment(int n, bool shuffledInsert, int trial) {
    vector<int> keys(n);
    for (int i = 0; i < n; ++i) keys[i] = i + 1;
    if (shuffledInsert) random_shuffle(keys.begin(), keys.end());
    Node* root = NIL;
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
        Node* z = search(root, keys[i]);
        root = deleteNode(root, z);
        int h = height(root);
        totalCmp += key_comparisons;
        totalPtr += pointer_accesses;
        maxCmp = max(maxCmp, (int)key_comparisons);
        maxPtr = max(maxPtr, (int)pointer_accesses);
        maxHeight = max(maxHeight, h);
    }
    cout << n << "," << (shuffledInsert ? "random" : "sorted") << "," << trial << "," << (2 * n) << "," << (totalCmp / (2 * n)) << "," << maxCmp << "," << (totalPtr / (2 * n)) << "," << maxPtr << "," << maxHeight << endl;
}

void runDemonstration() {
    const int n = 30;
    srand(time(nullptr));
    vector<int> keys(n);
    for (int i = 0; i < n; ++i) keys[i] = i + 1;

    cout << "=== Test 1: Insert increasing, delete random ===\n";
    Node* root1 = NIL;
    for (int key : keys) {
        root1 = insertNode(root1, key);
        cout << "After inserting " << key << ":\n";
        printTree(root1, 0, '-');
        cout << endl;
    }
    random_shuffle(keys.begin(), keys.end());
    for (int key : keys) {
        Node* z = search(root1, key);
        root1 = deleteNode(root1, z);
        cout << "After deleting " << key << ":\n";
        printTree(root1, 0, '-');
        cout << endl;
    }

    cout << "\n=== Test 2: Insert random, delete random ===\n";
    Node* root2 = NIL;
    random_shuffle(keys.begin(), keys.end());
    for (int key : keys) {
        root2 = insertNode(root2, key);
        cout << "After inserting " << key << ":\n";
        printTree(root2, 0, '-');
        cout << endl;
    }
    random_shuffle(keys.begin(), keys.end());
    for (int key : keys) {
        Node* z = search(root2, key);
        root2 = deleteNode(root2, z);
        cout << "After deleting " << key << ":\n";
        printTree(root2, 0, '-');
        cout << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <mode>\n";
        cerr << "Mode 0: Complexity experiment (large n, 20 trials, no printing)\n";
        cerr << "Mode 1: Demonstration with printing for n=30\n";
        return 1;
    }
    string mode = argv[1];
    if (mode == "0") {
        cout << "n,scenario,trial,operations,avg_key_cmp,max_key_cmp,avg_ptr_ops,max_ptr_ops,max_height\n";
        for (int n = 10000; n <= 100000; n += 10000) {
            for (int trial = 1; trial <= 1; ++trial) {
                runExperiment(n, false, trial);
                runExperiment(n, true, trial);
            }
        }
    } else if (mode == "1") {
        runDemonstration();
    } else {
        cerr << "Invalid mode. Use 0 or 1.\n";
        return 1;
    }
    return 0;
}
