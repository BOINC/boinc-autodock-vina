#include "calculate.h"

#include <iostream>

#include <autodock-vina/vina.h>
#include <magic_enum.hpp>

#include "common/config.h"

bool calculate(const std::filesystem::path& json) {
    config config;

    if (!config.load(json)) {
        std::cout << "Config load failed, cannot proceed further" << std::endl;
        return false;
    }

    if (!config.validate()) {
        std::cout << "Config validation failed, cannot proceed further" << std::endl;
        return false;
    }

    if (!config.advanced.score_only && config.output.out.empty() && config.input.ligands.size() == 1) {
        config.output.out = config.input.ligands[0];
    }

    Vina vina(std::string(magic_enum::enum_name(config.input.scoring)), config.misc.cpu,
        config.misc.seed, config.misc.verbosity, config.advanced.no_refine);

    if (!config.input.receptor.empty() || !config.input.flex.empty()) {
        vina.set_receptor(config.input.receptor, config.input.flex);
    }

    if (config.input.scoring == scoring::vina) {
        vina.set_vina_weights(config.advanced.weight_gauss1, config.advanced.weight_gauss2,
            config.advanced.weight_repulsion, config.advanced.weight_hydrophobic, config.advanced.weight_hydrogen,
            config.advanced.weight_glue, config.advanced.weight_rot);
    }
    else if (config.input.scoring == scoring::vinardo) {
        vina.set_vinardo_weights(config.advanced.weight_vinardo_gauss1, config.advanced.weight_vinardo_repulsion,
            config.advanced.weight_vinardo_hydrophobic, config.advanced.weight_vinardo_hydrogen,
            config.advanced.weight_glue, config.advanced.weight_vinardo_rot);
    }
    else if (config.input.scoring == scoring::ad4) {
        vina.set_ad4_weights(config.advanced.weight_ad4_vdw, config.advanced.weight_ad4_hb,
            config.advanced.weight_ad4_elec, config.advanced.weight_ad4_dsolv, config.advanced.weight_glue,
            config.advanced.weight_ad4_rot);
        vina.load_maps(config.search_area.maps);

        if (!config.output.write_maps.empty()) {
            vina.write_maps(config.output.write_maps);
        }
    }

    if (!config.input.ligands.empty()) {
        vina.set_ligand_from_file(config.input.ligands);

        if (config.input.scoring == scoring::vina || config.input.scoring == scoring::vinardo) {
            if (!config.search_area.maps.empty()) {
                vina.load_maps(config.search_area.maps);
            }
            else {
                /*if ((config.advanced.score_only || config.advanced.local_only) && config.search_area.autobox) {
                    const size_t buffer_size = 4;
                    std::vector<double> dim = vina.grid_dimensions_from_ligand(buffer_size);
                    vina.compute_vina_maps(dim[0], dim[1], dim[2], dim[3], dim[4], dim[5],
                        config.misc.spacing, config.advanced.force_even_voxels);
                }
                else */ {
                    vina.compute_vina_maps(config.search_area.center_x, config.search_area.center_y,
                        config.search_area.center_z, config.search_area.size_x, config.search_area.size_y,
                        config.search_area.size_z, config.misc.spacing,
                        config.advanced.force_even_voxels);
                }

                if (!config.output.write_maps.empty())
                    vina.write_maps(config.output.write_maps);
            }
        }

        if (config.advanced.randomize_only) {
            vina.randomize();
            vina.write_pose(config.output.out);
        }
        else if (config.advanced.score_only) {
            const auto& energies = vina.score();
            vina.show_score(energies);
        }
        else if (config.advanced.local_only) {
            const auto& energies = vina.optimize();
            vina.write_pose(config.output.out);
            vina.show_score(energies);
        }
        else {
            vina.global_search(config.misc.exhaustiveness, config.misc.num_modes, config.misc.min_rmsd,
                config.misc.max_evals);
            vina.write_poses(config.output.out, config.misc.num_modes, config.misc.energy_range);
        }
    }
    else if (!config.input.batch.empty()) {
        if (config.input.scoring == scoring::vina) {
            if (!config.search_area.maps.empty()) {
                vina.load_maps(config.search_area.maps);
            }
            else {
                vina.compute_vina_maps(config.search_area.center_x, config.search_area.center_y,
                    config.search_area.center_z, config.search_area.size_x, config.search_area.size_y,
                    config.search_area.size_z, config.misc.spacing);

                if (!config.output.write_maps.empty())
                    vina.write_maps(config.output.write_maps);
            }
        }

        for (const auto& b : config.input.batch) {
            vina.set_ligand_from_file(b);

            const auto& out_name = b;

            if (config.advanced.randomize_only) {
                vina.randomize();
                vina.write_pose(out_name);
            }
            else if (config.advanced.score_only) {
                vina.score();
            }
            else if (config.advanced.local_only) {
                vina.optimize();
                vina.write_pose(out_name);
            }
            else {
                vina.global_search(config.misc.exhaustiveness, config.misc.num_modes, config.misc.min_rmsd,
                    config.misc.max_evals);
                vina.write_poses(out_name, config.misc.num_modes, config.misc.energy_range);
            }
        }
    }

    return true;
}
