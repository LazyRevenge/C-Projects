#include "dice.h"
#include <iostream>
#include <stdexcept>
#include <iomanip>

Dice::Dice(int top, int front, int right) {
    if (top < 1 || top > 6 || front < 1 || front > 6 || right < 1 || right > 6) {
        throw std::invalid_argument("The face values ​​must be between 1 and 6.");
    }
    if (top + front == 7 || top + right == 7 || front + right == 7) {
        throw std::invalid_argument("The sum of opposite faces must be equal to 7");
    }
    if (top == front || top == right || front == right) {
        throw std::invalid_argument("The values ​​of the edges cannot match");
    }

    initializeFaces(top, front, right);
}

void Dice::initializeFaces(int top, int front, int right) {
    faces["top"] = top;
    faces["front"] = front;
    faces["right"] = right;
    faces["bottom"] = 7 - top;
    faces["back"] = 7 - front;
    faces["left"] = 7 - right;
}

void Dice::move(Direction direction) {
    rotate(direction);
}

void Dice::rotate(Direction direction) {
    int temp;
    switch (direction) {
    case Direction::FORWARD:
        temp = faces["top"];
        faces["top"] = faces["back"];
        faces["back"] = faces["bottom"];
        faces["bottom"] = faces["front"];
        faces["front"] = temp;
        break;

    case Direction::BACKWARD:
        temp = faces["top"];
        faces["top"] = faces["front"];
        faces["front"] = faces["bottom"];
        faces["bottom"] = faces["back"];
        faces["back"] = temp;
        break;

    case Direction::RIGHT:
        temp = faces["top"];
        faces["top"] = faces["left"];
        faces["left"] = faces["bottom"];
        faces["bottom"] = faces["right"];
        faces["right"] = temp;
        break;

    case Direction::LEFT:
        temp = faces["top"];
        faces["top"] = faces["right"];
        faces["right"] = faces["bottom"];
        faces["bottom"] = faces["left"];
        faces["left"] = temp;
        break;
    }
}

void Dice::printLayout() const {
    std::cout << "Òåêóùàÿ ðàñêëàäêà êóáèêà:\n";
    std::cout << std::setw(10) << " " << std::setw(5) << faces.at("back") << "\n";
    std::cout << std::setw(10) << " " << "  -----\n";
    std::cout << std::setw(9) << faces.at("left") << "  |" << std::setw(3) << faces.at("top") << "  |  " << faces.at("right") << "\n";
    std::cout << std::setw(10) << " " << "  -----\n";
    std::cout << std::setw(10) << " " << std::setw(5) << faces.at("front") << "\n";
    std::cout << std::setw(10) << " " << "  -----\n";
    std::cout << std::setw(10) << " " << std::setw(5) << faces.at("bottom") << "\n";
  

}
