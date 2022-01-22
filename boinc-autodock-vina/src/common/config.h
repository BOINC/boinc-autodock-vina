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

#include <string>
#include <vector>
#include <filesystem>

#include <jsoncons/json.hpp>
#include "jsoncons_helper/jsoncons_helper.h"

enum class scoring {
    ad4,
    vina,
    vinardo
};

class json_load {
public:
    virtual ~json_load() = default;

    [[nodiscard]] virtual bool load(const jsoncons::basic_json<char>& json, const std::filesystem::path& working_directory) = 0;
};

class json_save {
public:
    virtual ~json_save() = default;

    [[nodiscard]] virtual bool save(const json_encoder_helper& json, const std::filesystem::path& working_directory) const = 0;
};

class input final : public json_load, public json_save {
public:
    std::string receptor;
    std::string flex;
    std::vector<std::string> ligands;
    std::vector<std::string> batch;
    scoring scoring = scoring::vina;

    [[nodiscard]] bool load(const jsoncons::basic_json<char>& json, const std::filesystem::path& working_directory) override;
    [[nodiscard]] bool save(const json_encoder_helper& json, const std::filesystem::path& working_directory) const override;
};

class search_area final : public json_load, public json_save {
public:
    std::string maps;
    double center_x = .0;
    double center_y = .0;
    double center_z = .0;
    double size_x = .0;
    double size_y = .0;
    double size_z = .0;
    bool autobox = false;

    [[nodiscard]] bool load(const jsoncons::basic_json<char>& json, const std::filesystem::path& working_directory) override;
    [[nodiscard]] bool save(const json_encoder_helper& json, const std::filesystem::path& working_directory) const override;
};

class output final : public json_load, public json_save {
public:
    std::string out;
    std::string dir;
    std::string write_maps;

    [[nodiscard]] bool load(const jsoncons::basic_json<char>& json, const std::filesystem::path& working_directory) override;
    [[nodiscard]] bool save(const json_encoder_helper& json, const std::filesystem::path& working_directory) const override;
};

class advanced final : public json_load, public json_save {
public:
    bool score_only = false;
    bool local_only = false;
    bool no_refine = false;
    bool force_even_voxels = false;
    bool randomize_only = false;
    double weight_gauss1 = -0.035579;
    double weight_gauss2 = -0.005156;
    double weight_repulsion = 0.840245;
    double weight_hydrophobic = -0.035069;
    double weight_hydrogen = -0.587439;
    double weight_rot = 0.05846;
    double weight_vinardo_gauss1 = -0.045;
    double weight_vinardo_repulsion = 0.8;
    double weight_vinardo_hydrophobic = -0.035;
    double weight_vinardo_hydrogen = 0.600;
    double weight_vinardo_rot = 0.05846;
    double weight_ad4_vdw = 0.1662;
    double weight_ad4_hb = 0.1209;
    double weight_ad4_elec = 0.1406;
    double weight_ad4_dsolv = 0.1322;
    double weight_ad4_rot = 0.2983;
    double weight_glue = 50.000000;

    [[nodiscard]] bool load(const jsoncons::basic_json<char>& json, [[maybe_unused]] const std::filesystem::path& working_directory) override;
    [[nodiscard]] bool save(const json_encoder_helper& json, const std::filesystem::path& working_directory) const override;
};

class misc final : public json_load, public json_save {
public:
    int64_t seed = 0;
    int64_t exhaustiveness = 8;
    int64_t max_evals = 0;
    int64_t num_modes = 9;
    double min_rmsd = 1.0;
    double energy_range = 3.0;
    double spacing = 0.375;

    [[nodiscard]] bool load(const jsoncons::basic_json<char>& json, [[maybe_unused]] const std::filesystem::path& working_directory) override;
    [[nodiscard]] bool save(const json_encoder_helper& json, const std::filesystem::path& working_directory) const override;
};

class config {
public:
    input input;
    search_area search_area;
    output output;
    advanced advanced;
    misc misc;

    [[nodiscard]] bool validate() const;
    [[nodiscard]] bool check_files_exist() const;
    [[nodiscard]] bool load(const std::filesystem::path& config_file_path);
    [[nodiscard]] bool save(const std::filesystem::path& config_file_path) const;
    [[nodiscard]] std::vector<std::string> get_files() const;
    [[nodiscard]] std::vector<std::string> get_files_from_gpf() const;
    [[nodiscard]] std::filesystem::path get_gpf_filename() const;
};
