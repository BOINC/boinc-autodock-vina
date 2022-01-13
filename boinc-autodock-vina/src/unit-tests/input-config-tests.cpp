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

#include <gtest/gtest.h>

#include "dummy-ofstream.h"
#include "zip-extract.h"
#include "common/config.h"
#include "work-generator/input-config.h"
#include "work-generator/temp-folder.h"

class InputConfig_UnitTests : public ::testing::Test {
};

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
    remove_all(zip_extract_path);
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

TEST_F(InputConfig_UnitTests, ValidatePrepareReceptors_HasDataLoaded) {
    prepare_receptors pr;
    EXPECT_FALSE(pr.has_data_loaded());

    pr.receptors.push_back("Test");
    EXPECT_TRUE(pr.has_data_loaded());

    pr.receptors.pop_back();
    EXPECT_FALSE(pr.has_data_loaded());

    pr.repair = repair::bonds;
    EXPECT_TRUE(pr.has_data_loaded());

    pr.repair = repair::None;
    EXPECT_FALSE(pr.has_data_loaded());

    pr.preserves.push_back("Test");
    EXPECT_TRUE(pr.has_data_loaded());

    pr.preserves.pop_back();
    EXPECT_FALSE(pr.has_data_loaded());

    pr.cleanup = cleanup::deleteAltB;
    EXPECT_TRUE(pr.has_data_loaded());

    pr.cleanup = cleanup::none;
    EXPECT_FALSE(pr.has_data_loaded());

    pr.delete_nonstd_residue = true;
    EXPECT_TRUE(pr.has_data_loaded());

    pr.delete_nonstd_residue = false;
    EXPECT_FALSE(pr.has_data_loaded());

    pr.receptors.push_back("Test");
    EXPECT_TRUE(pr.has_data_loaded());
    pr.repair = repair::bonds_hydrogens;
    EXPECT_TRUE(pr.has_data_loaded());
    pr.preserves.push_back("Test");
    EXPECT_TRUE(pr.has_data_loaded());
    pr.cleanup = cleanup::lps;
    EXPECT_TRUE(pr.has_data_loaded());
    pr.delete_nonstd_residue = true;
    EXPECT_TRUE(pr.has_data_loaded());
}

TEST_F(InputConfig_UnitTests, FailOnAbsolutePathInReceptors) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_object("prepare_receptors");
    json_encoder.begin_array("receptors");
#ifdef WIN32
    json_encoder.value("C:\\test\\receptor_sample");
#else
    json_encoder.value("/home/test/receptor_sample");
#endif
    json_encoder.end_array();
    json_encoder.end_object();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();

    generator generator;

    const auto res = generator.process(dummy_json_file_path, std::filesystem::current_path());
    EXPECT_FALSE(res);
}

TEST_F(InputConfig_UnitTests, CheckThatReceptorFileIsPresent) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_object("prepare_receptors");
    json_encoder.begin_array("receptors");
#ifdef WIN32
    json_encoder.value("receptor_sample");
#else
    json_encoder.value("receptor_sample");
#endif
    json_encoder.end_array();
    json_encoder.end_object();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();

    generator generator;

    EXPECT_FALSE(generator.process(dummy_json_file_path, std::filesystem::current_path()));
}

TEST_F(InputConfig_UnitTests, ValidatePrepareReceptorsValues) {
    const auto& dummy_json_file_path = std::filesystem::current_path() / "dummy.json";

    dummy_ofstream json;
    json.open(dummy_json_file_path);

    jsoncons::json_stream_encoder jsoncons_encoder(json());
    const json_encoder_helper json_encoder(jsoncons_encoder);

    json_encoder.begin_object();
    json_encoder.begin_object("prepare_receptors");
    json_encoder.begin_array("receptors");
#ifdef WIN32
    json_encoder.value("receptor_sample");
#else
    json_encoder.value("receptor_sample");
#endif
    json_encoder.end_array();
    json_encoder.value("repair", std::string(magic_enum::enum_name(repair::bonds_hydrogens)));
    json_encoder.begin_array("preserves");
    json_encoder.value("test");
    json_encoder.end_array();
    json_encoder.value("cleanup", std::string(magic_enum::enum_name(cleanup::nonstdres)));
    json_encoder.value("delete_nonstd_residue", true);
    json_encoder.end_object();
    json_encoder.end_object();

    jsoncons_encoder.flush();
    json.close();

    dummy_ofstream dummy;
    create_dummy_file(dummy, std::filesystem::current_path() / "receptor_sample");

    const std::ifstream config_file(dummy_json_file_path.c_str());
    std::stringstream buffer;
    buffer << config_file.rdbuf();

    const auto& input_json = jsoncons::json::parse(buffer);

    const auto& working_directory = std::filesystem::current_path();

    prepare_receptors prepare_receptors;
    ASSERT_TRUE(input_json.contains("prepare_receptors"));
    ASSERT_TRUE(prepare_receptors.load(input_json["prepare_receptors"], working_directory));
    ASSERT_TRUE(prepare_receptors.validate());

    ASSERT_EQ(1, prepare_receptors.receptors.size());
    const auto receptor_sample = std::filesystem::current_path() / "receptor_sample";
    EXPECT_STREQ(receptor_sample.string().c_str(), prepare_receptors.receptors[0].c_str());
    EXPECT_EQ(repair::bonds_hydrogens, prepare_receptors.repair);
    ASSERT_EQ(1, prepare_receptors.preserves.size());
    EXPECT_STREQ("test", prepare_receptors.preserves[0].c_str());
    EXPECT_EQ(cleanup::nonstdres, prepare_receptors.cleanup);
    EXPECT_TRUE(prepare_receptors.delete_nonstd_residue);
}

TEST_F(InputConfig_UnitTests, TestSimpleVinaScenario) {
    const auto& json_file = std::filesystem::current_path() / "boinc-autodock-vina/samples/basic_docking_full/1iep_vina.json";

    generator generator;
    ASSERT_TRUE(generator.process(json_file, std::filesystem::current_path()));

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

    const auto res = calculator::calculate(conf, 0, [](double) {});
    EXPECT_TRUE(res);

    std::filesystem::remove(std::filesystem::current_path() / "boinc-autodock-vina/samples/basic_docking_full/1iep_receptor.pdbqt");

    std::filesystem::remove(zip_path);
    remove_all(zip_extract_path);
}
