// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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

        auto filename = input.filename();
        input.remove_filename();
        std::filesystem::current_path(input);

        SerialisedValue toml;
        bool knownGood;
        {
            CoreData coreData;
            Dissolve initial(coreData);
            initial.loadInput(std::string_view(std::string(filename)));
            toml = initial.serialise();
            initial.prepare();
            knownGood = initial.iterate(steps);
        }

        CoreData coreData2;
        Dissolve repeat(coreData2);
        repeat.setInputFilename(std::string(filename));
        EXPECT_NO_THROW(repeat.deserialise(toml));
        auto toml2 = repeat.serialise();

        compare_toml("", toml, toml2);

        repeat.prepare();
        auto confirmation = repeat.iterate(steps);
        EXPECT_EQ(knownGood, confirmation);
    }
}

TEST(TOMLTest, Parse_Accumulate) { runParse("accumulate/accumulate.txt", 20); }
TEST(TOMLTest, Parse_AtomShake) { runParse("atomshake/singlewater.txt", 100); }
TEST(TOMLTest, Parse_Bragg_Intensities) { runParse("bragg/intensities.txt", 1); }
TEST(TOMLTest, Parse_Bragg_Intensities_111) { runParse("bragg/intensities-111.txt", 1); }
TEST(TOMLTest, Parse_Bragg_Mgo) { runParse("bragg/mgo.txt", 1); }
TEST(TOMLTest, Parse_Inputs_Benzene) { runParse("inputs/benzene.txt", 1); }
TEST(TOMLTest, Parse_Inputs_Water) { runParse("inputs/water.txt", 1); }
TEST(TOMLTest, Parse_Inputs_Py5_Ntf2) { runParse("inputs/py5-ntf2.txt", 1); }

TEST(TOMLTest, Parse_argon_dep0_1_indep0_2) { runParse("broadening/argon_dep0.1indep0.2.txt", 1); }
TEST(TOMLTest, Parse_argon_dep0_2_indep0_1) { runParse("broadening/argon_dep0.2indep0.1.txt", 1); }
TEST(TOMLTest, Parse_argon_qdep0_1) { runParse("broadening/argon_qdep0.1.txt", 1); }
TEST(TOMLTest, Parse_argon_qdep0_2) { runParse("broadening/argon_qdep0.2.txt", 1); }
TEST(TOMLTest, Parse_argon_qindep0_1) { runParse("broadening/argon_qindep0.1.txt", 1); }
TEST(TOMLTest, Parse_argon_qindep0_2) { runParse("broadening/argon_qindep0.2.txt", 1); }

TEST(TOMLTest, Parse_AvgMol_Bendy) { runParse("calculate_avgmol/bendy.txt", 201); }
TEST(TOMLTest, Parse_AvgMol_Water) { runParse("calculate_avgmol/water.txt", 95); }
TEST(TOMLTest, Parse_Dangle) { runParse("calculate_dangle/dangle.txt", 95); }

TEST(TOMLTest, Parse_Calculate_CN) { runParse("calculate_cn/cn.txt", 1); }
TEST(TOMLTest, Parse_Calculate_RDF_npt) { runParse("calculate_rdf/npt.txt", 1); }
TEST(TOMLTest, Parse_Calculate_RDF_rdf) { runParse("calculate_rdf/rdf.txt", 1); }
TEST(TOMLTest, Parse_Calculate_SDF) { runParse("calculate_sdf/sdf.txt", 95); }
TEST(TOMLTest, Parse_Correlations) { runParse("correlations/sq.txt", 1); }

TEST(TOMLTest, Parse_EnergyForce1_water3000_coul) { runParse("energyforce1/water3000-coul.txt"); }
TEST(TOMLTest, Parse_EnergyForce1_water3000_elec) { runParse("energyforce1/water3000-elec.txt"); }
TEST(TOMLTest, Parse_EnergyForce1_water3000_full) { runParse("energyforce1/water3000-full.txt"); }
TEST(TOMLTest, Parse_EnergyForce1_water3000_intra) { runParse("energyforce1/water3000-intra.txt"); }
TEST(TOMLTest, Parse_EnergyForce1_water3000_vdw) { runParse("energyforce1/water3000-vdw.txt"); }

TEST(TOMLTest, Parse_EnergyForce2_full) { runParse("energyforce2/full.txt"); }
TEST(TOMLTest, Parse_EnergyForce2_one) { runParse("energyforce2/one.txt"); }
TEST(TOMLTest, Parse_EnergyForce2_torsions) { runParse("energyforce2/torsions.txt"); }
TEST(TOMLTest, Parse_EnergyForce2_two) { runParse("energyforce2/two.txt"); }

TEST(TOMLTest, Parse_EnergyForce3_poe) { runParse("energyforce3/poe.txt"); }
TEST(TOMLTest, Parse_EnergyForce3_py4oh_ntf2) { runParse("energyforce3/py4oh-ntf2.txt"); }
TEST(TOMLTest, Parse_EnergyForce3_py5_ntf2) { runParse("energyforce3/py5-ntf2.txt"); }

TEST(TOMLTest, Parse_EnergyForce4_py4oh_ntf2) { runParse("energyforce4/py4oh-ntf2.txt"); }
TEST(TOMLTest, Parse_EnergyForce4_py5_ntf2) { runParse("energyforce4/py5-ntf2.txt"); }

TEST(TOMLTest, Parse_EPSR_estimatedsq) { runParse("epsr/benzene.txt"); }
TEST(TOMLTest, Parse_EPSR_read_pcof) { runParse("epsr/pcof.txt"); }
TEST(TOMLTest, Parse_EPSR_neutron_xray) { runParse("epsr/water-neutron-xray.txt"); }
TEST(TOMLTest, Parse_EPSR_poisson) { runParse("epsr/water-poisson.txt"); }

TEST(TOMLTest, Parse_Exchangeable_mix) { runParse("exchangeable/watermeth.txt"); }

TEST(TOMLTest, Parse_ForceField_ff1) { runParse("ff/kulmala2010.txt"); }
TEST(TOMLTest, Parse_ForceField_ff2) { runParse("ff/ludwig-py5.txt"); }
TEST(TOMLTest, Parse_ForceField_ff3) { runParse("ff/oplsaa2005-alcohols.txt"); }
TEST(TOMLTest, Parse_ForceField_ff4) { runParse("ff/oplsaa2005-alkanes.txt"); }
TEST(TOMLTest, Parse_ForceField_ff5) { runParse("ff/oplsaa2005-aromatics.txt"); }
TEST(TOMLTest, Parse_ForceField_ff6) { runParse("ff/pcl2019-anions.txt"); }
TEST(TOMLTest, Parse_ForceField_ff7) { runParse("ff/pcl2019-cations.txt"); }
TEST(TOMLTest, Parse_ForceField_ff8) { runParse("ff/spcfw.txt"); }
TEST(TOMLTest, Parse_ForceField_ff9) { runParse("ff/uff-nmethylformamide.txt"); }
TEST(TOMLTest, Parse_ForceField_ff10) { runParse("ff/uff4mof-mof5.txt"); }

TEST(TOMLTest, Parse_MD_md1) { runParse("md/benzene.txt", 2); }
TEST(TOMLTest, Parse_MolShake_molshake1) { runParse("molshake/benzene.txt", 2); }

TEST(TOMLTest, Parse_RDF_rdf_cells) { runParse("rdfmethod/cells.txt"); }
TEST(TOMLTest, Parse_RDF_rdf_simple) { runParse("rdfmethod/simple.txt"); }

TEST(TOMLTest, Parse_Restart_Benzene) { runParse("restart/benzene.txt"); }
TEST(TOMLTest, Parse_Restart_rdf) { runParse("restart/rdf.txt"); }

TEST(TOMLTest, Parse_xray_water) { runParse("xray/water.txt"); }

} // namespace UnitTest
