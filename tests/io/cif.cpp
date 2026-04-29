// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/empiricalFormula.h"
#include "io/import/species.h"
#include "nodes/cif/io/cifContext.h"
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
    void testBox(const Configuration *cfg, const Vector3 &lengths, const Vector3 &angles, int nAtoms)
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
    // Check instance consistency with reference coordinates
    void testInstanceConsistency(const CIFMolecularSpecies &molSp, const Species &referenceCoordinates)
    {
        // Get the box from the reference species
        const auto *box = referenceCoordinates.box();

        // Loop over instances and ensure their stored atoms overlap exactly with one in the reference system
        for (const auto &instance : molSp.instances())
        {
            for (auto &&[instanceAtom, speciesAtom] : zip(instance.localAtoms(), molSp.species()->atoms()))
            {
                // Locate the atom in the reference system at the instance atom coordinates
                auto instanceR = instanceAtom.r();
                auto spAtomIt = std::find_if(referenceCoordinates.atoms().begin(), referenceCoordinates.atoms().end(),
                                             [box, instanceR](const auto &refAtom)
                                             { return box->minimumDistance(refAtom.r(), instanceR) < 0.01; });
                std::cout << std::format("{}  {} {} {}", Elements::symbol(speciesAtom.Z()), instanceAtom.r().x,
                                         instanceAtom.r().y, instanceAtom.r().z)
                          << std::endl;
                ASSERT_NE(spAtomIt, referenceCoordinates.atoms().end());
                EXPECT_EQ(spAtomIt->Z(), speciesAtom.Z());
            }
        }
    }
};

TEST_F(ImportCIFTest, Parse)
{
    // Test files
    auto cifPath = "cif/";
    std::vector<std::string> cifs = {"1557470.cif", "1557599.cif", "7705246.cif", "9000004.cif", "9000095.cif", "9000418.cif"};

    for (auto &cif : cifs)
    {
        CIFContext cifContext;
        ASSERT_TRUE(cifContext.read(cifPath + cif));
    }
}

TEST_F(ImportCIFTest, NaCl)
{
    CIFContext cifContext;
    ASSERT_TRUE(cifContext.read("cif/NaCl-1000041.cif"));
    EXPECT_TRUE(cifContext.generate());

    // Check basic info
    EXPECT_EQ(cifContext.spaceGroup(), SpaceGroups::SpaceGroup_225);
    constexpr double A = 5.62;
    testBox(cifContext.generatedConfiguration(), {A, A, A}, {90, 90, 90}, 8);

    // Calculating bonding is the default, but this gives a continuous framework...
    EXPECT_EQ(cifContext.molecularSpecies().size(), 0);

    // Get molecular species
    cifContext.setUseCIFBondingDefinitions(true);
    EXPECT_EQ(cifContext.molecularSpecies().size(), 2);
    testMolecularSpecies(cifContext.molecularSpecies()[0], {"Na", 4, 1});
    std::vector<Vector3> R = {{0.0, 0.0, 0.0}, {0.0, A / 2, A / 2}, {A / 2, 0.0, A / 2}, {A / 2, A / 2, 0.0}};
    for (auto &&[instance, r2] : zip(cifContext.molecularSpecies()[0].instances(), R))
        DissolveSystemTest::checkVec3(instance.localAtoms()[0].r(), r2);
    testMolecularSpecies(cifContext.molecularSpecies()[1], {"Cl", 4, 1});
    for (auto &&[instance, r2] : zip(cifContext.molecularSpecies()[1].instances(), R))
        DissolveSystemTest::checkVec3(instance.localAtoms()[0].r(), (r2 - A / 2).abs());

    // 2x2x2 supercell
    cifContext.setSupercellRepeat({2, 2, 2});
    EXPECT_TRUE(cifContext.generate());
    testBox(cifContext.generatedConfiguration(), {A * 2, A * 2, A * 2}, {90, 90, 90}, 8 * 8);
}

TEST_F(ImportCIFTest, NaClO3)
{
    CIFContext cifContext;
    ASSERT_TRUE(cifContext.read("cif/NaClO3-1010057.cif"));
    EXPECT_TRUE(cifContext.generate());

    // Check basic info
    EXPECT_EQ(cifContext.spaceGroup(), SpaceGroups::SpaceGroup_198);
    constexpr double A = 6.55;
    testBox(cifContext.generatedConfiguration(), {A, A, A}, {90, 90, 90}, 20);

    // Turn off automatic bond calculation - there are no bonding defs in the CIF, so we expect species for each atomic
    // component (4 Na, 4 Cl, and 12 O)
    cifContext.setUseCIFBondingDefinitions(true);
    auto &cifMols = cifContext.molecularSpecies();
    ASSERT_EQ(cifMols.size(), 3);
    testMolecularSpecies(cifMols[0], {"Na", 4, 1});
    testMolecularSpecies(cifMols[1], {"Cl", 4, 1});
    testMolecularSpecies(cifMols[2], {"O", 12, 1});

    // Calculate bonding ourselves to get the correct species
    cifContext.setUseCIFBondingDefinitions(false);
    ASSERT_EQ(cifMols.size(), 2);
    testMolecularSpecies(cifMols[0], {"Na", 4, 1});
    testMolecularSpecies(cifMols[1], {"ClO3", 4, 4});
}

TEST_F(ImportCIFTest, CuBTC)
{
    CIFContext cifContext;
    ASSERT_TRUE(cifContext.read("cif/CuBTC-7108574.cif"));
    EXPECT_TRUE(cifContext.generate());

    // Check basic info
    EXPECT_EQ(cifContext.spaceGroup(), SpaceGroups::SpaceGroup_225);
    constexpr auto A = 26.3336;
    testBox(cifContext.generatedConfiguration(), {A, A, A}, {90, 90, 90}, 672);

    // 16 basic formula units per unit cell
    constexpr auto N = 16;

    // Check basic formula (which includes bound water oxygens - with no H - at this point) and using O group
    EmpiricalFormula::EmpiricalFormulaMap cellFormulaH = {
        {Elements::Cu, 3 * N}, {Elements::C, 18 * N}, {Elements::H, 6 * N}, {Elements::O, 15 * N}};
    EXPECT_EQ(EmpiricalFormula::formula(cifContext.generatedConfiguration()->atoms(),
                                        [](const auto &i) { return i.speciesAtom()->Z(); }),
              EmpiricalFormula::formula(cellFormulaH));
    EXPECT_EQ(cifContext.molecularSpecies().size(), 2);

    // Change active assemblies to get amine-substituted structure
    EmpiricalFormula::EmpiricalFormulaMap cellFormulaNH2 = cellFormulaH;
    cellFormulaNH2[Elements::N] = 6 * N;
    cellFormulaNH2[Elements::H] *= 2;
    cifContext.getAssembly("A").getGroup("1").setActive(false);
    cifContext.getAssembly("B").getGroup("2").setActive(true);
    cifContext.getAssembly("C").getGroup("2").setActive(true);
    EXPECT_TRUE(cifContext.generate());
    EXPECT_EQ(EmpiricalFormula::formula(cifContext.generatedConfiguration()->atoms(),
                                        [](const auto &i) { return i.speciesAtom()->Z(); }),
              EmpiricalFormula::formula(cellFormulaNH2));

    // Remove those free oxygens so we just have a framework
    cifContext.setRemoveAtomics(true);
    EXPECT_EQ(cifContext.molecularSpecies().size(), 0);
}

TEST_F(ImportCIFTest, MoleculeOrdering)
{
    CIFContext cifContext;
    const auto cifFiles = {"cif/molecule-test-simple-ordered.cif", "cif/molecule-test-simple-unordered.cif",
                           "cif/molecule-test-simple-unordered-rotated.cif"};
    for (auto cifFile : cifFiles)
    {
        // Load the CIF file
        ASSERT_TRUE(cifContext.read(cifFile));
        EXPECT_TRUE(cifContext.generate());

        EXPECT_EQ(cifContext.molecularSpecies().size(), 1);

        auto &cifMolecule = cifContext.molecularSpecies().front();
        EmpiricalFormula::EmpiricalFormulaMap moleculeFormula = {
            {Elements::Cl, 1}, {Elements::O, 1}, {Elements::C, 1}, {Elements::H, 3}};
        testMolecularSpecies(cifMolecule, {EmpiricalFormula::formula(moleculeFormula), 6, 6});

        testInstanceConsistency(cifMolecule, cifContext.cleanedUnitCellSpecies());
    }
}

TEST_F(ImportCIFTest, BigMoleculeOrdering)
{
    CIFContext cifContext;
    const auto cifFile = "cif/Bisphen_n_arenes_1517789.cif";

    // Load the CIF file
    ASSERT_TRUE(cifContext.read(cifFile));
    EXPECT_TRUE(cifContext.generate());

    EXPECT_EQ(cifContext.molecularSpecies().size(), 1);

    auto &cifMolecule = cifContext.molecularSpecies().front();
    EmpiricalFormula::EmpiricalFormulaMap moleculeFormula = {{Elements::O, 6}, {Elements::C, 51}, {Elements::H, 54}};
    testMolecularSpecies(cifMolecule, {EmpiricalFormula::formula(moleculeFormula), 4, 111});

    testInstanceConsistency(cifMolecule, cifContext.cleanedUnitCellSpecies());
}

} // namespace UnitTest
