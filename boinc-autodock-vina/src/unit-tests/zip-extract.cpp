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

#include "zip-extract.h"

#include <functional>
#include <fstream>
#include <cstring>

#include <zip.h>

template <typename T>
using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

bool zip_extract::extract(const std::filesystem::path& zip_file, const std::filesystem::path& target) {
    int error = 0;
    const deleted_unique_ptr<zip_t> zip(zip_open(zip_file.string().data(), 0, &error), [](auto* z) {
        if (z != nullptr) {
            zip_close(z);
        }
    });
    if (!zip) {
        return false;
    }

    const auto& entries = zip_get_num_entries(zip.get(), 0);
    for (zip_int64_t i = 0; i < entries; ++i) {
        struct zip_stat file_stat {};
        if (zip_stat_index(zip.get(), i, 0, &file_stat)) {
            return false;
        }
        if (!(file_stat.valid & ZIP_STAT_NAME)) {
            continue;
        }
        if ((file_stat.name[0] != '\0') && (file_stat.name[strlen(file_stat.name) - 1] == '/')) {
            continue;
        }
        const deleted_unique_ptr<struct zip_file> file(zip_fopen_index(zip.get(), i, 0), [](auto* z) {
            if (z != nullptr) {
                zip_fclose(z);
            }
        });
        std::string buffer;
        buffer.resize(file_stat.size, '\0');
        if (zip_fread(file.get(), buffer.data(), file_stat.size) == -1) {
            return false;
        }
        const auto name = target / file_stat.name;
        if (!std::ofstream(name.string().data()).write(buffer.data(), file_stat.size)) {
            return false;
        }
    }

    return true;
}
