// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/empiricalFormula.h"
#include "data/elements.h"
#include "nodes/calculateBonding.h"
#include "nodes/cif/importCIFStructure.h"
#include "nodes/detectMolecules.h"
#include "nodes/exportXYZConfiguration.h"
#include "nodes/replicatedConfiguration.h"
#include "nodes/species.h"
#include "tests/testGraph.h"
#include <gtest/gtest.h>
#include <optional>
#include <string>

namespace UnitTest
{
class CIFNodeTest : public ::testing::Test
{
    public:
    CIFNodeTest() = default;
    ~CIFNodeTest() = default;

    protected:
    // The test graph
    TestGraph testGraph_;
    // Nodes
    ImportCIFStructureNode *importCIFStructureNode_{nullptr};
    CalculateBondingNode *calculateBondingNode_{nullptr};
    DetectMoleculesNode *detectMoleculesNode_{nullptr};
    // Molecular species information
    using MolecularSpeciesInfo = std::tuple<std::string, int, int>;

    protected:
    // Set up and load the specified CIF
    void setUp(std::string cifFile, bool calculateBonding = true)
    {
        importCIFStructureNode_ = static_cast<ImportCIFStructureNode *>(testGraph_.appendNode("ImportCIFStructure"));
        ASSERT_TRUE(importCIFStructureNode_);
        importCIFStructureNode_->setOption("FilePath", "cif/" + cifFile);

        if (calculateBonding)
        {
            calculateBondingNode_ = static_cast<CalculateBondingNode *>(testGraph_.appendNode("CalculateBonding"));
            ASSERT_TRUE(calculateBondingNode_);
            calculateBondingNode_->setOption("Clear", true);

            testGraph_.addEdge({std::string(importCIFStructureNode_->name()), "Structure",
                                std::string(calculateBondingNode_->name()), "Structure"});
        }

        detectMoleculesNode_ = static_cast<DetectMoleculesNode *>(testGraph_.appendNode("DetectMolecules"));
        ASSERT_TRUE(detectMoleculesNode_);
        testGraph_.addEdge({std::string(calculateBonding ? calculateBondingNode_->name() : importCIFStructureNode_->name()),
                            "Structure", std::string(detectMoleculesNode_->name()), "Structure"});
    }
    // Test supplied structure against that reconstructed from detected sub-structures
    [[nodiscard]] testing::AssertionResult testReconstructed(const Structure &cif, std::optional<Vector3i> repeat = {})
    {
        // Create a configuration
        auto *lastNode = testGraph_.appendNode("Configuration", "Reconstructed");
        if (!lastNode)
            return testing::AssertionFailure() << "Failed to create Configuration node.";

        // Instantiate detected structures
        for (const auto &[name, structure] : detectMoleculesNode_->detectedStructures())
        {
            // Take the output from detectedMoleculesNode and create a Species from it
            auto *speciesNode = static_cast<SpeciesNode *>(testGraph_.appendNode("Species", name));
            if (!speciesNode)
                return testing::AssertionFailure() << std::format("Failed to create Species node '{}'.", name);

            testGraph_.addEdge({std::string(detectMoleculesNode_->name()), name, name, "Structure"});

            // Instantiate the species in the configuration
            auto *instantiateNode = testGraph_.appendNode("Instantiate", std::format("Instantiate{}", name));
            if (!instantiateNode)
                return testing::AssertionFailure() << std::format("Failed to create Instantiate node 'Instantiate{}'.", name);
            testGraph_.addEdge({name, "Species", std::string(instantiateNode->name()), "Species"});
            testGraph_.addEdge(
                {std::string(lastNode->name()), "Configuration", std::string(instantiateNode->name()), "Configuration"});
            lastNode = instantiateNode;
        }

        // Replicate?
        if (repeat)
        {
            auto *replicatedConfigurationNode =
                static_cast<ReplicatedConfigurationNode *>(testGraph_.appendNode("ReplicatedConfiguration"));
            if (!replicatedConfigurationNode)
                return testing::AssertionFailure() << "Failed to create ReplicatedConfigurationNode.";

            replicatedConfigurationNode->setOption("Repeat", *repeat);
            testGraph_.addEdge({std::string(lastNode->name()), "Configuration",
                                std::string(replicatedConfigurationNode->name()), "Configuration"});
            lastNode = replicatedConfigurationNode;
        }

        // Run the graph from the last node
        if (lastNode->run() != NodeConstants::ProcessResult::Success)
            return testing::AssertionFailure() << "Failed to run the reconstruction graph.";

        // Get the configuration from the last node
        auto *cfg = lastNode->getOutputValue<Configuration *>("Configuration");
        if (!cfg)
            return testing::AssertionFailure() << "Failed to retrieve reconstructed configuration.";
        ExportXYZConfigurationNode::exportConfiguration(cfg, "THIS.xyz");

        // Check atom-for-atom - search for atoms in the original CIF structure in the reconstructed configuration
        auto repeats = repeat.value_or({1, 1, 1});
        for (auto repeatX = 0; repeatX < repeats.x; ++repeatX)
            for (auto repeatY = 0; repeatY < repeats.y; ++repeatY)
                for (auto repeatZ = 0; repeatZ < repeats.z; ++repeatZ)
                {
                    for (const auto &structureAtom : cif.atoms())
                    {
                        auto r = structureAtom->r() + cif.box().axes() * Vector3(repeatX, repeatY, repeatZ);
                        if (std::ranges::find_if(cfg->atoms(),
                                                 [&structureAtom, r](const auto &cfgAtom)
                                                 {
                                                     return structureAtom->Z() == cfgAtom.Z() &&
                                                            fabs(r.x - cfgAtom.r().x) < 1.0e-6 &&
                                                            fabs(r.y - cfgAtom.r().y) < 1.0e-6 &&
                                                            fabs(r.z - cfgAtom.r().z) < 1.0e-6;
                                                 }) == cfg->atoms().end())
                            return testing::AssertionFailure()
                                   << std::format("Failed to find atom {} @ {},{},{} in the reconstructed structure.",
                                                  Elements::symbol(structureAtom->Z()), r.x, r.y, r.z);
                    }
                }

        return testing::AssertionSuccess();
    }
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
    void testDetectedMolecularStructure(const std::map<std::string, Structure> &structures, const MolecularSpeciesInfo &info)
    {
        ASSERT_TRUE(structures.contains(std::get<0>(info)));
        EXPECT_EQ(structures.at(std::get<0>(info)).instances().size(), std::get<1>(info));
        EXPECT_EQ(structures.at(std::get<0>(info)).nAtoms(), std::get<2>(info));
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
    // Test files with expected number of structure atoms
    std::vector<std::pair<std::string, int>> cifs = {{"1557470.cif", 86}, {"1557599.cif", 56}, {"7705246.cif", 364},
                                                     {"9000004.cif", 6},  {"9000095.cif", 30}, {"9000418.cif", 64}};

    for (auto &[cif, nStructureAtoms] : cifs)
    {
        ASSERT_TRUE(testGraph_.appendNode("ImportCIFStructure", cif));
        testGraph_.fetchHead()->setOption("FilePath", "cif/" + cif);
        ASSERT_EQ(testGraph_.fetchHead()->run(), NodeConstants::ProcessResult::Success);
        const auto structure = testGraph_.fetchHead()->getOutputValue<Structure>("Structure");
        ASSERT_EQ(structure.atoms().size(), nStructureAtoms);
    }
}

TEST_F(CIFNodeTest, NaClContinuous)
{
    setUp("NaCl-1000041.cif");

    // Basic info
    ASSERT_EQ(importCIFStructureNode_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(importCIFStructureNode_->findOption("SpaceGroupID")->get<SpaceGroups::SpaceGroupId>(),
              SpaceGroups::SpaceGroup_225);

    // We should find a continuous framework after rebonding and the detect molecules node should fail accordingly
    ASSERT_EQ(detectMoleculesNode_->run(), NodeConstants::ProcessResult::Failed);
}

TEST_F(CIFNodeTest, NaCl)
{
    setUp("NaCl-1000041.cif", false);

    ASSERT_EQ(detectMoleculesNode_->run(), NodeConstants::ProcessResult::Success);

    // Check atomic positions
    constexpr double A = 5.62;
    std::vector<Vector3> R = {{0.0, 0.0, 0.0}, {0.0, A / 2, A / 2}, {A / 2, 0.0, A / 2}, {A / 2, A / 2, 0.0}};
    auto structures = detectMoleculesNode_->detectedStructures();
    EXPECT_EQ(structures.size(), 2);
    testDetectedMolecularStructure(structures, {"Na", 4, 1});
    for (auto &&[instance, r2] : zip(structures["Na"].instances(), R))
        EXPECT_TRUE(testVector3("Molecular instance coordinates", instance[0], r2));
    testDetectedMolecularStructure(structures, {"Cl", 4, 1});
    for (auto &&[instance, r2] : zip(structures["Cl"].instances(), R))
        EXPECT_TRUE(testVector3("Molecular instance coordinates", instance[0], (r2 - A / 2).abs()));

    ASSERT_TRUE(testReconstructed(importCIFStructureNode_->getOutputValue<Structure>("Structure"), Vector3i(2, 2, 2)));
}

TEST_F(CIFNodeTest, NaClO3Atomic)
{
    setUp("NaClO3-1010057", false);

    ASSERT_EQ(importCIFStructureNode_->run(), NodeConstants::ProcessResult::Success);

    // Check basic info
    auto detectMoleculesNode = static_cast<DetectMoleculesNode *>(testGraph_.findNode("DetectMolecules"));

    EXPECT_EQ(testGraph_.findNode("ImportCIFStructure")->findOption("SpaceGroupID")->get<SpaceGroups::SpaceGroupId>(),
              SpaceGroups::SpaceGroup_198);

    // No bonding defs in the CIF, so we expect species for each atomic component (4 Na, 4 Cl, and 12 O)
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(detectMoleculesNode_->detectedStructures().size(), 3);
    testDetectedMolecularStructure(detectMoleculesNode_->detectedStructures(), {"Na", 4, 1});
    testDetectedMolecularStructure(detectMoleculesNode_->detectedStructures(), {"Cl", 4, 1});
    testDetectedMolecularStructure(detectMoleculesNode_->detectedStructures(), {"O", 12, 1});

    ASSERT_TRUE(testReconstructed(importCIFStructureNode_->getOutputValue<Structure>("Structure"), Vector3i(2, 2, 2)));
}

TEST_F(CIFNodeTest, NaClO3Molecular)
{
    setUp("NaClO3-1010057");

    ASSERT_EQ(detectMoleculesNode_->run(), NodeConstants::ProcessResult::Success);

    ASSERT_EQ(detectMoleculesNode_->detectedStructures().size(), 2);
    testDetectedMolecularStructure(detectMoleculesNode_->detectedStructures(), {"Na", 4, 1});
    testDetectedMolecularStructure(detectMoleculesNode_->detectedStructures(), {"ClO3", 4, 4});

    ASSERT_TRUE(testReconstructed(importCIFStructureNode_->getOutputValue<Structure>("Structure"), Vector3i(2, 2, 2)));
}

TEST_F(CIFNodeTest, CuBTC)
{
    // Load the CIF file
    auto cif = std::string("CuBTC-7108574.cif");

    EXPECT_TRUE(testGraph_.appendNode("ImportCIFStructure"));
    testGraph_.fetchHead()->setOption("FilePath", "cif/" + cif);
    ASSERT_TRUE(testGraph_.appendNode("CalculateBonding"));
    testGraph_.fetchHead()->setOption("Clear", true);
    ASSERT_TRUE(testGraph_.appendNode("DetectMolecules"));
    testGraph_.addEdge({"ImportCIFStructure", "Structure", "CalculateBonding", "Structure"});
    testGraph_.addEdge({"CalculateBonding", "Structure", "DetectMolecules", "Structure"});

    ASSERT_EQ(testGraph_.findNode("ImportCIFStructure")->run(), NodeConstants::ProcessResult::Success);

    // Check basic info
    auto detectMoleculesNode = static_cast<DetectMoleculesNode *>(testGraph_.findNode("DetectMolecules"));
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);

    auto detectedMoleculeStructures = detectMoleculesNode->detectedStructures();
    EXPECT_EQ(detectedMoleculeStructures.size(), 2);

    EXPECT_EQ(testGraph_.findNode("ImportCIFStructure")->findOption("SpaceGroupID")->get<SpaceGroups::SpaceGroupId>(),
              SpaceGroups::SpaceGroup_225);

    /* TODO: Handle supercell configurations
    constexpr auto A = 26.3336;
    // testBox(detectMoleculesNode->getOutputValue<Configuration *>("SupercellConfiguration"), {A, A, A}, {90, 90, 90},
    672);

    // Check basic formula (which includes bound water oxygens - with no H - at this point) and using O group

    // 16 basic formula units per unit cell
    constexpr auto N = 16;
    EmpiricalFormula::EmpiricalFormulaMap cellFormulaH = {
        {Elements::Cu, 3 * N}, {Elements::C, 18 * N}, {Elements::H, 6 * N}, {Elements::O, 15 * N}};
    EXPECT_EQ(EmpiricalFormula::formula(detectMoleculesNode->getOutputValue<Configuration
    *>("SupercellConfiguration")->atoms(),
                                        [](const auto &i) { return i.speciesAtom()->Z(); }),
              EmpiricalFormula::formula(cellFormulaH));
              */
}
/*
TEST_F(CIFNodeTest, CuBTCActiveAssemblies)
{
    TestGraph testGraph;
    // Change active assemblies to get amine-substituted structure

    EmpiricalFormula::EmpiricalFormulaMap cellFormulaNH2 = cellFormulaH;
    cellFormulaNH2[Elements::N] = 6 * N;
    cellFormulaNH2[Elements::H] *= 2;
    EXPECT_TRUE(testGraph_.appendNode("SetCIFAtomGroupActivity", cifNameFromFile(cif) + "//AtomGroupA1"));
    testGraph_.fetchHead()->setOption("Assembly", std::string("A"));
    testGraph_.fetchHead()->setOption("AtomGroup", std::string("1"));
    testGraph_.fetchHead()->setOption("SetActive", false);
    EXPECT_TRUE(testGraph_.appendNode("SetCIFAtomGroupActivity", cifNameFromFile(cif) + "//AtomGroupB2"));
    testGraph_.fetchHead()->setOption("Assembly", std::string("B"));
    testGraph_.fetchHead()->setOption("AtomGroup", std::string("2"));
    testGraph_.fetchHead()->setOption("SetActive", true);
    EXPECT_TRUE(testGraph_.appendNode("SetCIFAtomGroupActivity", cifNameFromFile(cif) + "//AtomGroupC2"));
    testGraph_.fetchHead()->setOption("Assembly", std::string("C"));
    testGraph_.fetchHead()->setOption("AtomGroup", std::string("2"));
    testGraph_.fetchHead()->setOption("SetActive", true);
    testGraph_.removeEdge(
        {cifNameFromFile(cif) + "//StructureCleanup", "CIFContext", std::string(detectMoleculesNode->name()),
"CIFContext"}); testGraph_.addEdge( {cifNameFromFile(cif) + "//StructureCleanup", "CIFContext", cifNameFromFile(cif) +
"//AtomGroupA1", "CIFContext"}); testGraph_.addEdge( {cifNameFromFile(cif) + "//AtomGroupA1", "CIFContext",
cifNameFromFile(cif) + "//AtomGroupB2", "CIFContext"}); testGraph_.addEdge( {cifNameFromFile(cif) + "//AtomGroupB2",
"CIFContext", cifNameFromFile(cif) + "//AtomGroupC2", "CIFContext"}); testGraph_.addEdge( {cifNameFromFile(cif) +
"//AtomGroupC2", "CIFContext", std::string(detectMoleculesNode->name()), "CIFContext"}); testGraph_.setUpdateRequired();
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(EmpiricalFormula::formula(detectMoleculesNode->getOutputValue<Configuration
*>("SupercellConfiguration")->atoms(),
                                        [](const auto &i) { return i.speciesAtom()->Z(); }),
              EmpiricalFormula::formula(cellFormulaNH2));

    // Remove those free oxygens so we just have a framework
    auto removeAtomicsNode = testGraph_.findNode(cifNameFromFile(cif) + "//RemoveAtomic");
    removeAtomicsNode->setOption("RemoveAtomics", true);
    testGraph_.setUpdateRequired();
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);
    auto detectedMoleculeStructuresB = getDetectedMolecularStructures(detectMoleculesNode, 2);
    EXPECT_EQ(detectedMoleculeStructuresB.size(), 0);

}
*/

TEST_F(CIFNodeTest, MoleculeOrderingSimple)
{
    TestGraph testGraph;

    auto cif = std::string("molecule-test-simple-ordered.cif");

    EXPECT_TRUE(testGraph_.appendNode("ImportCIFStructure"));
    testGraph_.fetchHead()->setOption("FilePath", "cif/" + cif);
    ASSERT_TRUE(testGraph_.appendNode("CalculateBonding"));
    ASSERT_TRUE(testGraph_.appendNode("DetectMolecules", "DetectMolecules"));
    testGraph_.addEdge({"ImportCIFStructure", "Structure", "CalculateBonding", "Structure"});
    testGraph_.addEdge({"CalculateBonding", "Structure", "DetectMolecules", "Structure"});

    auto detectMoleculesNode = static_cast<DetectMoleculesNode *>(testGraph_.findNode("DetectMolecules"));
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);

    auto detectedMoleculeStructures = detectMoleculesNode->detectedStructures();
    EXPECT_EQ(detectedMoleculeStructures.size(), 1);

    EmpiricalFormula::EmpiricalFormulaMap moleculeFormula = {
        {Elements::Cl, 1}, {Elements::O, 1}, {Elements::C, 1}, {Elements::H, 3}};
    testDetectedMolecularStructure(detectedMoleculeStructures, {EmpiricalFormula::formula(moleculeFormula), 6, 6});

    // auto &unitCellSpecies = static_cast<CIFMolecularSpeciesNode *>(detectMoleculesNode)->cleanedUnitCellSpecies();
    // testInstanceConsistency(cifMolecule, unitCellSpecies);
}

TEST_F(CIFNodeTest, MoleculeOrderingSimpleUnordered)
{
    TestGraph testGraph;

    auto cif = std::string("molecule-test-simple-ordered.cif");

    EXPECT_TRUE(testGraph_.appendNode("ImportCIFStructure"));
    testGraph_.fetchHead()->setOption("FilePath", "cif/" + cif);
    ASSERT_TRUE(testGraph_.appendNode("CalculateBonding"));
    ASSERT_TRUE(testGraph_.appendNode("DetectMolecules", "DetectMolecules"));
    testGraph_.addEdge({"ImportCIFStructure", "Structure", "CalculateBonding", "Structure"});
    testGraph_.addEdge({"CalculateBonding", "Structure", "DetectMolecules", "Structure"});

    auto detectMoleculesNode = static_cast<DetectMoleculesNode *>(testGraph_.findNode("DetectMolecules"));
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);

    auto detectedMoleculeStructures = detectMoleculesNode->detectedStructures();
    EXPECT_EQ(detectedMoleculeStructures.size(), 1);

    EmpiricalFormula::EmpiricalFormulaMap moleculeFormula = {
        {Elements::Cl, 1}, {Elements::O, 1}, {Elements::C, 1}, {Elements::H, 3}};
    testDetectedMolecularStructure(detectedMoleculeStructures, {EmpiricalFormula::formula(moleculeFormula), 6, 6});

    // auto &unitCellSpecies = static_cast<CIFMolecularSpeciesNode *>(detectMoleculesNode)->cleanedUnitCellSpecies();
    // testInstanceConsistency(cifMolecule, unitCellSpecies);
}

TEST_F(CIFNodeTest, MoleculeOrderingSimpleUnorderedRotated)
{
    TestGraph testGraph;

    auto cif = std::string("molecule-test-simple-unordered-rotated.cif");

    EXPECT_TRUE(testGraph_.appendNode("ImportCIFStructure"));
    testGraph_.fetchHead()->setOption("FilePath", "cif/" + cif);
    ASSERT_TRUE(testGraph_.appendNode("CalculateBonding"));
    ASSERT_TRUE(testGraph_.appendNode("DetectMolecules", "DetectMolecules"));
    testGraph_.addEdge({"ImportCIFStructure", "Structure", "CalculateBonding", "Structure"});
    testGraph_.addEdge({"CalculateBonding", "Structure", "DetectMolecules", "Structure"});

    auto detectMoleculesNode = static_cast<DetectMoleculesNode *>(testGraph_.findNode("DetectMolecules"));
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);

    auto detectedMoleculeStructures = detectMoleculesNode->detectedStructures();
    EXPECT_EQ(detectedMoleculeStructures.size(), 1);

    EmpiricalFormula::EmpiricalFormulaMap moleculeFormula = {
        {Elements::Cl, 1}, {Elements::O, 1}, {Elements::C, 1}, {Elements::H, 3}};
    testDetectedMolecularStructure(detectedMoleculeStructures, {EmpiricalFormula::formula(moleculeFormula), 6, 6});

    // auto &unitCellSpecies = static_cast<CIFMolecularSpeciesNode *>(detectMoleculesNode)->cleanedUnitCellSpecies();
    // testInstanceConsistency(cifMolecule, unitCellSpecies);
}

TEST_F(CIFNodeTest, BigMoleculeOrdering)
{
    TestGraph testGraph;

    const auto cif = std::string("Bisphen_n_arenes_1517789.cif");

    EXPECT_TRUE(testGraph_.appendNode("ImportCIFStructure"));
    testGraph_.fetchHead()->setOption("FilePath", "cif/" + cif);
    ASSERT_TRUE(testGraph_.appendNode("DetectMolecules", "DetectMolecules"));
    testGraph_.addEdge({"ImportCIFStructure", "Structure", "DetectMolecules", "Structure"});

    auto detectMoleculesNode = static_cast<DetectMoleculesNode *>(testGraph_.findNode("DetectMolecules"));
    ASSERT_EQ(detectMoleculesNode->run(), NodeConstants::ProcessResult::Success);

    auto detectedStructures = detectMoleculesNode->detectedStructures();
    EXPECT_EQ(detectedStructures.size(), 1);

    EmpiricalFormula::EmpiricalFormulaMap moleculeFormula = {{Elements::O, 6}, {Elements::C, 51}, {Elements::H, 54}};
    testDetectedMolecularStructure(detectedStructures, {EmpiricalFormula::formula(moleculeFormula), 4, 111});

    // auto &unitCellSpecies = static_cast<CIFMolecularSpeciesNode *>(detectMoleculesNode)->cleanedUnitCellSpecies();
    // testInstanceConsistency(cifMolecule, unitCellSpecies);
}

} // namespace UnitTest