#pragma once
#include <string>

class myStrEd {
private:
    struct Operation {
        int type;       
        std::string arg; 
    };

    struct Node {
        Operation op; 
        Node* next;  
    };

    Node* undoStack; 
    Node* redoStack;
    std::string text; 

    void clearStack(Node*& stack); 
    void moveOperation(Node*& from, Node*& to); 

public:
    myStrEd();
    ~myStrEd();

    void run(); 
    void add(const std::string& s); 
    void del(int n); 
    void undo(); 
    void redo(); 
    void print();
};

