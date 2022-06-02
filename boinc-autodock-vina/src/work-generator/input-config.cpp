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
#include <jsoncons/basic_json.hpp>
#include <boost/process.hpp>
#include <magic_enum.hpp>

#include <common/zip-create.h>

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
            std::cerr << "'multimol_prefix' contains illegal symbols" << std::endl;
            return false;
        }

        multimol_prefix = prefix;
    }
    if (json.contains("break_macrocycle")) {
        break_macrocycle = json["break_macrocycle"].as<bool>();
    }
    if (json.contains("hydrate")) {
        hydrate = json["hydrate"].as<bool>();
    }
    if (json.contains("keep_nonpolar_hydrogens")) {
        keep_nonpolar_hydrogens = json["keep_nonpolar_hydrogens"].as<bool>();
    }
    if (json.contains("flex")) {
        flex = json["flex"].as<bool>();
    }
    if (json.contains("rigidity_bonds_smarts")) {
        for (const auto& r : json["rigidity_bonds_smarts"].array_range()) {
            rigidity_bonds_smarts.push_back(r.as<std::string>());
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
        apply_double_bond_penalty = true;
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

bool generator::save_config(const config& config, const std::filesystem::path& working_directory, const std::filesystem::path& out_path, const std::string& prefix) {
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

    return create_zip(temp_path(), out_path, prefix);
}

bool generator::create_zip(const std::filesystem::path& path, const std::filesystem::path& out_path, const std::string& prefix) {
    const auto name = "wu_" + prefix + "_" + std::to_string(++current_wu_number) + ".zip";
    const auto zip_file_name = (out_path / name).string();

    std::vector<std::filesystem::path> files;

    for (const auto& file : std::filesystem::directory_iterator(path)) {
        if (file.is_regular_file()) {
            files.push_back(file.path());
        }
    }

    return zip_create::create(zip_file_name, files);
}

uint64_t generator::get_files_processed() const {
    return current_wu_number;
}

bool generator::process(const std::filesystem::path& config_file_path, const std::filesystem::path& out_path, const std::string& prefix) {
#ifdef WIN32
    std::cerr << "This functionality doesn't work on Windows. 'mk_prepare_ligand.py' and 'prepare_receptor' are faked to pass tests on Windows." << std::endl;
#endif

    if (!exists(config_file_path) || !is_regular_file(config_file_path)) {
        std::cerr << "Error happened while opening <" << config_file_path.string() << "> file" << std::endl;
        return false;
    }

    const auto& working_directory = config_file_path.has_parent_path() ? config_file_path.parent_path() : std::filesystem::current_path();

    if (!process(std::ifstream(config_file_path.c_str()), working_directory, out_path, prefix)) {
        std::cerr << "Error happened while processing <" << config_file_path.string() << "> file" << std::endl;
        return false;
    }

    return true;
}

bool generator::process(const std::istream& config_stream, const std::filesystem::path& working_directory, const std::filesystem::path& out_path, const std::string& prefix) {
    try {
        std::stringstream buffer;
        buffer << config_stream.rdbuf();

        const auto& json = jsoncons::json::parse(buffer);

        prepare_ligands prepare_ligands;
        if (json.contains("prepare_ligands") && (!prepare_ligands.load(json["prepare_ligands"], working_directory) || !prepare_ligands.validate())) {
            return false;
        }

        prepare_receptors prepare_receptors;
        if (json.contains("prepare_receptors") && (!prepare_receptors.load(json["prepare_receptors"], working_directory) || !prepare_receptors.validate())) {
            return false;
        }

        config config;
        if (!config.load(json, working_directory)) {
            return false;
        }

        if (config.misc.seed == 0) {
            static std::random_device rd;
            static std::mt19937 mt(rd());
            static std::uniform_int_distribution dist(std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());
            config.misc.seed = dist(mt);
        }

        const auto need_prepare_receptors_step = !prepare_receptors.receptors.empty();
        const auto need_prepare_ligand_step = !prepare_ligands.ligand.empty();

        if (!need_prepare_receptors_step && !need_prepare_ligand_step) {
            if (config.validate()) {
                return save_config(config, working_directory, out_path, prefix);
            }
        }

        if (need_prepare_ligand_step) {
            std::stringstream cmd;
#ifdef WIN32
            const auto source_file = "boinc-autodock-vina\\samples\\basic_docking_full\\1iep_ligand.pdbqt.tmp";
            const auto target_file = "boinc-autodock-vina\\samples\\basic_docking_full\\1iep_ligand.pdbqt";
            cmd << "cmd /c copy " << source_file << " " << target_file;
            config.input.ligands.emplace_back((std::filesystem::current_path() / target_file).string());
#else
            cmd << "mk_prepare_ligand.py ";
            cmd << "-i " << prepare_ligands.ligand << " ";

            if (!prepare_ligands.multimol || prepare_ligands.multimol_prefix.empty() || prepare_ligands.selected_ligands.empty()) {
                std::filesystem::path output(prepare_ligands.ligand);
                output.replace_extension("pdbqt");
                cmd << "-o " << output.string() << " ";
                config.input.ligands.emplace_back(output.string());
            }
            if (prepare_ligands.break_macrocycle) {
                cmd << "-m ";
            }
            if (prepare_ligands.hydrate) {
                cmd << "-w ";
            }
            if (prepare_ligands.keep_nonpolar_hydrogens) {
                cmd << "--keep_nonpolar_hydrogens ";
            }
            if (prepare_ligands.flex) {
                cmd << "-f ";
            }
            if (!prepare_ligands.rigidity_bonds_smarts.empty()) {
                cmd << "-r ";
                for (const auto& r : prepare_ligands.rigidity_bonds_smarts) {
                    cmd << r << " ";
                }
            }
            if (!prepare_ligands.rigidity_bonds_indices.empty()) {
                cmd << "-b ";
                for (const auto& b : prepare_ligands.rigidity_bonds_indices) {
                    cmd << b.first << " " << b.second << " ";
                }
            }
            if (prepare_ligands.flexible_amides) {
                cmd << "-a ";
            }
            if (prepare_ligands.apply_double_bond_penalty) {
                cmd << "--double_bond_penalty " << prepare_ligands.double_bond_penalty << " ";
            }
            if (prepare_ligands.remove_index_map) {
                cmd << "--remove_index_map ";
            }
            if (prepare_ligands.remove_smiles) {
                cmd << "--remove_smiles ";
            }
            if (!prepare_ligands.multimol_prefix.empty()) {
                cmd << "--multimol_prefix " << prepare_ligands.multimol_prefix;
            }
#endif
            boost::process::child prepare_ligand(cmd.str());
            prepare_ligand.wait();
            if (prepare_ligand.exit_code() != 0) {
                std::cerr << "Failed to prepare ligand(s): <" << cmd.str() << ">" << std::endl;
                return false;
            }

            for (const auto& l : prepare_ligands.selected_ligands) {
                config.input.ligands.emplace_back(l);
            }
        }

        if (need_prepare_receptors_step) {
            //TODO: Could run in parallel
            for ([[maybe_unused]] const auto& r : prepare_receptors.receptors) {
                std::stringstream cmd;
#ifdef WIN32
                const auto source_file = "boinc-autodock-vina\\samples\\basic_docking_full\\1iep_receptor.pdbqt.tmp";
                const auto target_file = "boinc-autodock-vina\\samples\\basic_docking_full\\1iep_receptor.pdbqt";
                cmd << "cmd /c copy " << source_file << " " << target_file;
                config.input.receptor = (std::filesystem::current_path() / target_file).string();
#else
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
                    }
                    else {
                        cmd << "-p " << p << " ";
                    }
                }
                if (prepare_receptors.cleanup != cleanup::none) {
                    cmd << "-U " << magic_enum::enum_name(prepare_receptors.cleanup) << " ";
                }
                if (prepare_receptors.delete_nonstd_residue) {
                    cmd << "-e ";
                }
#endif

                boost::process::child prepare_receptor(cmd.str());
                prepare_receptor.wait();
                if (prepare_receptor.exit_code() != 0) {
                    std::cerr << "Failed to prepare receptors: <" << cmd.str() << ">" << std::endl;
                    return false;
                }

                if (!config.validate()) {
                    std::cerr << "Failed to validate generated config." << std::endl;
                    return false;
                }
                if (!save_config(config, working_directory, out_path, prefix)) {
                    std::cerr << "Failed to save generated config." << std::endl;
                    return false;
                }
            }
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error happened while processing input config: " << ex.what() << std::endl;
        return false;
    }
}
