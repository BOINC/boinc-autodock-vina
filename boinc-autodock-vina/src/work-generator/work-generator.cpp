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
#include <random>

#include <work-generator/input-config.h>

#ifndef BOINC_AUTODOCK_VINA_VERSION
#define BOINC_AUTODOCK_VINA_VERSION "unknown"
#endif

#ifndef BOINC_APPS_GIT_REVISION
#define BOINC_APPS_GIT_REVISION "unknown"
#endif

void help() {
    std::cout << "Usage:" << std::endl;
    std::cout << "work-generator OUT_DIR IN_DIR [--]" << std::endl;
}

inline void header() {
    std::cout << "Starting BOINC Autodock Vina work generator v" << BOINC_AUTODOCK_VINA_VERSION;
    std::cout << " (" << BOINC_APPS_GIT_REVISION << ")" << std::endl;
}

inline bool process_directory(const std::filesystem::path& directory, const std::function<bool(const std::filesystem::path& out_path)>& process) {
    for (const auto& e : std::filesystem::directory_iterator(directory)) {
        if (e.is_directory()) {
            if (!process_directory(e.path(), process)) {
                std::cerr << "Failed to process directory <" << e.path().string() << ">." << std::endl;
                return false;
            }
        }
        else if (e.is_regular_file() && e.path().extension() == ".json") {
            if (!process(e.path())) {
                std::cerr << "Failed to process <" << e.path().string() << ">." << std::endl;
                return false;
            }
        }
    }
    return true;
}

int main(int argc, char** argv) {
    header();

    if (argc < 3 || argc > 4) {
        help();
        return 1;
    }

    std::filesystem::path out_dir(argv[1]);
    const std::filesystem::path in_dir(argv[2]);

    const auto stream_mode = (argc == 4 && std::string(argv[3]) == std::string("--"));

    if (!exists(in_dir) || !is_directory(in_dir)) {
        std::cerr << "<" << argv[1] << "> is not a valid directory path." << std::endl;
        return 1;
    }

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution dist(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());

    std::stringstream ss;
    ss << std::setw(16) << std::setfill('0') << std::hex << dist(mt);
    const auto uid = ss.str();

    out_dir /= uid;
    create_directories(out_dir);

    generator generator;

    if (stream_mode) {
        if (!generator.process(std::cin, in_dir, out_dir, uid)) {
            std::cerr << "Failed to process input config." << std::endl;
            return 1;
        }
    }
    else {
        const auto& process = [&](const auto& file_path) {
            std::cout << "Processing file: <" << file_path.string() << ">." << std::endl;
            return generator.process(file_path, out_dir, uid);
        };

        if (!process_directory(in_dir, process)) {
            std::cerr << "Failed to process input directory <" << in_dir.string() << ">." << std::endl;
            return 1;
        }
    }

    std::cout << "WUs output directory: <" << out_dir.string() << ">." << std::endl;
    std::cout << "WUs generated: " << generator.get_files_processed() << std::endl;

    return 0;
}
