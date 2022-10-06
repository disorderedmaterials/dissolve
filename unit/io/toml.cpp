// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "main/dissolve.h"
#include <gtest/gtest.h>

namespace UnitTest
{

void runParse(std::string input)
{
    if constexpr (Dissolve::toml_testing_flag)
    {

        CoreData coreData, coreData2;
        Dissolve initial(coreData);
        initial.loadInput(input);
        auto toml = initial.serialise();
        Dissolve repeat(coreData2);
        EXPECT_NO_THROW(repeat.deserialise(toml));
        auto toml2 = repeat.serialise();

        EXPECT_EQ(toml, toml2);
    }
}

TEST(TOMLTest, Parse_Accumulate) { runParse("accumulate/accumulate.txt"); }
TEST(TOMLTest, Parse_AtomShake) { runParse("atomshake/singlewater.txt"); }
TEST(TOMLTest, Parse_Bragg_Intensities) { runParse("bragg/intensities.txt"); }
TEST(TOMLTest, Parse_Bragg_Intensities_111) { runParse("bragg/intensities-111.txt"); }
TEST(TOMLTest, Parse_Bragg_Mgo) { runParse("bragg/mgo.txt"); }
TEST(TOMLTest, Parse_Inputs_Benzene) { runParse("inputs/benzene.txt"); }
TEST(TOMLTest, Parse_Inputs_Water) { runParse("inputs/water.txt"); }
TEST(TOMLTest, Parse_Inputs_Py5_Ntf2) { runParse("inputs/py5-ntf2.txt"); }

TEST(TOMLTest, Parse_argon_dep0_1_indep0_2) { runParse("broadening/argon_dep0.1indep0.2.txt"); }
TEST(TOMLTest, Parse_argon_dep0_2_indep0_1) { runParse("broadening/argon_dep0.2indep0.1.txt"); }
TEST(TOMLTest, Parse_argon_qdep0_1) { runParse("broadening/argon_qdep0.1.txt"); }
TEST(TOMLTest, Parse_argon_qdep0_2) { runParse("broadening/argon_qdep0.2.txt"); }
TEST(TOMLTest, Parse_argon_qindep0_1) { runParse("broadening/argon_qindep0.1.txt"); }
TEST(TOMLTest, Parse_argon_qindep0_2) { runParse("broadening/argon_qindep0.2.txt"); }

TEST(TOMLTest, Parse_AvgMol_Bendy) { runParse("calculate_avgmol/bendy.txt"); }
TEST(TOMLTest, Parse_AvgMol_Water) { runParse("calculate_avgmol/water.txt"); }
TEST(TOMLTest, Parse_Dangle) { runParse("calculate_dangle/dangle.txt"); }

TEST(TOMLTest, Parse_Calculate_CN) { runParse("calculate_cn/cn.txt"); }
TEST(TOMLTest, Parse_Calculate_RDF_npt) { runParse("calculate_rdf/npt.txt"); }
TEST(TOMLTest, Parse_Calculate_RDF_rdf) { runParse("calculate_rdf/rdf.txt"); }
TEST(TOMLTest, Parse_Calculate_SDF) { runParse("calculate_sdf/sdf.txt"); }
TEST(TOMLTest, Parse_Correlations) { runParse("correlations/sq.txt"); }

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

TEST(TOMLTest, Parse_MD_md1) { runParse("md/benzene.txt"); }
TEST(TOMLTest, Parse_MolShake_molshake1) { runParse("molshake/benzene.txt"); }

TEST(TOMLTest, Parse_RDF_rdf_cells) { runParse("rdfmethod/cells.txt"); }
TEST(TOMLTest, Parse_RDF_rdf_simple) { runParse("rdfmethod/simple.txt"); }

TEST(TOMLTest, Parse_Restart_Benzene) { runParse("restart/benzene.txt"); }
TEST(TOMLTest, Parse_Restart_rdf) { runParse("restart/rdf.txt"); }

TEST(TOMLTest, Parse_xray_water) { runParse("xray/water.txt"); }

} // namespace UnitTest
