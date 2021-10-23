#include <iostream>
#include <fstream>
#include <sstream>

#include <jsoncons/json.hpp>

#include "config.h"

bool config::validate() const {
    if (!input.receptor.empty() && !search_area.maps.empty()) {
        std::cerr << "Cannot specify both Input.receptor and SearchArea.maps at the same time,";
        std::cerr << "Input.flex parameter is allowed with Input.receptor or SearchArea.maps";
        std::cerr << std::endl;
        return false;
    }

    if (input.scoring == scoring::vina || input.scoring == scoring::vinardo) {
        if (input.receptor.empty() && search_area.maps.empty()) {
            std::cerr << "The Input.receptor or SearchArea.maps must be specified.";
            std::cerr << std::endl;
            return false;
        }
    }
    else if (input.scoring == scoring::ad4) {
        if (!input.receptor.empty()) {
            std::cerr << "No Input.receptor allowed, only Input.flex parameter with the AD4 scoring function.";
            std::cerr << std::endl;
            return false;
        }
        if (search_area.maps.empty()) {
            std::cerr << "SearchArea.maps are missing.";
            std::cerr << std::endl;
            return false;
        }
    }
    else {
        std::cerr << "Unknown scoring function.";
        std::cerr << std::endl;
        return false;
    }

    if (input.ligands.empty() && input.batch.empty()) {
        std::cerr << "Missing ligand(s).";
        std::cerr << std::endl;
        return false;
    }

    if (!input.ligands.empty() && !input.batch.empty()) {
        std::cerr << "Can't use both Input.ligands and Input.batch parameters simultaneously.";
        std::cerr << std::endl;
        return false;
    }

    if (!input.batch.empty() && output.dir.empty()) {
        std::cerr << "Need to specify an output directory for batch mode.";
        std::cerr << std::endl;
        return false;
    }

    if (!advanced.score_only) {
        if (output.out.empty() && input.ligands.size() > 1) {
            std::cerr << "Output.out must be defined when docking simultaneously multiple ligands.";
            std::cerr << std::endl;
            return false;
        }
    }

    return true;
}

bool config::load(const std::filesystem::path& config_file_path) {
    if (!is_regular_file(config_file_path)) {
        std::cerr << "Error happened while opening <" << config_file_path.string() << "> file" << std::endl;
        return false;
    }

    try {
        const auto& working_directory = config_file_path.has_parent_path() ? config_file_path.parent_path() : std::filesystem::current_path();

        const std::ifstream config_file(config_file_path.c_str());
        std::stringstream buffer;
        buffer << config_file.rdbuf();

        const auto& json = jsoncons::json::parse(buffer);

        if (json.contains("input")) {
            const auto& json_input = json["input"];

            if (json_input.contains("receptor")) {
                const auto& receptor = std::filesystem::path(json_input["receptor"].as<std::string>());
                if (receptor.is_absolute()) {
                    std::cerr << "Config should not contain absolute paths" << std::endl;
                    return false;
                }
                input.receptor = std::filesystem::path(working_directory / receptor).string();
            }
            if (json_input.contains("flex")) {
                const auto& flex = std::filesystem::path(json_input["flex"].as<std::string>());
                if (flex.is_absolute()) {
                    std::cerr << "Config should not contain absolute paths" << std::endl;
                    return false;
                }
                input.flex = std::filesystem::path(working_directory / flex).string();
            }
            if (json_input.contains("ligands")) {
                for (const auto& l : json_input["ligands"].array_range()) {
                    const auto& ligand = std::filesystem::path(l.as<std::string>());
                    if (ligand.is_absolute()) {
                        std::cerr << "Config should not contain absolute paths" << std::endl;
                        return false;
                    }
                    input.ligands.push_back(std::filesystem::path(working_directory / ligand).string());
                }
            }
            if (json_input.contains("batch")) {
                for (const auto& b : json_input["batch"].array_range()) {
                    const auto& batch = std::filesystem::path(b.as<std::string>());
                    if (batch.is_absolute()) {
                        std::cerr << "Config should not contain absolute paths" << std::endl;
                        return false;
                    }
                    input.batch.push_back(std::filesystem::path(working_directory / batch).string());
                }
            }
            if (json_input.contains("scoring")) {
                auto s = json_input["scoring"].as<std::string>();
                std::transform(s.begin(), s.end(), s.begin(), [](const auto c) { return std::tolower(c); });
                if (s == "vina") {
                    input.scoring = scoring::vina;
                } else if (s == "vinardo") {
                    input.scoring = scoring::vinardo;
                } else if (s == "ad4") {
                    input.scoring = scoring::ad4;
                } else {
                    std::cerr << "Wrong scoring function: [" << s << "]" << std::endl;
                    return false;
                }
            }
        }

        if (json.contains("search_area")) {
            const auto& json_search_area = json["search_area"];

            if (json_search_area.contains("maps")) {
                const auto& maps = std::filesystem::path(json_search_area["maps"].as<std::string>());
                if (maps.is_absolute()) {
                    std::cerr << "Config should not contain absolute paths" << std::endl;
                    return false;
                }
                search_area.maps = std::filesystem::path(working_directory / maps).string();
            }
            if (json_search_area.contains("center_x")) {
                search_area.center_x = json_search_area["center_x"].as<double>();
            }
            if (json_search_area.contains("center_y")) {
                search_area.center_y = json_search_area["center_y"].as<double>();
            }
            if (json_search_area.contains("center_z")) {
                search_area.center_z = json_search_area["center_z"].as<double>();
            }
            if (json_search_area.contains("size_x")) {
                search_area.size_x = json_search_area["size_x"].as<double>();
            }
            if (json_search_area.contains("size_y")) {
                search_area.size_y = json_search_area["size_y"].as<double>();
            }
            if (json_search_area.contains("size_z")) {
                search_area.size_z = json_search_area["size_z"].as<double>();
            }
            if (json_search_area.contains("autobox")) {
                search_area.autobox = json_search_area["autobox"].as<bool>();
            }
        }

        if (json.contains("output")) {
            const auto& json_output = json["output"];

            if (json_output.contains("out")) {
                const auto& out = std::filesystem::path(json_output["out"].as<std::string>());
                if (out.is_absolute()) {
                    std::cerr << "Config should not contain absolute paths" << std::endl;
                    return false;
                }
                output.out = std::filesystem::path(working_directory / out).string();
            }
            if (json_output.contains("dir")) {
                const auto& dir = std::filesystem::path(json_output["dir"].as<std::string>());
                if (dir.is_absolute()) {
                    std::cerr << "Config should not contain absolute paths" << std::endl;
                    return false;
                }
                output.dir = std::filesystem::path(working_directory / dir).string();
            }
            if (json_output.contains("write_maps")) {
                const auto& write_maps = std::filesystem::path(json_output["write_maps"].as<std::string>());
                if (write_maps.is_absolute()) {
                    std::cerr << "Config should not contain absolute paths" << std::endl;
                    return false;
                }
                output.write_maps = std::filesystem::path(working_directory / write_maps).string();
            }
        }

        if (json.contains("advanced")) {
            const auto& json_advanced = json["advanced"];

            if (json_advanced.contains("score_only")) {
                advanced.score_only = json_advanced["score_only"].as<bool>();
            }
            if (json_advanced.contains("local_only")) {
                advanced.local_only = json_advanced["local_only"].as<bool>();
            }
            if (json_advanced.contains("no_refine")) {
                advanced.no_refine = json_advanced["no_refine"].as<bool>();
            }
            if (json_advanced.contains("force_even_voxels")) {
                advanced.force_even_voxels = json_advanced["force_even_voxels"].as<bool>();
            }
            if (json_advanced.contains("randomize_only")) {
                advanced.randomize_only = json_advanced["randomize_only"].as<bool>();
            }
            if (json_advanced.contains("weight_gauss1")) {
                advanced.weight_gauss1 = json_advanced["weight_gauss1"].as<double>();
            }
            if (json_advanced.contains("weight_gauss2")) {
                advanced.weight_gauss2 = json_advanced["weight_gauss2"].as<double>();
            }
            if (json_advanced.contains("weight_repulsion")) {
                advanced.weight_repulsion = json_advanced["weight_repulsion"].as<double>();
            }
            if (json_advanced.contains("weight_hydrophobic")) {
                advanced.weight_hydrophobic = json_advanced["weight_hydrophobic"].as<double>();
            }
            if (json_advanced.contains("weight_hydrogen")) {
                advanced.weight_hydrogen = json_advanced["weight_hydrogen"].as<double>();
            }
            if (json_advanced.contains("weight_rot")) {
                advanced.weight_rot = json_advanced["weight_rot"].as<double>();
            }
            if (json_advanced.contains("weight_vinardo_gauss1")) {
                advanced.weight_vinardo_gauss1 = json_advanced["weight_vinardo_gauss1"].as<double>();
            }
            if (json_advanced.contains("weight_vinardo_repulsion")) {
                advanced.weight_vinardo_repulsion = json_advanced["weight_vinardo_repulsion"].as<double>();
            }
            if (json_advanced.contains("weight_vinardo_hydrophobic")) {
                advanced.weight_vinardo_hydrophobic = json_advanced["weight_vinardo_hydrophobic"].as<double>();
            }
            if (json_advanced.contains("weight_vinardo_hydrogen")) {
                advanced.weight_vinardo_hydrogen = json_advanced["weight_vinardo_hydrogen"].as<double>();
            }
            if (json_advanced.contains("weight_vinardo_rot")) {
                advanced.weight_vinardo_rot = json_advanced["weight_vinardo_rot"].as<double>();
            }
            if (json_advanced.contains("weight_ad4_vdw")) {
                advanced.weight_ad4_vdw = json_advanced["weight_ad4_vdw"].as<double>();
            }
            if (json_advanced.contains("weight_ad4_hb")) {
                advanced.weight_ad4_hb = json_advanced["weight_ad4_hb"].as<double>();
            }
            if (json_advanced.contains("weight_ad4_elec")) {
                advanced.weight_ad4_elec = json_advanced["weight_ad4_elec"].as<double>();
            }
            if (json_advanced.contains("weight_ad4_dsolv")) {
                advanced.weight_ad4_dsolv = json_advanced["weight_ad4_dsolv"].as<double>();
            }
            if (json_advanced.contains("weight_ad4_rot")) {
                advanced.weight_ad4_rot = json_advanced["weight_ad4_rot"].as<double>();
            }
            if (json_advanced.contains("weight_glue")) {
                advanced.weight_glue = json_advanced["weight_glue"].as<double>();
            }
        }

        if (json.contains("misc")) {
            const auto& json_misc = json["misc"];

            if (json_misc.contains("cpu")) {
                misc.cpu = json_misc["cpu"].as<int>();
            }
            if (json_misc.contains("seed")) {
                misc.seed = json_misc["seed"].as<int>();
            }
            if (json_misc.contains("exhaustiveness")) {
                misc.exhaustiveness = json_misc["exhaustiveness"].as<int>();
            }
            if (json_misc.contains("max_evals")) {
                misc.max_evals = json_misc["max_evals"].as<int>();
            }
            if (json_misc.contains("num_modes")) {
                misc.num_modes = json_misc["num_modes"].as<int>();
            }
            if (json_misc.contains("min_rmsd")) {
                misc.min_rmsd = json_misc["min_rmsd"].as<double>();
            }
            if (json_misc.contains("energy_range")) {
                misc.energy_range = json_misc["energy_range"].as<double>();
            }
            if (json_misc.contains("spacing")) {
                misc.spacing = json_misc["spacing"].as<double>();
            }
            if (json_misc.contains("verbosity")) {
                misc.verbosity = json_misc["verbosity"].as<int>();
            }
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error happened while processing <" << config_file_path.string() << "> file: " << ex.what() << std::endl;
        return false;
    }

    return true;
}
