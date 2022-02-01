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

#pragma once

#include <fstream>
#include <filesystem>
#include <vector>

class dummy_ofstream final {
public:
    ~dummy_ofstream();
    std::ofstream& operator()();
    void open(const std::filesystem::path& file);
    void close();
private:
    std::ofstream stream;
    std::filesystem::path file_path;
    bool stream_open = false;
    std::vector<std::filesystem::path> open_files;
};

void create_dummy_file(dummy_ofstream& stream, const std::filesystem::path& file);
