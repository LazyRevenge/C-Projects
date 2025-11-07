#include <iostream>
#include <queue> 
using namespace std;

struct Node {
    int val;      
    Node* left;   
    Node* right;  
};

struct Tree {
    Node* root; 
};

void addToTree(Tree& tree, int value) {
    Node* newNode = new Node; 
    newNode->val = value;
    newNode->left = nullptr;
    newNode->right = nullptr;

    if (tree.root == nullptr) {
        tree.root = newNode;
        return;
    }

    Node* tmp = tree.root;
    while (true) {
        if (value < tmp->val) {
            if (tmp->left == nullptr) {
                tmp->left = newNode; 
                break;
            }
            else {
                tmp = tmp->left;
            }
        }
        else {           
            if (tmp->right == nullptr) {
                tmp->right = newNode; 
                break;
            }
            else {
                tmp = tmp->right;
            }
        }
    }
}

void del(Node* x) {
    if (x->left != nullptr) {
        del(x->left); 
    }
    if (x->right != nullptr) {
        del(x->right); 
    }
    delete x; 
}


void printTreeByLevels(Node* root) {
    if (root == nullptr) {
        cout << "Tree is empty." << endl;
        return;
    }

    queue<Node*> q;  
    q.push(root);

    int level = 0;
    while (!q.empty()) {
        int levelSize = static_cast<int>(q.size());  
        cout << "Level " << level++ << ": ";
        for (int i = 0; i < levelSize; i++) {
            Node* current = q.front();
            q.pop();
            if (current != nullptr) {
                cout << current->val << " "; 
                q.push(current->left);      
                q.push(current->right);     
            }
            else {
                cout << "-- "; 
            }
        }

        cout << endl;
    }
}

int main() {
    Tree tree = { nullptr }; 

    int n;
    cout << "Enter the number of elements: ";
    cin >> n;

    cout << "Enter the elements:" << endl;
    for (int i = 0; i < n; i++) {
        int value;
        cin >> value;
        addToTree(tree, value);
    }

    cout << "Binary Search Tree (level-order):" << endl;
    printTreeByLevels(tree.root); 

  
    if (tree.root != nullptr) {
        del(tree.root);
        tree.root = nullptr;
    }

    if (tree.root == nullptr) {
        cout << "Tree cleared successfully!" << endl;
    }

    return 0;
}
