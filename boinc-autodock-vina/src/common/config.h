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

enum class scoring {
    ad4,
    vina,
    vinardo
};

struct input {
    std::string receptor;
    std::string flex;
    std::vector<std::string> ligands;
    std::vector<std::string> batch;
    scoring scoring = scoring::vina;
};

struct search_area {
    std::string maps;
    double center_x;
    double center_y;
    double center_z;
    double size_x;
    double size_y;
    double size_z;
    bool autobox = false;
};

struct output {
    std::string out;
    std::string dir;
    std::string write_maps;
};

struct advanced {
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
};

struct misc {
    int seed = 0;
    int exhaustiveness = 8;
    int max_evals = 0;
    int num_modes = 9;
    double min_rmsd = 1.0;
    double energy_range = 3.0;
    double spacing = 0.375;
};

struct config {
    input input;
    search_area search_area;
    output output;
    advanced advanced;
    misc misc;

    [[nodiscard]] bool validate() const;
    [[nodiscard]] bool load(const std::filesystem::path& config_file_path);
};
