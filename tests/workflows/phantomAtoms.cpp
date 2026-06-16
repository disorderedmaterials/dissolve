// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/atomicMasses.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(PhantomAtomsTest, Basic)
{
    // Construct the test graph
    TestGraph testGraph;
    const auto nMolecules = 100;
    testGraph.createConfiguration("Box", {{"species/tetrahedral-argon.toml", nMolecules}}, {20.0, 20.0, 20.0},
                                  {90.0, 90.0, 90.0});

    // Run the graph to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Grab the species
    auto speciesNode = dynamic_cast<SpeciesNode *>(testGraph.findNode("TetrahedralArgon"));
    ASSERT_TRUE(speciesNode);
    auto &species = speciesNode->species();

    // Basic species checks
    EXPECT_EQ(species.nAtoms(), 5);
    EXPECT_EQ(species.nAtoms(AtomConstants::Presence::Phantom), 4);
    EXPECT_DOUBLE_EQ(species.mass(), AtomicMass::mass(Elements::Ar));

    // Get the configuration
    auto *cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    ASSERT_TRUE(cfg);

    // Basic configuration checks
    EXPECT_EQ(cfg->nMolecules(), nMolecules);
    EXPECT_EQ(cfg->nAtoms(), nMolecules * species.nAtoms());
    EXPECT_EQ(cfg->nAtoms(AtomConstants::Presence::Phantom), nMolecules * species.nAtoms(AtomConstants::Presence::Phantom));

    // Check density - should correspond to number density of physical atoms only
    EXPECT_NEAR(*cfg->atomicDensity(), (nMolecules * species.nAtoms(AtomConstants::Presence::Physical)) / cfg->box().volume(),
                1.0e-5);
}

TEST(PhantomAtomsTest, Water)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{"species/water-with-lps.toml", 1000}}, 0.1));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportXYZStructure", "xyz/water1000-phantom.xyz"));

    // Add correlation function nodes
    auto &&[grNode, sqNode] = testGraph.appendGRSQ(false, true);
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));
    ASSERT_TRUE(sqNode);

    // Add in NeutronSQ
    auto H2O = testGraph.appendNeutronSQ(sqNode, "H2O");
    auto D2O = testGraph.appendNeutronSQ(sqNode, "D2O", {{"Water", "D2O", 1.0}});
    auto HDO = testGraph.appendNeutronSQ(sqNode, "5050", {{"Water", "Natural", 1.0}, {"Water", "D2O", 1.0}});

    // Add XRaySQ
    auto H2Ox = testGraph.appendXRaySQ(sqNode, "H2Ox", {"epsr25/water1000-neutron-xray/PCCPfofq.txt", true});
    ASSERT_TRUE(H2Ox);
    ASSERT_TRUE(H2Ox->setOption<StructureFactors::NormalisationType>(
        "NormaliseTo", StructureFactors::NormalisationType::AverageOfSquaresNormalisation));
    ASSERT_TRUE(H2Ox->setOption<StructureFactors::NormalisationType>(
        "ReferenceNormalisedTo", StructureFactors::NormalisationType::AverageOfSquaresNormalisation));
    ASSERT_TRUE(H2Ox->setOption<std::optional<Number>>("ReferenceFTQMin", 0.5));

    // Run the graph from each NeutronSQ and XRaySQ node
    ASSERT_TRUE(H2O);
    ASSERT_EQ(H2O->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(H2O->versionIndex(), 0);
    ASSERT_TRUE(D2O);
    ASSERT_EQ(D2O->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(D2O->versionIndex(), 0);
    ASSERT_TRUE(HDO);
    ASSERT_EQ(HDO->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(HDO->versionIndex(), 0);
    ASSERT_EQ(H2Ox->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(H2Ox->versionIndex(), 0);

    // Get the raw GR
    auto rawGR = grNode->getOutputValue<PartialSet *>("RawGR");

    // Partial g(r) (unbound terms)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(rawGR->unboundPartials().get(DoubleKeyedMapKey("OW", "OW")),
                                                "OW-OW Unbound Partial", "epsr25/water1000-neutron/water.EPSR.g01", 1, 2,
                                                6.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(rawGR->unboundPartials().get(DoubleKeyedMapKey("HW", "OW")),
                                                "HW-OW Unbound Partial", "epsr25/water1000-neutron/water.EPSR.g01", 1, 4,
                                                2.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(rawGR->unboundPartials().get(DoubleKeyedMapKey("HW", "HW")),
                                                "HW-HW Unbound Partial", "epsr25/water1000-neutron/water.EPSR.g01", 1, 6,
                                                2.0e-2));

    // Partial g(r) (intramolecular terms)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(rawGR->boundPartials().get(DoubleKeyedMapKey("OW", "OW")),
                                                "OW-OW Bound Partial", "epsr25/water1000-neutron/water.EPSR.y01", 1, 2, 1.0e-5,
                                                Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(rawGR->boundPartials().get(DoubleKeyedMapKey("HW", "OW")),
                                                "HW-OW Bound Partial", "epsr25/water1000-neutron/water.EPSR.y01", 1, 4, 0.1));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(rawGR->boundPartials().get(DoubleKeyedMapKey("HW", "HW")),
                                                "HW-HW Bound Partial", "epsr25/water1000-neutron/water.EPSR.y01", 1, 6,
                                                1.5e-2));

    // Get the SQ
    auto unweightedSQ = sqNode->getOutputValue<PartialSet *>("UnweightedSQ");
    ASSERT_TRUE(unweightedSQ);

    // Partial S(Q) (unbound terms)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("OW", "OW")),
                                                "OW-OW Unbound Partial", "epsr25/water1000-neutron/water.EPSR.f01", 1, 2,
                                                1.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("OW", "HW")),
                                                "OW-HW Unbound Partial", "epsr25/water1000-neutron/water.EPSR.f01", 1, 4,
                                                3.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("HW", "HW")),
                                                "HW-HW Unbound Partial", "epsr25/water1000-neutron/water.EPSR.f01", 1, 6,
                                                3.0e-3));

    // Partial S(Q) (intramolecular terms)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("OW", "HW")),
                                                "OW-HW Bound Partial", "epsr25/water1000-neutron/water.EPSR.s01", 1, 4,
                                                1.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("HW", "HW")),
                                                "HW-HW Bound Partial", "epsr25/water1000-neutron/water.EPSR.s01", 1, 6,
                                                4.0e-5));

    // Partial S(Q) (intramolecular terms)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("OW", "OW")),
                                                "OW-OW Bound Partial", "epsr25/water1000-neutron/water.EPSR.s01", 1, 2, 1.0e-5,
                                                Error::ErrorType::RMSEError));

    // Total F(Q)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(D2O->getOutputValue<PartialSet *>("WeightedSQ")->total(), "D2O F(Q)",
                                                "epsr25/water1000-neutron/water.EPSR.u01", 1, 2, 3.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(H2O->getOutputValue<PartialSet *>("WeightedSQ")->total(), "H2O F(Q)",
                                                "epsr25/water1000-neutron/water.EPSR.u01", 1, 4, 6.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(HDO->getOutputValue<PartialSet *>("WeightedSQ")->total(), "HDO F(Q)",
                                                "epsr25/water1000-neutron/water.EPSR.u01", 1, 6, 2.0e-5));

    // Total F(Q)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(H2Ox->getOutputValue<PartialSet *>("WeightedSQ")->total(), "H2Ox F(Q)",
                                                "epsr25/water1000-neutron-xray/water.EPSR.u01", 1, 8, 1.5e-4));
}

} // namespace UnitTest
