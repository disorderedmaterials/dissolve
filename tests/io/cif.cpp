// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "io/import/cif.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(ImportCIFTest, Parse)
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

TEST(ImportCIFTest, NaCl)
{
    CIFHandler cifHandler;
    EXPECT_TRUE(cifHandler.read("cif/NaCl-1000041.cif"));
    EXPECT_TRUE(cifHandler.generate());

    EXPECT_EQ(cifHandler.spaceGroup(), SpaceGroups::SpaceGroup_225);

    // Check basic configuration
    auto *cfg = cifHandler.cleanedUnitCellConfiguration();
    constexpr double A = 5.62;
    ASSERT_TRUE(cfg);
    EXPECT_EQ(cfg->nAtoms(), 8);
    EXPECT_NEAR(cfg->box()->axisLengths().x, A, 1.0e-6);
    EXPECT_NEAR(cfg->box()->axisLengths().y, A, 1.0e-6);
    EXPECT_NEAR(cfg->box()->axisLengths().z, A, 1.0e-6);
    EXPECT_EQ(cifHandler.molecularSpecies().size(), 2);

    // Check molecular species
    std::vector<Vec3<double>> R = {{0.0, 0.0, 0.0}, {0.0, A / 2, A / 2}, {A / 2, 0.0, A / 2}, {A / 2, A / 2, 0.0}};
    auto &speciesNa = cifHandler.molecularSpecies()[0];
    EXPECT_EQ(speciesNa.instances().size(), 4);
    EXPECT_EQ(speciesNa.coordinates().size(), 4);
    for (auto &&[set, r2] : zip(speciesNa.coordinates(), R))
        DissolveSystemTest::checkVec3(set[0], r2);
    auto &speciesCl = cifHandler.molecularSpecies()[1];
    EXPECT_EQ(speciesCl.instances().size(), 4);
    EXPECT_EQ(speciesCl.coordinates().size(), 4);
    for (auto &&[set, r2] : zip(speciesCl.coordinates(), R))
        DissolveSystemTest::checkVec3(set[0], (r2 - A / 2).abs());

    // 2x2x2 supercell
    cifHandler.setSupercellRepeat({2, 2, 2});
    EXPECT_TRUE(cifHandler.generate());
    auto *superCell = cifHandler.supercellConfiguration();
    ASSERT_TRUE(superCell);
    EXPECT_EQ(superCell->nAtoms(), 8 * 8);
    EXPECT_NEAR(superCell->box()->axisLengths().x, A * 2, 1.0e-6);
    EXPECT_NEAR(superCell->box()->axisLengths().y, A * 2, 1.0e-6);
    EXPECT_NEAR(superCell->box()->axisLengths().z, A * 2, 1.0e-6);
    EXPECT_EQ(cifHandler.molecularSpecies().size(), 2);
}

TEST(ImportCIFTest, NaClO3)
{
    CIFHandler cifHandler;
    EXPECT_TRUE(cifHandler.read("cif/NaClO3-1010057.cif"));
    EXPECT_TRUE(cifHandler.generate());

    // Check basic info
    EXPECT_EQ(cifHandler.spaceGroup(), SpaceGroups::SpaceGroup_198);
    auto *cfg = cifHandler.structuralUnitCellConfiguration();
    constexpr double A = 6.55;
    ASSERT_TRUE(cfg);
    EXPECT_EQ(cfg->nAtoms(), 20);
    EXPECT_NEAR(cfg->box()->axisLengths().x, A, 1.0e-6);
    EXPECT_NEAR(cfg->box()->axisLengths().y, A, 1.0e-6);
    EXPECT_NEAR(cfg->box()->axisLengths().z, A, 1.0e-6);

    //
    EXPECT_EQ(1, 0);
}

} // namespace UnitTest
