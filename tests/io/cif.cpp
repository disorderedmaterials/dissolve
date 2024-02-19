// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/import/cif.h"
#include "classes/empiricalFormula.h"
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
    void testBox(const Configuration *cfg, const Vec3<double> &lengths, const Vec3<double> &angles, int nAtoms)
    {
        ASSERT_TRUE(cfg);
        EXPECT_EQ(cfg->nAtoms(), nAtoms);
        EXPECT_NEAR(cfg->box()->axisLengths().x, lengths.x, 1.0e-6);
        EXPECT_NEAR(cfg->box()->axisLengths().y, lengths.y, 1.0e-6);
        EXPECT_NEAR(cfg->box()->axisLengths().z, lengths.z, 1.0e-6);
        EXPECT_NEAR(cfg->box()->axisAngles().x, angles.x, 1.0e-6);
        EXPECT_NEAR(cfg->box()->axisAngles().y, angles.y, 1.0e-6);
        EXPECT_NEAR(cfg->box()->axisAngles().z, angles.z, 1.0e-6);
    }
    // Test molecular species information provided
    void testMolecularSpecies(const CIFMolecularSpecies &molSp, const MolecularSpeciesInfo &info)
    {
        EXPECT_EQ(molSp.species()->name(), std::get<0>(info));
        EXPECT_EQ(molSp.instances().size(), std::get<1>(info));
        EXPECT_EQ(molSp.species()->nAtoms(), std::get<2>(info));
    }
};

TEST_F(ImportCIFTest, Parse)
{
    // Test files
    auto cifPath = "cif/";
    std::vector<std::string> cifs = {"1557470.cif", "1557599.cif", "7705246.cif", "9000004.cif", "9000095.cif", "9000418.cif"};

    for (auto &cif : cifs)
    {
        CIFHandler cifHandler;
        ASSERT_TRUE(cifHandler.read(cifPath + cif));
    }
}

TEST_F(ImportCIFTest, NaCl)
{
    CIFHandler cifHandler;
    ASSERT_TRUE(cifHandler.read("cif/NaCl-1000041.cif"));
    EXPECT_TRUE(cifHandler.generate());

    // Check basic info
    EXPECT_EQ(cifHandler.spaceGroup(), SpaceGroups::SpaceGroup_225);
    constexpr double A = 5.62;
    testBox(cifHandler.cleanedUnitCellConfiguration(), {A, A, A}, {90, 90, 90}, 8);

    // Check molecular species
    EXPECT_EQ(cifHandler.molecularSpecies().size(), 2);
    testMolecularSpecies(cifHandler.molecularSpecies()[0], {"Na", 4, 1});
    std::vector<Vec3<double>> R = {{0.0, 0.0, 0.0}, {0.0, A / 2, A / 2}, {A / 2, 0.0, A / 2}, {A / 2, A / 2, 0.0}};
    for (auto &&[instance, r2] : zip(cifHandler.molecularSpecies()[0].instances(), R))
        DissolveSystemTest::checkVec3(instance.localAtoms()[0].r(), r2);
    testMolecularSpecies(cifHandler.molecularSpecies()[1], {"Cl", 4, 1});
    for (auto &&[instance, r2] : zip(cifHandler.molecularSpecies()[1].instances(), R))
        DissolveSystemTest::checkVec3(instance.localAtoms()[0].r(), (r2 - A / 2).abs());

    // 2x2x2 supercell
    cifHandler.setSupercellRepeat({2, 2, 2});
    EXPECT_TRUE(cifHandler.generate());
    testBox(cifHandler.supercellConfiguration(), {A * 2, A * 2, A * 2}, {90, 90, 90}, 8 * 8);
}

TEST_F(ImportCIFTest, NaClO3)
{
    CIFHandler cifHandler;
    ASSERT_TRUE(cifHandler.read("cif/NaClO3-1010057.cif"));
    EXPECT_TRUE(cifHandler.generate());

    // Check basic info
    EXPECT_EQ(cifHandler.spaceGroup(), SpaceGroups::SpaceGroup_198);
    constexpr double A = 6.55;
    testBox(cifHandler.structuralUnitCellConfiguration(), {A, A, A}, {90, 90, 90}, 20);

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

TEST_F(ImportCIFTest, CuBTC)
{
    CIFHandler cifHandler;
    ASSERT_TRUE(cifHandler.read("cif/CuBTC-7108574.cif"));
    EXPECT_TRUE(cifHandler.generate());

    // Check basic info
    EXPECT_EQ(cifHandler.spaceGroup(), SpaceGroups::SpaceGroup_225);
    constexpr auto A = 26.3336;
    testBox(cifHandler.structuralUnitCellConfiguration(), {A, A, A}, {90, 90, 90}, 672);

    // 16 basic formula units per unit cell
    constexpr auto N = 16;

    // Check basic formula (which includes bound water oxygens - with no H - at this point) and using O group
    EmpiricalFormula::EmpiricalFormulaMap cellFormulaH = {
        {Elements::Cu, 3 * N}, {Elements::C, 18 * N}, {Elements::H, 6 * N}, {Elements::O, 15 * N}};
    EXPECT_EQ(EmpiricalFormula::formula(cifHandler.cleanedUnitCellSpecies()->atoms(), [](const auto &i) { return i.Z(); }),
              EmpiricalFormula::formula(cellFormulaH));
    EXPECT_EQ(cifHandler.molecularSpecies().size(), 0);

    // Change active assemblies to get amine-substituted structure
    EmpiricalFormula::EmpiricalFormulaMap cellFormulaNH2 = cellFormulaH;
    cellFormulaNH2[Elements::N] = 6 * N;
    cellFormulaNH2[Elements::H] *= 2;
    cifHandler.getAssembly("A").getGroup("1").setActive(false);
    cifHandler.getAssembly("B").getGroup("2").setActive(true);
    cifHandler.getAssembly("C").getGroup("2").setActive(true);
    EXPECT_TRUE(cifHandler.generate());
    EXPECT_EQ(EmpiricalFormula::formula(cifHandler.cleanedUnitCellSpecies()->atoms(), [](const auto &i) { return i.Z(); }),
              EmpiricalFormula::formula(cellFormulaNH2));
}
} // namespace UnitTest
