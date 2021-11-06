#include <iostream>
#include <thread>
#include <atomic>

#include <boinc/boinc_api.h>

#include "calculate.h"

inline void help() {
    std::cerr << "Usage:" << std::endl;
    std::cerr << "boinc-autodock-vina config.json" << std::endl;
}

auto prev_value = 0.;
constexpr auto precision = 0.001;

inline void report_progress(double value) {
    if (std::abs(value - prev_value) > precision) {
        prev_value = value;
        boinc_fraction_done(value);
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        help();
        return 1;
    }

    char buf[256];

    try {

        if (const auto res = boinc_init()) {
            std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << " boinc_init failed with error code " << res << std::endl;
            return res;
        }

        std::atomic result(false);

        std::string json(argv[1]);

        std::thread worker([&result, &json] {
            try {
                result = calculate(json, [](auto value)
                {
                    report_progress(value);
                });
            }
            catch (const std::exception& ex)
            {
                char str[256];
                std::cerr << boinc_msg_prefix(str, sizeof(str)) << " docking failed: " << ex.what() << std::endl;
                result = false;
            }
        });

        worker.join();

        if (!result) {
            std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << " docking failed" << std::endl;
            boinc_finish(1);
            return 1;
        }

        boinc_fraction_done(1.);
        boinc_finish(0);
    }
    catch (const std::exception& ex) {
        std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << " exception was thrown while running: " << ex.what() << std::endl;
        boinc_finish(1);
        return 1;
    }

    return 0;
}
