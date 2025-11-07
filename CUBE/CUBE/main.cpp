#include <iostream>
#include "dice.h"       
#include "direction.h"   

void printHelp() {
    std::cout << "Êîìàíäû:\n";
    std::cout << "  forward (âïåðåä) - äâèãàòü âïåðåä\n";
    std::cout << "  backward (íàçàä) - äâèãàòü íàçàä\n";
    std::cout << "  right (âïðàâî) - äâèãàòü íàïðàâî\n";
    std::cout << "  left (âëåâî) - äâèãàòü íàëåâî\n";
    std::cout << "  print - ïîêàçàòü òåêóùóþ ðàñêëàäêó êóáèêó\n";
    std::cout << "  exit - âûõîä\n";
}

int main() {
    setlocale(LC_ALL, "rus");
    try {
        int top, front, right;
        std::cout << "Ââåäèòå ñòîðîíû êóáà (âåðõíÿÿ ëèöåâàÿ ïðàâàÿ): ";
        std::cin >> top >> front >> right;

        Dice dice(top, front, right);
        std::cout << "Êóáèê ñîçäàí\n";
        dice.printLayout();

        printHelp();

        std::string command;
        while (true) {
            std::cout << "\nÂâåäèòå êîìàíäó: ";
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
                    std::cout << "Ïåðåäâèíóò " << command << ". Êóáèê ïåðåâåðíóò.\n";
                }
                catch (const std::invalid_argument& e) {
                    std::cout << "Îøèáêà: " << e.what() << "\n";
                    std::cout << "Ââåäèòå 'help' äëÿ îòîáðàæåíèÿ ñïðàâêè.\n";
                }
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Îøèáêà: " << e.what() << "\n";
        return 1;
    }

    return 0;

}
