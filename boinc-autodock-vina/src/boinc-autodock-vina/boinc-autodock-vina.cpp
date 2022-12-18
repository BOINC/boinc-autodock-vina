// This file is part of BOINC.
// https://boinc.berkeley.edu
// Copyright (C) 2022 University of California
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
#include <fstream>
#include <thread>
#include <atomic>
#include <cmath>

#include <boinc/boinc_api.h>
#include <common/zip-extract.h>
#include <common/zip-create.h>

#include "calculate.h"

#ifndef BOINC_AUTODOCK_VINA_VERSION
#define BOINC_AUTODOCK_VINA_VERSION "unknown"
#endif

#ifndef BOINC_APPS_GIT_REVISION
#define BOINC_APPS_GIT_REVISION "unknown"
#endif

inline void help() {
    std::cerr << "Usage:" << std::endl;
    std::cerr << "boinc-autodock-vina input.zip output.zip" << std::endl;
}

inline void header() {
    std::cout << "Starting BOINC Autodock Vina v" << BOINC_AUTODOCK_VINA_VERSION;
    std::cout << " (" << BOINC_APPS_GIT_REVISION << ")" << std::endl;
}

auto prev_value = 0.;
constexpr auto precision = 0.001;

inline void report_progress(const double value) {
    if (std::abs(value - prev_value) > precision) {
        prev_value = value;
        boinc_fraction_done(value);
    }
}

bool unzip(const std::filesystem::path& zip, const std::filesystem::path& data_path) {
    char buf[256];
    if (!exists(zip) || !is_regular_file(zip)) {
        std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << "Failed to open ZIP file." << std::endl;
        return false;
    }

    if (!create_directories(data_path)) {
        std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << "Failed to create working directory." << std::endl;
        return false;
    }

    if (!zip_extract::extract(zip, data_path)) {
        std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << "Failed to extract data from ZIP archive to working directory." << std::endl;
        return false;
    }

    return true;
}

int perform_docking(const std::string& in_zip, const std::string& out_zip) noexcept {
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

        const auto& in_zip_path = std::filesystem::path(in_zip);
        const auto data_path = std::filesystem::current_path() / "data";

        if (!unzip(in_zip_path, data_path)) {
            std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << "Failed to extract data from ZIP archive to working directory, cannot proceed further" << std::endl;
            boinc_finish(1);
            return 1;
        }

        std::filesystem::path json_path;
        bool json_found = false;
        for (const auto& file : std::filesystem::directory_iterator(data_path)) {
            if (file.path().extension() == ".json") {
                if (json_found) {
                    std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << "Working directory contains more than one configuration JSON file, cannot proceed further" << std::endl;
                    boinc_finish(1);
                    return 1;
                }

                json_path = file.path();
                json_found = true;
            }
        }

        if (!json_found) {
            std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << "No configuration JSON file found in working directory, cannot proceed further" << std::endl;
            boinc_finish(1);
            return 1;
        }

        config conf;

        if (!conf.load(json_path)) {
            std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << "Config load failed, cannot proceed further" << std::endl;
            boinc_finish(1);
            return 1;
        }

        if (!conf.validate(true)) {
            std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << "Config validation failed, cannot proceed further" << std::endl;
            boinc_finish(1);
            return 1;
        }

        boinc_fraction_done(0.);

        std::thread worker([&result, &conf, &ncpus] {
            try {
                result = calculator::calculate(conf, ncpus, [](const auto value) {
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

        const auto& out_zip_path = std::filesystem::path(out_zip);

        std::vector<std::filesystem::path> out_files;
        for (const auto& file : conf.get_out_files()) {
            out_files.emplace_back(std::filesystem::path(file));
        }

        if (!zip_create::create(out_zip_path, out_files)) {
            std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << "Failed to create ZIP archive with results" << std::endl;
            boinc_finish(1);
            return 1;
        }

        remove_all(data_path);

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

int main(int argc, char** argv) {
    try {
        header();

        if (argc != 3) {
            help();
            return 1;
        }

        std::string in_zip;
		if (boinc_resolve_filename_s(argv[1], in_zip)) {
			std::cerr << "Failed to resolve input ZIP file name" << std::endl;
			return 1;
		}
        std::string out_zip;
		if (boinc_resolve_filename_s(argv[2], out_zip)) {
			std::cerr << "Failed to resolve output ZIP file name" << std::endl;
			return 1;
		}

        return perform_docking(in_zip, out_zip);
    }
    catch (std::exception& ex) {
        std::cerr << "Exception was thrown while running boinc-autodock-vina: " << ex.what() << std::endl;
        return 1;
    }
}
