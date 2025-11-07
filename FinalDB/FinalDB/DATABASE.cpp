#include "UserInterface.h"

int main() {
    try {
        UserInterface ui;
        ui.run();
    }
    catch (const std::exception& e) {
        std::cerr << "⛔ Critical error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}