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

#include "common/config.h"

#ifndef BOINC_AUTODOCK_VINA_VERSION
#define BOINC_AUTODOCK_VINA_VERSION "unknown"
#endif

#ifndef BOINC_APPS_GIT_REVISION
#define BOINC_APPS_GIT_REVISION "unknown"
#endif

#include <common/zip-extract.h>

void help() {
    std::cout << "Usage:" << std::endl;
    std::cout << "config-validator zip_to_validate.zip" << std::endl;
}

inline void header() {
    std::cout << "Starting BOINC Autodock Vina config validator v" << BOINC_AUTODOCK_VINA_VERSION;
    std::cout << " (" << BOINC_APPS_GIT_REVISION << ")" << std::endl;
}

bool unzip(const std::filesystem::path& zip, const std::filesystem::path& data_path) {
    if (!exists(zip) || !is_regular_file(zip)) {
        std::cerr << "Failed to open ZIP file." << std::endl;
        return false;
    }

    if (!create_directories(data_path)) {
        std::cerr << "Failed to create working directory." << std::endl;
        return false;
    }

    if (!zip_extract::extract(zip, data_path)) {
        std::cerr << "Failed to extract data from ZIP archive to working directory." << std::endl;
        return false;
    }

    return true;
}

int main(int argc, char **argv) {
    header();

    if (argc != 2) {
        help();
        return 1;
    }

    try {
        const auto& in_zip_path = std::filesystem::path(argv[1]);
        const auto data_path = std::filesystem::current_path() / "data";

        if (!unzip(in_zip_path, data_path)) {
            std::cerr << "Failed to extract data from ZIP archive to working directory, cannot proceed further" << std::endl;
            return 1;
        }

        std::filesystem::path json_path;
        bool json_found = false;
        for (const auto& file : std::filesystem::directory_iterator(data_path)) {
            if (file.path().extension() == ".json") {
                if (json_found) {
                    std::cerr << "Working directory contains more than one configuration JSON file, cannot proceed further" << std::endl;
                    remove_all(data_path);
                    return 1;
                }

                json_path = file.path();
                json_found = true;
            }
        }

        if (!json_found) {
            std::cerr << "No configuration JSON file found in working directory, cannot proceed further" << std::endl;
            remove_all(data_path);
            return 1;
        }

        config config;

        if (!config.load(json_path)) {
            std::cout << "Failed to load '" << argv[1] << "' file";
            remove_all(data_path);
            return 1;
        }

        if (!config.validate()) {
            std::cout << "Validation failed" << std::endl;
            remove_all(data_path);
            return 1;
        }

        std::cout << "Validation passed" << std::endl;
        remove_all(data_path);
    } catch(const std::exception& ex) {
        std::cout << "Failed to validate '" << argv[1] << "' file: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
