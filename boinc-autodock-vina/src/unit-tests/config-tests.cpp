// This file is part of BOINC.
// https://boinc.berkeley.edu
// Copyright (C) 2022 University of California
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

#include "common/config.h"
#include "jsoncons_helper/jsoncons_helper.h"
#include "boinc-autodock-vina/calculate.h"
#include "dummy-ofstream.h"

template <typename T>
using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

class Config_UnitTests : public ::testing::Test {};

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
    json_encoder.value("maps", "maps_sample");
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
    json_encoder.value("write_maps", "write_maps_sample");
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
    json_encoder.value("maps", "maps_sample");
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
    json_encoder.value("write_maps", "write_maps_sample");
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
    json_encoder.value("maps", "maps_sample");
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
    json_encoder.value("write_maps", "write_maps_sample");
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
    json_encoder.value("maps", "maps_sample");
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
    json_encoder.value("write_maps", "write_maps_sample");
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
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.value("flex", "flex_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample");
    json_encoder.end_array();
    json_encoder.begin_array("batch");
    json_encoder.value("batch_sample2");
    json_encoder.end_array();
#ifdef WIN32
    json_encoder.value("maps", "C:\\test\\maps_sample");
#else
    json_encoder.value("maps", "/home/test/maps_sample");
#endif
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
    json_encoder.value("write_maps", "write_maps_sample");
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
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.value("flex", "flex_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample2");
    json_encoder.end_array();
    json_encoder.begin_array("batch");
    json_encoder.value("batch_sample2");
    json_encoder.end_array();
    json_encoder.value("maps", "maps_sample");
#ifdef WIN32
    json_encoder.value("out", "C:\\test\\out_sample");
#else
    json_encoder.value("out", "/home/test/out_sample");
#endif
    json_encoder.value("dir", "dir_sample");
    json_encoder.value("write_maps", "write_maps_sample");
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
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.value("flex", "flex_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample2");
    json_encoder.end_array();
    json_encoder.begin_array("batch");
    json_encoder.value("batch_sample2");
    json_encoder.end_array();
    json_encoder.value("maps", "maps_sample");
    json_encoder.value("out", "out_sample");
#ifdef WIN32
    json_encoder.value("dir", "C:\\test\\dir_sample");
#else
    json_encoder.value("dir", "/home/test/dir_sample");
#endif
    json_encoder.value("write_maps", "write_maps_sample");
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
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.value("flex", "flex_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample2");
    json_encoder.end_array();
    json_encoder.begin_array("batch");
    json_encoder.value("batch_sample2");
    json_encoder.end_array();
    json_encoder.value("maps", "maps_sample");
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
#ifdef WIN32
    json_encoder.value("write_maps", "C:\\test\\write_maps_sample");
#else
    json_encoder.value("write_maps", "/home/test/write_maps_sample");
#endif
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();
    EXPECT_FALSE(config.load(dummy_json_file_path));
}

TEST_F(Config_UnitTests, CheckForDeaultValueInConfig_Out) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";
    config config;

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample2");
    json_encoder.end_array();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();
    ASSERT_TRUE(config.load(dummy_json_file_path));

    const auto out_sample = std::filesystem::current_path() /= "result.pdbqt";
    EXPECT_STREQ(out_sample.string().c_str(), config.output.out.c_str());
}

TEST_F(Config_UnitTests, LoadValidator) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
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
    json_encoder.value("maps", "maps_sample");
    json_encoder.value("center_x", 0.123456);
    json_encoder.value("center_y", 0.654321);
    json_encoder.value("center_z", -0.123456);
    json_encoder.value("size_x", -0.654321);
    json_encoder.value("size_y", 0.0);
    json_encoder.value("size_z", -0.000135);
    json_encoder.value("autobox", true);
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
    json_encoder.value("write_maps", "write_maps_sample");
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
    json_encoder.value("seed", static_cast<uint64_t>(2ull));
    json_encoder.value("exhaustiveness", static_cast<uint64_t>(3ull));
    json_encoder.value("max_evals", static_cast<uint64_t>(4ull));
    json_encoder.value("num_modes", static_cast<uint64_t>(5ull));
    json_encoder.value("min_rmsd", 2.0);
    json_encoder.value("energy_range", -2.0);
    json_encoder.value("spacing", -0.123);
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
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample1");
    json_encoder.end_array();
    json_encoder.value("center_x", 0.123456);
    json_encoder.value("center_y", 0.654321);
    json_encoder.value("center_z", -0.123456);
    json_encoder.value("size_x", -0.654321);
    json_encoder.value("size_y", 0.0);
    json_encoder.value("size_z", -0.000135);
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
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample1");
    json_encoder.value("ligand_sample2");
    json_encoder.end_array();
    json_encoder.value("center_x", 0.123456);
    json_encoder.value("center_y", 0.654321);
    json_encoder.value("center_z", -0.123456);
    json_encoder.value("size_x", -0.654321);
    json_encoder.value("size_y", 0.0);
    json_encoder.value("size_z", -0.000135);
    json_encoder.value("out", "out_sample");
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
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.value("flex", "flex_sample");
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample1");
    json_encoder.end_array();
    json_encoder.value("center_x", 0.123456);
    json_encoder.value("center_y", 0.654321);
    json_encoder.value("center_z", -0.123456);
    json_encoder.value("size_x", -0.654321);
    json_encoder.value("size_y", 0.0);
    json_encoder.value("size_z", -0.000135);
    json_encoder.value("out", "out_sample");
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
    json_encoder.value("receptor", "receptor_sample");
    json_encoder.begin_array("batch");
    json_encoder.value("batch_sample1");
    json_encoder.value("batch_sample2");
    json_encoder.end_array();
    json_encoder.value("center_x", 0.123456);
    json_encoder.value("center_y", 0.654321);
    json_encoder.value("center_z", -0.123456);
    json_encoder.value("size_x", -0.654321);
    json_encoder.value("size_y", 0.0);
    json_encoder.value("size_z", -0.000135);
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
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

TEST_F(Config_UnitTests, CheckThatMapsFilesArePresent) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_array("ligands");
    json_encoder.value("ligand_sample1");
    json_encoder.end_array();
    json_encoder.value("maps", "receptor_sample");
    json_encoder.value("center_x", 0.123456);
    json_encoder.value("center_y", 0.654321);
    json_encoder.value("center_z", -0.123456);
    json_encoder.value("size_x", -0.654321);
    json_encoder.value("size_y", 0.0);
    json_encoder.value("size_z", -0.000135);
    json_encoder.value("out", "out_sample");
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();

    config config;
    auto res = config.load(dummy_json_file_path);
    ASSERT_TRUE(res);

    dummy_ofstream dummy;
    create_dummy_file(dummy, std::filesystem::current_path() / "ligand_sample1");

    res = config.validate();
    ASSERT_FALSE(res);

    dummy_ofstream gpf;
    gpf.open(std::filesystem::current_path() / "receptor_sample.gpf");

    gpf() <<
        "npts 54 54 54                        # num.grid points in xyz" << std::endl <<
        "gridfld 1iep_receptor.maps.fld       # grid_data_file" << std::endl <<
        "spacing 0.375                        # spacing(A)" << std::endl <<
        "receptor_types A C OA N SA HD        # receptor atom types" << std::endl <<
        "ligand_types A C NA OA N HD          # ligand atom types" << std::endl <<
        "receptor 1iep_receptor.pdbqt         # macromolecule" << std::endl <<
        "gridcenter 15.190 53.903 16.917      # xyz - coordinates or auto" << std::endl <<
        "smooth 0.5                           # store minimum energy w / in rad(A)" << std::endl <<
        "map 1iep_receptor.A.map              # atom - specific affinity map" << std::endl <<
        "map 1iep_receptor.C.map              # atom - specific affinity map" << std::endl <<
        "map 1iep_receptor.NA.map             # atom - specific affinity map" << std::endl <<
        "map 1iep_receptor.OA.map             # atom - specific affinity map" << std::endl <<
        "map 1iep_receptor.N.map              # atom - specific affinity map" << std::endl <<
        "map 1iep_receptor.HD.map             # atom - specific affinity map" << std::endl <<
        "elecmap 1iep_receptor.e.map          # electrostatic potential map" << std::endl <<
        "dsolvmap 1iep_receptor.d.map         # desolvation potential map" << std::endl <<
        "dielectric - 0.1465                   #  < 0, AD4 distance - dep.diel; > 0, constant" << std::endl;

    gpf.close();

    create_dummy_file(dummy, std::filesystem::current_path() / "1iep_receptor.maps.fld");
    create_dummy_file(dummy, std::filesystem::current_path() / "1iep_receptor.A.map");
    create_dummy_file(dummy, std::filesystem::current_path() / "1iep_receptor.C.map");
    create_dummy_file(dummy, std::filesystem::current_path() / "1iep_receptor.NA.map");
    create_dummy_file(dummy, std::filesystem::current_path() / "1iep_receptor.OA.map");
    create_dummy_file(dummy, std::filesystem::current_path() / "1iep_receptor.N.map");
    create_dummy_file(dummy, std::filesystem::current_path() / "1iep_receptor.HD.map");
    create_dummy_file(dummy, std::filesystem::current_path() / "1iep_receptor.e.map");
    create_dummy_file(dummy, std::filesystem::current_path() / "1iep_receptor.d.map");

    res = config.validate();
    ASSERT_TRUE(res);
}

TEST_F(Config_UnitTests, TestConfigsEqualAfterReadWrite) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
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
    json_encoder.value("maps", "maps_sample");
    json_encoder.value("center_x", 0.123456);
    json_encoder.value("center_y", 0.654321);
    json_encoder.value("center_z", -0.123456);
    json_encoder.value("size_x", -0.654321);
    json_encoder.value("size_y", 0.0);
    json_encoder.value("size_z", -0.000135);
    json_encoder.value("autobox", true);
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
    json_encoder.value("write_maps", "write_maps_sample");
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
    json_encoder.value("seed", static_cast<uint64_t>(2ull));
    json_encoder.value("exhaustiveness", static_cast<uint64_t>(3ull));
    json_encoder.value("max_evals", static_cast<uint64_t>(4ull));
    json_encoder.value("num_modes", static_cast<uint64_t>(5ull));
    json_encoder.value("min_rmsd", 2.0);
    json_encoder.value("energy_range", -2.0);
    json_encoder.value("spacing", -0.123);
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

TEST_F(Config_UnitTests, Validate_GetGPF_Filename) {
    config config;
    config.search_area.maps = (std::filesystem::current_path() / "dummy").string();
    EXPECT_STREQ((std::filesystem::current_path() / "dummy.gpf").string().c_str(), config.get_gpf_filename().string().c_str());
}

TEST_F(Config_UnitTests, CheckReadOfGPF) {
    const auto& dummy_gpf_file_path = std::filesystem::current_path() / "dummy.gpf";

    dummy_ofstream gpf;
    gpf.open(dummy_gpf_file_path);

    gpf() <<
        "npts 54 54 54                        # num.grid points in xyz" << std::endl <<
        "gridfld 1iep_receptor.maps.fld       # grid_data_file" << std::endl <<
        "spacing 0.375                        # spacing(A)" << std::endl <<
        "receptor_types A C OA N SA HD        # receptor atom types" << std::endl <<
        "ligand_types A C NA OA N HD          # ligand atom types" << std::endl <<
        "receptor 1iep_receptor.pdbqt         # macromolecule" << std::endl <<
        "gridcenter 15.190 53.903 16.917      # xyz - coordinates or auto" << std::endl <<
        "smooth 0.5                           # store minimum energy w / in rad(A)" << std::endl <<
        "map 1iep_receptor.A.map              # atom - specific affinity map" << std::endl <<
        "map 1iep_receptor.C.map              # atom - specific affinity map" << std::endl <<
        "map 1iep_receptor.NA.map             # atom - specific affinity map" << std::endl <<
        "map 1iep_receptor.OA.map             # atom - specific affinity map" << std::endl <<
        "map 1iep_receptor.N.map              # atom - specific affinity map" << std::endl <<
        "map 1iep_receptor.HD.map             # atom - specific affinity map" << std::endl <<
        "elecmap 1iep_receptor.e.map          # electrostatic potential map" << std::endl <<
        "dsolvmap 1iep_receptor.d.map         # desolvation potential map" << std::endl <<
        "dielectric - 0.1465                   #  < 0, AD4 distance - dep.diel; > 0, constant" << std::endl;

    gpf.close();

    config config;
    config.search_area.maps = (std::filesystem::current_path() / "dummy").string();

    const auto& files = config.get_files_from_gpf();
    const auto& lookup = [&](const auto& file) {
        return std::any_of(files.cbegin(), files.cend(), [&](const auto& f) {
            return f == file;
            });
    };

    EXPECT_EQ(9, files.size());
    EXPECT_TRUE(lookup((std::filesystem::current_path() / "1iep_receptor.maps.fld").string()));
    EXPECT_TRUE(lookup((std::filesystem::current_path() / "1iep_receptor.A.map").string()));
    EXPECT_TRUE(lookup((std::filesystem::current_path() / "1iep_receptor.C.map").string()));
    EXPECT_TRUE(lookup((std::filesystem::current_path() / "1iep_receptor.NA.map").string()));
    EXPECT_TRUE(lookup((std::filesystem::current_path() / "1iep_receptor.OA.map").string()));
    EXPECT_TRUE(lookup((std::filesystem::current_path() / "1iep_receptor.N.map").string()));
    EXPECT_TRUE(lookup((std::filesystem::current_path() / "1iep_receptor.HD.map").string()));
    EXPECT_TRUE(lookup((std::filesystem::current_path() / "1iep_receptor.e.map").string()));
    EXPECT_TRUE(lookup((std::filesystem::current_path() / "1iep_receptor.d.map").string()));
}

TEST_F(Config_UnitTests, TestGetOutFiles) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.value("out", "out_sample");
    json_encoder.value("dir", "dir_sample");
    json_encoder.value("write_maps", "write_maps/sample");
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();

    config config;

    ASSERT_TRUE(config.load(dummy_json_file_path));

    create_directories(std::filesystem::current_path() / "dir_sample");
    create_directories(std::filesystem::current_path() / "write_maps");

    dummy_ofstream dummy;
    create_dummy_file(dummy, "out_sample");
    create_dummy_file(dummy, std::filesystem::path("dir_sample") / "sample1");
    create_dummy_file(dummy, std::filesystem::path("dir_sample") / "sample2");
    create_dummy_file(dummy, std::filesystem::path("write_maps") / "sample_1.map");
    create_dummy_file(dummy, std::filesystem::path("write_maps") / "sample_2.map");
    create_dummy_file(dummy, std::filesystem::path("write_maps") / "sample_3.map");

    const auto& files = config.get_out_files();

    const auto& lookup = [&](const auto& file) {
        return std::any_of(files.cbegin(), files.cend(), [&](const auto& f) {
            return f == file;
            });
    };

    EXPECT_EQ(6, files.size());
    EXPECT_TRUE(lookup((std::filesystem::current_path() / "out_sample").string()));
    EXPECT_TRUE(lookup((std::filesystem::current_path() / "dir_sample" / "sample1").string()));
    EXPECT_TRUE(lookup((std::filesystem::current_path() / "dir_sample" / "sample2").string()));
    EXPECT_TRUE(lookup((std::filesystem::current_path() / "write_maps" / "sample_1.map").string()));
    EXPECT_TRUE(lookup((std::filesystem::current_path() / "write_maps" / "sample_2.map").string()));
    EXPECT_TRUE(lookup((std::filesystem::current_path() / "write_maps" / "sample_3.map").string()));
}

TEST_F(Config_UnitTests, TestSimpleVinaScenario) {
    const auto& json_file = std::filesystem::current_path() / "boinc-autodock-vina/samples/basic_docking/1iep_vina.json";
    config config;
    ASSERT_TRUE(config.load(json_file));
    ASSERT_TRUE(config.validate());
    const auto res = calculator::calculate(config, 0, [](double) {});
    EXPECT_TRUE(res);
    std::filesystem::remove(std::filesystem::current_path() / "boinc-autodock-vina/samples/basic_docking/1iep_ligand_vina_out.pdbqt");
}
