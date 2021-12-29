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

#include <fstream>

#include <gtest/gtest.h>
#include <magic_enum.hpp>
#include <zip.h>

#include "common/config.h"
#include "jsoncons_helper/jsoncons_helper.h"
#include "work-generator/input-config.h"
#include "boinc-autodock-vina/calculate.h"

class dummy_ofstream final {
public:
    ~dummy_ofstream() {
        if (stream_open) {
            stream.close();
            stream_open = false;
        }

        for (const auto& file : open_files) {
            if (exists(file)) {
                std::filesystem::remove(file);
            }
        }
    }

    std::ofstream& operator()() {
        return stream;
    }

    void open(const std::filesystem::path& file) {
        if (!stream_open) {
            stream.open(file);
            stream_open = true;
            file_path = file;
            open_files.push_back(file);
        }
        else {
            if (file != file_path) {
                close();
                open(file);
            }
        }
    }

    void close() {
        if (stream_open) {
            stream.close();
            stream_open = false;
        }
    }
private:
    std::ofstream stream;
    std::filesystem::path file_path;
    bool stream_open = false;
    std::vector<std::filesystem::path> open_files;
};

void create_dummy_file(dummy_ofstream& stream, const std::filesystem::path& file) {
    stream.open(std::filesystem::current_path() / file);
    stream() << "Dummy" << std::endl;
    stream.close();
}

template <typename T>
using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

class zip_extract final {
public:
    static bool extract(const std::filesystem::path& zip_file, const std::filesystem::path& target) {
        int error = 0;
        const deleted_unique_ptr<zip_t> zip(zip_open(zip_file.string().data(), 0, &error), [](auto* z) {
            if (z != nullptr) {
                zip_close(z);
            }
        });
        if (!zip) {
            return false;
        }

        const auto& entries = zip_get_num_entries(zip.get(), 0);
        for (zip_int64_t i = 0; i < entries; ++i) {
            struct zip_stat file_stat {};
            if (zip_stat_index(zip.get(), i, 0, &file_stat)) {
                return false;
            }
            if (!(file_stat.valid & ZIP_STAT_NAME)) {
                continue;
            }
            if ((file_stat.name[0] != '\0') && (file_stat.name[strlen(file_stat.name) - 1] == '/')) {
                continue;
            }
            const deleted_unique_ptr<struct zip_file> file(zip_fopen_index(zip.get(), i, 0), [](auto* z) {
                if (z != nullptr) {
                    zip_fclose(z);
                }
            });
            std::string buffer;
            buffer.resize(file_stat.size, '\0');
            if (zip_fread(file.get(), buffer.data(), file_stat.size) == -1) {
                return false;
            }
            const auto name = target / file_stat.name;
            if (!std::ofstream(name.string().data()).write(buffer.data(), file_stat.size)) {
                return false;
            }
        }

        return true;
    }
};

class Config_UnitTests : public ::testing::Test {
};

class InputConfig_UnitTests : public ::testing::Test {
};

TEST_F(Config_UnitTests, FailOnConfigDefaults) {
    config config;
    EXPECT_FALSE(config.validate());
}

TEST_F(Config_UnitTests, FailOnAbsolutePathInConfig_Receptor) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";
    config config;

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_object("input");
#ifdef WIN32
    json_encoder.value("receptor", "C:\\test\\receptor_sample");
#else
    json_encoder.value("receptor", "/home/test/receptor_sample");
#endif
    json_encoder.value("flex", "flex_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample2");
    json_encoder.end_array();
    json_encoder.begin_array("batch");
    json_encoder.value("batch_sample2");
    json_encoder.end_array();
    json_encoder.end_object();
    json_encoder.begin_object("search_area");
    json_encoder.value("maps", "maps_sample");
    json_encoder.end_object();
    json_encoder.begin_object("output");
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
    json_encoder.value("write_maps", "write_maps_sample");
    json_encoder.end_object();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();
    EXPECT_FALSE(config.load(dummy_json_file_path));
}

TEST_F(Config_UnitTests, FailOnAbsolutePathInConfig_Flex) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";
    config config;

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_object("input");
    json_encoder.value("receptor", "receptor_sample");
#ifdef WIN32
    json_encoder.value("flex", "C:\\test\\flex_sample");
#else
    json_encoder.value("flex", "/home/test/flex_sample");
#endif
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample2");
    json_encoder.end_array();
    json_encoder.begin_array("batch");
    json_encoder.value("batch_sample2");
    json_encoder.end_array();
    json_encoder.end_object();
    json_encoder.begin_object("search_area");
    json_encoder.value("maps", "maps_sample");
    json_encoder.end_object();
    json_encoder.begin_object("output");
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
    json_encoder.value("write_maps", "write_maps_sample");
    json_encoder.end_object();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();
    EXPECT_FALSE(config.load(dummy_json_file_path));
}

TEST_F(Config_UnitTests, FailOnAbsolutePathInConfig_Ligand) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";
    config config;

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_object("input");
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.value("flex", "flex_sample");
    json_encoder.begin_array("ligands");
#ifdef WIN32
    json_encoder.value("C:\\test\\ligand_sample");
#else
    json_encoder.value("/home/test/ligand_sample");
#endif
    json_encoder.end_array();
    json_encoder.begin_array("batch");
    json_encoder.value("batch_sample2");
    json_encoder.end_array();
    json_encoder.end_object();
    json_encoder.begin_object("search_area");
    json_encoder.value("maps", "maps_sample");
    json_encoder.end_object();
    json_encoder.begin_object("output");
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
    json_encoder.value("write_maps", "write_maps_sample");
    json_encoder.end_object();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();

    EXPECT_FALSE(config.load(dummy_json_file_path));
}

TEST_F(Config_UnitTests, FailOnAbsolutePathInConfig_Batch) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";
    config config;

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_object("input");
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.value("flex", "flex_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample");
    json_encoder.end_array();
    json_encoder.begin_array("batch");
#ifdef WIN32
    json_encoder.value("C:\\test\\batch_sample");
#else
    json_encoder.value("/home/test/batch_sample");
#endif
    json_encoder.end_array();
    json_encoder.end_object();
    json_encoder.begin_object("search_area");
    json_encoder.value("maps", "maps_sample");
    json_encoder.end_object();
    json_encoder.begin_object("output");
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
    json_encoder.value("write_maps", "write_maps_sample");
    json_encoder.end_object();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();
    EXPECT_FALSE(config.load(dummy_json_file_path));
}

TEST_F(Config_UnitTests, FailOnAbsolutePathInConfig_Maps) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";
    config config;

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_object("input");
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.value("flex", "flex_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample");
    json_encoder.end_array();
    json_encoder.begin_array("batch");
    json_encoder.value("batch_sample2");
    json_encoder.end_array();
    json_encoder.end_object();
    json_encoder.begin_object("search_area");
#ifdef WIN32
    json_encoder.value("maps", "C:\\test\\maps_sample");
#else
    json_encoder.value("maps", "/home/test/maps_sample");
#endif
    json_encoder.end_object();
    json_encoder.begin_object("output");
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
    json_encoder.value("write_maps", "write_maps_sample");
    json_encoder.end_object();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();
    EXPECT_FALSE(config.load(dummy_json_file_path));
}

TEST_F(Config_UnitTests, FailOnAbsolutePathInConfig_Out) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";
    config config;

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_object("input");
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.value("flex", "flex_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample2");
    json_encoder.end_array();
    json_encoder.begin_array("batch");
    json_encoder.value("batch_sample2");
    json_encoder.end_array();
    json_encoder.end_object();
    json_encoder.begin_object("search_area");
    json_encoder.value("maps", "maps_sample");
    json_encoder.end_object();
    json_encoder.begin_object("output");
#ifdef WIN32
    json_encoder.value("out", "C:\\test\\out_sample");
#else
    json_encoder.value("out", "/home/test/out_sample");
#endif
    json_encoder.value("dir", "dir_sample");
    json_encoder.value("write_maps", "write_maps_sample");
    json_encoder.end_object();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();
    EXPECT_FALSE(config.load(dummy_json_file_path));
}

TEST_F(Config_UnitTests, FailOnAbsolutePathInConfig_Dir) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";
    config config;

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_object("input");
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.value("flex", "flex_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample2");
    json_encoder.end_array();
    json_encoder.begin_array("batch");
    json_encoder.value("batch_sample2");
    json_encoder.end_array();
    json_encoder.end_object();
    json_encoder.begin_object("search_area");
    json_encoder.value("maps", "maps_sample");
    json_encoder.end_object();
    json_encoder.begin_object("output");
    json_encoder.value("out", "out_sample");
#ifdef WIN32
    json_encoder.value("dir", "C:\\test\\dir_sample");
#else
    json_encoder.value("dir", "/home/test/dir_sample");
#endif
    json_encoder.value("write_maps", "write_maps_sample");
    json_encoder.end_object();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();
    EXPECT_FALSE(config.load(dummy_json_file_path));
}

TEST_F(Config_UnitTests, FailOnAbsolutePathInConfig_WriteMaps) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";
    config config;

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_object("input");
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.value("flex", "flex_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample2");
    json_encoder.end_array();
    json_encoder.begin_array("batch");
    json_encoder.value("batch_sample2");
    json_encoder.end_array();
    json_encoder.end_object();
    json_encoder.begin_object("search_area");
    json_encoder.value("maps", "maps_sample");
    json_encoder.end_object();
    json_encoder.begin_object("output");
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
#ifdef WIN32
    json_encoder.value("write_maps", "C:\\test\\write_maps_sample");
#else
    json_encoder.value("write_maps", "/home/test/write_maps_sample");
#endif
    json_encoder.end_object();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();
    EXPECT_FALSE(config.load(dummy_json_file_path));
}

TEST_F(Config_UnitTests, LoadValidator) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_object("input");
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.value("flex", "flex_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample1");
    json_encoder.value("ligand_sample2");
    json_encoder.end_array();
    json_encoder.begin_array("batch");
    json_encoder.value("batch_sample1");
    json_encoder.value("batch_sample2");
    json_encoder.end_array();
    json_encoder.value("scoring", std::string(magic_enum::enum_name(scoring::vinardo)));
    json_encoder.end_object();
    json_encoder.begin_object("search_area");
    json_encoder.value("maps", "maps_sample");
    json_encoder.value("center_x", 0.123456);
    json_encoder.value("center_y", 0.654321);
    json_encoder.value("center_z" , -0.123456);
    json_encoder.value("size_x" , -0.654321);
    json_encoder.value("size_y", 0.0);
    json_encoder.value("size_z", -0.000135);
    json_encoder.value("autobox", true);
    json_encoder.end_object();
    json_encoder.begin_object("output");
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
    json_encoder.value("write_maps", "write_maps_sample");
    json_encoder.end_object();
    json_encoder.begin_object("advanced");
    json_encoder.value("score_only", true);
    json_encoder.value("local_only", true);
    json_encoder.value("no_refine", true);
    json_encoder.value("force_even_voxels", true);
    json_encoder.value("randomize_only", true);
    json_encoder.value("weight_gauss1", 0.123456);
    json_encoder.value("weight_gauss2", -0.123456);
    json_encoder.value("weight_repulsion", 0.654321);
    json_encoder.value("weight_hydrophobic" , -0.654321);
    json_encoder.value("weight_hydrogen", 0.135246);
    json_encoder.value("weight_rot" , -0.135246);
    json_encoder.value("weight_vinardo_gauss1" , -0.642531);
    json_encoder.value("weight_vinardo_repulsion", 0.642531);
    json_encoder.value("weight_vinardo_hydrophobic" , -0.010011);
    json_encoder.value("weight_vinardo_hydrogen", 0.010011);
    json_encoder.value("weight_vinardo_rot" , -1.023456);
    json_encoder.value("weight_ad4_vdw", 1.023456);
    json_encoder.value("weight_ad4_hb" , -1.654320);
    json_encoder.value("weight_ad4_elec", 1.065432);
    json_encoder.value("weight_ad4_dsolv" , -1.064235);
    json_encoder.value("weight_ad4_rot", 1.064235);
    json_encoder.value("weight_glue", 1.024653);
    json_encoder.end_object();
    json_encoder.begin_object("misc");
    json_encoder.value("seed", 2ull);
    json_encoder.value("exhaustiveness", 3ull);
    json_encoder.value("max_evals", 4ull);
    json_encoder.value("num_modes", 5ull);
    json_encoder.value("min_rmsd", 2.0);
    json_encoder.value("energy_range" , -2.0);
    json_encoder.value("spacing", -0.123);
    json_encoder.end_object();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();

    config config;

    const auto res = config.load(dummy_json_file_path);
    ASSERT_TRUE(res);

    const auto receptor_sample = std::filesystem::current_path() /= "receptor_sample";
    EXPECT_STREQ(receptor_sample.string().c_str(), config.input.receptor.c_str());
    const auto flex_sample = std::filesystem::current_path() /= "flex_sample";
    EXPECT_STREQ(flex_sample.string().c_str(), config.input.flex.c_str());
    ASSERT_EQ(2, config.input.ligands.size());
    const auto ligand_sample1 = std::filesystem::current_path() /= "ligand_sample1";
    EXPECT_STREQ(ligand_sample1.string().c_str(), config.input.ligands[0].c_str());
    const auto ligand_sample2 = std::filesystem::current_path() /= "ligand_sample2";
    EXPECT_STREQ(ligand_sample2.string().c_str(), config.input.ligands[1].c_str());
    ASSERT_EQ(2, config.input.batch.size());
    const auto batch_sample1 = std::filesystem::current_path() /= "batch_sample1";
    EXPECT_STREQ(batch_sample1.string().c_str(), config.input.batch[0].c_str());
    const auto batch_sample2 = std::filesystem::current_path() /= "batch_sample2";
    EXPECT_STREQ(batch_sample2.string().c_str(), config.input.batch[1].c_str());
    EXPECT_EQ(scoring::vinardo, config.input.scoring);
    const auto maps_sample = std::filesystem::current_path() /= "maps_sample";
    EXPECT_STREQ(maps_sample.string().c_str(), config.search_area.maps.c_str());
    EXPECT_DOUBLE_EQ(0.123456, config.search_area.center_x);
    EXPECT_DOUBLE_EQ(0.654321, config.search_area.center_y);
    EXPECT_DOUBLE_EQ(-0.123456, config.search_area.center_z);
    EXPECT_DOUBLE_EQ(-0.654321, config.search_area.size_x);
    EXPECT_DOUBLE_EQ(0.0, config.search_area.size_y);
    EXPECT_DOUBLE_EQ(-0.000135, config.search_area.size_z);
    EXPECT_TRUE(config.search_area.autobox);
    const auto out_sample = std::filesystem::current_path() /= "out_sample";
    EXPECT_STREQ(out_sample.string().c_str(), config.output.out.c_str());
    const auto dir_sample = std::filesystem::current_path() /= "dir_sample";
    EXPECT_STREQ(dir_sample.string().c_str(), config.output.dir.c_str());
    const auto write_maps_sample = std::filesystem::current_path() /= "write_maps_sample";
    EXPECT_STREQ(write_maps_sample.string().c_str(), config.output.write_maps.c_str());
    EXPECT_TRUE(config.advanced.score_only);
    EXPECT_TRUE(config.advanced.local_only);
    EXPECT_TRUE(config.advanced.no_refine);
    EXPECT_TRUE(config.advanced.force_even_voxels);
    EXPECT_TRUE(config.advanced.randomize_only);
    EXPECT_DOUBLE_EQ(0.123456, config.advanced.weight_gauss1);
    EXPECT_DOUBLE_EQ(-0.123456, config.advanced.weight_gauss2);
    EXPECT_DOUBLE_EQ(0.654321, config.advanced.weight_repulsion);
    EXPECT_DOUBLE_EQ(-0.654321, config.advanced.weight_hydrophobic);
    EXPECT_DOUBLE_EQ(0.135246, config.advanced.weight_hydrogen);
    EXPECT_DOUBLE_EQ(-0.135246, config.advanced.weight_rot);
    EXPECT_DOUBLE_EQ(-0.642531, config.advanced.weight_vinardo_gauss1);
    EXPECT_DOUBLE_EQ(0.642531, config.advanced.weight_vinardo_repulsion);
    EXPECT_DOUBLE_EQ(-0.010011, config.advanced.weight_vinardo_hydrophobic);
    EXPECT_DOUBLE_EQ(0.010011, config.advanced.weight_vinardo_hydrogen);
    EXPECT_DOUBLE_EQ(-1.023456, config.advanced.weight_vinardo_rot);
    EXPECT_DOUBLE_EQ(1.023456, config.advanced.weight_ad4_vdw);
    EXPECT_DOUBLE_EQ(-1.654320, config.advanced.weight_ad4_hb);
    EXPECT_DOUBLE_EQ(1.065432, config.advanced.weight_ad4_elec);
    EXPECT_DOUBLE_EQ(-1.064235, config.advanced.weight_ad4_dsolv);
    EXPECT_DOUBLE_EQ(1.064235, config.advanced.weight_ad4_rot);
    EXPECT_DOUBLE_EQ(1.024653, config.advanced.weight_glue);
    EXPECT_EQ(2, config.misc.seed);
    EXPECT_EQ(3, config.misc.exhaustiveness);
    EXPECT_EQ(4, config.misc.max_evals);
    EXPECT_EQ(5, config.misc.num_modes);
    EXPECT_DOUBLE_EQ(2.0, config.misc.min_rmsd);
    EXPECT_DOUBLE_EQ(-2.0, config.misc.energy_range);
    EXPECT_DOUBLE_EQ(-0.123, config.misc.spacing);
}

TEST_F(Config_UnitTests, FailOn_output_out_NotSpecified) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_object("input");
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample1");
    json_encoder.end_array();
    json_encoder.end_object();
    json_encoder.begin_object("search_area");
    json_encoder.value("center_x", 0.123456);
    json_encoder.value("center_y", 0.654321);
    json_encoder.value("center_z", -0.123456);
    json_encoder.value("size_x", -0.654321);
    json_encoder.value("size_y", 0.0);
    json_encoder.value("size_z", -0.000135);
    json_encoder.end_object();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();

    config config;

    auto res = config.load(dummy_json_file_path);
    ASSERT_TRUE(res);
    res = config.validate();
    ASSERT_FALSE(res);
}

TEST_F(Config_UnitTests, CheckThatReceptorAndLigandFilesArePresent) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_object("input");
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample1");
    json_encoder.value("ligand_sample2");
    json_encoder.end_array();
    json_encoder.end_object();
    json_encoder.begin_object("search_area");
    json_encoder.value("center_x", 0.123456);
    json_encoder.value("center_y", 0.654321);
    json_encoder.value("center_z", -0.123456);
    json_encoder.value("size_x", -0.654321);
    json_encoder.value("size_y", 0.0);
    json_encoder.value("size_z", -0.000135);
    json_encoder.end_object();
    json_encoder.begin_object("output");
    json_encoder.value("out", "out_sample");
    json_encoder.end_object();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();

    config config;
    auto res = config.load(dummy_json_file_path);
    ASSERT_TRUE(res);
    res = config.validate();
    ASSERT_FALSE(res);

    dummy_ofstream dummy;
    const auto& receptor_sample = std::filesystem::current_path() / "receptor_sample";    
    const auto& ligand_sample1 = std::filesystem::current_path() / "ligand_sample1";
    const auto& ligand_sample2 = std::filesystem::current_path() / "ligand_sample2";

    create_dummy_file(dummy, receptor_sample);
    res = config.validate();
    ASSERT_FALSE(res);
    std::filesystem::remove(receptor_sample);

    create_dummy_file(dummy, ligand_sample1);
    create_dummy_file(dummy, ligand_sample2);
    res = config.validate();
    ASSERT_FALSE(res);

    create_dummy_file(dummy, receptor_sample);
    res = config.validate();
    ASSERT_TRUE(res);
}

TEST_F(Config_UnitTests, CheckThatFlexFileIsPresent) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_object("input");
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.value("flex", "flex_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample1");
    json_encoder.end_array();
    json_encoder.end_object();
    json_encoder.begin_object("search_area");
    json_encoder.value("center_x", 0.123456);
    json_encoder.value("center_y", 0.654321);
    json_encoder.value("center_z", -0.123456);
    json_encoder.value("size_x", -0.654321);
    json_encoder.value("size_y", 0.0);
    json_encoder.value("size_z", -0.000135);
    json_encoder.end_object();
    json_encoder.begin_object("output");
    json_encoder.value("out", "out_sample");
    json_encoder.end_object();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();

    config config;
    auto res = config.load(dummy_json_file_path);
    ASSERT_TRUE(res);

    dummy_ofstream dummy;
    create_dummy_file(dummy, std::filesystem::current_path() / "receptor_sample");
    create_dummy_file(dummy, std::filesystem::current_path() / "ligand_sample1");

    res = config.validate();
    ASSERT_FALSE(res);

    create_dummy_file(dummy, std::filesystem::current_path() / "flex_sample");
    res = config.validate();
    ASSERT_TRUE(res);
}

TEST_F(Config_UnitTests, CheckThatReceptorAndBatchFilesArePresent) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_object("input");
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.begin_array("batch");
    json_encoder.value("batch_sample1");
    json_encoder.value("batch_sample2");
    json_encoder.end_array();
    json_encoder.end_object();
    json_encoder.begin_object("search_area");
    json_encoder.value("center_x", 0.123456);
    json_encoder.value("center_y", 0.654321);
    json_encoder.value("center_z", -0.123456);
    json_encoder.value("size_x", -0.654321);
    json_encoder.value("size_y", 0.0);
    json_encoder.value("size_z", -0.000135);
    json_encoder.end_object();
    json_encoder.begin_object("output");
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
    json_encoder.end_object();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();

    config config;
    auto res = config.load(dummy_json_file_path);
    ASSERT_TRUE(res);
    res = config.validate();
    ASSERT_FALSE(res);

    dummy_ofstream dummy;
    const auto& receptor_sample = std::filesystem::current_path() / "receptor_sample";
    const auto& batch_sample1 = std::filesystem::current_path() / "batch_sample1";
    const auto& batch_sample2 = std::filesystem::current_path() / "batch_sample2";

    create_dummy_file(dummy, receptor_sample);
    res = config.validate();
    ASSERT_FALSE(res);
    std::filesystem::remove(receptor_sample);

    create_dummy_file(dummy, batch_sample1);
    create_dummy_file(dummy, batch_sample2);
    res = config.validate();
    ASSERT_FALSE(res);

    create_dummy_file(dummy, receptor_sample);
    res = config.validate();
    ASSERT_TRUE(res);
}

TEST_F(Config_UnitTests, TestSimpleVinaScenario) {
    const auto& json_file = std::filesystem::current_path() / "boinc-autodock-vina/samples/basic_docking/1iep_vina.json";
    config config;
    ASSERT_TRUE(config.load(json_file));
    ASSERT_TRUE(config.validate());
    const auto res = calculator::calculate(config, 0, [](double) { });
    EXPECT_TRUE(res);
    std::filesystem::remove(std::filesystem::current_path() / "boinc-autodock-vina/samples/basic_docking/1iep_ligand_vina_out.pdbqt");
}

TEST_F(Config_UnitTests, TestConfigsEqualAfterReadWrite) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_object("input");
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.value("flex", "flex_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample1");
    json_encoder.value("ligand_sample2");
    json_encoder.end_array();
    json_encoder.begin_array("batch");
    json_encoder.value("batch_sample1");
    json_encoder.value("batch_sample2");
    json_encoder.end_array();
    json_encoder.value("scoring", std::string(magic_enum::enum_name(scoring::vinardo)));
    json_encoder.end_object();
    json_encoder.begin_object("search_area");
    json_encoder.value("maps", "maps_sample");
    json_encoder.value("center_x", 0.123456);
    json_encoder.value("center_y", 0.654321);
    json_encoder.value("center_z", -0.123456);
    json_encoder.value("size_x", -0.654321);
    json_encoder.value("size_y", 0.0);
    json_encoder.value("size_z", -0.000135);
    json_encoder.value("autobox", true);
    json_encoder.end_object();
    json_encoder.begin_object("output");
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
    json_encoder.value("write_maps", "write_maps_sample");
    json_encoder.end_object();
    json_encoder.begin_object("advanced");
    json_encoder.value("score_only", true);
    json_encoder.value("local_only", true);
    json_encoder.value("no_refine", true);
    json_encoder.value("force_even_voxels", true);
    json_encoder.value("randomize_only", true);
    json_encoder.value("weight_gauss1", 0.123456);
    json_encoder.value("weight_gauss2", -0.123456);
    json_encoder.value("weight_repulsion", 0.654321);
    json_encoder.value("weight_hydrophobic", -0.654321);
    json_encoder.value("weight_hydrogen", 0.135246);
    json_encoder.value("weight_rot", -0.135246);
    json_encoder.value("weight_vinardo_gauss1", -0.642531);
    json_encoder.value("weight_vinardo_repulsion", 0.642531);
    json_encoder.value("weight_vinardo_hydrophobic", -0.010011);
    json_encoder.value("weight_vinardo_hydrogen", 0.010011);
    json_encoder.value("weight_vinardo_rot", -1.023456);
    json_encoder.value("weight_ad4_vdw", 1.023456);
    json_encoder.value("weight_ad4_hb", -1.654320);
    json_encoder.value("weight_ad4_elec", 1.065432);
    json_encoder.value("weight_ad4_dsolv", -1.064235);
    json_encoder.value("weight_ad4_rot", 1.064235);
    json_encoder.value("weight_glue", 1.024653);
    json_encoder.end_object();
    json_encoder.begin_object("misc");
    json_encoder.value("seed", 2ull);
    json_encoder.value("exhaustiveness", 3ull);
    json_encoder.value("max_evals", 4ull);
    json_encoder.value("num_modes", 5ull);
    json_encoder.value("min_rmsd", 2.0);
    json_encoder.value("energy_range", -2.0);
    json_encoder.value("spacing", -0.123);
    json_encoder.end_object();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();

    config config_copy;
    config config;

    ASSERT_TRUE(config.load(dummy_json_file_path));

    const auto& dummy_copy_json_file_path = std::filesystem::current_path() / "dummy_copy.json";
    ASSERT_TRUE(config.save(dummy_copy_json_file_path));

    ASSERT_TRUE(config_copy.load(dummy_copy_json_file_path));

    EXPECT_STREQ(config.input.receptor.c_str(), config_copy.input.receptor.c_str());
    EXPECT_STREQ(config.input.flex.c_str(), config_copy.input.flex.c_str());
    ASSERT_EQ(config.input.ligands.size(), config_copy.input.ligands.size());
    EXPECT_STREQ(config.input.ligands[0].c_str(), config_copy.input.ligands[0].c_str());
    EXPECT_STREQ(config.input.ligands[1].c_str(), config_copy.input.ligands[1].c_str());
    ASSERT_EQ(config.input.batch.size(), config_copy.input.batch.size());
    EXPECT_STREQ(config.input.batch[0].c_str(), config_copy.input.batch[0].c_str());
    EXPECT_STREQ(config.input.batch[1].c_str(), config_copy.input.batch[1].c_str());
    EXPECT_EQ(config.input.scoring, config_copy.input.scoring);
    EXPECT_STREQ(config.search_area.maps.c_str(), config_copy.search_area.maps.c_str());
    EXPECT_DOUBLE_EQ(config.search_area.center_x, config_copy.search_area.center_x);
    EXPECT_DOUBLE_EQ(config.search_area.center_y, config_copy.search_area.center_y);
    EXPECT_DOUBLE_EQ(config.search_area.center_z, config_copy.search_area.center_z);
    EXPECT_DOUBLE_EQ(config.search_area.size_x, config_copy.search_area.size_x);
    EXPECT_DOUBLE_EQ(config.search_area.size_y, config_copy.search_area.size_y);
    EXPECT_DOUBLE_EQ(config.search_area.size_z, config_copy.search_area.size_z);
    EXPECT_EQ(config.search_area.autobox, config_copy.search_area.autobox);
    EXPECT_STREQ(config.output.out.c_str(), config_copy.output.out.c_str());
    EXPECT_STREQ(config.output.dir.c_str(), config_copy.output.dir.c_str());
    EXPECT_STREQ(config.output.write_maps.c_str(), config_copy.output.write_maps.c_str());
    EXPECT_EQ(config.advanced.score_only, config_copy.advanced.score_only);
    EXPECT_EQ(config.advanced.local_only, config_copy.advanced.local_only);
    EXPECT_EQ(config.advanced.no_refine, config_copy.advanced.no_refine);
    EXPECT_EQ(config.advanced.force_even_voxels, config_copy.advanced.force_even_voxels);
    EXPECT_EQ(config.advanced.randomize_only, config_copy.advanced.randomize_only);
    EXPECT_DOUBLE_EQ(config.advanced.weight_gauss1, config_copy.advanced.weight_gauss1);
    EXPECT_DOUBLE_EQ(config.advanced.weight_gauss2, config_copy.advanced.weight_gauss2);
    EXPECT_DOUBLE_EQ(config.advanced.weight_repulsion, config_copy.advanced.weight_repulsion);
    EXPECT_DOUBLE_EQ(config.advanced.weight_hydrophobic, config_copy.advanced.weight_hydrophobic);
    EXPECT_DOUBLE_EQ(config.advanced.weight_hydrogen, config_copy.advanced.weight_hydrogen);
    EXPECT_DOUBLE_EQ(config.advanced.weight_rot, config_copy.advanced.weight_rot);
    EXPECT_DOUBLE_EQ(config.advanced.weight_vinardo_gauss1, config_copy.advanced.weight_vinardo_gauss1);
    EXPECT_DOUBLE_EQ(config.advanced.weight_vinardo_repulsion, config_copy.advanced.weight_vinardo_repulsion);
    EXPECT_DOUBLE_EQ(config.advanced.weight_vinardo_hydrophobic, config_copy.advanced.weight_vinardo_hydrophobic);
    EXPECT_DOUBLE_EQ(config.advanced.weight_vinardo_hydrogen, config_copy.advanced.weight_vinardo_hydrogen);
    EXPECT_DOUBLE_EQ(config.advanced.weight_vinardo_rot, config_copy.advanced.weight_vinardo_rot);
    EXPECT_DOUBLE_EQ(config.advanced.weight_ad4_vdw, config_copy.advanced.weight_ad4_vdw);
    EXPECT_DOUBLE_EQ(config.advanced.weight_ad4_hb, config_copy.advanced.weight_ad4_hb);
    EXPECT_DOUBLE_EQ(config.advanced.weight_ad4_elec, config_copy.advanced.weight_ad4_elec);
    EXPECT_DOUBLE_EQ(config.advanced.weight_ad4_dsolv, config_copy.advanced.weight_ad4_dsolv);
    EXPECT_DOUBLE_EQ(config.advanced.weight_ad4_rot, config_copy.advanced.weight_ad4_rot);
    EXPECT_DOUBLE_EQ(config.advanced.weight_glue, config_copy.advanced.weight_glue);
    EXPECT_EQ(config.misc.seed, config_copy.misc.seed);
    EXPECT_EQ(config.misc.exhaustiveness, config_copy.misc.exhaustiveness);
    EXPECT_EQ(config.misc.max_evals, config_copy.misc.max_evals);
    EXPECT_EQ(config.misc.num_modes, config_copy.misc.num_modes);
    EXPECT_DOUBLE_EQ(config.misc.min_rmsd, config_copy.misc.min_rmsd);
    EXPECT_DOUBLE_EQ(config.misc.energy_range, config_copy.misc.energy_range);
    EXPECT_DOUBLE_EQ(config.misc.spacing, config_copy.misc.spacing);

    std::filesystem::remove(dummy_copy_json_file_path);
}

TEST_F(InputConfig_UnitTests, TestThatWorkGeneratorIsAbleToProcessAlreadyPreparedConfig) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_object("input");
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample1");
    json_encoder.end_array();
    json_encoder.end_object();
    json_encoder.begin_object("search_area");
    json_encoder.value("center_x", 0.123456);
    json_encoder.value("center_y", 0.654321);
    json_encoder.value("center_z", -0.123456);
    json_encoder.value("size_x", -0.654321);
    json_encoder.value("size_y", 0.0);
    json_encoder.value("size_z", -0.000135);
    json_encoder.end_object();
    json_encoder.begin_object("output");
    json_encoder.value("out", "out_sample");
    json_encoder.end_object();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();

    dummy_ofstream dummy;
    create_dummy_file(dummy, "receptor_sample");
    create_dummy_file(dummy, "ligand_sample1");

    generator generator;

    auto res = generator.process(dummy_json_file_path, std::filesystem::current_path());
    ASSERT_TRUE(res);
    res = generator.validate();
    ASSERT_TRUE(res);

    const auto zip_path = std::filesystem::current_path() / "wu_1.zip";
    ASSERT_TRUE(exists(zip_path));

    const auto zip_extract_path = std::filesystem::current_path() / "wu_1_zip";
    create_directories(zip_extract_path);
    ASSERT_TRUE(zip_extract::extract(zip_path, zip_extract_path));

    const auto config_path = zip_extract_path / "config.json";
    ASSERT_TRUE(exists(config_path));

    config conf;
    ASSERT_TRUE(conf.load(config_path));
    ASSERT_TRUE(conf.validate());

    EXPECT_STREQ((zip_extract_path / "receptor_sample").string().data(), conf.input.receptor.data());
    ASSERT_EQ(1, conf.input.ligands.size());
    EXPECT_STREQ((zip_extract_path / "ligand_sample1").string().data(), conf.input.ligands[0].data());
    EXPECT_DOUBLE_EQ(0.123456, conf.search_area.center_x);
    EXPECT_DOUBLE_EQ(0.654321, conf.search_area.center_y);
    EXPECT_DOUBLE_EQ(-0.123456, conf.search_area.center_z);
    EXPECT_DOUBLE_EQ(-0.654321, conf.search_area.size_x);
    EXPECT_DOUBLE_EQ(0.0, conf.search_area.size_y);
    EXPECT_DOUBLE_EQ(-0.000135, conf.search_area.size_z);
    EXPECT_STREQ((zip_extract_path / "out_sample").string().data(), conf.output.out.data());

    std::filesystem::remove(zip_path);
}

TEST_F(InputConfig_UnitTests, TestThatGetTempFolderNameAlwaysReturnsDifferentNames) {
    ASSERT_STRNE(temp_folder::get_temp_folder_name().data(), temp_folder::get_temp_folder_name().data());
}

TEST_F(InputConfig_UnitTests, TestThatTempFolderIsCreatedAndThenCleared) {
    std::filesystem::path directory;
    {
        const temp_folder temp(std::filesystem::current_path());
        directory = temp();
        ASSERT_TRUE(std::filesystem::is_directory(temp()));

        dummy_ofstream stream;
        create_dummy_file(stream, temp() / "test");

        ASSERT_TRUE(std::filesystem::is_regular_file(temp() / "test"));
    }

    ASSERT_FALSE(std::filesystem::exists(directory));
}