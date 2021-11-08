// This file is part of BOINC.
// https://boinc.berkeley.edu
// Copyright (C) 2021 University of California
//
// BOINC is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// BOINC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with BOINC.  If not, see <http://www.gnu.org/licenses/>.

#include <iostream>
#include <thread>
#include <atomic>
#include <cmath>

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
        BOINC_OPTIONS options;
        boinc_options_defaults(options);
        options.multi_thread = true;

        if (const auto res = boinc_init_options(&options)) {
            std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << " boinc_init failed with error code " << res << std::endl;
            return res;
        }

        APP_INIT_DATA app_data;
        boinc_get_init_data(app_data);
        const int ncpus = static_cast<int>(ceil(app_data.ncpus));

        std::atomic result(false);

        std::string json(argv[1]);

        boinc_fraction_done(0.);

        std::thread worker([&result, &json, &ncpus] {
            try {
                result = calculate(json, ncpus, [](auto value)
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
