// This file is part of BOINC.
// https://boinc.berkeley.edu
// Copyright (C) 2023 University of California
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

#include <iostream>
#include <fstream>
#include <sstream>

#include <magic_enum.hpp>

#include "jsoncons_helper/jsoncons_helper.h"

#include "config.h"

bool config::validate() const {
    if (!receptor.empty() && !maps.empty()) {
        std::cerr << "Cannot specify both receptor and maps at the same time,";
        std::cerr << "flex parameter is allowed with receptor or maps";
        std::cerr << std::endl;
        return false;
    }

    if (scoring == scoring::vina || scoring == scoring::vinardo) {
        if (receptor.empty() && maps.empty()) {
            std::cerr << "The receptor or maps must be specified.";
            std::cerr << std::endl;
            return false;
        }
    }
    else if (scoring == scoring::ad4) {
        if (!receptor.empty()) {
            std::cerr << "No receptor allowed, only flex parameter with the AD4 scoring function.";
            std::cerr << std::endl;
            return false;
        }
        if (maps.empty()) {
            std::cerr << "Maps are missing.";
            std::cerr << std::endl;
            return false;
        }
    }
    else {
        std::cerr << "Unknown scoring function.";
        std::cerr << std::endl;
        return false;
    }

    if (ligands.empty() && batch.empty()) {
        std::cerr << "Missing ligand(s).";
        std::cerr << std::endl;
        return false;
    }

    if (!ligands.empty() && !batch.empty()) {
        std::cerr << "Can't use both ligands and batch parameters simultaneously.";
        std::cerr << std::endl;
        return false;
    }

    if (!batch.empty() && dir.empty()) {
        std::cerr << "Need to specify an output directory for batch mode.";
        std::cerr << std::endl;
        return false;
    }

    if (out.empty()) {
        std::cerr << "Need to specify an output.out parameter";
        std::cerr << std::endl;
        return false;
    }

    return check_files_exist();
}

bool config::check_files_exist() const {
    for (const auto& file : get_files()) {
        if (!std::filesystem::exists(file) || !std::filesystem::is_regular_file(file)) {
            std::cerr << "Missing [" << std::filesystem::path(file).filename().string() << "] file specified in config.";
            std::cerr << std::endl;
            return false;
        }
    }

    return true;
}

bool config::load(const std::filesystem::path& config_file_path) {
    if (!is_regular_file(config_file_path)) {
        std::cerr << "Error happened while opening <" << config_file_path.filename().string() << "> file" << std::endl;
        return false;
    }

    const auto& working_directory = config_file_path.has_parent_path() ? config_file_path.parent_path() : std::filesystem::current_path();

    if (!load(std::ifstream(config_file_path), working_directory)) {
        std::cerr << "Error happened while processing <" << config_file_path.filename().string() << "> file" << std::endl;
        return false;
    }

    return true;
}

bool config::load(const std::istream& config_stream, const std::filesystem::path& working_directory) {
    try {
        std::stringstream buffer;
        buffer << config_stream.rdbuf();

        const auto& json = jsoncons::json::parse(buffer);
        return load(json, working_directory);
    }
    catch (const std::exception& ex) {
        std::cerr << "Error happened while processing config: " << ex.what() << std::endl;
        return false;
    }
}

bool config::load(const jsoncons::basic_json<char>& json, const std::filesystem::path& working_directory) {
    if (json.contains("receptor")) {
        const auto& value = std::filesystem::path(json["receptor"].as<std::string>());
        if (value.is_absolute()) {
            std::cerr << "Config should not contain absolute paths" << std::endl;
            return false;
        }
        receptor = std::filesystem::path(working_directory / value).string();
    }
    if (json.contains("flex")) {
        const auto& value = std::filesystem::path(json["flex"].as<std::string>());
        if (value.is_absolute()) {
            std::cerr << "Config should not contain absolute paths" << std::endl;
            return false;
        }
        flex = std::filesystem::path(working_directory / value).string();
    }
    const std::string ligand_field_name = json.contains("ligands") ? "ligands" : json.contains("ligand") ? "ligand" : "";
    if (!ligand_field_name.empty()) {
        if (json[ligand_field_name].is_array()) {
            for (const auto& r : json[ligand_field_name].array_range()) {
                const auto& value = std::filesystem::path(r.as<std::string>());
                if (value.is_absolute()) {
                    std::cerr << "Config should not contain absolute paths" << std::endl;
                    return false;
                }
                ligands.emplace_back(std::filesystem::path(working_directory / value).string());
            }
        }
        else {
            const auto& value = std::filesystem::path(json[ligand_field_name].as<std::string>());
            if (value.is_absolute()) {
                std::cerr << "Config should not contain absolute paths" << std::endl;
                return false;
            }
            ligands.emplace_back(std::filesystem::path(working_directory / value).string());
        }
    }
    if (json.contains("batch")) {
        for (const auto& b : json["batch"].array_range()) {
            const auto& value = std::filesystem::path(b.as<std::string>());
            if (value.is_absolute()) {
                std::cerr << "Config should not contain absolute paths" << std::endl;
                return false;
            }
            batch.emplace_back(std::filesystem::path(working_directory / value).string());
        }
    }
    if (json.contains("scoring")) {
        auto s = json["scoring"].as<std::string>();
        std::transform(s.begin(), s.end(), s.begin(), [](const auto c) { return std::tolower(c); });
        if (s == "vina") {
            scoring = scoring::vina;
        }
        else if (s == "vinardo") {
            scoring = scoring::vinardo;
        }
        else if (s == "ad4") {
            scoring = scoring::ad4;
        }
        else {
            std::cerr << "Wrong scoring function: [" << s << "]" << std::endl;
            return false;
        }
    }

    if (json.contains("maps")) {
        const auto& value = std::filesystem::path(json["maps"].as<std::string>());
        if (value.is_absolute()) {
            std::cerr << "Config should not contain absolute paths" << std::endl;
            return false;
        }
        maps = std::filesystem::path(working_directory / value).string();
    }
    if (json.contains("center_x")) {
        center_x = json["center_x"].as<double>();
    }
    if (json.contains("center_y")) {
        center_y = json["center_y"].as<double>();
    }
    if (json.contains("center_z")) {
        center_z = json["center_z"].as<double>();
    }
    if (json.contains("size_x")) {
        size_x = json["size_x"].as<double>();
    }
    if (json.contains("size_y")) {
        size_y = json["size_y"].as<double>();
    }
    if (json.contains("size_z")) {
        size_z = json["size_z"].as<double>();
    }

    if (json.contains("out")) {
        const auto& value = std::filesystem::path(json["out"].as<std::string>());
        if (value.is_absolute()) {
            std::cerr << "Config should not contain absolute paths" << std::endl;
            return false;
        }
        out = std::filesystem::path(working_directory / value).string();
    }
    if (json.contains("dir")) {
        const auto& value = std::filesystem::path(json["dir"].as<std::string>());
        if (value.is_absolute()) {
            std::cerr << "Config should not contain absolute paths" << std::endl;
            return false;
        }
        dir = std::filesystem::path(working_directory / value).string();
    }
    if (json.contains("write_maps")) {
        const auto& value = std::filesystem::path(json["write_maps"].as<std::string>());
        if (value.is_absolute()) {
            std::cerr << "Config should not contain absolute paths" << std::endl;
            return false;
        }
        write_maps = std::filesystem::path(working_directory / value).string();
    }

    if (json.contains("no_refine")) {
        no_refine = json["no_refine"].as<bool>();
    }
    if (json.contains("force_even_voxels")) {
        force_even_voxels = json["force_even_voxels"].as<bool>();
    }
    if (json.contains("weight_gauss1")) {
        weight_gauss1 = json["weight_gauss1"].as<double>();
    }
    else
    {
        if (scoring == scoring::vina)
        {
            weight_gauss1 = -0.035579;
        }
        else if (scoring == scoring::vinardo)
        {
            weight_gauss1 = -0.045;
        }
    }
    if (json.contains("weight_gauss2")) {
        weight_gauss2 = json["weight_gauss2"].as<double>();
    }
    if (json.contains("weight_repulsion")) {
        weight_repulsion = json["weight_repulsion"].as<double>();
    }
    else
    {
        if (scoring == scoring::vina)
        {
            weight_repulsion = 0.840245;
        }
        else if (scoring == scoring::vinardo)
        {
            weight_repulsion = 0.8;
        }
    }
    if (json.contains("weight_hydrophobic")) {
        weight_hydrophobic = json["weight_hydrophobic"].as<double>();
    }
    else
    {
        if (scoring == scoring::vina)
        {
            weight_hydrophobic = -0.035069;
        }
        else if (scoring == scoring::vinardo)
        {
            weight_hydrophobic = -0.035;
        }
    }
    if (json.contains("weight_hydrogen")) {
        weight_hydrogen = json["weight_hydrogen"].as<double>();
    }
    else
    {
        if (scoring == scoring::vina)
        {
            weight_hydrogen = -0.587439;
        }
        else if (scoring == scoring::vinardo)
        {
            weight_hydrogen = 0.600;
        }
    }
    if (json.contains("weight_rot")) {
        weight_rot = json["weight_rot"].as<double>();
    }
    if (json.contains("weight_ad4_vdw")) {
        weight_ad4_vdw = json["weight_ad4_vdw"].as<double>();
    }
    if (json.contains("weight_ad4_hb")) {
        weight_ad4_hb = json["weight_ad4_hb"].as<double>();
    }
    if (json.contains("weight_ad4_elec")) {
        weight_ad4_elec = json["weight_ad4_elec"].as<double>();
    }
    if (json.contains("weight_ad4_dsolv")) {
        weight_ad4_dsolv = json["weight_ad4_dsolv"].as<double>();
    }
    if (json.contains("weight_ad4_rot")) {
        weight_ad4_rot = json["weight_ad4_rot"].as<double>();
    }
    if (json.contains("weight_glue")) {
        weight_glue = json["weight_glue"].as<double>();
    }

    if (json.contains("seed")) {
        seed = json["seed"].as<int>();
    }
    if (json.contains("exhaustiveness")) {
        exhaustiveness = json["exhaustiveness"].as<int64_t>();
    }
    if (json.contains("max_evals")) {
        max_evals = json["max_evals"].as<int64_t>();
    }
    if (json.contains("num_modes")) {
        num_modes = json["num_modes"].as<int64_t>();
    }
    if (json.contains("min_rmsd")) {
        min_rmsd = json["min_rmsd"].as<double>();
    }
    if (json.contains("energy_range")) {
        energy_range = json["energy_range"].as<double>();
    }
    if (json.contains("spacing")) {
        spacing = json["spacing"].as<double>();
    }

    if (out.empty()) {
        out = std::filesystem::path(working_directory / "result.pdbqt").string();
    }

    return true;
}

bool config::save(const std::filesystem::path& config_file_path) const {
    const auto& error_message = [](const auto& value) {
        std::cerr << "Failed to write [" << value << "] value to json file";
        std::cerr << std::endl;
    };

    std::ofstream stream;
    stream.open(config_file_path);
    jsoncons::json_stream_encoder encoder(stream);
    json_encoder_helper json(encoder);

    if (!json.begin_object()) {
        std::cerr << "Failed to write [" << config_file_path.filename().string() << "] file";
        std::cerr << std::endl;
        return false;
    }

    const auto& filename_from_file = [](const auto& file) -> auto {
        return std::filesystem::path(file).filename().string();
    };

    if (!receptor.empty()) {
        if (!json.value("receptor", filename_from_file(receptor))) {
            error_message("receptor");
            return false;
        }
    }

    if (!flex.empty()) {
        if (!json.value("flex", filename_from_file(flex))) {
            error_message("flex");
            return false;
        }
    }

    if (!ligands.empty()) {
        if (ligands.size() > 1) {
            if (!json.begin_array("ligands")) {
                error_message("ligands");
                return false;
            }
            for (const auto& ligand : ligands) {
                if (!json.value(filename_from_file(ligand))) {
                    error_message("ligand");
                    return false;
                }
            }
            if (!json.end_array()) {
                error_message("ligands");
                return false;
            }
        }
        else {
            if (!json.value("ligand", filename_from_file(ligands.front()))) {
                error_message("ligand");
                return false;
            }
        }
    }

    if (!batch.empty()) {
        if (!json.begin_array("batch")) {
            error_message("batch");
            return false;
        }
        for (const auto& b : batch) {
            if (!json.value(filename_from_file(b))) {
                error_message("batch");
                return false;
            }
        }
        if (!json.end_array()) {
            error_message("batch");
            return false;
        }
    }

    if (!json.value("scoring", std::string(magic_enum::enum_name(scoring)))) {
        error_message("scoring");
        return false;
    }

    if (!maps.empty()) {
        if (!json.value("maps", filename_from_file(maps))) {
            error_message("maps");
            return false;
        }
    }

    if (!json.value("center_x", center_x)) {
        error_message("center_x");
        return false;
    }

    if (!json.value("center_y", center_y)) {
        error_message("center_y");
        return false;
    }

    if (!json.value("center_z", center_z)) {
        error_message("center_z");
        return false;
    }

    if (!json.value("size_x", size_x)) {
        error_message("size_x");
        return false;
    }

    if (!json.value("size_y", size_y)) {
        error_message("size_y");
        return false;
    }

    if (!json.value("size_z", size_z)) {
        error_message("size_z");
        return false;
    }

    if (!out.empty()) {
        if (!json.value("out", filename_from_file(out))) {
            error_message("out");
            return false;
        }
    }

    if (!dir.empty()) {
        if (!json.value("dir", filename_from_file(dir))) {
            error_message("dir");
            return false;
        }
    }

    if (!write_maps.empty()) {
        if (!json.value("write_maps", filename_from_file(write_maps))) {
            error_message("write_maps");
            return false;
        }
    }

    if (!json.value("no_refine", no_refine)) {
        error_message("no_refine");
        return false;
    }

    if (!json.value("force_even_voxels", force_even_voxels)) {
        error_message("force_even_voxels");
        return false;
    }

    if (!json.value("weight_gauss1", weight_gauss1)) {
        error_message("weight_gauss1");
        return false;
    }

    if (!json.value("weight_gauss2", weight_gauss2)) {
        error_message("weight_gauss2");
        return false;
    }

    if (!json.value("weight_repulsion", weight_repulsion)) {
        error_message("weight_repulsion");
        return false;
    }

    if (!json.value("weight_hydrophobic", weight_hydrophobic)) {
        error_message("weight_hydrophobic");
        return false;
    }

    if (!json.value("weight_hydrogen", weight_hydrogen)) {
        error_message("weight_hydrogen");
        return false;
    }

    if (!json.value("weight_rot", weight_rot)) {
        error_message("weight_rot");
        return false;
    }

    if (!json.value("weight_ad4_vdw", weight_ad4_vdw)) {
        error_message("weight_ad4_vdw");
        return false;
    }

    if (!json.value("weight_ad4_hb", weight_ad4_hb)) {
        error_message("weight_ad4_hb");
        return false;
    }

    if (!json.value("weight_ad4_elec", weight_ad4_elec)) {
        error_message("weight_ad4_elec");
        return false;
    }

    if (!json.value("weight_ad4_dsolv", weight_ad4_dsolv)) {
        error_message("weight_ad4_dsolv");
        return false;
    }

    if (!json.value("weight_ad4_rot", weight_ad4_rot)) {
        error_message("weight_ad4_rot");
        return false;
    }

    if (!json.value("weight_glue", weight_glue)) {
        error_message("weight_glue");
        return false;
    }

    if (!json.value("seed", seed)) {
        error_message("seed");
        return false;
    }

    if (!json.value("exhaustiveness", exhaustiveness)) {
        error_message("exhaustiveness");
        return false;
    }

    if (!json.value("max_evals", max_evals)) {
        error_message("max_evals");
        return false;
    }

    if (!json.value("num_modes", num_modes)) {
        error_message("num_modes");
        return false;
    }

    if (!json.value("min_rmsd", min_rmsd)) {
        error_message("min_rmsd");
        return false;
    }

    if (!json.value("energy_range", energy_range)) {
        error_message("energy_range");
        return false;
    }

    if (!json.value("spacing", spacing)) {
        error_message("spacing");
        return false;
    }

    if (!json.end_object()) {
        std::cerr << "Failed to write [" << config_file_path.filename().string() << "] file";
        std::cerr << std::endl;
        return false;
    }

    return true;
}

std::vector<std::string> config::get_files() const {
    std::vector<std::string> files;

    if (!receptor.empty()) {
        files.push_back(receptor);
    }

    if (!ligands.empty()) {
        for (const auto& ligand : ligands) {
            files.push_back(ligand);
        }
    }

    if (!flex.empty()) {
        files.push_back(flex);
    }

    if (!batch.empty()) {
        for (const auto& b : batch) {
            files.push_back(b);
        }
    }

    if (!maps.empty()) {
        files.emplace_back(get_gpf_filename().string());
        const auto& gpf_files = get_files_from_gpf();
        files.insert(files.end(), gpf_files.cbegin(), gpf_files.cend());
    }

    return files;
}

std::vector<std::string> config::get_files_from_gpf() const {
    return get_files_from_gpf(get_gpf_filename());
}

std::vector<std::string> config::get_files_from_gpf(const std::filesystem::path& maps) {
    if (!exists(maps) || !is_regular_file(maps)) {
        std::cerr << "Failed to find maps file <" << maps.filename().string() << ">." << std::endl;
        return {};
    }

    const auto& path = maps.parent_path();

    const std::array<std::string, 4> types{ "map", "gridfld", "elecmap", "dsolvmap" };

    std::vector<std::string> files;

    std::ifstream gpf(maps);
    std::string line;
    while (std::getline(gpf, line)) {
        std::istringstream iss(line);
        const std::vector results((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());
        if (results.size() > 1 && std::any_of(types.cbegin(), types.cend(), [&](const auto& t) { return t == results[0]; })) {
            files.emplace_back((path / results[1]).string());
        }
    }

    return files;
}

std::filesystem::path config::get_gpf_filename() const {
    return std::filesystem::path(maps).replace_extension("gpf");
}

std::vector<std::string> config::get_out_files() const {
    std::vector<std::string> files;

    if (!out.empty()) {
        files.push_back(out);
    }
    if (!dir.empty()) {
        for (const auto& file : std::filesystem::directory_iterator(dir)) {
            if (file.is_regular_file()) {
                files.push_back(file.path().string());
            }
        }
    }
    if (!write_maps.empty()) {
        const auto m = std::filesystem::path(write_maps);
        for (const auto& file : std::filesystem::directory_iterator(m.parent_path())) {
            if (file.path().filename().string().rfind(m.filename().string(), 0) == 0 && file.is_regular_file()) {
                files.push_back(file.path().string());
            }
        }
    }

    return files;
}
