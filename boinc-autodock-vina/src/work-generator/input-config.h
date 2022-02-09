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
    deleteAltB,
    none
};

class data_validate {
public:
    virtual ~data_validate() = default;

    [[nodiscard]] virtual bool validate() const = 0;
};

class prepare_receptors final : public json_load, public data_validate {
public:
    std::vector<std::string> receptors;
    repair repair = repair::None;
    std::vector<std::string> preserves;
    cleanup cleanup = cleanup::none;
    bool delete_nonstd_residue = false;

    [[nodiscard]] bool load(const jsoncons::basic_json<char>& json, const std::filesystem::path& working_directory) override;
    [[nodiscard]] bool validate() const override;
};

class prepare_ligands final : public json_load, public data_validate {
public:
    std::string ligand;
    std::vector<std::string> selected_ligands;
    bool multimol = false;
    std::string multimol_prefix;
    bool break_macrocycle = false;
    bool hydrate = false;
    bool keep_nonpolar_hydrogens = false;
    bool correct_protonation_for_ph = false;
    double pH = .0;
    bool flex = false;
    std::vector<std::string> rigidity_bonds_smarts;
    std::vector<std::pair<uint64_t, uint64_t>> rigidity_bonds_indices;
    bool flexible_amides = false;
    bool apply_double_bond_penalty = false;
    double double_bond_penalty = .0;
    bool remove_index_map = false;
    bool remove_smiles = false;

    [[nodiscard]] bool load(const jsoncons::basic_json<char>& json, const std::filesystem::path& working_directory) override;
    [[nodiscard]] bool validate() const override;
};

class generator {
public:
    [[nodiscard]] bool process(const std::filesystem::path& config_file_path, const std::filesystem::path& out_path, const std::string& prefix);
    [[nodiscard]] bool save_config(const config& config, const std::filesystem::path& working_directory, const std::filesystem::path& out_path, const std::string& prefix);
    [[nodiscard]] bool create_zip(const std::filesystem::path& path, const std::filesystem::path& out_path, const std::string& prefix);
    [[nodiscard]] uint64_t get_files_processed() const;

private:
    uint64_t current_wu_number = 0;
};
