#include <iostream>
#include "dice.h"       
#include "direction.h"   

void printHelp() {
    std::cout << "Команды:\n";
    std::cout << "  forward (вперед) - двигать вперед\n";
    std::cout << "  backward (назад) - двигать назад\n";
    std::cout << "  right (вправо) - двигать направо\n";
    std::cout << "  left (влево) - двигать налево\n";
    std::cout << "  print - показать текущую раскладку кубику\n";
    std::cout << "  exit - выход\n";
}

int main() {
    setlocale(LC_ALL, "rus");
    try {
        int top, front, right;
        std::cout << "Введите стороны куба (верхняя лицевая правая): ";
        std::cin >> top >> front >> right;

        Dice dice(top, front, right);
        std::cout << "Кубик создан\n";
        dice.printLayout();

        printHelp();

        std::string command;
        while (true) {
            std::cout << "\nВведите команду: ";
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
                    std::cout << "Передвинут " << command << ". Кубик перевернут.\n";
                }
                catch (const std::invalid_argument& e) {
                    std::cout << "Ошибка: " << e.what() << "\n";
                    std::cout << "Введите 'help' для отображения справки.\n";
                }
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
        return 1;
    }

    return 0;
}