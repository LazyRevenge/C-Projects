#include "myStrEd.h"
#include <iostream>
#include <sstream>

myStrEd::myStrEd() : undoStack(nullptr), redoStack(nullptr) {}

myStrEd::~myStrEd() {
    clearStack(undoStack);
    clearStack(redoStack);
}

void myStrEd::clearStack(Node*& stack) {
    while (stack) {
        Node* temp = stack;
        stack = stack->next;
        delete temp;
    }
}
void myStrEd::moveOperation(Node*& from, Node*& to) {
    if (!from) return;

    Node* temp = from;
    from = from->next;

    temp->next = to;
    to = temp;
}

void myStrEd::add(const std::string& s) {
    text += s;
    Node* newNode = new Node{ {1, s}, undoStack }; 
    undoStack = newNode;
    clearStack(redoStack);
}

void myStrEd::del(int n) {
    if (n <= 0 || text.empty()) return;

    n = std::min(n, static_cast<int>(text.size()));
    std::string deleted = text.substr(text.size() - n);
    text.resize(text.size() - n);

    Node* newNode = new Node{ {2, deleted}, undoStack }; 
    undoStack = newNode;
    clearStack(redoStack);
}

void myStrEd::undo() {
    if (!undoStack) return;

    Node* opNode = undoStack;
    undoStack = undoStack->next;

    if (opNode->op.type == 1) {
        text.resize(text.size() - opNode->op.arg.size());
    }
    else { 
        text += opNode->op.arg;
    }

    opNode->next = redoStack;
    redoStack = opNode;
}

void myStrEd::redo() {
    if (!redoStack) return;

    Node* opNode = redoStack;
    redoStack = redoStack->next;

    if (opNode->op.type == 1) { 
        text += opNode->op.arg;
    }
    else { 
        text.resize(text.size() - opNode->op.arg.size());
    }

    opNode->next = undoStack;
    undoStack = opNode;
}

void myStrEd::print() {
    std::cout << (text.empty() ? "\"\"" : text) << "\n";
}

void myStrEd::run() {
    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input); 

        if (input == "exit") break; 

        // Находим первый пробел
        size_t spacePos = input.find(' ');
        std::string cmd;
        std::string args;

        if (spacePos != std::string::npos) {
            cmd = input.substr(0, spacePos); 
            args = input.substr(spacePos + 1); 
        }
        else {
            cmd = input;
        }

        if (cmd == "add") {
            add(args); 
        }
        else if (cmd == "del") {
            int n;
            try {
                n = std::stoi(args); // Преобразуем аргументы в число
                del(n);
            }
            catch (const std::invalid_argument&) {
                std::cout << "Invalid number for delete command\n";
            }
        }
        else if (cmd == "undo") {
            undo();
        }
        else if (cmd == "redo") {
            redo();
        }
        else {
            std::cout << "Unknown command\n";
        }

        print(); // Выводим текущее состояние строки
    }
}
