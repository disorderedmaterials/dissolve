// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "main/dissolve.h"
#include <filesystem>
#include <gtest/gtest.h>

namespace UnitTest
{

void compare_toml(std::string location, SerialisedValue toml, SerialisedValue toml2)
{
    if (toml.is_table())
    {
        ASSERT_TRUE(toml2.is_table()) << location;
        for (auto &[k, v] : toml.as_table())
        {
            ASSERT_TRUE(toml2.contains(k)) << location << "." << k << std::endl << "Expected:" << std::endl << toml[k];
            compare_toml(fmt::format("{}.{}", location, k), v, toml2.at(k));
        }
    }
    else if (toml.is_array())
    {
        auto arr = toml.as_array();
        auto arr2 = toml2.as_array();
        ASSERT_EQ(arr.size(), arr2.size()) << location << std::endl << "Expected" << std::endl << toml;
        for (int i = 0; i < arr.size(); ++i)
            compare_toml(fmt::format("{}[{}]", location, i), arr[i], arr2[i]);
    }
    else
    {
        EXPECT_EQ(toml, toml2) << location;
    }
}

void runParse(std::filesystem::path input, int steps = 1)
{
    if constexpr (Dissolve::toml_testing_flag)
    {

        SerialisedValue toml;
        bool knownGood;
        {
            CoreData coreData;
            Dissolve initial(coreData);
            initial.loadInput(std::string_view(std::string(input)));
            toml = initial.serialise();
            initial.prepare();
            knownGood = initial.iterate(steps);
        }

        CoreData coreData2;
        Dissolve repeat(coreData2);
        repeat.setInputFilename(std::string(input));
        EXPECT_NO_THROW(repeat.deserialise(toml));
        auto toml2 = repeat.serialise();

        compare_toml("", toml, toml2);

        repeat.prepare();
        auto confirmation = repeat.iterate(steps);
        EXPECT_EQ(knownGood, confirmation);
    }
}

TEST(TOMLTest, Parse_accumulate) { runParse("dissolve/input/accumulate.txt", 20); }
TEST(TOMLTest, Parse_atomShake_water) { runParse("dissolve/input/atomShake-water.txt", 100); }
TEST(TOMLTest, Parse_avgMol_bendy) { runParse("dissolve/input/avgMol-bendy.txt", 201); }
TEST(TOMLTest, Parse_avgMol_water) { runParse("dissolve/input/avgMol-water.txt", 95); }
TEST(TOMLTest, Parse_bragg_MgO) { runParse("dissolve/input/bragg-MgO.txt", 1); }
TEST(TOMLTest, Parse_broadening_argon) { runParse("dissolve/input/broadening-argon.txt", 1); }
TEST(TOMLTest, Parse_correlations_water) { runParse("dissolve/input/correlations-water.txt", 1); }
TEST(TOMLTest, Parse_correlations_waterMethanol) { runParse("dissolve/input/correlations-waterMethanol.txt", 1); }
TEST(TOMLTest, Parse_correlations_waterXRay) { runParse("dissolve/input/correlations-waterXRay.txt", 1); }
TEST(TOMLTest, Parse_dAngle) { runParse("dissolve/input/dAngle.txt", 95); }
TEST(TOMLTest, Parse_energyForce_POE) { runParse("dissolve/input/energyForce-POE.txt", 1); }
TEST(TOMLTest, Parse_energyForce_Py4OH_NTf2) { runParse("dissolve/input/energyForce-Py4OH-NTf2.txt", 1); }
TEST(TOMLTest, Parse_energyForce_Py4OH_NTf2_impropers) { runParse("dissolve/input/energyForce-Py4OH_NTf2-impropers.txt", 1); }
TEST(TOMLTest, Parse_energyForce_Py5_NTf2) { runParse("dissolve/input/energyForce-Py5-NTf2.txt", 1); }
TEST(TOMLTest, Parse_energyForce_Py5_NTf2_impropers) { runParse("dissolve/input/energyForce-Py5-NTf2-impropers.txt", 1); }
TEST(TOMLTest, Parse_energyForce_benzene181) { runParse("dissolve/input/energyForce-benzene181.txt", 1); }
TEST(TOMLTest, Parse_energyForce_hexane1) { runParse("dissolve/input/energyForce-hexane1.txt", 1); }
TEST(TOMLTest, Parse_energyForce_hexane2) { runParse("dissolve/input/energyForce-hexane2.txt", 1); }
TEST(TOMLTest, Parse_energyForce_hexane200) { runParse("dissolve/input/energyForce-hexane200.txt", 1); }
TEST(TOMLTest, Parse_energyForce_water3000) { runParse("dissolve/input/energyForce-water3000.txt", 1); }
TEST(TOMLTest, Parse_epsr_5datasets) { runParse("dissolve/input/epsr-5datasets.txt", 1); }
TEST(TOMLTest, Parse_epsr_5datasets_weighted) { runParse("dissolve/input/epsr-5datasets-weighted.txt", 1); }
TEST(TOMLTest, Parse_epsr_benzene_3n) { runParse("dissolve/input/epsr-benzene-3n.txt", 1); }
TEST(TOMLTest, Parse_epsr_benzene_3n_pcof) { runParse("dissolve/input/epsr-benzene-3n-pcof.txt", 1); }
TEST(TOMLTest, Parse_epsr_water_3n_x) { runParse("dissolve/input/epsr-water-3n-x.txt", 1); }
TEST(TOMLTest, Parse_epsr_water_inpa) { runParse("dissolve/input/epsr-water-inpa.txt", 1); }
TEST(TOMLTest, Parse_ff_kulmala2010) { runParse("dissolve/input/ff-kulmala2010.txt", 1); }
TEST(TOMLTest, Parse_ff_ludwig_py5) { runParse("dissolve/input/ff-ludwig-py5.txt", 1); }
TEST(TOMLTest, Parse_ff_oplsaa2005_alcohols) { runParse("dissolve/input/ff-oplsaa2005-alcohols.txt", 1); }
TEST(TOMLTest, Parse_ff_oplsaa2005_alkanes) { runParse("dissolve/input/ff-oplsaa2005-alkanes.txt", 1); }
TEST(TOMLTest, Parse_ff_oplsaa2005_aromatics) { runParse("dissolve/input/ff-oplsaa2005-aromatics.txt", 1); }
TEST(TOMLTest, Parse_ff_pcl2019_anions) { runParse("dissolve/input/ff-pcl2019-anions.txt", 1); }
TEST(TOMLTest, Parse_ff_pcl2019_cations) { runParse("dissolve/input/ff-pcl2019-cations.txt", 1); }
TEST(TOMLTest, Parse_ff_spcfw) { runParse("dissolve/input/ff-spcfw.txt", 1); }
TEST(TOMLTest, Parse_ff_uff4mof_mof5) { runParse("dissolve/input/ff-uff4mof-mof5.txt", 1); }
TEST(TOMLTest, Parse_ff_uff_nmethylformamide) { runParse("dissolve/input/ff-uff-nmethylformamide.txt", 1); }
TEST(TOMLTest, Parse_full_benzene) { runParse("dissolve/input/full-benzene.txt", 1); }
TEST(TOMLTest, Parse_histogramCN_simple) { runParse("dissolve/input/histogramCN-simple.txt", 1); }
TEST(TOMLTest, Parse_histogramCN_water) { runParse("dissolve/input/histogramCN-water.txt", 1); }
TEST(TOMLTest, Parse_intraDistance) { runParse("dissolve/input/intraDistance.txt", 1); }
TEST(TOMLTest, Parse_md_benzene) { runParse("dissolve/input/md-benzene.txt", 1); }
TEST(TOMLTest, Parse_orientedSDF_benzene) { runParse("dissolve/input/orientedSDF-benzene.txt", 1); }
TEST(TOMLTest, Parse_py5_ntf2) { runParse("dissolve/input/py5-ntf2.txt", 1); }
TEST(TOMLTest, Parse_rdfMethod) { runParse("dissolve/input/rdfMethod.txt", 1); }
TEST(TOMLTest, Parse_sdf_benzene) { runParse("dissolve/input/sdf-benzene.txt", 1); }
TEST(TOMLTest, Parse_sdf_water) { runParse("dissolve/input/sdf-water.txt", 1); }
TEST(TOMLTest, Parse_siteRDF_water) { runParse("dissolve/input/siteRDF-water.txt", 1); }
TEST(TOMLTest, Parse_siteRDF_waterDynamic) { runParse("dissolve/input/siteRDF-waterDynamic.txt", 1); }
TEST(TOMLTest, Parse_siteRDF_waterFragments) { runParse("dissolve/input/siteRDF-waterFragments.txt", 1); }
TEST(TOMLTest, Parse_siteRDF_waterNPT) { runParse("dissolve/input/siteRDF-waterNPT.txt", 1); }
TEST(TOMLTest, Parse_water) { runParse("dissolve/input/water.txt", 1); }

} // namespace UnitTest
