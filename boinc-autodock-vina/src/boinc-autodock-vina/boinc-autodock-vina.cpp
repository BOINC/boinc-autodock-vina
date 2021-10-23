#include <iostream>

#include "calculate.h"

int main(int argc, char** argv) {
    try {
        const auto res = calculate(argv[1]);
        if (!res) {
            std::cout << "Docking failed" << std::endl;
            return 1;
        }
    }
    catch (const std::exception& ex) {
        std::cout << "Exception was thrown while running: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
