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

#include "zip-create.h"

#include <functional>
#include <zip.h>
#include <iostream>

template <typename T>
using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

bool zip_create::create(const std::filesystem::path& zip_file, const std::vector<std::filesystem::path>& files) {
    int error = 0;
    const deleted_unique_ptr<zip_t> zip(zip_open(zip_file.string().data(), ZIP_CREATE | ZIP_EXCL, &error), [](auto* z) {
        if (z != nullptr) {
            zip_close(z);
        }
        });

    if (!zip) {
        zip_error_t zip_error;
        zip_error_init_with_code(&zip_error, error);
        std::cerr << "Failed to create archive <" << zip_file.filename().string() << ">: " << zip_error_strerror(&zip_error) << std::endl;
        return false;
    }

    for (const auto& file : files) {
        if (is_regular_file(file)) {
            const deleted_unique_ptr<zip_source_t> source(zip_source_file(zip.get(), file.string().data(), 0, 0), [](auto*) {});
            if (!source) {
                std::cerr << "Failed to open file <" << file.filename().string() << ">: " << zip_strerror(zip.get()) << std::endl;
            }
            if (zip_file_add(zip.get(), file.filename().string().data(), source.get(), ZIP_FL_ENC_UTF_8) < 0) {
                zip_source_free(source.get());
                std::cerr << "Failed to add file <" << file.filename().string() << "> to archive : " << zip_strerror(zip.get()) << std::endl;
                return false;
            }
        }
    }

    return true;
}
