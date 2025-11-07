#include "direction.h"
#include <stdexcept>
#include <string>

Direction parseDirection(const std::string& dirStr) {
    std::string str = dirStr;

    if (str == "forward") return Direction::FORWARD;
    if (str == "backward") return Direction::BACKWARD;
    if (str == "right") return Direction::RIGHT;
    if (str == "left") return Direction::LEFT;

    throw std::invalid_argument("Wrong way: " + dirStr);

}
