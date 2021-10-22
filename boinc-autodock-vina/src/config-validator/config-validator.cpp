#include <iostream>

#include "common/config.h"

int main(int argc, char **argv) {
    config config;

    if (!config.validate()) {
        std::cout << "Validation failed" << std::endl;
        return 1;
    }

    std::cout << "Validation passed" << std::endl;

    return 0;
}
