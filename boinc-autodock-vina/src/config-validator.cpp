#include <iostream>

#include "config.h"

int main(int argc, char **argv) {
    Config config;

    if (!ValidateConfig(config)) {
        std::cout << "Validation failed" << std::endl;
        return 1;
    }

    std::cout << "Validation passed" << std::endl;

    return 0;
}
