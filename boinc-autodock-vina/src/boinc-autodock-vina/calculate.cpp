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

#include "calculate.h"

#include <autodock-vina/vina.h>
#include <magic_enum.hpp>

bool calculator::calculate(const config& config, const int& ncpus, const std::function<void(double)>& progress_callback) {
    constexpr int vina_verbosity = 1;

    Vina vina(std::string(magic_enum::enum_name(config.scoring)), ncpus,
        config.seed, vina_verbosity, config.no_refine,
        const_cast<std::function<void(double)>*>(&progress_callback));

    if (!config.receptor.empty() || !config.flex.empty()) {
        vina.set_receptor(config.receptor, config.flex);
    }

    if (config.scoring == scoring::vina) {
        vina.set_vina_weights(config.weight_gauss1, config.weight_gauss2,
            config.weight_repulsion, config.weight_hydrophobic, config.weight_hydrogen,
            config.weight_glue, config.weight_rot);
    }
    else if (config.scoring == scoring::vinardo) {
        vina.set_vinardo_weights(config.weight_gauss1, config.weight_repulsion,
            config.weight_hydrophobic, config.weight_hydrogen,
            config.weight_glue, config.weight_rot);
    }
    else if (config.scoring == scoring::ad4) {
        vina.set_ad4_weights(config.weight_ad4_vdw, config.weight_ad4_hb,
            config.weight_ad4_elec, config.weight_ad4_dsolv, config.weight_glue,
            config.weight_ad4_rot);
        vina.load_maps(config.maps);

        if (!config.write_maps.empty()) {
            vina.write_maps(config.write_maps);
        }
    }

    if (!config.ligands.empty()) {
        vina.set_ligand_from_file(config.ligands);

        if (config.scoring == scoring::vina || config.scoring == scoring::vinardo) {
            if (!config.maps.empty()) {
                vina.load_maps(config.maps);
            }
            else {
                vina.compute_vina_maps(config.center_x, config.center_y,
                    config.center_z, config.size_x, config.size_y,
                    config.size_z, config.spacing,
                    config.force_even_voxels);

                if (!config.write_maps.empty())
                    vina.write_maps(config.write_maps);
            }
        }

        vina.global_search(config.exhaustiveness, config.num_modes, config.min_rmsd,
            config.max_evals);
        vina.write_poses(config.out, config.num_modes, config.energy_range);
    }
    else if (!config.batch.empty()) {
        if (config.scoring == scoring::vina) {
            if (!config.maps.empty()) {
                vina.load_maps(config.maps);
            }
            else {
                vina.compute_vina_maps(config.center_x, config.center_y,
                    config.center_z, config.size_x, config.size_y,
                    config.size_z, config.spacing);

                if (!config.write_maps.empty())
                    vina.write_maps(config.write_maps);
            }
        }

        for (const auto& b : config.batch) {
            vina.set_ligand_from_file(b);

            const auto& out_name = (std::filesystem::path(config.dir) / b).string();

            vina.global_search(config.exhaustiveness, config.num_modes, config.min_rmsd,
                config.max_evals);
            vina.write_poses(out_name, config.num_modes, config.energy_range);
        }
    }

    return true;
}
