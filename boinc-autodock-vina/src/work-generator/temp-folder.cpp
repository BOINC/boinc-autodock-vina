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

#include "temp-folder.h"

#include <random>
#include <sstream>

temp_folder::temp_folder(const std::filesystem::path& working_directory) : folder(working_directory / get_temp_folder_name()) {
    create_directories(folder);
}

temp_folder::~temp_folder() {
    remove_all(folder);
}

const std::filesystem::path& temp_folder::operator()() const {
    return folder;
}

std::string temp_folder::get_temp_folder_name() {
    static std::random_device rd;
    static std::mt19937 mt(rd());
    static std::uniform_int_distribution dist(std::numeric_limits<unsigned>::min(), std::numeric_limits<unsigned>::max());
    std::stringstream ss;
    ss << dist(mt);
    return ss.str();
}
