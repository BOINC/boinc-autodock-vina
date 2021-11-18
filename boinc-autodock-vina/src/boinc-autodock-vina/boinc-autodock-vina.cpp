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

#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
#include <cmath>

#include <boinc/boinc_api.h>

#include "calculate.h"

#ifndef BOINC_AUTODOCK_VINA_VERSION
#define BOINC_AUTODOCK_VINA_VERSION "unknown"
#endif

#ifndef BOINC_APPS_GIT_REVISION
#define BOINC_APPS_GIT_REVISION "unknown"
#endif

inline void help() {
    std::cerr << "Usage:" << std::endl;
    std::cerr << "boinc-autodock-vina config.json" << std::endl;
}

inline void header() {
    std::cout << "Starting BOINC Autodock Vina v" << BOINC_AUTODOCK_VINA_VERSION;
    std::cout << " (" << BOINC_APPS_GIT_REVISION << ")" << std::endl;
}

auto prev_value = 0.;
constexpr auto precision = 0.001;

inline void report_progress(double value) {
    if (std::abs(value - prev_value) > precision) {
        prev_value = value;
        boinc_fraction_done(value);
    }
}

inline std::filesystem::path get_checkpoint_file_path(const std::string& json) {
    const auto& json_path = std::filesystem::path(json);
    const auto& checkpoint_file_name = json_path.stem().string() + ".checkpoint";
    const auto& working_dir = json_path.has_parent_path() ? json_path.parent_path() : std::filesystem::current_path();
    const auto& checkpoint_file_path = (working_dir / checkpoint_file_name);

    return checkpoint_file_path;
}

inline std::vector<std::string> get_checkpoint_data(const std::string& json) {
    const auto& checkpoint_file_path = get_checkpoint_file_path(json);

    if (!exists(checkpoint_file_path)) {
        return {};
    }

    std::ifstream checkpoint(checkpoint_file_path.string());
    std::string line;
    std::vector<std::string> data;
    while(std::getline(checkpoint, line)) {
        if (!line.empty()) {
            data.push_back(line);
        }
    }
    checkpoint.close();

    return data;
}

inline void save_checkpoint_data(const std::string& json, const std::string& data) {
    const auto& checkpoint_file_path = get_checkpoint_file_path(json);

    std::ofstream checkpoint;
    checkpoint.open(checkpoint_file_path.string(), std::ios_base::app);
    checkpoint << data << std::endl;
    checkpoint.close();
}

inline void remove_checkpoint_data_file(const std::string& json) {
    if (const auto& checkpoint_file_path = get_checkpoint_file_path(json); exists(checkpoint_file_path)) {
        std::filesystem::remove(checkpoint_file_path);
    }
}

int perform_docking(const std::string& json) noexcept {
    char buf[256];

    try {
        BOINC_OPTIONS options;
        boinc_options_defaults(options);
        options.multi_thread = true;

        if (const auto res = boinc_init_options(&options)) {
            std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << " boinc_init failed with error code " << res << std::endl;
            return res;
        }

        APP_INIT_DATA app_data;
        boinc_get_init_data(app_data);
        const int ncpus = static_cast<int>(ceil(app_data.ncpus));

        std::atomic result(false);

        std::vector<config> configs;

        config conf;

        if (!conf.load(json)) {
            std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << "Config load failed, cannot proceed further" << std::endl;
            boinc_finish(1.);
            return 1;
        }

        if (!conf.validate()) {
            std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << "Config validation failed, cannot proceed further" << std::endl;
            boinc_finish(1.);
            return 1;
        }

        if (conf.input.ligands.size() == 1 || conf.input.batch.size() == 1) {
            configs.push_back(conf);
        }
        else if (conf.input.ligands.size() > 1) {
            const auto& out_path = std::filesystem::path(conf.output.dir);
            if (!exists(out_path)) {
                create_directory(out_path);
            }

            for (const auto& ligand : conf.input.ligands) {
                auto new_conf = conf;
                new_conf.input.ligands.clear();
                new_conf.input.ligands.push_back(ligand);
                const auto& ligand_out_name = out_path / std::filesystem::path(ligand).filename();
                new_conf.output.out = ligand_out_name.string();
                configs.push_back(std::move(new_conf));
            }
        }
        else if (conf.input.batch.size() > 1) {
            for (const auto& batch : conf.input.batch) {
                auto new_conf = conf;
                new_conf.input.batch.clear();
                new_conf.input.batch.push_back(batch);
                configs.push_back(std::move(new_conf));
            }
        }

        boinc_fraction_done(0.);

        std::thread worker([&result, &configs, &ncpus, &json] {
            char str[256];
            try {
                const auto total = static_cast<double>(configs.size());
                const auto& checkpoint_data = get_checkpoint_data(json);
                for (std::vector<config>::size_type i = 0; i < configs.size(); ++i) {
                    const auto& current_config = configs[i];

                    std::string current_data;
                    if (!current_config.input.ligands.empty() && current_config.input.ligands.size() == 1) {
                        current_data = current_config.input.ligands.front();
                    }
                    else if (!current_config.input.batch.empty() && current_config.input.batch.size() == 1) {
                        current_data = current_config.input.batch.front();
                    }

                    if (!checkpoint_data.empty()) {
                        if (!current_data.empty()) {
                            if (std::find(checkpoint_data.cbegin(), checkpoint_data.cend(), current_data) != checkpoint_data.cend()) {
                                continue;
                            }
                        }
                    }

                    const auto current = static_cast<double>(i) + 1.;
                    result = calculate(configs[i], ncpus, [&current, &total](auto value) {
                        report_progress(value * current / total);
                        });

                    if (!result) {
                        std::cerr << boinc_msg_prefix(str, sizeof(buf)) << " docking failed" << std::endl;
                        boinc_finish(1);
                        break;
                    }

                    if (!current_data.empty()) {
                        save_checkpoint_data(json, current_data);
                    }
                }
            }
            catch (const std::exception& ex)
            {
                std::cerr << boinc_msg_prefix(str, sizeof(str)) << " docking failed: " << ex.what() << std::endl;
                result = false;
            }
            });

        worker.join();

        if (!result) {
            std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << " docking failed" << std::endl;
            boinc_finish(1);
            return 1;
        }

        remove_checkpoint_data_file(json);

        boinc_fraction_done(1.);
        boinc_finish(0);
    }
    catch (const std::exception& ex) {
        std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << " exception was thrown while running: " << ex.what() << std::endl;
        boinc_finish(1);
        return 1;
    }

    return 0;
}

int main(int argc, char** argv) {
    try {
        header();

        if (argc != 2) {
            help();
            return 1;
        }

        const std::string json(argv[1]);

        return perform_docking(json);
    }
    catch (std::exception& ex) {
        std::cerr << "Exception was thrown while running boinc-autodock-vina: " << ex.what() << std::endl;
        return 1;
    }
}
