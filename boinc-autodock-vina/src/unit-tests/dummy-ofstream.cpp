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

#include "dummy-ofstream.h"

dummy_ofstream::~dummy_ofstream() {
    if (stream_open) {
        stream.close();
        stream_open = false;
    }

    for (const auto& file : open_files) {
        if (exists(file)) {
            std::filesystem::remove(file);
        }
    }
}

std::ofstream& dummy_ofstream::operator()() {
    return stream;
}

void dummy_ofstream::open(const std::filesystem::path& file) {
    if (!stream_open) {
        stream.open(file);
        stream_open = true;
        file_path = file;
        open_files.push_back(file);
    }
    else {
        if (file != file_path) {
            close();
            open(file);
        }
    }
}

void dummy_ofstream::close() {
    if (stream_open) {
        stream.close();
        stream_open = false;
    }
}

void create_dummy_file(dummy_ofstream& stream, const std::filesystem::path& file) {
    stream.open(std::filesystem::current_path() / file);
    stream() << "Dummy" << std::endl;
    stream.close();
}
