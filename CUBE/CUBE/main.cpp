#include <iostream>
#include "dice.h"       
#include "direction.h"   

void printHelp() {
    std::cout << "Menu:\n";
    std::cout << "  move forward\n";
    std::cout << "  move backward\n";
    std::cout << "  move right\n";
    std::cout << "  move left\n";
    std::cout << "  move print\n";
    std::cout << "  move exit\n";
}

int main() {
    setlocale(LC_ALL, "rus");
    try {
        int top, front, right;
        std::cout << "Enter the sides of the cube (top, front, right): ";
        std::cin >> top >> front >> right;

        Dice dice(top, front, right);
        std::cout << "The cube is created\n";
        dice.printLayout();

        printHelp();

        std::string command;
        while (true) {
            std::cout << "\nEnter the command: ";
            std::cin >> command;

            if (command == "exit" || command == "quit") {
                break;
            }
            else if (command == "print") {
                dice.printLayout();
            }
            else {
                try {
                    Direction dir = parseDirection(command);
                    dice.move(dir);
                    std::cout << "Moved " << command << ". The cube is rotated.\n";
                }
                catch (const std::invalid_argument& e) {
                    std::cout << "Error: " << e.what() << "\n";
                    std::cout << "Write 'help' to display help.\n";
                }
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;

}

