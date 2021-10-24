#include <iostream>

#include <boinc/boinc_api.h>

#include "calculate.h"

void help() {
    std::cout << "Usage:" << std::endl;
    std::cout << "boinc-autodock-vina config.json" << std::endl;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        help();
        return 1;
    }

    try {
        char buf[256];

        if (const auto res = boinc_init()) {
            std::cout << boinc_msg_prefix(buf, sizeof(buf)) << " boinc_init failed with error code " << res << std::endl;
            return res;
        }

        if (const auto res = calculate(argv[1]); !res) {
            std::cout << boinc_msg_prefix(buf, sizeof(buf)) << " docking failed" << std::endl;
            return 1;
        }
        boinc_fraction_done(1);
        boinc_finish(0);
    }
    catch (const std::exception& ex) {
        std::cout << "Exception was thrown while running: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
