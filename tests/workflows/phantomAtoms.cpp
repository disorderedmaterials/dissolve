// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/atomicMasses.h"
#include "generator/add.h"
#include "generator/box.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class PhantomAtomsTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(PhantomAtomsTest, Basic)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    // Set up species
    auto arType = coreData.addAtomType(Elements::Ar);
    auto artType = coreData.addAtomType(Elements::Phantom);
    auto *artAr = coreData.copySpecies(&tetrahedralArgonSpecies());
    artAr->atom(0).setAtomType(arType);
    for (auto n = 1; n < 5; ++n)
        artAr->atom(n).setAtomType(artType);

    // Create a configuration
    auto *cfg = coreData.addConfiguration();
    auto &procedure = cfg->generator();
    auto boxLength = 20.0;
    auto box = procedure.createRootNode<BoxGeneratorNode>("Box", Vec3<NodeValue>(boxLength, boxLength, boxLength),
                                                          Vec3<NodeValue>(90, 90, 90));

    // Add in some artificial argon atoms
    const auto nMolecules = 100;
    procedure.createRootNode<AddGeneratorNode>("ArtAr", artAr, nMolecules);

    // Set up the prior configuration
    cfg->generate({ProcessPool(), dissolve});

    // Basic species checks
    EXPECT_EQ(artAr->nAtoms(), 5);
    EXPECT_EQ(artAr->nAtoms(SpeciesAtom::Presence::Phantom), 4);
    EXPECT_DOUBLE_EQ(artAr->mass(), AtomicMass::mass(Elements::Ar));

    // Basic configuration checks
    EXPECT_EQ(cfg->nMolecules(), nMolecules);
    EXPECT_EQ(cfg->nAtoms(), nMolecules * artAr->nAtoms());
    EXPECT_EQ(cfg->nAtoms(SpeciesAtom::Presence::Phantom), nMolecules * artAr->nAtoms(SpeciesAtom::Presence::Phantom));

    // Check density - should correspond to number density of physical atoms only
    EXPECT_NEAR(*cfg->atomicDensity(), (nMolecules * artAr->nAtoms(SpeciesAtom::Presence::Physical)) / cfg->box()->volume(),
                1.0e-5);
}

TEST_F(PhantomAtomsTest, Water)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/water-with-phantom-atoms.txt"));
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

    // Partial S(Q) (unbound terms)
    EXPECT_TRUE(systemTest.checkData1D(
        "SQ01//UnweightedSQ//OW-OW//Unbound",
        {"epsr25/water1000-neutron/water.EPSR.f01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQ01//UnweightedSQ//OW-HW//Unbound",
        {"epsr25/water1000-neutron/water.EPSR.f01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 3.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQ01//UnweightedSQ//HW-HW//Unbound",
        {"epsr25/water1000-neutron/water.EPSR.f01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 3.0e-3));

    // Partial S(Q) (intramolecular terms)
    EXPECT_TRUE(systemTest.checkData1D(
        "SQ01//UnweightedSQ//OW-HW//Bound",
        {"epsr25/water1000-neutron/water.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 1.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "SQ01//UnweightedSQ//HW-HW//Bound",
        {"epsr25/water1000-neutron/water.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 4.0e-5));

    // Partial S(Q) (intramolecular terms)
    EXPECT_TRUE(systemTest.checkData1D(
        "SQ01//UnweightedSQ//OW-OW//Bound",
        {"epsr25/water1000-neutron/water.EPSR.s01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0e-5,
        Error::ErrorType::RMSEError));

    // Total F(Q)
    EXPECT_TRUE(systemTest.checkData1D(
        "D2O//WeightedSQ//Total",
        {"epsr25/water1000-neutron/water.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 3.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "H2O//WeightedSQ//Total",
        {"epsr25/water1000-neutron/water.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 6.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "HDO//WeightedSQ//Total",
        {"epsr25/water1000-neutron/water.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 2.0e-5));
}

} // namespace UnitTest
