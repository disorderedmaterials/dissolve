// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/add.h"
#include "procedure/nodes/box.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class ArtificialAtomsTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(ArtificialAtomsTest, Basic)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    // Set up species
    auto arType = coreData.addAtomType(Elements::Ar);
    auto artType = coreData.addAtomType(Elements::Art);
    auto *artAr = coreData.copySpecies(&tetrahedralArgonSpecies());
    artAr->atom(0).setAtomType(arType);
    for (auto n = 1; n < 5; ++n)
        artAr->atom(n).setAtomType(artType);

    // Create a configuration
    auto *cfg = coreData.addConfiguration();
    auto &procedure = cfg->generator();
    auto boxLength = 20.0;
    auto box = procedure.createRootNode<BoxProcedureNode>("Box", Vec3<NodeValue>(boxLength, boxLength, boxLength),
                                                          Vec3<NodeValue>(90, 90, 90));

    // Add in some artificial argon atoms
    const auto nMolecules = 100;
    procedure.createRootNode<AddProcedureNode>("ArtAr", artAr, nMolecules);

    // Set up the prior configuration
    cfg->generate({ProcessPool(), dissolve});

    // Basic checks
    EXPECT_EQ(artAr->nAtoms(), 5);
    EXPECT_EQ(artAr->nArtificialAtoms(), 4);
    EXPECT_EQ(cfg->nMolecules(), nMolecules);
    EXPECT_EQ(cfg->nAtoms(), nMolecules * artAr->nAtoms());
    EXPECT_EQ(cfg->nArtificialAtoms(), nMolecules * artAr->nArtificialAtoms());

    // Check density - should correspond to number density of non-artificial atoms only
    EXPECT_NEAR(*cfg->atomicDensity(), (nMolecules * (artAr->nAtoms() - artAr->nArtificialAtoms())) / cfg->box()->volume(),
                1.0e-5);
}

TEST_F(ArtificialAtomsTest, Water)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/water-with-artificial-atoms.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Partial g(r) (unbound terms)
    EXPECT_TRUE(systemTest.checkData1D(
        "GR01//Bulk//OriginalGR//OW-OW//Unbound",
        {"epsr25/water1000-neutron/water.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 6.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "GR01//Bulk//OriginalGR//OW-HW//Unbound",
        {"epsr25/water1000-neutron/water.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 2.0e-2));
    EXPECT_TRUE(systemTest.checkData1D(
        "GR01//Bulk//OriginalGR//HW-HW//Unbound",
        {"epsr25/water1000-neutron/water.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 2.0e-2));

    // Partial g(r) (intramolecular terms)
    EXPECT_TRUE(systemTest.checkData1D(
        "GR01//Bulk//OriginalGR//OW-HW//Bound",
        {"epsr25/water1000-neutron/water.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 0.1));
    EXPECT_TRUE(systemTest.checkData1D(
        "GR01//Bulk//OriginalGR//HW-HW//Bound",
        {"epsr25/water1000-neutron/water.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 1.5e-2));

    // Partial g(r) (intramolecular terms)
    EXPECT_TRUE(systemTest.checkData1D(
        "GR01//Bulk//OriginalGR//OW-OW//Bound",
        {"epsr25/water1000-neutron/water.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0e-5,
        Error::ErrorType::RMSEError));
}

} // namespace UnitTest
