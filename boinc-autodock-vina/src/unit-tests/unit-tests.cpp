#include <fstream>

#include <gtest/gtest.h>

#include "common/config.h"
#include "boinc-autodock-vina/calculate.h"

class Config_UnitTests : public ::testing::Test
{
};

TEST_F(Config_UnitTests, FailOnConfigDefaults) {
    config config;
    EXPECT_FALSE(config.validate());
}

TEST_F(Config_UnitTests, FailOnAbsolutePathInConfig) {
    const auto dummy_json_file_path = std::filesystem::current_path() /= "dummy.json";
    config config;
    std::ofstream json;

    json.open(dummy_json_file_path.c_str());
    json << "{" << std::endl;
    json << "\t\"input\": {" << std::endl;
#ifdef WIN32
    json << "\t\t\"receptor\": \"C:\\\\test\\\\receptor_sample\"," << std::endl;
#else
    json << "\t\t\"receptor\": \"/home/test/receptor_sample\"," << std::endl;
#endif
    json << "\t\t\"flex\": \"flex_sample\"," << std::endl;
    json << "\t\t\"ligands\": [" << std::endl;
    json << "\t\t\t\"ligand_sample2\"" << std::endl;
    json << "\t\t]," << std::endl;
    json << "\t\t\"batch\": [" << std::endl;
    json << "\t\t\t\"batch_sample2\"" << std::endl;
    json << "\t\t]" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"search_area\": {" << std::endl;
    json << "\t\t\"maps\": \"maps_sample\"" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"output\": {" << std::endl;
    json << "\t\t\"out\": \"out_sample\"," << std::endl;
    json << "\t\t\"dir\": \"dir_sample\"," << std::endl;
    json << "\t\t\"write_maps\": \"write_maps_sample\"" << std::endl;
    json << "\t}" << std::endl;
    json << "}" << std::endl;
    json.close();
    EXPECT_FALSE(config.load(dummy_json_file_path));
    std::filesystem::remove(dummy_json_file_path);

    json.open(dummy_json_file_path.c_str());
    json << "{" << std::endl;
    json << "\t\"input\": {" << std::endl;
    json << "\t\t\"receptor\": \"receptor_sample\"," << std::endl;
#ifdef WIN32
    json << "\t\t\"flex\": \"C:\\\\test\\\\flex_sample\"," << std::endl;
#else
    json << "\t\t\"flex\": \"/home/test/flex_sample\"," << std::endl;
#endif
    json << "\t\t\"ligands\": [" << std::endl;
    json << "\t\t\t\"ligand_sample2\"" << std::endl;
    json << "\t\t]," << std::endl;
    json << "\t\t\"batch\": [" << std::endl;
    json << "\t\t\t\"batch_sample2\"" << std::endl;
    json << "\t\t]" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"search_area\": {" << std::endl;
    json << "\t\t\"maps\": \"maps_sample\"" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"output\": {" << std::endl;
    json << "\t\t\"out\": \"out_sample\"," << std::endl;
    json << "\t\t\"dir\": \"dir_sample\"," << std::endl;
    json << "\t\t\"write_maps\": \"write_maps_sample\"" << std::endl;
    json << "\t}" << std::endl;
    json << "}" << std::endl;
    json.close();
    EXPECT_FALSE(config.load(dummy_json_file_path));
    std::filesystem::remove(dummy_json_file_path);

    json.open(dummy_json_file_path.c_str());
    json << "{" << std::endl;
    json << "\t\"input\": {" << std::endl;
    json << "\t\t\"receptor\": \"receptor_sample\"," << std::endl;
    json << "\t\t\"flex\": \"flex_sample\"," << std::endl;
    json << "\t\t\"ligands\": [" << std::endl;
#ifdef WIN32
    json << "\t\t\t\"C:\\\\test\\\\ligand_sample\"" << std::endl;
#else
    json << "\t\t\t\"/home/test/ligand_sample\"" << std::endl;
#endif
    json << "\t\t]," << std::endl;
    json << "\t\t\"batch\": [" << std::endl;
    json << "\t\t\t\"batch_sample2\"" << std::endl;
    json << "\t\t]" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"search_area\": {" << std::endl;
    json << "\t\t\"maps\": \"maps_sample\"" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"output\": {" << std::endl;
    json << "\t\t\"out\": \"out_sample\"," << std::endl;
    json << "\t\t\"dir\": \"dir_sample\"," << std::endl;
    json << "\t\t\"write_maps\": \"write_maps_sample\"" << std::endl;
    json << "\t}" << std::endl;
    json << "}" << std::endl;
    json.close();
    EXPECT_FALSE(config.load(dummy_json_file_path));
    std::filesystem::remove(dummy_json_file_path);

    json.open(dummy_json_file_path.c_str());
    json << "{" << std::endl;
    json << "\t\"input\": {" << std::endl;
    json << "\t\t\"receptor\": \"receptor_sample\"," << std::endl;
    json << "\t\t\"flex\": \"flex_sample\"," << std::endl;
    json << "\t\t\"ligands\": [" << std::endl;
    json << "\t\t\t\"ligand_sample2\"" << std::endl;
    json << "\t\t]," << std::endl;
    json << "\t\t\"batch\": [" << std::endl;
#ifdef WIN32
    json << "\t\t\t\"C:\\\\test\\\\batch_sample\"" << std::endl;
#else
    json << "\t\t\t\"/home/test/batch_sample\"" << std::endl;
#endif
    json << "\t\t]" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"search_area\": {" << std::endl;
    json << "\t\t\"maps\": \"maps_sample\"" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"output\": {" << std::endl;
    json << "\t\t\"out\": \"out_sample\"," << std::endl;
    json << "\t\t\"dir\": \"dir_sample\"," << std::endl;
    json << "\t\t\"write_maps\": \"write_maps_sample\"" << std::endl;
    json << "\t}" << std::endl;
    json << "}" << std::endl;
    json.close();
    EXPECT_FALSE(config.load(dummy_json_file_path));
    std::filesystem::remove(dummy_json_file_path);

    json.open(dummy_json_file_path.c_str());
    json << "{" << std::endl;
    json << "\t\"input\": {" << std::endl;
    json << "\t\t\"receptor\": \"receptor_sample\"," << std::endl;
    json << "\t\t\"flex\": \"flex_sample\"," << std::endl;
    json << "\t\t\"ligands\": [" << std::endl;
    json << "\t\t\t\"ligand_sample2\"" << std::endl;
    json << "\t\t]," << std::endl;
    json << "\t\t\"batch\": [" << std::endl;
    json << "\t\t\t\"batch_sample2\"" << std::endl;
    json << "\t\t]" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"search_area\": {" << std::endl;
#ifdef WIN32
    json << "\t\t\"maps\": \"C:\\\\test\\\\maps_sample\"" << std::endl;
#else
    json << "\t\t\"maps\": \"/home/test/maps_sample\"" << std::endl;
#endif
    json << "\t}," << std::endl;
    json << "\t\"output\": {" << std::endl;
    json << "\t\t\"out\": \"out_sample\"," << std::endl;
    json << "\t\t\"dir\": \"dir_sample\"," << std::endl;
    json << "\t\t\"write_maps\": \"write_maps_sample\"" << std::endl;
    json << "\t}" << std::endl;
    json << "}" << std::endl;
    json.close();
    EXPECT_FALSE(config.load(dummy_json_file_path));
    std::filesystem::remove(dummy_json_file_path);

    json.open(dummy_json_file_path.c_str());
    json << "{" << std::endl;
    json << "\t\"input\": {" << std::endl;
    json << "\t\t\"receptor\": \"receptor_sample\"," << std::endl;
    json << "\t\t\"flex\": \"flex_sample\"," << std::endl;
    json << "\t\t\"ligands\": [" << std::endl;
    json << "\t\t\t\"ligand_sample2\"" << std::endl;
    json << "\t\t]," << std::endl;
    json << "\t\t\"batch\": [" << std::endl;
    json << "\t\t\t\"batch_sample2\"" << std::endl;
    json << "\t\t]" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"search_area\": {" << std::endl;
    json << "\t\t\"maps\": \"maps_sample\"" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"output\": {" << std::endl;
#ifdef WIN32
    json << "\t\t\"out\": \"C:\\\\test\\\\out_sample\"," << std::endl;
#else
    json << "\t\t\"out\": \"/home/test/out_sample\"," << std::endl;
#endif
    json << "\t\t\"dir\": \"dir_sample\"," << std::endl;
    json << "\t\t\"write_maps\": \"write_maps_sample\"" << std::endl;
    json << "\t}" << std::endl;
    json << "}" << std::endl;
    json.close();
    EXPECT_FALSE(config.load(dummy_json_file_path));
    std::filesystem::remove(dummy_json_file_path);

    json.open(dummy_json_file_path.c_str());
    json << "{" << std::endl;
    json << "\t\"input\": {" << std::endl;
    json << "\t\t\"receptor\": \"receptor_sample\"," << std::endl;
    json << "\t\t\"flex\": \"flex_sample\"," << std::endl;
    json << "\t\t\"ligands\": [" << std::endl;
    json << "\t\t\t\"ligand_sample2\"" << std::endl;
    json << "\t\t]," << std::endl;
    json << "\t\t\"batch\": [" << std::endl;
    json << "\t\t\t\"batch_sample2\"" << std::endl;
    json << "\t\t]" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"search_area\": {" << std::endl;
    json << "\t\t\"maps\": \"maps_sample\"" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"output\": {" << std::endl;
    json << "\t\t\"out\": \"out_sample\"," << std::endl;
#ifdef WIN32
    json << "\t\t\"dir\": \"C:\\\\test\\\\dir_sample\"," << std::endl;
#else
    json << "\t\t\"dir\": \"/home/test/dir_sample\"," << std::endl;
#endif
    json << "\t\t\"write_maps\": \"write_maps_sample\"" << std::endl;
    json << "\t}" << std::endl;
    json << "}" << std::endl;
    json.close();
    EXPECT_FALSE(config.load(dummy_json_file_path));
    std::filesystem::remove(dummy_json_file_path);

    json.open(dummy_json_file_path.c_str());
    json << "{" << std::endl;
    json << "\t\"input\": {" << std::endl;
    json << "\t\t\"receptor\": \"receptor_sample\"," << std::endl;
    json << "\t\t\"flex\": \"flex_sample\"," << std::endl;
    json << "\t\t\"ligands\": [" << std::endl;
    json << "\t\t\t\"ligand_sample2\"" << std::endl;
    json << "\t\t]," << std::endl;
    json << "\t\t\"batch\": [" << std::endl;
    json << "\t\t\t\"batch_sample2\"" << std::endl;
    json << "\t\t]" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"search_area\": {" << std::endl;
    json << "\t\t\"maps\": \"maps_sample\"" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"output\": {" << std::endl;
    json << "\t\t\"out\": \"out_sample\"," << std::endl;
    json << "\t\t\"dir\": \"dir_sample\"," << std::endl;
#ifdef WIN32
    json << "\t\t\"write_maps\": \"C:\\\\test\\\\write_maps_sample\"" << std::endl;
#else
    json << "\t\t\"write_maps\": \"/home/test/write_maps_sample\"" << std::endl;
#endif
    json << "\t}" << std::endl;
    json << "}" << std::endl;
    json.close();
    EXPECT_FALSE(config.load(dummy_json_file_path));
    std::filesystem::remove(dummy_json_file_path);
}

TEST_F(Config_UnitTests, LoadValidator) {
    const auto dummy_json_file_path = std::filesystem::current_path() /= "dummy.json";
    std::ofstream json(dummy_json_file_path.c_str());

    json << "{" << std::endl;
    json << "\t\"input\": {" << std::endl;
    json << "\t\t\"receptor\": \"receptor_sample\"," << std::endl;
    json << "\t\t\"flex\": \"flex_sample\"," << std::endl;
    json << "\t\t\"ligands\": [" << std::endl;
    json << "\t\t\t\"ligand_sample1\"," << std::endl;
    json << "\t\t\t\"ligand_sample2\"" << std::endl;
    json << "\t\t]," << std::endl;
    json << "\t\t\"batch\": [" << std::endl;
    json << "\t\t\t\"batch_sample1\"," << std::endl;
    json << "\t\t\t\"batch_sample2\"" << std::endl;
    json << "\t\t]," << std::endl;
    json << "\t\t\"scoring\": \"vinardo\"" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"search_area\": {" << std::endl;
    json << "\t\t\"maps\": \"maps_sample\"," << std::endl;
    json << "\t\t\"center_x\": 0.123456," << std::endl;
    json << "\t\t\"center_y\": 0.654321," << std::endl;
    json << "\t\t\"center_z\": -0.123456," << std::endl;
    json << "\t\t\"size_x\": -0.654321," << std::endl;
    json << "\t\t\"size_y\": 0.0," << std::endl;
    json << "\t\t\"size_z\": -0.000135," << std::endl;
    json << "\t\t\"autobox\": true" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"output\": {" << std::endl;
    json << "\t\t\"out\": \"out_sample\"," << std::endl;
    json << "\t\t\"dir\": \"dir_sample\"," << std::endl;
    json << "\t\t\"write_maps\": \"write_maps_sample\"" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"advanced\": {" << std::endl;
    json << "\t\t\"score_only\": true," << std::endl;
    json << "\t\t\"local_only\": true," << std::endl;
    json << "\t\t\"no_refine\": true," << std::endl;
    json << "\t\t\"force_even_voxels\": true," << std::endl;
    json << "\t\t\"randomize_only\": true," << std::endl;
    json << "\t\t\"weight_gauss1\": 0.123456," << std::endl;
    json << "\t\t\"weight_gauss2\": -0.123456," << std::endl;
    json << "\t\t\"weight_repulsion\": 0.654321," << std::endl;
    json << "\t\t\"weight_hydrophobic\": -0.654321," << std::endl;
    json << "\t\t\"weight_hydrogen\": 0.135246," << std::endl;
    json << "\t\t\"weight_rot\": -0.135246," << std::endl;
    json << "\t\t\"weight_vinardo_gauss1\": -0.642531," << std::endl;
    json << "\t\t\"weight_vinardo_repulsion\": 0.642531," << std::endl;
    json << "\t\t\"weight_vinardo_hydrophobic\": -0.010011," << std::endl;
    json << "\t\t\"weight_vinardo_hydrogen\": 0.010011," << std::endl;
    json << "\t\t\"weight_vinardo_rot\": -1.023456," << std::endl;
    json << "\t\t\"weight_ad4_vdw\": 1.023456," << std::endl;
    json << "\t\t\"weight_ad4_hb\": -1.654320," << std::endl;
    json << "\t\t\"weight_ad4_elec\": 1.065432," << std::endl;
    json << "\t\t\"weight_ad4_dsolv\": -1.064235," << std::endl;
    json << "\t\t\"weight_ad4_rot\": 1.064235," << std::endl;
    json << "\t\t\"weight_glue\": 1.024653" << std::endl;
    json << "\t}," << std::endl;
    json << "\t\"misc\": {" << std::endl;
    json << "\t\t\"cpu\": 1," << std::endl;
    json << "\t\t\"seed\": 2," << std::endl;
    json << "\t\t\"exhaustiveness\": 3," << std::endl;
    json << "\t\t\"max_evals\": 4," << std::endl;
    json << "\t\t\"num_modes\": 5," << std::endl;
    json << "\t\t\"min_rmsd\": 2.0," << std::endl;
    json << "\t\t\"energy_range\": -2.0," << std::endl;
    json << "\t\t\"spacing\": -0.123," << std::endl;
    json << "\t\t\"verbosity\": 6" << std::endl;
    json << "\t}" << std::endl;
    json << "}" << std::endl;
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
    EXPECT_EQ(1, config.misc.cpu);
    EXPECT_EQ(2, config.misc.seed);
    EXPECT_EQ(3, config.misc.exhaustiveness);
    EXPECT_EQ(4, config.misc.max_evals);
    EXPECT_EQ(5, config.misc.num_modes);
    EXPECT_DOUBLE_EQ(2.0, config.misc.min_rmsd);
    EXPECT_DOUBLE_EQ(-2.0, config.misc.energy_range);
    EXPECT_DOUBLE_EQ(-0.123, config.misc.spacing);
    EXPECT_EQ(6, config.misc.verbosity);

    std::filesystem::remove(dummy_json_file_path);
}

TEST_F(Config_UnitTests, TestSimpleVinaScenario) {
    const auto json_file = std::filesystem::current_path() /= "boinc-autodock-vina/samples/basic_docking/1iep_vina.json";
    const auto res = calculate(json_file, [](double) { });
    EXPECT_TRUE(res);
    std::filesystem::remove(std::filesystem::current_path() /= "boinc-autodock-vina/samples/basic_docking/1iep_ligand_vina_out.pdbqt");
}
