#ifndef DIRECTION_H  
#define DIRECTION_H

#include <string> 

enum class Direction {
    FORWARD,
    BACKWARD,
    RIGHT,
    LEFT
};

Direction parseDirection(const std::string& dirStr);

#endif 