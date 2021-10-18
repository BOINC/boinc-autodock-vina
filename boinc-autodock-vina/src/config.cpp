#include <iostream>

#include "config.h"

bool ValidateConfig(const Config& config) {
    if (!config.input.receptor.empty() && !config.search_area.maps.empty()) {
        std::cerr << "Cannot specify both Input.receptor and SearchArea.maps at the same time,";
        std::cerr << "Input.flex parameter is allowed with Input.receptor or SearchArea.maps";
        std::cerr << std::endl;
        return false;
    }

    if (config.input.scoring == scoring::vina || config.input.scoring == scoring::vinardo) {
        if (config.input.receptor.empty() && config.search_area.maps.empty()) {
            std::cerr << "The Input.receptor or SearchArea.maps must be specified.";
            std::cerr << std::endl;
            return false;
        }
    }
    else if (config.input.scoring == scoring::ad4) {
        if (!config.input.receptor.empty()) {
            std::cerr << "No Input.receptor allowed, only Input.flex parameter with the AD4 scoring function.";
            std::cerr << std::endl;
            return false;
        }
        if (config.search_area.maps.empty()) {
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

    if (config.input.ligands.empty() && config.input.batch.empty()) {
        std::cerr << "Missing ligand(s).";
        std::cerr << std::endl;
        return false;
    }

    if (!config.input.ligands.empty() && !config.input.batch.empty()) {
        std::cerr << "Can't use both Input.ligands and Input.batch parameters simultaneously.";
        std::cerr << std::endl;
        return false;
    }

    if (!config.input.batch.empty() && config.output.dir.empty()) {
        std::cerr << "Need to specify an output directory for batch mode.";
        std::cerr << std::endl;
        return false;
    }

    if (!config.advanced.score_only) {
        if (config.output.out.empty() && config.input.ligands.size() > 1) {
            std::cerr << "Output.out must be defined when docking simultaneously multiple ligands.";
            std::cerr << std::endl;
            return false;
        }
    }

    return true;
}
