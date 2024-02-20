// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/import/cif.h"
#include "classes/empiricalFormula.h"
#include "io/import/species.h"
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
    // Check instance consistency with reference coordinates
    void testInstanceConsistency(const CIFMolecularSpecies &molSp, const Species &referenceCoordinates)
    {
        // Loop over instances and ensure their stored atoms overlap exactly with one in the reference system
        for (const auto &instance : molSp.instances())
        {
            for (auto &&[instanceAtom, speciesAtom] : zip(instance.localAtoms(), molSp.species()->atoms()))
            {
                // Locate the atom in the reference system at the instance atom coordinates
                auto instanceR = instanceAtom.r();
                auto spAtomIt =
                    std::find_if(referenceCoordinates.atoms().begin(), referenceCoordinates.atoms().end(),
                                 [instanceR](const auto &refAtom) { return (refAtom.r() - instanceR).magnitude() < 0.01; });
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
    testBox(cifHandler.generatedConfiguration(), {A, A, A}, {90, 90, 90}, 8);

    // Calculating bonding is the default, but this gives a continuous framework...
    EXPECT_EQ(cifHandler.molecularSpecies().size(), 0);

    // Get molecular species
    cifHandler.setUseCIFBondingDefinitions(true);
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
    testBox(cifHandler.generatedConfiguration(), {A * 2, A * 2, A * 2}, {90, 90, 90}, 8 * 8);
}

TEST_F(ImportCIFTest, NaClO3)
{
    CIFHandler cifHandler;
    ASSERT_TRUE(cifHandler.read("cif/NaClO3-1010057.cif"));
    EXPECT_TRUE(cifHandler.generate());

    // Check basic info
    EXPECT_EQ(cifHandler.spaceGroup(), SpaceGroups::SpaceGroup_198);
    constexpr double A = 6.55;
    testBox(cifHandler.generatedConfiguration(), {A, A, A}, {90, 90, 90}, 20);

    // Turn off automatic bond calculation - there are no bonding defs in the CIF, so we expect species for each atomic
    // component (4 Na, 4 Cl, and 12 O)
    cifHandler.setUseCIFBondingDefinitions(true);
    auto &cifMols = cifHandler.molecularSpecies();
    ASSERT_EQ(cifMols.size(), 3);
    testMolecularSpecies(cifMols[0], {"Na", 4, 1});
    testMolecularSpecies(cifMols[1], {"Cl", 4, 1});
    testMolecularSpecies(cifMols[2], {"O", 12, 1});

    // Calculate bonding ourselves to get the correct species
    cifHandler.setUseCIFBondingDefinitions(false);
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
    testBox(cifHandler.generatedConfiguration(), {A, A, A}, {90, 90, 90}, 672);

    // 16 basic formula units per unit cell
    constexpr auto N = 16;

    // Check basic formula (which includes bound water oxygens - with no H - at this point) and using O group
    EmpiricalFormula::EmpiricalFormulaMap cellFormulaH = {
        {Elements::Cu, 3 * N}, {Elements::C, 18 * N}, {Elements::H, 6 * N}, {Elements::O, 15 * N}};
    EXPECT_EQ(EmpiricalFormula::formula(cifHandler.generatedConfiguration()->atoms(),
                                        [](const auto &i) { return i.speciesAtom()->Z(); }),
              EmpiricalFormula::formula(cellFormulaH));
    EXPECT_EQ(cifHandler.molecularSpecies().size(), 2);

    // Change active assemblies to get amine-substituted structure
    EmpiricalFormula::EmpiricalFormulaMap cellFormulaNH2 = cellFormulaH;
    cellFormulaNH2[Elements::N] = 6 * N;
    cellFormulaNH2[Elements::H] *= 2;
    cifHandler.getAssembly("A").getGroup("1").setActive(false);
    cifHandler.getAssembly("B").getGroup("2").setActive(true);
    cifHandler.getAssembly("C").getGroup("2").setActive(true);
    EXPECT_TRUE(cifHandler.generate());
    EXPECT_EQ(EmpiricalFormula::formula(cifHandler.generatedConfiguration()->atoms(),
                                        [](const auto &i) { return i.speciesAtom()->Z(); }),
              EmpiricalFormula::formula(cellFormulaNH2));

    // Remove those free oxygens so we just have a framework
    cifHandler.setRemoveAtomics(true);
    EXPECT_EQ(cifHandler.molecularSpecies().size(), 0);
}

TEST_F(ImportCIFTest, MoleculesOrdered)
{
    CIFHandler cifHandler;
    ASSERT_TRUE(cifHandler.read("cif/molecule-test-simple-ordered.cif"));
    EXPECT_TRUE(cifHandler.generate());

    EXPECT_EQ(cifHandler.molecularSpecies().size(), 1);

    auto &cifMolecule = cifHandler.molecularSpecies().front();
    EmpiricalFormula::EmpiricalFormulaMap moleculeFormula = {
        {Elements::Cl, 1}, {Elements::O, 1}, {Elements::C, 1}, {Elements::H, 3}};
    testMolecularSpecies(cifMolecule, {EmpiricalFormula::formula(moleculeFormula), 6, 6});

    // Load the reference coordinates into a Species
    Species referenceCoordinates;
    SpeciesImportFileFormat speciesImportFileFormat("cif/molecule-test-simple-ordered.xyz");
    ASSERT_TRUE(speciesImportFileFormat.importData(&referenceCoordinates));
    ASSERT_EQ(referenceCoordinates.nAtoms(), 36);
    testInstanceConsistency(cifMolecule, referenceCoordinates);
}

TEST_F(ImportCIFTest, MoleculesUnrdered)
{
    CIFHandler cifHandler;
    ASSERT_TRUE(cifHandler.read("cif/molecule-test-simple-unordered.cif"));
    EXPECT_TRUE(cifHandler.generate());

    EXPECT_EQ(cifHandler.molecularSpecies().size(), 1);

    auto &cifMolecule = cifHandler.molecularSpecies().front();
    EmpiricalFormula::EmpiricalFormulaMap moleculeFormula = {
        {Elements::Cl, 1}, {Elements::O, 1}, {Elements::C, 1}, {Elements::H, 3}};
    testMolecularSpecies(cifMolecule, {EmpiricalFormula::formula(moleculeFormula), 6, 6});

    // Load the reference coordinates into a Species
    Species referenceCoordinates;
    SpeciesImportFileFormat speciesImportFileFormat("cif/molecule-test-simple-ordered.xyz");
    ASSERT_TRUE(speciesImportFileFormat.importData(&referenceCoordinates));
    ASSERT_EQ(referenceCoordinates.nAtoms(), 36);
    testInstanceConsistency(cifMolecule, referenceCoordinates);
}
} // namespace UnitTest
