#include <iostream>
    void clearScreen(void) {
        std::cout << "\033[2J\033[H";
    }