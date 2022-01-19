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

#include "input-config.h"

#include <iostream>
#include <fstream>
#include <random>
#include <zip.h>
#include <jsoncons/basic_json.hpp>
#include <boost/process.hpp>
#include <magic_enum.hpp>

#include "temp-folder.h"

bool prepare_receptors::load(const jsoncons::basic_json<char>& json, [[maybe_unused]] const std::filesystem::path& working_directory) {
    if (json.contains("receptors")) {
        for (const auto& r : json["receptors"].array_range()) {
            const auto& value = std::filesystem::path(r.as<std::string>());
            if (value.is_absolute()) {
                std::cerr << "Config should not contain absolute paths" << std::endl;
                return false;
            }
            receptors.emplace_back(std::filesystem::path(working_directory / value).string());
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
    if (receptors.empty()) {
        std::cerr << "No receptor file specified" << std::endl;
        return false;
    }

    for (const auto& r : receptors) {
        if (!std::filesystem::exists(r) || !std::filesystem::is_regular_file(r)) {
            std::cerr << "Receptor file <" << r << "> is not found." << std::endl;
            return false;
        }
    }
    return true;
}

bool prepare_ligands::load(const jsoncons::basic_json<char>& json, const std::filesystem::path& working_directory) {
    if (json.contains("ligand")) {
        const auto& value = std::filesystem::path(json["ligand"].as<std::string>());
        if (value.is_absolute()) {
            std::cerr << "Config should not contain absolute paths" << std::endl;
            return false;
        }
        ligand = std::filesystem::path(working_directory / value).string();
    }
    if (json.contains("selected_ligands")) {
        for (const auto& l : json["selected_ligands"].array_range()) {
            const auto& value = std::filesystem::path(l.as<std::string>());
            if (value.is_absolute()) {
                std::cerr << "Config should not contain absolute paths" << std::endl;
                return false;
            }
            selected_ligands.emplace_back(std::filesystem::path(working_directory / value).string());
        }
    }
    if (json.contains("multimol")) {
        multimol = json["multimol"].as<bool>();
    }
    if (json.contains("multimol_prefix")) {
        const auto& prefix = json["multimol_prefix"].as<std::string>();
        if (std::filesystem::path(prefix).is_absolute()) {
            std::cerr << "Config should not contain absolute paths" << std::endl;
            return false;
        }
        const std::string illegal = "/<>:\"\\|?*";

        if (std::any_of(prefix.cbegin(), prefix.cend(), [&](const auto& p) {
            return std::any_of(illegal.cbegin(), illegal.cend(), [&](const auto& i) {
                return i == p;
                });
            })) {
            std::cerr << "'multimol_prefix' contains illegal symbol" << std::endl;
            return false;
        }

        multimol_prefix = prefix;
    }
    if (json.contains("break_macrocycles")) {
        break_macrocycles = json["break_macrocycles"].as<bool>();
    }
    if (json.contains("hydrate")) {
        hydrate = json["hydrate"].as<bool>();
    }
    if (json.contains("keep_nonpolar_hydrogens")) {
        keep_nonpolar_hydrogens = json["keep_nonpolar_hydrogens"].as<bool>();
    }
    if (json.contains("correct_protonation_for_ph")) {
        correct_protonation_for_ph = json["correct_protonation_for_ph"].as<bool>();
    }
    if (json.contains("pH")) {
        pH = json["pH"].as<double>();
    }
    if (json.contains("flex")) {
        flex = json["flex"].as<bool>();
    }
    if (json.contains("rigidity_bonds_smarts")) {
        for (const auto& r : json["rigidity_bonds_smarts"].array_range()) {
            rigidity_bonds_smarts.push_back(json["rigidity_bonds_smarts"].as<std::string>());
        }
    }
    if (json.contains("rigidity_bonds_indices")) {
        for (const auto& r : json["rigidity_bonds_indices"].array_range()) {
            if (!r.is_array() || r.size() != 2) {
                std::cerr << "Rigidity Bonds indices should always be an array of pairs." << std::endl;
                return false;
            }
            bool first = true;
            std::pair<uint64_t, uint64_t> p;
            for (const auto& i : r.array_range()) {
                if (first) {
                    p.first = i.as<uint64_t>();
                    first = false;
                }
                else {
                    p.second = i.as<uint64_t>();
                }
            }
            rigidity_bonds_indices.push_back(p);
        }
    }
    if (json.contains("flexible_amides")) {
        flexible_amides = json["flexible_amides"].as<bool>();
    }
    if (json.contains("double_bond_penalty")) {
        double_bond_penalty = json["double_bond_penalty"].as<double>();
    }
    if (json.contains("remove_index_map")) {
        remove_index_map = json["remove_index_map"].as<bool>();
    }
    if (json.contains("remove_smiles")) {
        remove_smiles = json["remove_smiles"].as<bool>();
    }
    return true;
}

bool prepare_ligands::validate() const {
    if (ligand.empty()) {
        std::cerr << "No ligand file specified." << std::endl;
        return false;
    }

    if (!std::filesystem::exists(ligand) || !std::filesystem::is_regular_file(ligand)) {
        std::cerr << "Ligand file <" << ligand << "> is not found." << std::endl;
        return false;
    }
    if (rigidity_bonds_smarts.size() != rigidity_bonds_indices.size()) {
        std::cerr << "Count of Rigidity Bonds indices pairs should be equal to Rigidity Bonds SMARTS count." << std::endl;
        return false;
    }
    return true;
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
            const auto& file_path = file.path();
            const deleted_unique_ptr<zip_source_t> source(zip_source_file(zip.get(), file_path.string().data(), 0, 0), [](auto*) {});
            if (!source) {
                std::cerr << "Failed to open file <" << file.path().string().data() << ">: " << zip_strerror(zip.get()) << std::endl;
            }
            if (zip_file_add(zip.get(), file_path.filename().string().data(), source.get(), ZIP_FL_ENC_UTF_8) < 0) {
                zip_source_free(source.get());
                std::cerr << "Failed to add file <" << file.path().string().data() << "> to archive : " << zip_strerror(zip.get()) << std::endl;
                return false;
            }
        }
    }

    return true;
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

        prepare_ligands prepare_ligands;
        if (json.contains("prepare_ligands") && (!prepare_ligands.load(json["prepare_ligands"], working_directory) || !prepare_ligands.validate())) {
            return false;
        }

        prepare_receptors prepare_receptors;
        if (json.contains("prepare_receptors") && (!prepare_receptors.load(json["prepare_receptors"], working_directory) || !prepare_receptors.validate())) {
            return false;
        }

        config config;
        if (!config.load(config_file_path)) {
            return false;
        }

        const auto need_prepare_receptors_step = !prepare_receptors.receptors.empty();

        if (!need_prepare_receptors_step) {
            if (config.validate()) {
                return save_config(config, working_directory, out_path);
            }
        }

        if (need_prepare_receptors_step) {
            //TODO: Could run in parallel
            for (const auto& r : prepare_receptors.receptors) {
                std::stringstream cmd;
#ifdef WIN32
                cmd << "cmd /c ";
#endif
                cmd << "prepare_receptor ";
                cmd << "-r " << r << " ";

                std::filesystem::path output(r);
                output.replace_extension("pdbqt");
                config.input.receptor = output.string();
                cmd << "-o " << config.input.receptor << " ";

                if (prepare_receptors.repair != repair::None) {
                    cmd << "- A " << magic_enum::enum_name(prepare_receptors.repair) << " ";
                }
                for (const auto& p : prepare_receptors.preserves) {
                    if (p == "all") {
                        cmd << "-C ";
                    } else {
                        cmd << "-p " << p << " ";
                    }
                }
                if (prepare_receptors.cleanup != cleanup::none) {
                    cmd << "-U " << magic_enum::enum_name(prepare_receptors.cleanup) << " ";
                }
                if (prepare_receptors.delete_nonstd_residue) {
                    cmd << "-e ";
                }

                boost::process::child prepare_receptor(cmd.str());
                prepare_receptor.wait();
                if (prepare_receptor.exit_code() != 0) {
                    std::cerr << "Failed to prepare receptors: <" << cmd.str() << ">" << std::endl;
                }

                if (!config.validate()) {
                    std::cerr << "Failed to validate generated config." << std::endl;
                    return false;
                }
                if (!save_config(config, working_directory, out_path)) {
                    std::cerr << "Failed to save generated config." << std::endl;
                    return false;
                }
            }
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error happened while processing <" << config_file_path.string() << "> file: " << ex.what() << std::endl;
        return false;
    }
}
