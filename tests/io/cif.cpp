// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "io/import/cif.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class ImportCIFTest : public ::testing::Test
{
    public:
    // Molecular species information
    using MolecularSpeciesInfo = std::tuple<std::string, int, int>;
    // Test Box definition
    void testBox(const Configuration *cfg, const Vec3<double> &lengths, int nAtoms)
    {
        ASSERT_TRUE(cfg);
        EXPECT_EQ(cfg->nAtoms(), nAtoms);
        EXPECT_NEAR(cfg->box()->axisLengths().x, lengths.x, 1.0e-6);
        EXPECT_NEAR(cfg->box()->axisLengths().y, lengths.y, 1.0e-6);
        EXPECT_NEAR(cfg->box()->axisLengths().z, lengths.z, 1.0e-6);
    }
    // Test molecular species information provided
    void testMolecularSpecies(const CIFMolecularSpecies &molSp, const MolecularSpeciesInfo &info)
    {
        EXPECT_EQ(molSp.species()->name(), std::get<0>(info));
        EXPECT_EQ(molSp.instances().size(), std::get<1>(info));
        EXPECT_EQ(molSp.coordinates().size(), std::get<1>(info));
        EXPECT_EQ(molSp.species()->nAtoms(), std::get<2>(info));
    }
};
TEST_F(ImportCIFTest, Parse)
{
    // Test files
    auto cifPath = "cif/";
    std::vector<std::string> cifs = {"1517789.cif", "1557470.cif", "1557599.cif", "7705246.cif",
                                     "9000004.cif", "9000095.cif", "9000418.cif"};

    for (auto &cif : cifs)
    {
        CIFHandler cifHandler;
        EXPECT_TRUE(cifHandler.read(cifPath + cif));
    }
}

TEST_F(ImportCIFTest, NaCl)
{
    CIFHandler cifHandler;
    EXPECT_TRUE(cifHandler.read("cif/NaCl-1000041.cif"));
    EXPECT_TRUE(cifHandler.generate());

    // Check basic info
    EXPECT_EQ(cifHandler.spaceGroup(), SpaceGroups::SpaceGroup_225);
    auto *cfg = cifHandler.cleanedUnitCellConfiguration();
    constexpr double A = 5.62;
    testBox(cifHandler.cleanedUnitCellConfiguration(), {A, A, A}, 8);

    // Check molecular species
    EXPECT_EQ(cifHandler.molecularSpecies().size(), 2);
    testMolecularSpecies(cifHandler.molecularSpecies()[0], {"Na", 4, 1});
    std::vector<Vec3<double>> R = {{0.0, 0.0, 0.0}, {0.0, A / 2, A / 2}, {A / 2, 0.0, A / 2}, {A / 2, A / 2, 0.0}};
    for (auto &&[set, r2] : zip(cifHandler.molecularSpecies()[0].coordinates(), R))
        DissolveSystemTest::checkVec3(set[0], r2);
    testMolecularSpecies(cifHandler.molecularSpecies()[1], {"Cl", 4, 1});
    for (auto &&[set, r2] : zip(cifHandler.molecularSpecies()[1].coordinates(), R))
        DissolveSystemTest::checkVec3(set[0], (r2 - A / 2).abs());

    // 2x2x2 supercell
    cifHandler.setSupercellRepeat({2, 2, 2});
    EXPECT_TRUE(cifHandler.generate());
    testBox(cifHandler.supercellConfiguration(), {A * 2, A * 2, A * 2}, 8 * 8);
}

TEST_F(ImportCIFTest, NaClO3)
{
    CIFHandler cifHandler;
    EXPECT_TRUE(cifHandler.read("cif/NaClO3-1010057.cif"));
    EXPECT_TRUE(cifHandler.generate());

    // Check basic info
    EXPECT_EQ(cifHandler.spaceGroup(), SpaceGroups::SpaceGroup_198);
    constexpr double A = 6.55;
    testBox(cifHandler.structuralUnitCellConfiguration(), {A, A, A}, 20);

    // No geometry definitions present in the CIF file, so we expect species for each atomic component (4 Na, 4 Cl, and 12 O)
    auto &cifMols = cifHandler.molecularSpecies();
    ASSERT_EQ(cifMols.size(), 3);
    testMolecularSpecies(cifMols[0], {"Na", 4, 1});
    testMolecularSpecies(cifMols[1], {"Cl", 4, 1});
    testMolecularSpecies(cifMols[2], {"O", 12, 1});

    // Calculate bonding ourselves to get the correct species
    EXPECT_TRUE(cifHandler.generate(CIFHandler::UpdateFlags::CalculateBonding));
    ASSERT_EQ(cifMols.size(), 2);
    testMolecularSpecies(cifMols[0], {"Na", 4, 1});
    testMolecularSpecies(cifMols[1], {"ClO3", 4, 4});
}

} // namespace UnitTest
