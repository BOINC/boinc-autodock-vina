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

void help() {
    std::cout << "Usage:" << std::endl;
    std::cout << "config-validator config_to_validate.json" << std::endl;
}

inline void header() {
    std::cout << "Starting BOINC Autodock Vina config validator v" << BOINC_AUTODOCK_VINA_VERSION;
    std::cout << " (" << BOINC_APPS_GIT_REVISION << ")" << std::endl;
}

int main(int argc, char **argv) {
    header();

    if (argc != 2) {
        help();
        return 1;
    }

    try {
        const auto& json_path = std::filesystem::path(argv[1]);
        const auto& working_directory = json_path.has_parent_path() ? json_path.parent_path() : std::filesystem::current_path();
        config config;

        if (!config.load(json_path)) {
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
