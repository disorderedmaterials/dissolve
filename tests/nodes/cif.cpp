// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/empiricalFormula.h"
#include "nodes/calculateBonding.h"
#include "nodes/cif/importCIFStructure.h"
#include "nodes/detectMolecules.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <string>

namespace UnitTest
{
class CIFNodeTest : public ::testing::Test
{
    public:
    CIFNodeTest() = default;
    ~CIFNodeTest() = default;

    public:
    // Molecular species information
    using MolecularSpeciesInfo = std::tuple<std::string, int, int>;
    // Test Box definition
    void testBox(const Configuration *cfg, const Vector3 &lengths, const Vector3 &angles, int nAtoms)
    {
        ASSERT_TRUE(cfg);
        EXPECT_EQ(cfg->nAtoms(), nAtoms);
        EXPECT_NEAR(cfg->box().axisLengths().x, lengths.x, 1.0e-6);
        EXPECT_NEAR(cfg->box().axisLengths().y, lengths.y, 1.0e-6);
        EXPECT_NEAR(cfg->box().axisLengths().z, lengths.z, 1.0e-6);
        EXPECT_NEAR(cfg->box().axisAngles().x, angles.x, 1.0e-6);
        EXPECT_NEAR(cfg->box().axisAngles().y, angles.y, 1.0e-6);
        EXPECT_NEAR(cfg->box().axisAngles().z, angles.z, 1.0e-6);
    }
    // Test molecular species information provided
    void testDetectedMolecularStructure(const Structure &structure, const MolecularSpeciesInfo &info)
    {
        // EXPECT_EQ(structure.name(), std::get<0>(info));
        EXPECT_EQ(structure.instances().size(), std::get<1>(info));
        EXPECT_EQ(structure.nAtoms(), std::get<2>(info));
    }
    /*
    // Check instance consistency with reference coordinates
    void testInstanceConsistency(const CIFMolecularSpecies &molSp, const Species &referenceCoordinates)
    {
        // Get the box from the reference species
        const auto &box = referenceCoordinates.box();

        // Loop over instances and ensure their stored atoms overlap exactly with one in the reference system
        for (const auto &instance : molSp.instances())
        {
            for (auto &&[instanceAtom, speciesAtom] : zip(instance.localAtoms(), molSp.species()->atoms()))
            {
                // Locate the atom in the reference system at the instance atom coordinates
                auto instanceR = instanceAtom.r();
                auto spAtomIt = std::find_if(referenceCoordinates.atoms().begin(), referenceCoordinates.atoms().end(),
                                             [box, instanceR](const auto &refAtom)
                                             { return box.minimumDistance(refAtom.r(), instanceR) < 0.01; });
                std::cout << std::format("{}  {} {} {}", Elements::symbol(speciesAtom.Z()), instanceAtom.r().x,
                                         instanceAtom.r().y, instanceAtom.r().z)
                          << std::endl;
                ASSERT_NE(spAtomIt, referenceCoordinates.atoms().end());
                EXPECT_EQ(spAtomIt->Z(), speciesAtom.Z());
            }
        }
    }
    */
};

TEST_F(CIFNodeTest, Parse)
{
    TestGraph testGraph;

    // Test files with expected number of structure atoms
    std::vector<std::pair<std::string, int>> cifs = {{"1557470.cif", 86}, {"1557599.cif", 56}, {"7705246.cif", 364},
                                                     {"9000004.cif", 6},  {"9000095.cif", 30}, {"9000418.cif", 64}};

    for (auto &[cif, nStructureAtoms] : cifs)
    {
        ASSERT_TRUE(testGraph.appendNode("ImportCIFStructure", cif));
        testGraph.fetchHead()->setOption("FilePath", "cif/" + cif);
        ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
        const auto structure = testGraph.fetchHead()->getOutputValue<Structure>("Structure");
        ASSERT_EQ(structure.atoms().size(), nStructureAtoms);
    }
}
TEST_F(CIFNodeTest, NaClContinuous)
{
    TestGraph testGraph;

    // Load the CIF file
    auto cif = std::string("NaCl-1000041.cif");

    EXPECT_TRUE(testGraph.appendNode("ImportCIFStructure"));
    testGraph.fetchHead()->setOption("FilePath", "cif/" + cif);
    EXPECT_TRUE(testGraph.appendNode("CalculateBonding"));
    ASSERT_TRUE(testGraph.appendNode("DetectMolecules"));
    testGraph.addEdge({"ImportCIFStructure", "Structure", "CalculateBonding", "Structure"});
    testGraph.addEdge({"CalculateBonding", "Structure", "DetectMolecules", "Structure"});

    // Basic info
    ASSERT_EQ(testGraph.findNode("CalculateBonding")->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(testGraph.findNode("ImportCIFStructure")->findOption("SpaceGroupID")->get<SpaceGroups::SpaceGroupId>(),
              SpaceGroups::SpaceGroup_225);

    // constexpr double A = 5.62;

    // We should find a continuous framework after rebonding and the detect molecules node should fail accordingly
    ASSERT_EQ(testGraph.findNode("DetectMolecules")->run(), NodeConstants::ProcessResult::Failed);
}

TEST_F(CIFNodeTest, NaClMolecules)
{
    TestGraph testGraph;

    // Load the CIF file
    auto cif = std::string("NaCl-1000041.cif");

    EXPECT_TRUE(testGraph.appendNode("ImportCIFStructure"));
    testGraph.fetchHead()->setOption("FilePath", "cif/" + cif);
    ASSERT_TRUE(testGraph.appendNode("DetectMolecules"));
    testGraph.addEdge({"ImportCIFStructure", "Structure", "DetectMolecules", "Structure"});

    auto detectMoleculesNode = static_cast<DetectMoleculesNode *>(testGraph.findNode("DetectMolecules"));
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);

    // Basic info
    EXPECT_EQ(testGraph.findNode("ImportCIFStructure")->findOption("SpaceGroupID")->get<SpaceGroups::SpaceGroupId>(),
              SpaceGroups::SpaceGroup_225);
    constexpr double A = 5.62;

    EXPECT_EQ(detectMoleculesNode->detectedStructures().size(), 2);
    testDetectedMolecularStructure(detectMoleculesNode->detectedStructures().at(0), {"Na", 4, 1});
    std::vector<Vector3> R = {{0.0, 0.0, 0.0}, {0.0, A / 2, A / 2}, {A / 2, 0.0, A / 2}, {A / 2, A / 2, 0.0}};
    for (auto &&[instance, r2] : zip(detectMoleculesNode->detectedStructures().at(0).instances(), R))
        DissolveSystemTest::checkVec3(instance[0], r2);
    testDetectedMolecularStructure(detectMoleculesNode->detectedStructures().at(1), {"Cl", 4, 1});
    for (auto &&[instance, r2] : zip(detectMoleculesNode->detectedStructures().at(1).instances(), R))
        DissolveSystemTest::checkVec3(instance[0], (r2 - A / 2).abs());

    // 2x2x2 supercell
    /* TODO: Handle supercell configurations
    detectMoleculesNode->setOption<Vector3i>("SupercellRepeat", {2, 2, 2});
    testGraph.dissolveGraph()->setUpdateRequired();
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);
    testBox(detectMoleculesNode->getOutputValue<Configuration *>("SupercellConfiguration"), {A * 2, A * 2, A * 2},
            {90, 90, 90}, 8 * 8);
    */
}

TEST_F(CIFNodeTest, NaClO3)
{
    TestGraph testGraph;

    // Load the CIF file
    auto cif = std::string("NaClO3-1010057.cif");

    EXPECT_TRUE(testGraph.appendNode("ImportCIFStructure"));
    testGraph.fetchHead()->setOption("FilePath", "cif/" + cif);
    ASSERT_TRUE(testGraph.appendNode("DetectMolecules"));
    testGraph.addEdge({"ImportCIFStructure", "Structure", "DetectMolecules", "Structure"});

    ASSERT_EQ(testGraph.findNode("ImportCIFStructure")->run(), NodeConstants::ProcessResult::Success);

    // Check basic info
    auto detectMoleculesNode = static_cast<DetectMoleculesNode *>(testGraph.findNode("DetectMolecules"));

    EXPECT_EQ(testGraph.findNode("ImportCIFStructure")->findOption("SpaceGroupID")->get<SpaceGroups::SpaceGroupId>(),
              SpaceGroups::SpaceGroup_198);
    constexpr double A = 6.55;
    // TODO: Handle supercell configurations
    //  testBox(detectMoleculesNode->getOutputValue<Configuration *>("SupercellConfiguration"), {A, A, A}, {90, 90, 90}, 20);

    // No bonding defs in the CIF, so we expect species for each atomic
    // component (4 Na, 4 Cl, and 12 O)
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);

    auto detectedMoleculeStructureA = detectMoleculesNode->detectedStructures();
    testDetectedMolecularStructure(detectedMoleculeStructureA.at(0), {"Na", 4, 1});
    testDetectedMolecularStructure(detectedMoleculeStructureA.at(1), {"Cl", 4, 1});
    testDetectedMolecularStructure(detectedMoleculeStructureA.at(2), {"O", 12, 1});

    // Calculate bonding ourselves to get the correct species
    EXPECT_TRUE(testGraph.appendNode("CalculateBonding"));
    testGraph.removeEdge({"ImportCIFStructure", "Structure", "DetectMolecules", "Structure"});
    testGraph.addEdge({"ImportCIFStructure", "Structure", "CalculateBonding", "Structure"});
    testGraph.addEdge({"CalculateBonding", "Structure", "DetectMolecules", "Structure"});

    testGraph.setUpdateRequired();
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);
    auto detectedMoleculeStructureB = detectMoleculesNode->detectedStructures();
    ASSERT_EQ(detectedMoleculeStructureB.size(), 2);
    testDetectedMolecularStructure(detectedMoleculeStructureB.at(0), {"Na", 4, 1});
    testDetectedMolecularStructure(detectedMoleculeStructureB.at(1), {"ClO3", 4, 4});
}

TEST_F(CIFNodeTest, CuBTC)
{
    TestGraph testGraph;

    // Load the CIF file
    auto cif = std::string("CuBTC-7108574.cif");

    EXPECT_TRUE(testGraph.appendNode("ImportCIFStructure"));
    testGraph.fetchHead()->setOption("FilePath", "cif/" + cif);
    ASSERT_TRUE(testGraph.appendNode("CalculateBonding"));
    testGraph.fetchHead()->setOption("Clear", true);
    ASSERT_TRUE(testGraph.appendNode("DetectMolecules"));
    testGraph.addEdge({"ImportCIFStructure", "Structure", "CalculateBonding", "Structure"});
    testGraph.addEdge({"CalculateBonding", "Structure", "DetectMolecules", "Structure"});

    ASSERT_EQ(testGraph.findNode("ImportCIFStructure")->run(), NodeConstants::ProcessResult::Success);

    // Check basic info
    auto detectMoleculesNode = static_cast<DetectMoleculesNode *>(testGraph.findNode("DetectMolecules"));
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);

    EXPECT_EQ(testGraph.findNode("ImportCIFStructure")->findOption("SpaceGroupID")->get<SpaceGroups::SpaceGroupId>(),
              SpaceGroups::SpaceGroup_225);
    constexpr auto A = 26.3336;
    // testBox(detectMoleculesNode->getOutputValue<Configuration *>("SupercellConfiguration"), {A, A, A}, {90, 90, 90}, 672);

    // Check basic formula (which includes bound water oxygens - with no H - at this point) and using O group
    /* TODO: Handle supercell configurations
    // 16 basic formula units per unit cell
    constexpr auto N = 16;
    EmpiricalFormula::EmpiricalFormulaMap cellFormulaH = {
        {Elements::Cu, 3 * N}, {Elements::C, 18 * N}, {Elements::H, 6 * N}, {Elements::O, 15 * N}};
    EXPECT_EQ(EmpiricalFormula::formula(detectMoleculesNode->getOutputValue<Configuration *>("SupercellConfiguration")->atoms(),
                                        [](const auto &i) { return i.speciesAtom()->Z(); }),
              EmpiricalFormula::formula(cellFormulaH));
              */
    auto detectedMoleculeStructureA = detectMoleculesNode->detectedStructures();
    EXPECT_EQ(detectedMoleculeStructureA.size(), 2);
}
/*
TEST_F(CIFNodeTest, CuBTCActiveAssemblies)
{
    TestGraph testGraph;
    // Change active assemblies to get amine-substituted structure

    EmpiricalFormula::EmpiricalFormulaMap cellFormulaNH2 = cellFormulaH;
    cellFormulaNH2[Elements::N] = 6 * N;
    cellFormulaNH2[Elements::H] *= 2;
    EXPECT_TRUE(testGraph.appendNode("SetCIFAtomGroupActivity", cifNameFromFile(cif) + "//AtomGroupA1"));
    testGraph.fetchHead()->setOption("Assembly", std::string("A"));
    testGraph.fetchHead()->setOption("AtomGroup", std::string("1"));
    testGraph.fetchHead()->setOption("SetActive", false);
    EXPECT_TRUE(testGraph.appendNode("SetCIFAtomGroupActivity", cifNameFromFile(cif) + "//AtomGroupB2"));
    testGraph.fetchHead()->setOption("Assembly", std::string("B"));
    testGraph.fetchHead()->setOption("AtomGroup", std::string("2"));
    testGraph.fetchHead()->setOption("SetActive", true);
    EXPECT_TRUE(testGraph.appendNode("SetCIFAtomGroupActivity", cifNameFromFile(cif) + "//AtomGroupC2"));
    testGraph.fetchHead()->setOption("Assembly", std::string("C"));
    testGraph.fetchHead()->setOption("AtomGroup", std::string("2"));
    testGraph.fetchHead()->setOption("SetActive", true);
    testGraph.removeEdge(
        {cifNameFromFile(cif) + "//StructureCleanup", "CIFContext", std::string(detectMoleculesNode->name()), "CIFContext"});
    testGraph.addEdge(
        {cifNameFromFile(cif) + "//StructureCleanup", "CIFContext", cifNameFromFile(cif) + "//AtomGroupA1", "CIFContext"});
    testGraph.addEdge(
        {cifNameFromFile(cif) + "//AtomGroupA1", "CIFContext", cifNameFromFile(cif) + "//AtomGroupB2", "CIFContext"});
    testGraph.addEdge(
        {cifNameFromFile(cif) + "//AtomGroupB2", "CIFContext", cifNameFromFile(cif) + "//AtomGroupC2", "CIFContext"});
    testGraph.addEdge(
        {cifNameFromFile(cif) + "//AtomGroupC2", "CIFContext", std::string(detectMoleculesNode->name()), "CIFContext"});
    testGraph.setUpdateRequired();
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(EmpiricalFormula::formula(detectMoleculesNode->getOutputValue<Configuration *>("SupercellConfiguration")->atoms(),
                                        [](const auto &i) { return i.speciesAtom()->Z(); }),
              EmpiricalFormula::formula(cellFormulaNH2));

    // Remove those free oxygens so we just have a framework
    auto removeAtomicsNode = testGraph.findNode(cifNameFromFile(cif) + "//RemoveAtomic");
    removeAtomicsNode->setOption("RemoveAtomics", true);
    testGraph.setUpdateRequired();
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);
    auto detectedMoleculeStructureB = detectMoleculesNode->detectedStructures();
    EXPECT_EQ(detectedMoleculeStructureB.size(), 0);

}
*/

TEST_F(CIFNodeTest, MoleculeOrderingSimple)
{
    TestGraph testGraph;

    auto cif = std::string("molecule-test-simple-ordered.cif");

    EXPECT_TRUE(testGraph.appendNode("ImportCIFStructure"));
    testGraph.fetchHead()->setOption("FilePath", "cif/" + cif);
    ASSERT_TRUE(testGraph.appendNode("DetectMolecules", "DetectMolecules"));
    testGraph.addEdge({"ImportCIFStructure", "Structure", "DetectMolecules", "Structure"});

    auto detectMoleculesNode = static_cast<DetectMoleculesNode *>(testGraph.findNode("DetectMolecules"));
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);

    auto detectedMoleculeStructures = detectMoleculesNode->detectedStructures();
    EXPECT_EQ(detectedMoleculeStructures.size(), 1);

    auto &molStructure = detectedMoleculeStructures.front();
    EmpiricalFormula::EmpiricalFormulaMap moleculeFormula = {
        {Elements::Cl, 1}, {Elements::O, 1}, {Elements::C, 1}, {Elements::H, 3}};
    testDetectedMolecularStructure(molStructure, {EmpiricalFormula::formula(moleculeFormula), 6, 6});

    // auto &unitCellSpecies = static_cast<CIFMolecularSpeciesNode *>(detectMoleculesNode)->cleanedUnitCellSpecies();
    // testInstanceConsistency(cifMolecule, unitCellSpecies);
}

TEST_F(CIFNodeTest, MoleculeOrderingSimpleUnordered)
{
    TestGraph testGraph;

    auto cif = std::string("molecule-test-simple-ordered.cif");

    EXPECT_TRUE(testGraph.appendNode("ImportCIFStructure"));
    testGraph.fetchHead()->setOption("FilePath", "cif/" + cif);
    ASSERT_TRUE(testGraph.appendNode("DetectMolecules", "DetectMolecules"));
    testGraph.addEdge({"ImportCIFStructure", "Structure", "DetectMolecules", "Structure"});

    auto detectMoleculesNode = static_cast<DetectMoleculesNode *>(testGraph.findNode("DetectMolecules"));
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);

    auto detectedMoleculeStructures = detectMoleculesNode->detectedStructures();
    EXPECT_EQ(detectedMoleculeStructures.size(), 1);

    auto &molStructure = detectedMoleculeStructures.front();
    EmpiricalFormula::EmpiricalFormulaMap moleculeFormula = {
        {Elements::Cl, 1}, {Elements::O, 1}, {Elements::C, 1}, {Elements::H, 3}};
    testDetectedMolecularStructure(molStructure, {EmpiricalFormula::formula(moleculeFormula), 6, 6});

    // auto &unitCellSpecies = static_cast<CIFMolecularSpeciesNode *>(detectMoleculesNode)->cleanedUnitCellSpecies();
    // testInstanceConsistency(cifMolecule, unitCellSpecies);
}

TEST_F(CIFNodeTest, MoleculeOrderingSimpleUnorderedRotated)
{
    TestGraph testGraph;

    auto cif = std::string("molecule-test-simple-unordered-rotated.cif");

    EXPECT_TRUE(testGraph.appendNode("ImportCIFStructure"));
    testGraph.fetchHead()->setOption("FilePath", "cif/" + cif);
    ASSERT_TRUE(testGraph.appendNode("DetectMolecules", "DetectMolecules"));
    testGraph.addEdge({"ImportCIFStructure", "Structure", "DetectMolecules", "Structure"});

    auto detectMoleculesNode = static_cast<DetectMoleculesNode *>(testGraph.findNode("DetectMolecules"));
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);

    auto detectedMoleculeStructures = detectMoleculesNode->detectedStructures();
    EXPECT_EQ(detectedMoleculeStructures.size(), 1);

    auto &molStructure = detectedMoleculeStructures.front();
    EmpiricalFormula::EmpiricalFormulaMap moleculeFormula = {
        {Elements::Cl, 1}, {Elements::O, 1}, {Elements::C, 1}, {Elements::H, 3}};
    testDetectedMolecularStructure(molStructure, {EmpiricalFormula::formula(moleculeFormula), 6, 6});

    // auto &unitCellSpecies = static_cast<CIFMolecularSpeciesNode *>(detectMoleculesNode)->cleanedUnitCellSpecies();
    // testInstanceConsistency(cifMolecule, unitCellSpecies);
}

TEST_F(CIFNodeTest, BigMoleculeOrdering)
{
    TestGraph testGraph;

    const auto cif = std::string("Bisphen_n_arenes_1517789.cif");

    EXPECT_TRUE(testGraph.appendNode("ImportCIFStructure"));
    testGraph.fetchHead()->setOption("FilePath", "cif/" + cif);
    ASSERT_TRUE(testGraph.appendNode("DetectMolecules", "DetectMolecules"));
    testGraph.addEdge({"ImportCIFStructure", "Structure", "DetectMolecules", "Structure"});

    auto detectMoleculesNode = static_cast<DetectMoleculesNode *>(testGraph.findNode("DetectMolecules"));
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);

    auto detectedStructures = detectMoleculesNode->detectedStructures();
    EXPECT_EQ(detectedStructures.size(), 1);

    auto &molStructure = detectedStructures.front();
    EmpiricalFormula::EmpiricalFormulaMap moleculeFormula = {{Elements::O, 6}, {Elements::C, 51}, {Elements::H, 54}};
    testDetectedMolecularStructure(molStructure, {EmpiricalFormula::formula(moleculeFormula), 4, 111});

    // auto &unitCellSpecies = static_cast<CIFMolecularSpeciesNode *>(detectMoleculesNode)->cleanedUnitCellSpecies();
    // testInstanceConsistency(cifMolecule, unitCellSpecies);
}

} // namespace UnitTest
