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

#pragma once

#include "common/config.h"

enum class repair {
    bonds_hydrogens,
    bonds,
    hydrogens,
    checkhydrogens,
    None
};

enum class cleanup {
    nphs,
    lps,
    waters,
    nonstdres,
    deleteAltB
};

class prepare_receptors final : public json_load {
public:
    std::vector<std::string> receptors;
    repair repair;
    std::vector<std::string> preserves;
    cleanup cleanup;
    bool delete_nonstd_residue = false;

    [[nodiscard]] bool load(const jsoncons::basic_json<char>& json, const std::filesystem::path& working_directory) override;
};

class generator {
public:
    prepare_receptors prepare_receptors;

    [[nodiscard]] bool validate() const;
    [[nodiscard]] bool load(const std::filesystem::path& config_file_path);
    [[nodiscard]] bool process();
    [[nodiscard]] bool save_config(const config& config, const std::filesystem::path& working_directory);
    [[nodiscard]] bool create_zip(const std::filesystem::path& path);

private:
    uint64_t current_wu_number = 0;
};

class temp_folder {
public:
    explicit temp_folder(const std::filesystem::path& working_directory);
    ~temp_folder();
    const std::filesystem::path& operator()() const;

    temp_folder(temp_folder&&) = delete;
    temp_folder& operator=(temp_folder&&) = delete;
    temp_folder(const temp_folder&) = delete;
    temp_folder& operator=(const temp_folder&) = delete;

    [[nodiscard]] static std::string get_temp_folder_name();

private:
    const std::filesystem::path folder;

};
