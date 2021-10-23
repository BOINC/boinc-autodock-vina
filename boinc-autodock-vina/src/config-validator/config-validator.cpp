#include <iostream>

#include "common/config.h"

void help() {
    std::cout << "Usage:" << std::endl;
    std::cout << "config-validator config_to_validate.json" << std::endl;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        help();
        return 1;
    }

    try {
        config config;

        if (!config.load(argv[1])) {
            std::cout << "Failed to load '" << argv[1] << "' file";
            return 1;
        }

        if (!config.validate()) {
            std::cout << "Validation failed" << std::endl;
            return 1;
        }

        std::cout << "Validation passed" << std::endl;
    } catch(const std::exception& ex) {
        std::cout << "Failed to validate '" << argv[1] << "' file: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
