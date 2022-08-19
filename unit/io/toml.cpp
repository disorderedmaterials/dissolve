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

TEST(TOMLTest, Parse_Calculate_RDF_npt) { runParse("calculate_rdf/npt.txt"); }
TEST(TOMLTest, Parse_Calculate_RDF_rdf) { runParse("calculate_rdf/rdf.txt"); }
TEST(TOMLTest, Parse_Calculate_SDF) { runParse("calculate_sdf/sdf.txt"); }
TEST(TOMLTest, Parse_Correlations) { runParse("correlations/sq.txt"); }

TEST(TOMLTest, Parse_Restart_Benzene) { runParse("restart/benzene.txt"); }

} // namespace UnitTest
