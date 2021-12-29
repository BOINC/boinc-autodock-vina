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

#include "input-config.h"

#include <iostream>
#include <fstream>
#include <random>
#include <zip.h>
#include <jsoncons/basic_json.hpp>

bool prepare_receptors::load(const jsoncons::basic_json<char>& json, [[maybe_unused]] const std::filesystem::path& working_directory) {
    if (json.contains("receptors")) {
        for (const auto& r : json["receptors"].array_range()) {
            const auto& value = std::filesystem::path(r.as<std::string>());
            if (value.is_absolute()) {
                std::cerr << "Config should not contain absolute paths" << std::endl;
                return false;
            }
            receptors.push_back(std::filesystem::path(working_directory / value).string());
        }
    }
    if (json.contains("repair")) {
        auto value = json["repair"].as<std::string>();
        std::transform(value.begin(), value.end(), value.begin(), [](const auto c) { return std::tolower(c); });
        if (value == "bonds_hydrogens") {
            repair = repair::bonds_hydrogens;
        }
        else if (value == "bonds") {
            repair = repair::bonds;
        }
        else if (value == "hydrogens") {
            repair = repair::hydrogens;
        }
        else if (value == "checkhydrogens") {
            repair = repair::checkhydrogens;
        }
        else if (value == "none") {
            repair = repair::None;
        }
        else {
            std::cerr << "Wrong repair value: [" << value << "]" << std::endl;
            return false;
        }
    }
    if (json.contains("preserves")) {
        for (const auto& p : json["preserves"].array_range()) {
            preserves.push_back(p.as<std::string>());
        }
    }
    if (json.contains("cleanup")) {
        auto value = json["cleanup"].as<std::string>();
        std::transform(value.begin(), value.end(), value.begin(), [](const auto c) { return std::tolower(c); });
        if (value == "nphs") {
            cleanup = cleanup::nphs;
        }
        else if (value == "lps") {
            cleanup = cleanup::lps;
        }
        else if (value == "waters") {
            cleanup = cleanup::waters;
        }
        else if (value == "nonstdres") {
            cleanup = cleanup::nonstdres;
        }
        else if (value == "deletealtb") {
            cleanup = cleanup::deleteAltB;
        }
        else if (value == "none") {
            cleanup = cleanup::none;
        }
        else {
            std::cerr << "Wrong cleanup value: [" << value << "]" << std::endl;
            return false;
        }
    }
    if (json.contains("delete_nonstd_residue")) {
        delete_nonstd_residue = json["delete_nonstd_residue"].as<bool>();
    }

    return true;
}

bool prepare_receptors::validate() const {
    return true;
}

bool prepare_receptors::has_data_loaded() const {
    return !receptors.empty() || repair != repair::None || !preserves.empty() || cleanup != cleanup::none || delete_nonstd_residue == true;
}

bool generator::validate() const {
    return true;
}

bool generator::save_config(const config& config, const std::filesystem::path& working_directory, const std::filesystem::path& out_path) {
    const temp_folder temp_path(working_directory);
    const auto& config_path = temp_path() / "config.json";
    if (!config.save(config_path)) {
        return false;
    }

    for (const auto& file : config.get_files()) {
        if (!copy_file(file, temp_path() / std::filesystem::path(file).filename())) {
            std::cerr << "Cannot copy <" << file << "> to <" << temp_path().string() << ">" << std::endl;
            return false;
        }
    }

    return create_zip(temp_path(), out_path);
}

template <typename T>
using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

bool generator::create_zip(const std::filesystem::path& path, const std::filesystem::path& out_path) {
    const auto name = "wu_" + std::to_string(++current_wu_number) + ".zip";
    const auto zip_file_name = (out_path / name).string();
    int error = 0;
    const deleted_unique_ptr<zip_t> zip(zip_open(zip_file_name.data(), ZIP_CREATE | ZIP_EXCL, &error), [](auto* z) {
        if (z != nullptr) {
            zip_close(z);
        }
    });

    if (!zip) {
        zip_error_t zip_error;
        zip_error_init_with_code(&zip_error, error);
        std::cerr << "Failed to create archive <" << zip_file_name << ">: " << zip_error_strerror(&zip_error) << std::endl;
        return false;
    }

    for (const auto& file : std::filesystem::directory_iterator(path)) {
        if (file.is_regular_file()) {
            const deleted_unique_ptr<zip_source_t> source(zip_source_file(zip.get(), file.path().string().data(), 0, 0), [](auto*) {});
            if (!source) {
                std::cerr << "Failed to open file <" << file.path().string().data() << ">: " << zip_strerror(zip.get()) << std::endl;
            }
            if (zip_file_add(zip.get(), file.path().filename().string().data(), source.get(), ZIP_FL_ENC_UTF_8) < 0) {
                zip_source_free(source.get());
                std::cerr << "Failed to add file <" << file.path().string().data() << "> to archive : " << zip_strerror(zip.get()) << std::endl;
                return false;
            }
        }
    }

    return true;
}

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

bool generator::process(const std::filesystem::path& config_file_path, const std::filesystem::path& out_path) {
    if (!exists(config_file_path) || !is_regular_file(config_file_path)) {
        std::cerr << "Error happened while opening <" << config_file_path.string() << "> file" << std::endl;
        return false;
    }

    try {
        const std::ifstream config_file(config_file_path.c_str());
        std::stringstream buffer;
        buffer << config_file.rdbuf();

        const auto& json = jsoncons::json::parse(buffer);

        const auto& working_directory = config_file_path.has_parent_path() ? config_file_path.parent_path() : std::filesystem::current_path();

        prepare_receptors prepare_receptors;
        if (json.contains("prepare_receptors") && !prepare_receptors.load(json["prepare_receptors"], working_directory) && !prepare_receptors.validate()) {
            return false;
        }

        const auto need_prepare_receptors_step = prepare_receptors.has_data_loaded();

        config config;
        if (!config.load(config_file_path)) {
            return false;
        }

        if (!need_prepare_receptors_step && config.validate()) {
            return save_config(config, working_directory, out_path);
        }

    }
    catch (const std::exception& ex) {
        std::cerr << "Error happened while processing <" << config_file_path.string() << "> file: " << ex.what() << std::endl;
        return false;
    }

    return false;
}
