#include <iostream>

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
