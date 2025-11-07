#ifndef DICE_H  
#define DICE_H

#include <map>
#include <string>
#include "direction.h"  

class Dice {
public:
    Dice(int top, int front, int right);

    void move(Direction direction);

    int getTop() const { return faces.at("top"); }
    int getFront() const { return faces.at("front"); }
    int getRight() const { return faces.at("right"); }
    int getBottom() const { return faces.at("bottom"); }
    int getBack() const { return faces.at("back"); }
    int getLeft() const { return faces.at("left"); }

    void printLayout() const;

private:
    std::map<std::string, int> faces;

    void initializeFaces(int top, int front, int right);
    void rotate(Direction direction);
};

#endif 