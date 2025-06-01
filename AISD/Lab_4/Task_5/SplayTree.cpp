#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <cstring>

using namespace std;

struct Node {
    int key;
    Node *left, *right;
    Node(int k) : key(k), left(nullptr), right(nullptr) {}
};

Node* rightRotate(Node* x) {
    Node* y = x->left;
    x->left = y->right;
    y->right = x;
    return y;
}

Node* leftRotate(Node* x) {
    Node* y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
}

Node* splay(Node* root, int key) {
    if (root == nullptr || root->key == key)
        return root;

    if (key < root->key) {
        if (!root->left) return root;
        if (key < root->left->key) {
            root->left->left = splay(root->left->left, key);
            root = rightRotate(root);
        } else if (key > root->left->key) {
            root->left->right = splay(root->left->right, key);
            if (root->left->right) root->left = leftRotate(root->left);
        }
        return (root->left == nullptr) ? root : rightRotate(root);
    } else {
        if (!root->right) return root;
        if (key > root->right->key) {
            root->right->right = splay(root->right->right, key);
            root = leftRotate(root);
        } else if (key < root->right->key) {
            root->right->left = splay(root->right->left, key);
            if (root->right->left) root->right = rightRotate(root->right);
        }
        return (root->right == nullptr) ? root : leftRotate(root);
    }
}

Node* insert(Node* root, int key) {
    if (!root) return new Node(key);
    root = splay(root, key);
    if (root->key == key) return root;

    Node* node = new Node(key);
    if (key < root->key) {
        node->right = root;
        node->left = root->left;
        root->left = nullptr;
    } else {
        node->left = root;
        node->right = root->right;
        root->right = nullptr;
    }
    return node;
}

Node* remove(Node* root, int key) {
    if (!root) return nullptr;
    root = splay(root, key);
    if (root->key != key) return root;

    Node* temp;
    if (!root->left) {
        temp = root->right;
    } else {
        temp = splay(root->left, key);
        temp->right = root->right;
    }
    delete root;
    return temp;
}

void printTree(Node* root, string indent = "", bool last = true) {
    if (root != nullptr) {
        cout << indent;
        if (last) {
            cout << "R----";
            indent += "     ";
        } else {
            cout << "L----";
            indent += "|    ";
        }
        cout << root->key << endl;
        printTree(root->left, indent, false);
        printTree(root->right, indent, true);
    }
}

int height(Node* root) {
    if (!root) return 0;
    return 1 + max(height(root->left), height(root->right));
}

void detailedTest() {
    Node* root = nullptr;
    vector<int> values;
    int n = 20;

    // Increasing insert test
    cout << "== Increasing Insert ==" << endl;
    for (int i = 0; i < n; ++i) {
        root = insert(root, i);
        cout << "Inserted: " << i << endl;
        printTree(root);
        cout << "Height: " << height(root) << "\n\n";
    }

    // Remove all
    cout << "== Increasing Removal ==" << endl;
    for (int i = 0; i < n; ++i) {
        root = remove(root, i);
        cout << "Removed: " << i << endl;
        printTree(root);
        cout << "Height: " << height(root) << "\n\n";
    }

    // Random insert test
    root = nullptr;
    values.clear();
    for (int i = 0; i < n; ++i) values.push_back(i);
    shuffle(values.begin(), values.end(), default_random_engine(42));
    cout << "== Random Insert ==" << endl;
    for (int v : values) {
        root = insert(root, v);
        cout << "Inserted: " << v << endl;
        printTree(root);
        cout << "Height: " << height(root) << "\n\n";
    }

    // Random remove test
    shuffle(values.begin(), values.end(), default_random_engine(43));
    cout << "== Random Removal ==" << endl;
    for (int v : values) {
        root = remove(root, v);
        cout << "Removed: " << v << endl;
        printTree(root);
        cout << "Height: " << height(root) << "\n\n";
    }
}

void performanceTest() {
    vector<int> sizes = {10000, 20000, 30000, 40000, 50000,
                         60000, 70000, 80000, 90000, 100000};
    for (int n : sizes) {
        vector<int> values(n);
        for (int i = 0; i < n; ++i) values[i] = i;

        // Increasing Insert
        Node* root = nullptr;
        for (int i = 0; i < n; ++i) {
            root = insert(root, i);
        }
        int h1 = height(root);
        for (int i = 0; i < n; ++i) {
            root = remove(root, i);
        }
        int h2 = height(root);

        // Random Insert
        shuffle(values.begin(), values.end(), default_random_engine(42));
        root = nullptr;
        for (int v : values) root = insert(root, v);
        int h3 = height(root);
        shuffle(values.begin(), values.end(), default_random_engine(43));
        for (int v : values) root = remove(root, v);
        int h4 = height(root);

        cout << "splay," << n << ",insert," << h1 << "\n";
        cout << "splay," << n << ",remove," << h2 << "\n";
        cout << "splay," << n << ",insert_random," << h3 << "\n";
        cout << "splay," << n << ",remove_random," << h4 << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "1") == 0) {
        detailedTest();
    } else {
        performanceTest();
    }
    return 0;
}