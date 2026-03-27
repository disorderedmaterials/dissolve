// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/windowFunction.h"
#include "nodes/gr/gr.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(BroadeningTest, ArgonBroadening)
{
    // Set up the test graph
    GraphTestData data;
    auto lastNode = createConfiguration(&data.graphRoot, {{createArgon, 10000}}, 0.0213);
    lastNode = appendImportCoordinates(
        &data.graphRoot, lastNode,
        CoordinateImportFileFormat("epsr25/argon10000/argonbox.ato", CoordinateImportFileFormat::CoordinateImportFormat::EPSR));

    // Append GR and SQ nodes
    auto sqNode = appendGRSQ(&data.graphRoot, lastNode, true, true);
    auto arSpeciesNode = data.graphRoot.findNode("Ar");
    ASSERT_TRUE(arSpeciesNode);

    // Get argon species and set up neutron SQ
    auto arSpecies = arSpeciesNode->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(arSpecies);
    auto neutronSQNode =
        appendNeutronSQ(&data.graphRoot, sqNode, "Yarnell", IsotopologueSet({{arSpecies->findIsotopologue("Ar36"), 1.0}}));

    std::vector<std::tuple<std::string, Functions1D::Form, std::vector<double>>> tests = {
        {"epsr25/argon10000/argon_dep0.1indep0.2.EPSR.u01", Functions1D::Form::GaussianC2, {0.2, 0.1}},
        {"epsr25/argon10000/argon_dep0.2indep0.1.EPSR.u01", Functions1D::Form::GaussianC2, {0.1, 0.2}},
        {"epsr25/argon10000/argon_dep0.1.EPSR.u01", Functions1D::Form::OmegaDependentGaussian, {0.1}},
        {"epsr25/argon10000/argon_dep0.2.EPSR.u01", Functions1D::Form::OmegaDependentGaussian, {0.2}},
        {"epsr25/argon10000/argon_indep0.1.EPSR.u01", Functions1D::Form::Gaussian, {0.1}},
        {"epsr25/argon10000/argon_indep0.2.EPSR.u01", Functions1D::Form::Gaussian, {0.2}}};

    auto count = 0;
    for (const auto &[dataFile, form, parameters] : tests)
    {
        // Set broadening parameters
        ASSERT_TRUE(sqNode->setOption("QBroadening", Function1DWrapper(form, parameters)));

        // Run from the NeutronSQ node
        ASSERT_EQ(neutronSQNode->run(), NodeConstants::ProcessResult::Success);
        ASSERT_EQ(neutronSQNode->versionIndex(), count++);

        // Get the weighted SQ
        auto weightedSQ = neutronSQNode->getOutputValue<PartialSet *>("WeightedSQ");
        ASSERT_TRUE(weightedSQ);
        EXPECT_TRUE(DissolveSystemTest::checkData1D(
            weightedSQ->total(), std::format("{} {}", Functions1D::forms().keyword(form), joinStrings(parameters)),
            {dataFile, Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 3.0e-3));
    }
}

} // namespace UnitTest